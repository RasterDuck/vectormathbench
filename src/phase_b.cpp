#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <string>
#include <vector>

#include <mv/math/PhaseB.hpp>

namespace
{
    constexpr std::array<std::size_t, 4> WorkingSetSizes = {
        256U, 4U * 1024U, 64U * 1024U, 1024U * 1024U};

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

    [[nodiscard]] mv::math::RigidTransform3f MakeTransform()
    {
        using namespace mv::math;

        const auto axis = Direction3f::TryFrom(Vec3f(0.25F, 1.0F, -0.125F));
        if (!axis)
        {
            std::abort();
        }
        return RigidTransform3f(
            Rotation3f::FromAxisAngle(*axis, Radiansf(0.37F)),
            Vec3f(100.0F, -20.0F, 3.0F));
    }

    void BenchmarkPointFacade(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<mv::math::Point3f> semanticInput(count);
        std::vector<mv::math::Point3f> semanticOutput(count);
        std::vector<mv::math::Vec3f> rawInput(count);
        std::vector<mv::math::Vec3f> rawOutput(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            const mv::math::Vec3f value(Seed(index, 0U) * 1000.0F,
                Seed(index, 1U) * 1000.0F, Seed(index, 2U) * 1000.0F);
            semanticInput[index] = mv::math::Point3f::FromVector(value);
            rawInput[index] = value;
        }

        const mv::math::RigidTransform3f transform = MakeTransform();
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run("phase-b/rigid-point/semantic-point3" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    semanticOutput[index] = mv::math::TransformPoint(
                        transform, semanticInput[index]);
                }
                Observe(semanticOutput);
            });

        bench.batch(count).run("phase-b/rigid-point/raw-vec3" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    rawOutput[index] = mv::math::Rotate(transform.Rotation(),
                                           rawInput[index]) +
                                       transform.Translation();
                }
                Observe(rawOutput);
            });
    }

    void BenchmarkDirectionInvariant(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<mv::math::Vec3f> untrustedInput(count);
        std::vector<mv::math::Direction3f> directions(count);
        std::vector<mv::math::Direction3f> semanticOutput(count);
        std::vector<mv::math::Vec3f> rawOutput(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            const mv::math::Vec3f value(Seed(index, 0U) + 0.125F,
                Seed(index, 1U) + 0.25F, Seed(index, 2U) + 0.5F);
            const auto direction = mv::math::Direction3f::TryFrom(value);
            if (!direction)
            {
                std::abort();
            }
            untrustedInput[index] = value;
            directions[index] = *direction;
        }

        const mv::math::RigidTransform3f transform = MakeTransform();
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run(
            "phase-b/rigid-direction/prevalidated-semantic" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    semanticOutput[index] = mv::math::TransformDirection(
                        transform, directions[index]);
                }
                Observe(semanticOutput);
            });

        bench.batch(count).run(
            "phase-b/rigid-direction/prevalidated-raw-vec3" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    rawOutput[index] = mv::math::Rotate(
                        transform.Rotation(), directions[index].Vector());
                }
                Observe(rawOutput);
            });

        bench.batch(count).run(
            "phase-b/rigid-direction/validate-every-use" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const auto direction =
                        mv::math::Direction3f::TryFrom(untrustedInput[index]);
                    if (!direction)
                    {
                        std::abort();
                    }
                    semanticOutput[index] =
                        mv::math::TransformDirection(transform, *direction);
                }
                Observe(semanticOutput);
            });
    }
}  // namespace

int main()
{
    ankerl::nanobench::Bench bench;
    bench.title("Move math API-v2 Phase B semantic transform proof")
        .epochs(15)
        .warmup(2)
        .minEpochIterations(4)
        .minEpochTime(std::chrono::milliseconds(50))
        .relative(false)
        .performanceCounters(true);

    for (const std::size_t count : WorkingSetSizes)
    {
        BenchmarkPointFacade(bench, count);
        BenchmarkDirectionInvariant(bench, count);
    }

    return EXIT_SUCCESS;
}
