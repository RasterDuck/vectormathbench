#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <string_view>
#include <vector>

#include <move/math/mat3x3.hpp>
#include <move/math/mat4x4.hpp>
#include <mv/math/Mat3.hpp>
#include <mv/math/Mat4.hpp>
#include <mv/math/transforms/Projection.hpp>
#include <mv/math/transforms/View.hpp>

namespace
{
    constexpr std::array<std::size_t, 3> WorkingSetSizes = {
        256U, 4U * 1024U, 64U * 1024U};

    struct Inputs
    {
        std::vector<mv::math::Vec3f> CurrentVectors;
        std::vector<move::math::float3> LegacyVectors;
        std::vector<mv::math::Mat3f> CurrentLeft;
        std::vector<mv::math::Mat3f> CurrentRight;
        std::vector<move::math::mat3x3f> LegacyLeft;
        std::vector<move::math::mat3x3f> LegacyRight;
        std::vector<mv::math::Rotation3f> CurrentRotations;
        std::vector<move::math::quatf> LegacyRotations;
        std::vector<mv::math::Vec4f> CurrentVectors4;
        std::vector<move::math::float4> LegacyVectors4;
        std::vector<mv::math::Mat4f> CurrentLeft4;
        std::vector<mv::math::Mat4f> CurrentRight4;
        std::vector<move::math::mat4x4f> LegacyLeft4;
        std::vector<move::math::mat4x4f> LegacyRight4;
        std::vector<float> FieldOfViews;
        std::vector<float> AspectRatios;
        std::vector<float> NearPlanes;
        std::vector<float> FarPlanes;
        std::vector<mv::math::Point3f> CurrentEyes;
        std::vector<mv::math::Point3f> CurrentTargets;
        std::vector<move::math::float3> LegacyEyes;
        std::vector<move::math::float3> LegacyTargets;
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

    [[nodiscard]] mv::math::Mat3f MakeCurrentMatrix(
        std::size_t index, std::size_t offset)
    {
        const float a = Seed(index + offset, 0U) * 0.2F - 0.1F;
        const float b = Seed(index + offset, 1U) * 0.2F - 0.1F;
        const float c = Seed(index + offset, 2U) * 0.2F - 0.1F;
        return mv::math::Mat3f(
            1.5F + a, b, c, -c, 1.75F + b, a, b, -a, 2.0F + c);
    }

    [[nodiscard]] move::math::mat3x3f MakeLegacyMatrix(
        std::size_t index, std::size_t offset)
    {
        const float a = Seed(index + offset, 0U) * 0.2F - 0.1F;
        const float b = Seed(index + offset, 1U) * 0.2F - 0.1F;
        const float c = Seed(index + offset, 2U) * 0.2F - 0.1F;
        return move::math::mat3x3f(
            1.5F + a, b, c, -c, 1.75F + b, a, b, -a, 2.0F + c);
    }

