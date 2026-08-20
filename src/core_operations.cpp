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

#include <move/math/quat.hpp>
#include <move/math/vec2.hpp>
#include <move/math/vec3.hpp>
#include <move/math/vec4.hpp>
#include <mv/math/Core.hpp>

namespace
{
    constexpr std::array<std::size_t, 3> WorkingSetSizes = {
        256U, 4U * 1024U, 64U * 1024U};

    struct Inputs
    {
        std::vector<mv::math::Vec3f> CurrentLeft;
        std::vector<mv::math::Vec3f> CurrentRight;
        std::vector<move::math::float3> LegacyLeft;
        std::vector<move::math::float3> LegacyRight;
        std::vector<mv::math::Vec2f> CurrentLeft2;
        std::vector<mv::math::Vec2f> CurrentRight2;
        std::vector<move::math::float2> LegacyLeft2;
        std::vector<move::math::float2> LegacyRight2;
        std::vector<mv::math::Vec4f> CurrentLeft4;
        std::vector<mv::math::Vec4f> CurrentRight4;
        std::vector<move::math::float4> LegacyLeft4;
        std::vector<move::math::float4> LegacyRight4;
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

    [[nodiscard]] Inputs MakeInputs(std::size_t count)
    {
        Inputs inputs{
            std::vector<mv::math::Vec3f>(count),
            std::vector<mv::math::Vec3f>(count),
            std::vector<move::math::float3>(count),
            std::vector<move::math::float3>(count),
            std::vector<mv::math::Vec2f>(count),
            std::vector<mv::math::Vec2f>(count),
            std::vector<move::math::float2>(count),
            std::vector<move::math::float2>(count),
            std::vector<mv::math::Vec4f>(count),
            std::vector<mv::math::Vec4f>(count),
            std::vector<move::math::float4>(count),
            std::vector<move::math::float4>(count),
        };

        for (std::size_t index = 0; index < count; ++index)
        {
            const float leftX = Seed(index, 0U) * 20.0F - 10.0F;
            const float leftY = Seed(index, 1U) * 20.0F - 10.0F;
            const float leftZ = Seed(index, 2U) * 20.0F - 10.0F;
            const float rightX = Seed(index + 17U, 0U) * 10.0F - 5.0F;
            const float rightY = Seed(index + 17U, 1U) * 10.0F - 5.0F;
            const float rightZ = Seed(index + 17U, 2U) * 10.0F - 5.0F;

            inputs.CurrentLeft[index] = mv::math::Vec3f(leftX, leftY, leftZ);
            inputs.CurrentRight[index] =
                mv::math::Vec3f(rightX, rightY, rightZ);
            inputs.LegacyLeft[index] = move::math::float3(leftX, leftY, leftZ);
            inputs.LegacyRight[index] =
                move::math::float3(rightX, rightY, rightZ);
            inputs.CurrentLeft2[index] = mv::math::Vec2f(leftX, leftY);
            inputs.CurrentRight2[index] = mv::math::Vec2f(rightX, rightY);
            inputs.LegacyLeft2[index] = move::math::float2(leftX, leftY);
            inputs.LegacyRight2[index] = move::math::float2(rightX, rightY);
            const float leftW = Seed(index, 3U) * 20.0F - 10.0F;
            const float rightW = Seed(index + 17U, 3U) * 10.0F - 5.0F;
            inputs.CurrentLeft4[index] =
                mv::math::Vec4f(leftX, leftY, leftZ, leftW);
            inputs.CurrentRight4[index] =
                mv::math::Vec4f(rightX, rightY, rightZ, rightW);
            inputs.LegacyLeft4[index] =
                move::math::float4(leftX, leftY, leftZ, leftW);
            inputs.LegacyRight4[index] =
                move::math::float4(rightX, rightY, rightZ, rightW);
        }
        return inputs;
    }

    [[nodiscard]] bool NearlyEqual(
        float left, float right, float tolerance = 2.0e-5F)
    {
        const float scale = std::max({1.0F, std::abs(left), std::abs(right)});
        return std::abs(left - right) <= tolerance * scale;
    }

