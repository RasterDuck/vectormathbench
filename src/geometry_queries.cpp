#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <glm/gtx/closest_point.hpp>
#include <mv/math/Math.hpp>

// Keep DirectXMath's Linux SAL compatibility macros after all headers that can
// include the C++ standard library; its legacy `__in` macro otherwise collides
// with libstdc++ implementation identifiers.
#include <DirectXCollision.h>

namespace
{
    constexpr std::array<std::size_t, 3> WorkingSetSizes = {
        256U, 4U * 1024U, 64U * 1024U};

    struct RawRay
    {
        mv::math::Vec3f Origin;
        mv::math::Vec3f Direction;
    };

    struct RawAabb
    {
        mv::math::Vec3f Minimum;
        mv::math::Vec3f Maximum;
    };

    struct RawPreparedRay
    {
        RawRay Ray;
        mv::math::Vec3f ReciprocalDirection;
        unsigned int ParallelMask;
    };

    struct RawTriangle
    {
        mv::math::Vec3f First;
        mv::math::Vec3f Second;
        mv::math::Vec3f Third;
    };

    struct RawTriangleHit
    {
        float Distance;
        mv::math::Vec3f Barycentric;
    };

    struct RawSegment
    {
        mv::math::Vec3f Start;
        mv::math::Vec3f End;
    };

    template <typename T>
    void Observe(const std::vector<T>& values)
    {
        ankerl::nanobench::doNotOptimizeAway(values.data());
        ankerl::nanobench::doNotOptimizeAway(values.size());
    }

    [[nodiscard]] float Seed(std::size_t index, std::size_t lane)
    {
        const std::size_t value =
            (index * 1664525U + lane * 1013904223U) & 0xFFFFU;
        return static_cast<float>(value) / 65535.0F;
    }

    [[nodiscard]] float Component(
        const mv::math::Vec3f& value, std::size_t axis)
    {
        return axis == 0U ? value.X() : axis == 1U ? value.Y() : value.Z();
    }

    [[nodiscard]] bool IsParallel(float value)
    {
        return value == 0.0F || !std::isfinite(1.0F / value);
    }

    [[nodiscard]] RawPreparedRay Prepare(const RawRay& ray)
    {
        const auto reciprocal = [](float value)
        {
            return value == 0.0F
                       ? std::copysign(
                             std::numeric_limits<float>::infinity(), value)
                       : 1.0F / value;
        };
        return {ray,
            mv::math::Vec3f(reciprocal(ray.Direction.X()),
                reciprocal(ray.Direction.Y()), reciprocal(ray.Direction.Z())),
            (IsParallel(ray.Direction.X()) ? 1U : 0U) |
                (IsParallel(ray.Direction.Y()) ? 2U : 0U) |
                (IsParallel(ray.Direction.Z()) ? 4U : 0U)};
    }

    [[nodiscard]] bool Intersects(const RawPreparedRay& ray, const RawAabb& box)
    {
        float entry = 0.0F;
        float exit = std::numeric_limits<float>::infinity();
        for (std::size_t axis = 0U; axis < 3U; ++axis)
        {
            const float origin = Component(ray.Ray.Origin, axis);
            const float minimum = Component(box.Minimum, axis);
            const float maximum = Component(box.Maximum, axis);
            if ((ray.ParallelMask & (1U << axis)) != 0U)
            {
                if (origin < minimum || origin > maximum)
                {
                    return false;
                }
                continue;
            }

            const float reciprocal = Component(ray.ReciprocalDirection, axis);
            float nearDistance = (minimum - origin) * reciprocal;
            float farDistance = (maximum - origin) * reciprocal;
            if (nearDistance > farDistance)
            {
                std::swap(nearDistance, farDistance);
            }
            entry = std::max(entry, nearDistance);
            exit = std::min(exit, farDistance);
            if (entry > exit)
            {
                return false;
            }
        }
        return exit >= 0.0F && std::isfinite(exit);
    }

    [[nodiscard]] bool Intersects(const RawRay& ray, const RawAabb& box)
    {
        return Intersects(Prepare(ray), box);
    }