    [[nodiscard]] Inputs MakeInputs(std::size_t count)
    {
        Inputs inputs{
            std::vector<mv::math::Vec3f>(count),
            std::vector<move::math::float3>(count),
            std::vector<mv::math::Mat3f>(count),
            std::vector<mv::math::Mat3f>(count),
            std::vector<move::math::mat3x3f>(count),
            std::vector<move::math::mat3x3f>(count),
            std::vector<mv::math::Rotation3f>(count),
            std::vector<move::math::quatf>(count),
            std::vector<mv::math::Vec4f>(count),
            std::vector<move::math::float4>(count),
            std::vector<mv::math::Mat4f>(count),
            std::vector<mv::math::Mat4f>(count),
            std::vector<move::math::mat4x4f>(count),
            std::vector<move::math::mat4x4f>(count),
            std::vector<float>(count),
            std::vector<float>(count),
            std::vector<float>(count),
            std::vector<float>(count),
            std::vector<mv::math::Point3f>(count),
            std::vector<mv::math::Point3f>(count),
            std::vector<move::math::float3>(count),
            std::vector<move::math::float3>(count),
        };

        for (std::size_t index = 0; index < count; ++index)
        {
            const float x = Seed(index, 0U) * 20.0F - 10.0F;
            const float y = Seed(index, 1U) * 20.0F - 10.0F;
            const float z = Seed(index, 2U) * 20.0F - 10.0F;
            inputs.CurrentVectors[index] = mv::math::Vec3f(x, y, z);
            inputs.LegacyVectors[index] = move::math::float3(x, y, z);
            inputs.CurrentLeft[index] = MakeCurrentMatrix(index, 0U);
            inputs.CurrentRight[index] = MakeCurrentMatrix(index, 31U);
            inputs.LegacyLeft[index] = MakeLegacyMatrix(index, 0U);
            inputs.LegacyRight[index] = MakeLegacyMatrix(index, 31U);
            const float angle = Seed(index, 3U) * 6.0F - 3.0F;
            inputs.CurrentRotations[index] =
                mv::math::Rotation3f::FromAxisAngle(
                    mv::math::Direction3f::AxisY(), mv::math::Radiansf(angle));
            inputs.LegacyRotations[index] =
                move::math::quatf::rotation_y(angle);

            const float w = Seed(index, 3U) * 2.0F - 1.0F;
            inputs.CurrentVectors4[index] = mv::math::Vec4f(x, y, z, w);
            inputs.LegacyVectors4[index] = move::math::float4(x, y, z, w);

            const float a = Seed(index, 4U) * 0.2F - 0.1F;
            const float b = Seed(index, 5U) * 0.2F - 0.1F;
            const float c = Seed(index, 6U) * 0.2F - 0.1F;
            inputs.CurrentLeft4[index] =
                mv::math::Mat4f(1.5F + a, b, c, 0.0F, -c, 1.75F + b, a, 0.0F, b,
                    -a, 2.0F + c, 0.0F, x * 0.1F, y * 0.1F, z * 0.1F, 1.0F);
            inputs.CurrentRight4[index] = mv::math::Mat4f(2.0F + c, -a, b, 0.0F,
                a, 1.25F + a, -c, 0.0F, -b, c, 1.5F + b, 0.0F, z * 0.05F,
                x * 0.05F, y * 0.05F, 1.0F);
            inputs.LegacyLeft4[index] = move::math::mat4x4f(1.5F + a, b, c,
                0.0F, -c, 1.75F + b, a, 0.0F, b, -a, 2.0F + c, 0.0F, x * 0.1F,
                y * 0.1F, z * 0.1F, 1.0F);
            inputs.LegacyRight4[index] = move::math::mat4x4f(2.0F + c, -a, b,
                0.0F, a, 1.25F + a, -c, 0.0F, -b, c, 1.5F + b, 0.0F, z * 0.05F,
                x * 0.05F, y * 0.05F, 1.0F);

            inputs.FieldOfViews[index] = 0.7F + Seed(index, 7U) * 0.8F;
            inputs.AspectRatios[index] = 1.2F + Seed(index, 8U) * 0.8F;
            inputs.NearPlanes[index] = 0.01F + Seed(index, 9U) * 0.4F;
            inputs.FarPlanes[index] = 100.0F + Seed(index, 10U) * 1900.0F;
            inputs.CurrentEyes[index] = mv::math::Point3f(x, y, z);
            inputs.CurrentTargets[index] =
                mv::math::Point3f(x + 0.25F, y + 0.1F, z + 1.0F);
            inputs.LegacyEyes[index] = move::math::float3(x, y, z);
            inputs.LegacyTargets[index] =
                move::math::float3(x + 0.25F, y + 0.1F, z + 1.0F);
        }
        return inputs;
    }

    [[nodiscard]] bool NearlyEqual(
        float left, float right, float tolerance = 5.0e-5F)
    {
        const float scale = std::max({1.0F, std::abs(left), std::abs(right)});
        return std::abs(left - right) <= tolerance * scale;
    }

    void RequireEquivalent(const mv::math::Vec3f& current,
        const move::math::float3& legacy, float tolerance = 5.0e-5F)
    {
        if (!NearlyEqual(current.X(), legacy.get_x(), tolerance) ||
            !NearlyEqual(current.Y(), legacy.get_y(), tolerance) ||
            !NearlyEqual(current.Z(), legacy.get_z(), tolerance))
        {
            std::abort();
        }
    }

    void RequireEquivalent(const mv::math::Mat3f& current,
        const move::math::mat3x3f& legacy, float tolerance = 2.0e-4F)
    {
        float values[9]{};
        legacy.store_array(values);
        for (std::size_t row = 0; row < 3U; ++row)
        {
            for (std::size_t column = 0; column < 3U; ++column)
            {
                if (!NearlyEqual(current.Element(row, column),
                        values[row * 3U + column], tolerance))
                {
                    std::abort();
                }
            }
        }
    }