    void RequireEquivalent(const mv::math::Vec3f& current,
        const move::math::float3& legacy, float tolerance = 2.0e-5F)
    {
        if (!NearlyEqual(current.X(), legacy.get_x(), tolerance) ||
            !NearlyEqual(current.Y(), legacy.get_y(), tolerance) ||
            !NearlyEqual(current.Z(), legacy.get_z(), tolerance))
        {
            std::abort();
        }
    }

    void RequireEquivalent(const mv::math::Vec2f& current,
        const move::math::float2& legacy, float tolerance = 2.0e-5F)
    {
        if (!NearlyEqual(current.X(), legacy.get_x(), tolerance) ||
            !NearlyEqual(current.Y(), legacy.get_y(), tolerance))
        {
            std::abort();
        }
    }

    void RequireEquivalent(const mv::math::Vec4f& current,
        const move::math::float4& legacy, float tolerance = 2.0e-5F)
    {
        if (!NearlyEqual(current.X(), legacy.get_x(), tolerance) ||
            !NearlyEqual(current.Y(), legacy.get_y(), tolerance) ||
            !NearlyEqual(current.Z(), legacy.get_z(), tolerance) ||
            !NearlyEqual(current.W(), legacy.get_w(), tolerance))
        {
            std::abort();
        }
    }

    [[nodiscard]] mv::math::Rotation3f MakeCurrentRotation()
    {
        const auto axis = mv::math::Direction3f::TryFrom(
            mv::math::Vec3f(0.25F, 1.0F, -0.125F));
        if (!axis)
        {
            std::abort();
        }
        return mv::math::Rotation3f::FromAxisAngle(
            *axis, mv::math::Radiansf(0.37F));
    }

    [[nodiscard]] move::math::quatf MakeLegacyRotation()
    {
        const move::math::float3 axis =
            move::math::float3(0.25F, 1.0F, -0.125F).normalized();
        return move::math::quatf::angle_axis(axis, 0.37F);
    }

    void VerifyParity()
    {
        constexpr std::size_t Count = 1024U;
        const Inputs inputs = MakeInputs(Count);
        const mv::math::Rotation3f currentRotation = MakeCurrentRotation();
        const move::math::quatf legacyRotation = MakeLegacyRotation();

        for (std::size_t index = 0; index < Count; ++index)
        {
            const mv::math::Vec3f currentChain =
                (inputs.CurrentLeft[index] + inputs.CurrentRight[index]) *
                0.37F;
            const move::math::float3 legacyChain =
                (inputs.LegacyLeft[index] + inputs.LegacyRight[index]) * 0.37F;
            RequireEquivalent(currentChain, legacyChain);

            const mv::math::Vec2f currentChain2 =
                (inputs.CurrentLeft2[index] * inputs.CurrentRight2[index] +
                    mv::math::Vec2f(2.5F, 2.5F)) /
                (inputs.CurrentRight2[index] * inputs.CurrentRight2[index] +
                    mv::math::Vec2f(1.0F, 1.0F));
            const move::math::float2 legacyChain2 =
                (inputs.LegacyLeft2[index] * inputs.LegacyRight2[index] +
                    move::math::float2(2.5F, 2.5F)) /
                (inputs.LegacyRight2[index] * inputs.LegacyRight2[index] +
                    move::math::float2(1.0F, 1.0F));
            RequireEquivalent(currentChain2, legacyChain2);

            const mv::math::Vec3f currentComponentChain =
                (inputs.CurrentLeft[index] * inputs.CurrentRight[index] +
                    mv::math::Vec3f(2.5F, 2.5F, 2.5F)) /
                (inputs.CurrentRight[index] * inputs.CurrentRight[index] +
                    mv::math::Vec3f(1.0F, 1.0F, 1.0F));
            const move::math::float3 legacyComponentChain =
                (inputs.LegacyLeft[index] * inputs.LegacyRight[index] +
                    move::math::float3(2.5F, 2.5F, 2.5F)) /
                (inputs.LegacyRight[index] * inputs.LegacyRight[index] +
                    move::math::float3(1.0F, 1.0F, 1.0F));
            RequireEquivalent(currentComponentChain, legacyComponentChain);

            const mv::math::Vec4f currentChain4 =
                (inputs.CurrentLeft4[index] * inputs.CurrentRight4[index] +
                    mv::math::Vec4f(2.5F, 2.5F, 2.5F, 2.5F)) /
                (inputs.CurrentRight4[index] * inputs.CurrentRight4[index] +
                    mv::math::Vec4f(1.0F, 1.0F, 1.0F, 1.0F));
            const move::math::float4 legacyChain4 =
                (inputs.LegacyLeft4[index] * inputs.LegacyRight4[index] +
                    move::math::float4(2.5F, 2.5F, 2.5F, 2.5F)) /
                (inputs.LegacyRight4[index] * inputs.LegacyRight4[index] +
                    move::math::float4(1.0F, 1.0F, 1.0F, 1.0F));
            RequireEquivalent(currentChain4, legacyChain4);

            if (!NearlyEqual(mv::math::Dot(inputs.CurrentLeft[index],
                                 inputs.CurrentRight[index]),
                    move::math::float3::dot(
                        inputs.LegacyLeft[index], inputs.LegacyRight[index])))
            {
                std::abort();
            }

            RequireEquivalent(mv::math::Cross(inputs.CurrentLeft[index],
                                  inputs.CurrentRight[index]),
                move::math::float3::cross(
                    inputs.LegacyLeft[index], inputs.LegacyRight[index]));

            const auto currentDirection =
                mv::math::Direction3f::TryFrom(inputs.CurrentLeft[index]);
            if (!currentDirection)
            {
                std::abort();
            }
            RequireEquivalent(currentDirection->Vector(),
                inputs.LegacyLeft[index].normalized(), 5.0e-5F);

            RequireEquivalent(
                mv::math::Rotate(currentRotation, inputs.CurrentLeft[index]),
                inputs.LegacyLeft[index] * legacyRotation, 5.0e-5F);
        }
    }