    [[nodiscard]] std::optional<RawTriangleHit> Intersect(
        const RawRay& ray, const RawTriangle& triangle)
    {
        constexpr float ParallelTolerance = 1.0e-7F;
        const mv::math::Vec3f edge01 = triangle.Second - triangle.First;
        const mv::math::Vec3f edge02 = triangle.Third - triangle.First;
        const mv::math::Vec3f directionCrossEdge02 =
            mv::math::Cross(ray.Direction, edge02);
        const float determinant = mv::math::Dot(edge01, directionCrossEdge02);
        if (!std::isfinite(determinant) ||
            std::abs(determinant) <= ParallelTolerance)
        {
            return std::nullopt;
        }

        const float reciprocalDeterminant = 1.0F / determinant;
        const mv::math::Vec3f fromFirst = ray.Origin - triangle.First;
        const float secondWeight =
            mv::math::Dot(fromFirst, directionCrossEdge02) *
            reciprocalDeterminant;
        if (!(secondWeight >= 0.0F && secondWeight <= 1.0F))
        {
            return std::nullopt;
        }

        const mv::math::Vec3f fromFirstCrossEdge01 =
            mv::math::Cross(fromFirst, edge01);
        const float thirdWeight =
            mv::math::Dot(ray.Direction, fromFirstCrossEdge01) *
            reciprocalDeterminant;
        if (!(thirdWeight >= 0.0F && secondWeight + thirdWeight <= 1.0F))
        {
            return std::nullopt;
        }

        const float distance =
            mv::math::Dot(edge02, fromFirstCrossEdge01) * reciprocalDeterminant;
        if (!(distance >= 0.0F) || !std::isfinite(distance))
        {
            return std::nullopt;
        }
        return RawTriangleHit{
            distance, mv::math::Vec3f(1.0F - secondWeight - thirdWeight,
                          secondWeight, thirdWeight)};
    }

    // Projection and endpoint clamp follow Geometric Tools
    // DistPointSegment.h (BSL-1.0); this raw form isolates facade overhead.
    [[nodiscard]] mv::math::Vec3f ClosestPoint(
        const mv::math::Vec3f& point, const RawSegment& segment)
    {
        const mv::math::Vec3f displacement = segment.End - segment.Start;
        const float lengthSquared = mv::math::LengthSquared(displacement);
        const float fraction =
            lengthSquared > 0.0F
                ? std::clamp(
                      mv::math::Dot(point - segment.Start, displacement) /
                          lengthSquared,
                      0.0F, 1.0F)
                : 0.0F;
        return segment.Start + displacement * fraction;
    }

    [[nodiscard]] bool Intersects(
        const RawRay& ray, const RawTriangle& triangle)
    {
        constexpr float ParallelTolerance = 1.0e-7F;
        const mv::math::Vec3f edge01 = triangle.Second - triangle.First;
        const mv::math::Vec3f edge02 = triangle.Third - triangle.First;
        const mv::math::Vec3f directionCrossEdge02 =
            mv::math::Cross(ray.Direction, edge02);
        const float determinant = mv::math::Dot(edge01, directionCrossEdge02);
        if (!std::isfinite(determinant) ||
            std::abs(determinant) <= ParallelTolerance)
        {
            return false;
        }

        const float reciprocalDeterminant = 1.0F / determinant;
        const mv::math::Vec3f fromFirst = ray.Origin - triangle.First;
        const float secondWeight =
            mv::math::Dot(fromFirst, directionCrossEdge02) *
            reciprocalDeterminant;
        if (!(secondWeight >= 0.0F && secondWeight <= 1.0F))
        {
            return false;
        }

        const mv::math::Vec3f fromFirstCrossEdge01 =
            mv::math::Cross(fromFirst, edge01);
        const float thirdWeight =
            mv::math::Dot(ray.Direction, fromFirstCrossEdge01) *
            reciprocalDeterminant;
        if (!(thirdWeight >= 0.0F && secondWeight + thirdWeight <= 1.0F))
        {
            return false;
        }

        const float distance =
            mv::math::Dot(edge02, fromFirstCrossEdge01) * reciprocalDeterminant;
        return distance >= 0.0F && std::isfinite(distance);
    }

    [[nodiscard]] mv::math::Ray3f MakeSemanticRay(std::size_t index)
    {
        const mv::math::Vec3f rawDirection(0.75F + Seed(index, 3U),
            Seed(index, 4U) - 0.5F, Seed(index, 5U) - 0.5F);
        const auto direction = mv::math::Direction3f::TryFrom(rawDirection);
        if (!direction)
        {
            std::abort();
        }
        const auto ray = mv::math::Ray3f::TryFromOriginDirection(
            mv::math::Point3f(-20.0F, Seed(index, 0U) * 8.0F - 4.0F,
                Seed(index, 1U) * 8.0F - 4.0F),
            *direction);
        if (!ray)
        {
            std::abort();
        }
        return *ray;
    }