    void RequireEquivalent(const mv::math::Vec4f& current,
        const move::math::float4& legacy, float tolerance = 5.0e-5F)
    {
        if (!NearlyEqual(current.X(), legacy.get_x(), tolerance) ||
            !NearlyEqual(current.Y(), legacy.get_y(), tolerance) ||
            !NearlyEqual(current.Z(), legacy.get_z(), tolerance) ||
            !NearlyEqual(current.W(), legacy.get_w(), tolerance))
        {
            std::abort();
        }
    }

    void RequireEquivalent(const mv::math::Mat4f& current,
        const move::math::mat4x4f& legacy, float tolerance = 3.0e-4F)
    {
        float values[16]{};
        legacy.store_array(values);
        for (std::size_t row = 0; row < 4U; ++row)
        {
            for (std::size_t column = 0; column < 4U; ++column)
            {
                if (!NearlyEqual(current.Element(row, column),
                        values[row * 4U + column], tolerance))
                {
                    std::abort();
                }
            }
        }
    }

    void VerifyParity()
    {
        constexpr std::size_t Count = 1024U;
        const Inputs inputs = MakeInputs(Count);
        for (std::size_t index = 0; index < Count; ++index)
        {
            RequireEquivalent(
                inputs.CurrentVectors[index] * inputs.CurrentLeft[index],
                inputs.LegacyVectors[index] * inputs.LegacyLeft[index]);
            RequireEquivalent(
                inputs.CurrentLeft[index] * inputs.CurrentRight[index],
                inputs.LegacyLeft[index] * inputs.LegacyRight[index]);
            if (!NearlyEqual(inputs.CurrentLeft[index].Determinant(),
                    inputs.LegacyLeft[index].determinant()))
            {
                std::abort();
            }

            const auto currentInverse = inputs.CurrentLeft[index].TryInverse();
            if (!currentInverse)
            {
                std::abort();
            }
            RequireEquivalent(
                *currentInverse, inputs.LegacyLeft[index].inverse());
            RequireEquivalent(
                mv::math::Mat3f::FromRotation(inputs.CurrentRotations[index]),
                move::math::mat3x3f::rotation(inputs.LegacyRotations[index]));

            RequireEquivalent(
                inputs.CurrentVectors4[index] * inputs.CurrentLeft4[index],
                inputs.LegacyVectors4[index] * inputs.LegacyLeft4[index]);
            RequireEquivalent(
                inputs.CurrentLeft4[index] * inputs.CurrentRight4[index],
                inputs.LegacyLeft4[index] * inputs.LegacyRight4[index]);
            if (!NearlyEqual(inputs.CurrentLeft4[index].Determinant(),
                    inputs.LegacyLeft4[index].determinant()))
            {
                std::abort();
            }
            const auto currentInverse4 =
                inputs.CurrentLeft4[index].TryInverse();
            if (!currentInverse4)
            {
                std::abort();
            }
            RequireEquivalent(
                *currentInverse4, inputs.LegacyLeft4[index].inverse());

            const auto currentProjection = mv::math::TryPerspectiveFov(
                mv::math::Radiansf(inputs.FieldOfViews[index]),
                inputs.AspectRatios[index], inputs.NearPlanes[index],
                inputs.FarPlanes[index], mv::math::Handedness::Left,
                mv::math::ClipDepth::ZeroToOne,
                mv::math::DepthDirection::Forward);
            if (!currentProjection)
            {
                std::abort();
            }
            RequireEquivalent(*currentProjection,
                move::math::mat4x4f::perspective(inputs.FieldOfViews[index],
                    inputs.AspectRatios[index], inputs.NearPlanes[index],
                    inputs.FarPlanes[index]));

            const float halfWidth = inputs.AspectRatios[index] * 5.0F;
            const auto currentOrthographic =
                mv::math::TryOrthographicOffCenter(-halfWidth, halfWidth, -5.0F,
                    5.0F, inputs.NearPlanes[index], inputs.FarPlanes[index],
                    mv::math::Handedness::Left, mv::math::ClipDepth::ZeroToOne,
                    mv::math::DepthDirection::Forward);
            if (!currentOrthographic)
            {
                std::abort();
            }
            RequireEquivalent(*currentOrthographic,
                move::math::mat4x4f::orthographic(-halfWidth, halfWidth, -5.0F,
                    5.0F, inputs.NearPlanes[index], inputs.FarPlanes[index]));

            const auto currentView = mv::math::TryLookAtLH(
                inputs.CurrentEyes[index], inputs.CurrentTargets[index],
                mv::math::Direction3f::AxisY());
            if (!currentView)
            {
                std::abort();
            }
            RequireEquivalent(*currentView,
                move::math::mat4x4f::look_at(inputs.LegacyEyes[index],
                    inputs.LegacyTargets[index],
                    move::math::float3(0.0F, 1.0F, 0.0F)));
        }
    }