    void BenchmarkArithmetic(ankerl::nanobench::Bench& bench, std::size_t count)
    {
        const Inputs inputs = MakeInputs(count);
        std::vector<mv::math::Vec3f> currentOutput(count);
        std::vector<move::math::float3> legacyOutput(count);
        std::vector<float> currentScalars(count);
        std::vector<float> legacyScalars(count);
        std::vector<mv::math::Vec2f> currentOutput2(count);
        std::vector<move::math::float2> legacyOutput2(count);
        std::vector<mv::math::Vec4f> currentOutput4(count);
        std::vector<move::math::float4> legacyOutput4(count);
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run("core/vec3-chain/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentOutput[index] = (inputs.CurrentLeft[index] +
                                               inputs.CurrentRight[index]) *
                                           0.37F;
                }
                Observe(currentOutput);
            });

        bench.batch(count).run("core/vec3-chain/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyOutput[index] =
                        (inputs.LegacyLeft[index] + inputs.LegacyRight[index]) *
                        0.37F;
                }
                Observe(legacyOutput);
            });

        bench.batch(count).run("core/vec2-component-chain/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentOutput2[index] =
                        (inputs.CurrentLeft2[index] *
                                inputs.CurrentRight2[index] +
                            mv::math::Vec2f(2.5F, 2.5F)) /
                        (inputs.CurrentRight2[index] *
                                inputs.CurrentRight2[index] +
                            mv::math::Vec2f(1.0F, 1.0F));
                }
                Observe(currentOutput2);
            });
        bench.batch(count).run("core/vec2-component-chain/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyOutput2[index] = (inputs.LegacyLeft2[index] *
                                                   inputs.LegacyRight2[index] +
                                               move::math::float2(2.5F, 2.5F)) /
                                           (inputs.LegacyRight2[index] *
                                                   inputs.LegacyRight2[index] +
                                               move::math::float2(1.0F, 1.0F));
                }
                Observe(legacyOutput2);
            });

        bench.batch(count).run("core/vec3-component-chain/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentOutput[index] =
                        (inputs.CurrentLeft[index] *
                                inputs.CurrentRight[index] +
                            mv::math::Vec3f(2.5F, 2.5F, 2.5F)) /
                        (inputs.CurrentRight[index] *
                                inputs.CurrentRight[index] +
                            mv::math::Vec3f(1.0F, 1.0F, 1.0F));
                }
                Observe(currentOutput);
            });
        bench.batch(count).run("core/vec3-component-chain/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyOutput[index] =
                        (inputs.LegacyLeft[index] * inputs.LegacyRight[index] +
                            move::math::float3(2.5F, 2.5F, 2.5F)) /
                        (inputs.LegacyRight[index] * inputs.LegacyRight[index] +
                            move::math::float3(1.0F, 1.0F, 1.0F));
                }
                Observe(legacyOutput);
            });

        bench.batch(count).run("core/vec4-component-chain/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentOutput4[index] =
                        (inputs.CurrentLeft4[index] *
                                inputs.CurrentRight4[index] +
                            mv::math::Vec4f(2.5F, 2.5F, 2.5F, 2.5F)) /
                        (inputs.CurrentRight4[index] *
                                inputs.CurrentRight4[index] +
                            mv::math::Vec4f(1.0F, 1.0F, 1.0F, 1.0F));
                }
                Observe(currentOutput4);
            });
        bench.batch(count).run("core/vec4-component-chain/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyOutput4[index] =
                        (inputs.LegacyLeft4[index] *
                                inputs.LegacyRight4[index] +
                            move::math::float4(2.5F, 2.5F, 2.5F, 2.5F)) /
                        (inputs.LegacyRight4[index] *
                                inputs.LegacyRight4[index] +
                            move::math::float4(1.0F, 1.0F, 1.0F, 1.0F));
                }
                Observe(legacyOutput4);
            });

        bench.batch(count).run("core/dot/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentScalars[index] = mv::math::Dot(
                        inputs.CurrentLeft[index], inputs.CurrentRight[index]);
                }
                Observe(currentScalars);
            });

        bench.batch(count).run("core/dot/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyScalars[index] = move::math::float3::dot(
                        inputs.LegacyLeft[index], inputs.LegacyRight[index]);
                }
                Observe(legacyScalars);
            });

        bench.batch(count).run("core/cross/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentOutput[index] = mv::math::Cross(
                        inputs.CurrentLeft[index], inputs.CurrentRight[index]);
                }
                Observe(currentOutput);
            });

        bench.batch(count).run("core/cross/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyOutput[index] = move::math::float3::cross(
                        inputs.LegacyLeft[index], inputs.LegacyRight[index]);
                }
                Observe(legacyOutput);
            });
    }

    void BenchmarkNormalizationAndRotation(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        const Inputs inputs = MakeInputs(count);
        std::vector<mv::math::Direction3f> currentDirections(count);
        std::vector<move::math::float3> legacyDirections(count);
        std::vector<mv::math::Vec3f> currentOutput(count);
        std::vector<move::math::float3> legacyOutput(count);
        const mv::math::Rotation3f currentRotation = MakeCurrentRotation();
        const move::math::quatf legacyRotation = MakeLegacyRotation();
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run("core/normalize-checked/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const auto direction = mv::math::Direction3f::TryFrom(
                        inputs.CurrentLeft[index]);
                    if (!direction)
                    {
                        std::abort();
                    }
                    currentDirections[index] = *direction;
                }
                Observe(currentDirections);
            });

        bench.batch(count).run(
            "core/normalize-checked/legacy-unchecked" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyDirections[index] =
                        inputs.LegacyLeft[index].normalized();
                }
                Observe(legacyDirections);
            });

        bench.batch(count).run("core/rotate-vector/mv" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    currentOutput[index] = mv::math::Rotate(
                        currentRotation, inputs.CurrentLeft[index]);
                }
                Observe(currentOutput);
            });

        bench.batch(count).run("core/rotate-vector/legacy" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    legacyOutput[index] =
                        inputs.LegacyLeft[index] * legacyRotation;
                }
                Observe(legacyOutput);
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
    bench.title("Move math core old/new migration benchmarks")
        .epochs(15)
        .warmup(2)
        .minEpochIterations(4)
        .minEpochTime(std::chrono::milliseconds(50))
        .relative(false)
        .performanceCounters(true);

    for (const std::size_t count : WorkingSetSizes)
    {
        BenchmarkArithmetic(bench, count);
        BenchmarkNormalizationAndRotation(bench, count);
    }

    return EXIT_SUCCESS;
}