    [[nodiscard]] mv::math::Aabb3f MakeSemanticAabb(std::size_t index)
    {
        const mv::math::Point3f center(Seed(index, 6U) * 40.0F - 5.0F,
            Seed(index, 7U) * 12.0F - 6.0F, Seed(index, 8U) * 12.0F - 6.0F);
        const mv::math::Vec3f halfExtents(0.25F + Seed(index, 9U) * 2.0F,
            0.25F + Seed(index, 10U) * 2.0F, 0.25F + Seed(index, 11U) * 2.0F);
        const auto box =
            mv::math::Aabb3f::TryFromCenterHalfExtents(center, halfExtents);
        if (!box)
        {
            std::abort();
        }
        return *box;
    }

    [[nodiscard]] mv::math::Triangle3f MakeSemanticTriangle(std::size_t index)
    {
        const float x = Seed(index, 12U) * 8.0F - 4.0F;
        const float y = Seed(index, 13U) * 8.0F - 4.0F;
        const float size = 0.25F + Seed(index, 14U) * 2.0F;
        const auto triangle = mv::math::Triangle3f::TryFromPoints(
            mv::math::Point3f(x, y, 0.0F), mv::math::Point3f(x + size, y, 0.0F),
            mv::math::Point3f(x, y + size, 0.0F));
        if (!triangle)
        {
            std::abort();
        }
        return *triangle;
    }

    [[nodiscard]] mv::math::Point3f MakeSemanticPoint(std::size_t index)
    {
        return mv::math::Point3f(Seed(index, 17U) * 30.0F - 15.0F,
            Seed(index, 18U) * 20.0F - 10.0F, Seed(index, 19U) * 20.0F - 10.0F);
    }

    [[nodiscard]] mv::math::Segment3f MakeSemanticSegment(std::size_t index)
    {
        const mv::math::Point3f start(Seed(index, 20U) * 20.0F - 10.0F,
            Seed(index, 21U) * 20.0F - 10.0F, Seed(index, 22U) * 20.0F - 10.0F);
        const mv::math::Vec3f displacement(0.25F + Seed(index, 23U) * 4.0F,
            Seed(index, 24U) * 4.0F - 2.0F, Seed(index, 25U) * 4.0F - 2.0F);
        return mv::math::Segment3f(start, start + displacement);
    }

    [[nodiscard]] mv::math::Capsule3f MakeSemanticCapsule(std::size_t index)
    {
        const auto capsule = mv::math::Capsule3f::TryFromSegmentRadius(
            MakeSemanticSegment(index), 0.1F + Seed(index, 26U) * 1.5F);
        if (!capsule)
        {
            std::abort();
        }
        return *capsule;
    }