    void Benchmark(std::size_t count, ankerl::nanobench::Bench& bench)
    {
        const Inputs inputs = MakeInputs(count);
        std::vector<mv::math::Vec3f> currentVectors(count);
        std::vector<move::math::float3> legacyVectors(count);
        std::vector<mv::math::Mat3f> currentMatrices(count);
        std::vector<move::math::mat3x3f> legacyMatrices(count);
        std::vector<float> currentScalars(count);
        std::vector<float> legacyScalars(count);
        std::vector<mv::math::Vec4f> currentVectors4(count);
        std::vector<move::math::float4> legacyVectors4(count);
        std::vector<mv::math::Mat4f> currentMatrices4(count);
        std::vector<move::math::mat4x4f> legacyMatrices4(count);
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run("matrix/mat3-transform/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentVectors[index] = inputs.CurrentVectors[index] *
                                            inputs.CurrentLeft[index];
                }
                Observe(currentVectors);
            });
        bench.batch(count).run("matrix/mat3-transform/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyVectors[index] =
                        inputs.LegacyVectors[index] * inputs.LegacyLeft[index];
                }
                Observe(legacyVectors);
            });

        bench.batch(count).run("matrix/mat3-multiply/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentMatrices[index] =
                        inputs.CurrentLeft[index] * inputs.CurrentRight[index];
                }
                Observe(currentMatrices);
            });
        bench.batch(count).run("matrix/mat3-multiply/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyMatrices[index] =
                        inputs.LegacyLeft[index] * inputs.LegacyRight[index];
                }
                Observe(legacyMatrices);
            });

        bench.batch(count).run("matrix/mat3-determinant/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentScalars[index] =
                        inputs.CurrentLeft[index].Determinant();
                }
                Observe(currentScalars);
            });
        bench.batch(count).run("matrix/mat3-determinant/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyScalars[index] =
                        inputs.LegacyLeft[index].determinant();
                }
                Observe(legacyScalars);
            });

        bench.batch(count).run("matrix/mat3-inverse/mv-checked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const auto inverse = inputs.CurrentLeft[index].TryInverse();
                    if (!inverse)
                    {
                        std::abort();
                    }
                    currentMatrices[index] = *inverse;
                }
                Observe(currentMatrices);
            });
        bench.batch(count).run("matrix/mat3-inverse/legacy-unchecked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyMatrices[index] = inputs.LegacyLeft[index].inverse();
                }
                Observe(legacyMatrices);
            });

        bench.batch(count).run("matrix/mat3-from-rotation/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentMatrices[index] = mv::math::Mat3f::FromRotation(
                        inputs.CurrentRotations[index]);
                }
                Observe(currentMatrices);
            });
        bench.batch(count).run("matrix/mat3-from-rotation/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyMatrices[index] = move::math::mat3x3f::rotation(
                        inputs.LegacyRotations[index]);
                }
                Observe(legacyMatrices);
            });

        bench.batch(count).run("matrix/mat4-transform/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentVectors4[index] = inputs.CurrentVectors4[index] *
                                             inputs.CurrentLeft4[index];
                }
                Observe(currentVectors4);
            });
        bench.batch(count).run("matrix/mat4-transform/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyVectors4[index] = inputs.LegacyVectors4[index] *
                                            inputs.LegacyLeft4[index];
                }
                Observe(legacyVectors4);
            });

        bench.batch(count).run("matrix/mat4-multiply/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentMatrices4[index] = inputs.CurrentLeft4[index] *
                                              inputs.CurrentRight4[index];
                }
                Observe(currentMatrices4);
            });
        bench.batch(count).run("matrix/mat4-multiply/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyMatrices4[index] =
                        inputs.LegacyLeft4[index] * inputs.LegacyRight4[index];
                }
                Observe(legacyMatrices4);
            });

        bench.batch(count).run("matrix/mat4-determinant/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentScalars[index] =
                        inputs.CurrentLeft4[index].Determinant();
                }
                Observe(currentScalars);
            });
        bench.batch(count).run("matrix/mat4-determinant/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyScalars[index] =
                        inputs.LegacyLeft4[index].determinant();
                }
                Observe(legacyScalars);
            });

        bench.batch(count).run("matrix/mat4-inverse/mv-checked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const auto inverse =
                        inputs.CurrentLeft4[index].TryInverse();
                    if (!inverse)
                    {
                        std::abort();
                    }
                    currentMatrices4[index] = *inverse;
                }
                Observe(currentMatrices4);
            });
        bench.batch(count).run("matrix/mat4-inverse/legacy-unchecked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyMatrices4[index] =
                        inputs.LegacyLeft4[index].inverse();
                }
                Observe(legacyMatrices4);
            });

        bench.batch(count).run("matrix/perspective-lh-zo/mv-checked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const auto projection = mv::math::TryPerspectiveFov(
                        mv::math::Radiansf(inputs.FieldOfViews[index]),
                        inputs.AspectRatios[index], inputs.NearPlanes[index],
                        inputs.FarPlanes[index], mv::math::Handedness::Left,
                        mv::math::ClipDepth::ZeroToOne,
                        mv::math::DepthDirection::Forward);
                    if (!projection)
                    {
                        std::abort();
                    }
                    currentMatrices4[index] = *projection;
                }
                Observe(currentMatrices4);
            });
        bench.batch(count).run(
            "matrix/perspective-lh-zo/legacy-unchecked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyMatrices4[index] = move::math::mat4x4f::perspective(
                        inputs.FieldOfViews[index], inputs.AspectRatios[index],
                        inputs.NearPlanes[index], inputs.FarPlanes[index]);
                }
                Observe(legacyMatrices4);
            });

        bench.batch(count).run("matrix/orthographic-lh-zo/mv-checked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const float halfWidth = inputs.AspectRatios[index] * 5.0F;
                    const auto projection =
                        mv::math::TryOrthographicOffCenter(-halfWidth,
                            halfWidth, -5.0F, 5.0F, inputs.NearPlanes[index],
                            inputs.FarPlanes[index], mv::math::Handedness::Left,
                            mv::math::ClipDepth::ZeroToOne,
                            mv::math::DepthDirection::Forward);
                    if (!projection)
                    {
                        std::abort();
                    }
                    currentMatrices4[index] = *projection;
                }
                Observe(currentMatrices4);
            });
        bench.batch(count).run(
            "matrix/orthographic-lh-zo/legacy-unchecked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const float halfWidth = inputs.AspectRatios[index] * 5.0F;
                    legacyMatrices4[index] = move::math::mat4x4f::orthographic(
                        -halfWidth, halfWidth, -5.0F, 5.0F,
                        inputs.NearPlanes[index], inputs.FarPlanes[index]);
                }
                Observe(legacyMatrices4);
            });

        bench.batch(count).run("matrix/look-at-lh/mv-checked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const auto view = mv::math::TryLookAtLH(
                        inputs.CurrentEyes[index], inputs.CurrentTargets[index],
                        mv::math::Direction3f::AxisY());
                    if (!view)
                    {
                        std::abort();
                    }
                    currentMatrices4[index] = *view;
                }
                Observe(currentMatrices4);
            });
        bench.batch(count).run("matrix/look-at-lh/legacy-unchecked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyMatrices4[index] = move::math::mat4x4f::look_at(
                        inputs.LegacyEyes[index], inputs.LegacyTargets[index],
                        move::math::float3(0.0F, 1.0F, 0.0F));
                }
                Observe(legacyMatrices4);
            });
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
    bench.title("Move matrix old/new migration benchmarks")
        .epochs(15)
        .warmup(2)
        .minEpochIterations(4)
        .minEpochTime(std::chrono::milliseconds(50))
        .relative(false)
        .performanceCounters(true);

    for (const std::size_t count : WorkingSetSizes)
    {
        Benchmark(count, bench);
    }
    return EXIT_SUCCESS;
}