    void BenchmarkRayAabb(ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<mv::math::Ray3f> semanticRays(count);
        std::vector<mv::math::Aabb3f> semanticBoxes(count);
        std::vector<RawRay> rawRays(count);
        std::vector<RawAabb> rawBoxes(count);
        std::vector<std::uint8_t> output(count);
        for (std::size_t index = 0U; index < count; ++index)
        {
            semanticRays[index] = MakeSemanticRay(index);
            semanticBoxes[index] = MakeSemanticAabb(index);
            rawRays[index] = {semanticRays[index].Origin().Vector(),
                semanticRays[index].Direction().Vector()};
            rawBoxes[index] = {semanticBoxes[index].Minimum().Vector(),
                semanticBoxes[index].Maximum().Vector()};
        }

        const mv::math::PreparedRay3f semanticPrepared(semanticRays[0]);
        const RawPreparedRay rawPrepared = Prepare(rawRays[0]);
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run("geometry/ray-aabb/prepared-semantic" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    output[index] =
                        static_cast<std::uint8_t>(mv::math::Intersects(
                            semanticPrepared, semanticBoxes[index]));
                }
                Observe(output);
            });

        bench.batch(count).run("geometry/ray-aabb/prepared-raw-vec3" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    output[index] = static_cast<std::uint8_t>(
                        Intersects(rawPrepared, rawBoxes[index]));
                }
                Observe(output);
            });

        bench.batch(count).run("geometry/ray-aabb/per-query-semantic" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    output[index] =
                        static_cast<std::uint8_t>(mv::math::Intersects(
                            semanticRays[index], semanticBoxes[index]));
                }
                Observe(output);
            });

        bench.batch(count).run("geometry/ray-aabb/per-query-raw-vec3" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    output[index] = static_cast<std::uint8_t>(
                        Intersects(rawRays[index], rawBoxes[index]));
                }
                Observe(output);
            });
    }

    void BenchmarkRayTriangle(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<mv::math::Ray3f> semanticRays(count);
        std::vector<mv::math::Triangle3f> semanticTriangles(count);
        std::vector<RawRay> rawRays(count);
        std::vector<RawTriangle> rawTriangles(count);
        std::vector<std::uint8_t> predicateOutput(count);
        std::vector<float> distanceOutput(count);
        for (std::size_t index = 0U; index < count; ++index)
        {
            const float x = Seed(index, 15U) * 10.0F - 5.0F;
            const float y = Seed(index, 16U) * 10.0F - 5.0F;
            const auto ray = mv::math::Ray3f::TryFromOriginDirection(
                mv::math::Point3f(x, y, 10.0F),
                -mv::math::Direction3f::AxisZ());
            if (!ray)
            {
                std::abort();
            }
            semanticRays[index] = *ray;
            semanticTriangles[index] = MakeSemanticTriangle(index);
            rawRays[index] = {semanticRays[index].Origin().Vector(),
                semanticRays[index].Direction().Vector()};
            rawTriangles[index] = {semanticTriangles[index].First().Vector(),
                semanticTriangles[index].Second().Vector(),
                semanticTriangles[index].Third().Vector()};
        }

        const std::string suffix = "/" + std::to_string(count);
        bench.batch(count).run(
            "geometry/ray-triangle/predicate-semantic" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    predicateOutput[index] =
                        static_cast<std::uint8_t>(mv::math::Intersects(
                            semanticRays[index], semanticTriangles[index]));
                }
                Observe(predicateOutput);
            });

        bench.batch(count).run(
            "geometry/ray-triangle/predicate-raw-vec3" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    predicateOutput[index] = static_cast<std::uint8_t>(
                        Intersects(rawRays[index], rawTriangles[index]));
                }
                Observe(predicateOutput);
            });

        bench.batch(count).run(
            "geometry/ray-triangle/detailed-semantic" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    const auto hit = mv::math::Intersect(
                        semanticRays[index], semanticTriangles[index]);
                    distanceOutput[index] = hit ? hit->Distance : -1.0F;
                }
                Observe(distanceOutput);
            });

        bench.batch(count).run(
            "geometry/ray-triangle/detailed-raw-vec3" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    const auto hit =
                        Intersect(rawRays[index], rawTriangles[index]);
                    distanceOutput[index] = hit ? hit->Distance : -1.0F;
                }
                Observe(distanceOutput);
            });
    }

    void BenchmarkPointSegment(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<mv::math::Point3f> semanticPoints(count);
        std::vector<mv::math::Segment3f> semanticSegments(count);
        std::vector<mv::math::Point3f> semanticOutput(count);
        std::vector<mv::math::Vec3f> rawPoints(count);
        std::vector<RawSegment> rawSegments(count);
        std::vector<mv::math::Vec3f> rawOutput(count);
        std::vector<glm::vec3> glmPoints(count);
        std::vector<glm::vec3> glmStarts(count);
        std::vector<glm::vec3> glmEnds(count);
        std::vector<glm::vec3> glmOutput(count);
        std::vector<DirectX::XMFLOAT3> dxPoints(count);
        std::vector<DirectX::XMFLOAT3> dxStarts(count);
        std::vector<DirectX::XMFLOAT3> dxEnds(count);
        std::vector<DirectX::XMFLOAT3> dxOutput(count);

        for (std::size_t index = 0U; index < count; ++index)
        {
            semanticPoints[index] = MakeSemanticPoint(index);
            semanticSegments[index] = MakeSemanticSegment(index);
            rawPoints[index] = semanticPoints[index].Vector();
            rawSegments[index] = {semanticSegments[index].Start().Vector(),
                semanticSegments[index].End().Vector()};
            glmPoints[index] = glm::vec3(rawPoints[index].X(),
                rawPoints[index].Y(), rawPoints[index].Z());
            glmStarts[index] = glm::vec3(rawSegments[index].Start.X(),
                rawSegments[index].Start.Y(), rawSegments[index].Start.Z());
            glmEnds[index] = glm::vec3(rawSegments[index].End.X(),
                rawSegments[index].End.Y(), rawSegments[index].End.Z());
            dxPoints[index] = DirectX::XMFLOAT3(rawPoints[index].X(),
                rawPoints[index].Y(), rawPoints[index].Z());
            dxStarts[index] = DirectX::XMFLOAT3(rawSegments[index].Start.X(),
                rawSegments[index].Start.Y(), rawSegments[index].Start.Z());
            dxEnds[index] = DirectX::XMFLOAT3(rawSegments[index].End.X(),
                rawSegments[index].End.Y(), rawSegments[index].End.Z());
        }

        const std::string suffix = "/" + std::to_string(count);
        bench.batch(count).run("geometry/point-segment/move" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    semanticOutput[index] = mv::math::ClosestPoint(
                        semanticPoints[index], semanticSegments[index]);
                }
                Observe(semanticOutput);
            });

        bench.batch(count).run("geometry/point-segment/move-raw-vec3" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    rawOutput[index] =
                        ClosestPoint(rawPoints[index], rawSegments[index]);
                }
                Observe(rawOutput);
            });

        bench.batch(count).run("geometry/point-segment/glm" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    glmOutput[index] = glm::closestPointOnLine(
                        glmPoints[index], glmStarts[index], glmEnds[index]);
                }
                Observe(glmOutput);
            });

        bench.batch(count).run("geometry/point-segment/directxmath" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    const DirectX::XMVECTOR point =
                        DirectX::XMLoadFloat3(&dxPoints[index]);
                    const DirectX::XMVECTOR start =
                        DirectX::XMLoadFloat3(&dxStarts[index]);
                    const DirectX::XMVECTOR end =
                        DirectX::XMLoadFloat3(&dxEnds[index]);
                    DirectX::XMStoreFloat3(&dxOutput[index],
                        DirectX::Internal::PointOnLineSegmentNearestPoint(
                            start, end, point));
                }
                Observe(dxOutput);
            });
    }

    void BenchmarkCapsulePairs(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<mv::math::Capsule3f> first(count);
        std::vector<mv::math::Capsule3f> second(count);
        std::vector<std::uint8_t> output(count);
        for (std::size_t index = 0U; index < count; ++index)
        {
            first[index] = MakeSemanticCapsule(index);
            second[index] = MakeSemanticCapsule(index + count / 3U + 17U);
        }

        const std::string suffix = "/" + std::to_string(count);
        bench.batch(count).run("geometry/capsule-capsule/move" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    output[index] = static_cast<std::uint8_t>(
                        mv::math::Intersects(first[index], second[index]));
                }
                Observe(output);
            });

        // Ericson's sphere-swept-volume reduction (RTCD 2005, section 4.5.1)
        // written at the public primitive level to expose facade-only cost.
        bench.batch(count).run("geometry/capsule-capsule/decomposed" + suffix,
            [&]
            {
                for (std::size_t index = 0U; index < count; ++index)
                {
                    const float radius =
                        first[index].Radius() + second[index].Radius();
                    output[index] = static_cast<std::uint8_t>(
                        mv::math::DistanceSquared(first[index].CenterLine(),
                            second[index].CenterLine()) <= radius * radius);
                }
                Observe(output);
            });
    }

    void VerifyParity()
    {
        constexpr std::size_t Count = 4096U;
        for (std::size_t index = 0U; index < Count; ++index)
        {
            const mv::math::Ray3f ray = MakeSemanticRay(index);
            const mv::math::Aabb3f box = MakeSemanticAabb(index);
            const RawRay rawRay{
                ray.Origin().Vector(), ray.Direction().Vector()};
            const RawAabb rawBox{
                box.Minimum().Vector(), box.Maximum().Vector()};
            if (mv::math::Intersects(ray, box) != Intersects(rawRay, rawBox))
            {
                std::abort();
            }

            const mv::math::Triangle3f triangle = MakeSemanticTriangle(index);
            const RawTriangle rawTriangle{triangle.First().Vector(),
                triangle.Second().Vector(), triangle.Third().Vector()};
            const auto semanticHit = mv::math::Intersect(ray, triangle);
            const auto rawHit = Intersect(rawRay, rawTriangle);
            if (semanticHit.has_value() != rawHit.has_value())
            {
                std::abort();
            }
            if (semanticHit &&
                (std::abs(semanticHit->Distance - rawHit->Distance) > 1.0e-5F ||
                    std::abs(semanticHit->Barycentric.X() -
                             rawHit->Barycentric.X()) > 1.0e-5F ||
                    std::abs(semanticHit->Barycentric.Y() -
                             rawHit->Barycentric.Y()) > 1.0e-5F ||
                    std::abs(semanticHit->Barycentric.Z() -
                             rawHit->Barycentric.Z()) > 1.0e-5F))
            {
                std::abort();
            }

            const mv::math::Point3f point = MakeSemanticPoint(index);
            const mv::math::Segment3f segment = MakeSemanticSegment(index);
            const mv::math::Point3f semanticClosest =
                mv::math::ClosestPoint(point, segment);
            const RawSegment rawSegment{
                segment.Start().Vector(), segment.End().Vector()};
            const mv::math::Vec3f rawClosest =
                ClosestPoint(point.Vector(), rawSegment);
            const glm::vec3 glmClosest = glm::closestPointOnLine(
                glm::vec3(point.X(), point.Y(), point.Z()),
                glm::vec3(segment.Start().X(), segment.Start().Y(),
                    segment.Start().Z()),
                glm::vec3(
                    segment.End().X(), segment.End().Y(), segment.End().Z()));
            const auto differs = [](float left, float right)
            {
                return std::abs(left - right) > 1.0e-5F;
            };
            if (differs(semanticClosest.X(), rawClosest.X()) ||
                differs(semanticClosest.Y(), rawClosest.Y()) ||
                differs(semanticClosest.Z(), rawClosest.Z()) ||
                differs(semanticClosest.X(), glmClosest.x) ||
                differs(semanticClosest.Y(), glmClosest.y) ||
                differs(semanticClosest.Z(), glmClosest.z))
            {
                std::abort();
            }

            const DirectX::XMFLOAT3 dxPoint(point.X(), point.Y(), point.Z());
            const DirectX::XMFLOAT3 dxStart(
                segment.Start().X(), segment.Start().Y(), segment.Start().Z());
            const DirectX::XMFLOAT3 dxEnd(
                segment.End().X(), segment.End().Y(), segment.End().Z());
            DirectX::XMFLOAT3 dxClosest;
            DirectX::XMStoreFloat3(
                &dxClosest, DirectX::Internal::PointOnLineSegmentNearestPoint(
                                DirectX::XMLoadFloat3(&dxStart),
                                DirectX::XMLoadFloat3(&dxEnd),
                                DirectX::XMLoadFloat3(&dxPoint)));
            if (differs(semanticClosest.X(), dxClosest.x) ||
                differs(semanticClosest.Y(), dxClosest.y) ||
                differs(semanticClosest.Z(), dxClosest.z))
            {
                std::abort();
            }

            const mv::math::Capsule3f firstCapsule = MakeSemanticCapsule(index);
            const mv::math::Capsule3f secondCapsule =
                MakeSemanticCapsule(index + Count / 3U + 17U);
            const float combinedRadius =
                firstCapsule.Radius() + secondCapsule.Radius();
            const bool decomposed =
                mv::math::DistanceSquared(
                    firstCapsule.CenterLine(), secondCapsule.CenterLine()) <=
                combinedRadius * combinedRadius;
            if (mv::math::Intersects(firstCapsule, secondCapsule) != decomposed)
            {
                std::abort();
            }
        }
    }
}  // namespace

int main(int argumentCount, char** arguments)
{
    VerifyParity();
    if (argumentCount == 2 && std::string_view(arguments[1]) == "--verify-only")
    {
        return EXIT_SUCCESS;
    }

    ankerl::nanobench::Bench bench;
    bench.title("Move math geometry and spatial query benchmarks")
        .epochs(15)
        .warmup(2)
        .minEpochIterations(4)
        .minEpochTime(std::chrono::milliseconds(50))
        .relative(false)
        .performanceCounters(true);

    for (const std::size_t count : WorkingSetSizes)
    {
        BenchmarkRayAabb(bench, count);
        BenchmarkRayTriangle(bench, count);
        BenchmarkPointSegment(bench, count);
        BenchmarkCapsulePairs(bench, count);
    }

    return EXIT_SUCCESS;
}
