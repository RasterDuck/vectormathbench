#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <span>
#include <string>
#include <vector>

#include <mv/math/Math.hpp>

#include "support/RepresentationFixtures.hpp"

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

    std::vector<mv::math::PackedVec3f> MakePackedVec3(
        std::size_t count, std::size_t seedOffset)
    {
        std::vector<mv::math::PackedVec3f> result(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            result[index] = {Seed(index + seedOffset, 0U),
                Seed(index + seedOffset, 1U), Seed(index + seedOffset, 2U)};
        }
        return result;
    }

    void BenchmarkVec3Representations(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        using vmb::support::FixedStorageVec3f;
        using vmb::support::NativeVec3f;

        const auto left = MakePackedVec3(count, 11U);
        const auto right = MakePackedVec3(count, 37U);
        std::vector<mv::math::PackedVec3f> output(count);
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run(
            "representation/vec3/public-native-storage" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const mv::math::Vec3f value =
                        (mv::math::Load(left[index]) +
                            mv::math::Load(right[index])) *
                        1.0001F;
                    mv::math::Store(output[index], value);
                }
                Observe(output);
            });

        bench.batch(count).run(
            "representation/vec3/fixed-scalar-storage" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const FixedStorageVec3f leftValue(
                        left[index].X, left[index].Y, left[index].Z);
                    const FixedStorageVec3f rightValue(
                        right[index].X, right[index].Y, right[index].Z);
                    const FixedStorageVec3f value =
                        (leftValue + rightValue) * 1.0001F;
                    value.Store(&output[index].X);
                }
                Observe(output);
            });

        bench.batch(count).run("representation/vec3/raw-native-proof" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const NativeVec3f leftValue(
                        left[index].X, left[index].Y, left[index].Z);
                    const NativeVec3f rightValue(
                        right[index].X, right[index].Y, right[index].Z);
                    const NativeVec3f value =
                        (leftValue + rightValue) * 1.0001F;
                    value.Store(&output[index].X);
                }
                Observe(output);
            });
    }

    void BenchmarkVec2Storage(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        using vmb::support::AlignedVec2f16;

        std::vector<mv::math::Vec2f> compact(count);
        std::vector<AlignedVec2f16> aligned(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            compact[index] = mv::math::Vec2f(Seed(index, 0U), Seed(index, 1U));
            aligned[index] = AlignedVec2f16(Seed(index, 0U), Seed(index, 1U));
        }

        const mv::math::Vec2f compactIncrement(0.001F, -0.002F);
        const AlignedVec2f16 alignedIncrement(0.001F, -0.002F);
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run("representation/vec2/compact-8-byte" + suffix,
            [&]
            {
                for (auto& value : compact)
                {
                    value = (value + compactIncrement) * 1.00001F;
                }
                Observe(compact);
            });

        bench.batch(count).run("representation/vec2/aligned-16-byte" + suffix,
            [&]
            {
                for (auto& value : aligned)
                {
                    value = (value + alignedIncrement) * 1.00001F;
                }
                Observe(aligned);
            });
    }

    void BenchmarkSpriteIntegration(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        using vmb::support::AlignedVec2f16;

        std::vector<mv::math::Vec2f> compactPositions(count);
        std::vector<mv::math::Vec2f> compactVelocities(count);
        std::vector<AlignedVec2f16> alignedPositions(count);
        std::vector<AlignedVec2f16> alignedVelocities(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            const float positionX = Seed(index, 0U) * 1920.0F;
            const float positionY = Seed(index, 1U) * 1080.0F;
            const float velocityX = Seed(index, 2U) * 200.0F - 100.0F;
            const float velocityY = Seed(index, 3U) * 200.0F - 100.0F;
            compactPositions[index] = mv::math::Vec2f(positionX, positionY);
            compactVelocities[index] = mv::math::Vec2f(velocityX, velocityY);
            alignedPositions[index] = AlignedVec2f16(positionX, positionY);
            alignedVelocities[index] = AlignedVec2f16(velocityX, velocityY);
        }

        constexpr float DeltaTime = 1.0F / 60.0F;
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run(
            "representation/vec2-sprite-integration/compact-8-byte" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    compactPositions[index] +=
                        compactVelocities[index] * DeltaTime;
                }
                Observe(compactPositions);
            });

        bench.batch(count).run(
            "representation/vec2-sprite-integration/aligned-16-byte" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    alignedPositions[index] =
                        alignedPositions[index] +
                        alignedVelocities[index] * DeltaTime;
                }
                Observe(alignedPositions);
            });
    }

    void BenchmarkUiTransform(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        using vmb::support::AlignedVec2f16;

        std::vector<mv::math::Vec2f> compactInput(count);
        std::vector<mv::math::Vec2f> compactOutput(count);
        std::vector<AlignedVec2f16> alignedInput(count);
        std::vector<AlignedVec2f16> alignedOutput(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            const float x = Seed(index, 0U) * 640.0F;
            const float y = Seed(index, 1U) * 360.0F;
            compactInput[index] = mv::math::Vec2f(x, y);
            alignedInput[index] = AlignedVec2f16(x, y);
        }

        constexpr float M00 = 1.0392305F;
        constexpr float M01 = -0.4F;
        constexpr float M10 = 0.6F;
        constexpr float M11 = 0.6928203F;
        constexpr float TranslationX = 128.0F;
        constexpr float TranslationY = 72.0F;
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run(
            "representation/vec2-ui-transform/compact-8-byte" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const float x = compactInput[index].X();
                    const float y = compactInput[index].Y();
                    compactOutput[index] =
                        mv::math::Vec2f(x * M00 + y * M01 + TranslationX,
                            x * M10 + y * M11 + TranslationY);
                }
                Observe(compactOutput);
            });

        bench.batch(count).run(
            "representation/vec2-ui-transform/aligned-16-byte" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    const float x = alignedInput[index].X();
                    const float y = alignedInput[index].Y();
                    alignedOutput[index] =
                        AlignedVec2f16(x * M00 + y * M01 + TranslationX,
                            x * M10 + y * M11 + TranslationY);
                }
                Observe(alignedOutput);
            });
    }

    void BenchmarkParticleStorage(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<mv::math::Vec3f> computePositions(count);
        std::vector<mv::math::Vec3f> computeVelocities(count);
        auto packedPositions = MakePackedVec3(count, 19U);
        const auto packedVelocities = MakePackedVec3(count, 43U);

        for (std::size_t index = 0; index < count; ++index)
        {
            computePositions[index] = mv::math::Load(packedPositions[index]);
            computeVelocities[index] = mv::math::Load(packedVelocities[index]);
        }

        constexpr float DeltaTime = 1.0F / 60.0F;
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run(
            "representation/particles/compute-16-byte" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    computePositions[index] +=
                        computeVelocities[index] * DeltaTime;
                }
                Observe(computePositions);
            });

        bench.batch(count).run(
            "representation/particles/packed-12-byte" + suffix,
            [&]
            {
                for (std::size_t index = 0; index < count; ++index)
                {
                    mv::math::Vec3f position =
                        mv::math::Load(packedPositions[index]);
                    position +=
                        mv::math::Load(packedVelocities[index]) * DeltaTime;
                    mv::math::Store(packedPositions[index], position);
                }
                Observe(packedPositions);
            });
    }

    void BenchmarkPackedToGpu(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        const auto input = MakePackedVec3(count, 71U);
        std::vector<mv::math::gpu::Float3Slot16> output(count);
        const mv::math::AffineTransform3f transform(
            mv::math::Vec3f(1.1F, 0.1F, 0.0F),
            mv::math::Vec3f(0.0F, 0.9F, 0.2F),
            mv::math::Vec3f(0.05F, 0.0F, 1.2F),
            mv::math::Vec3f(100.0F, -20.0F, 3.0F));
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run(
            "representation/transfer/packed-transform-gpu16" + suffix,
            [&]
            {
                mv::math::TransformPoints(
                    std::span<const mv::math::PackedVec3f>(input), transform,
                    std::span<mv::math::gpu::Float3Slot16>(output));
                Observe(output);
            });
    }

    struct InterleavedVertex
    {
        mv::math::PackedVec3f Position{};
        mv::math::PackedVec3f Normal{};
        mv::math::PackedVec2f Uv{};
    };

    struct InterleavedOutput
    {
        std::uint32_t Instance{};
        mv::math::PackedVec3f Position{};
        mv::math::PackedVec3f PreviousPosition{};
        float SortKey{};
    };

    void BenchmarkStridedTransform(
        ankerl::nanobench::Bench& bench, std::size_t count)
    {
        std::vector<InterleavedVertex> input(count);
        std::vector<InterleavedOutput> output(count);
        for (std::size_t index = 0; index < count; ++index)
        {
            input[index].Position = {
                Seed(index, 0U), Seed(index, 1U), Seed(index, 2U)};
        }

        const auto inputView =
            mv::math::MakeFieldView(std::span<const InterleavedVertex>(input),
                &InterleavedVertex::Position);
        auto outputView = mv::math::MakeFieldView(
            std::span<InterleavedOutput>(output), &InterleavedOutput::Position);

        const mv::math::AffineTransform3f transform(
            mv::math::Vec3f(1.1F, 0.1F, 0.0F),
            mv::math::Vec3f(0.0F, 0.9F, 0.2F),
            mv::math::Vec3f(0.05F, 0.0F, 1.2F),
            mv::math::Vec3f(100.0F, -20.0F, 3.0F));
        const std::string suffix = "/" + std::to_string(count);

        bench.batch(count).run(
            "representation/transfer/strided-transform-packed" + suffix,
            [&]
            {
                mv::math::TransformPoints(inputView, transform, outputView);
                Observe(output);
            });
    }
}  // namespace

int main()
{
    ankerl::nanobench::Bench bench;
    bench.title("Move math representation and data-flow benchmarks")
        .epochs(15)
        .warmup(2)
        .minEpochIterations(4)
        .minEpochTime(std::chrono::milliseconds(50))
        .relative(false)
        .performanceCounters(true);

    for (const std::size_t count : WorkingSetSizes)
    {
        BenchmarkVec3Representations(bench, count);
        BenchmarkVec2Storage(bench, count);
        BenchmarkSpriteIntegration(bench, count);
        BenchmarkUiTransform(bench, count);
        BenchmarkParticleStorage(bench, count);
        BenchmarkPackedToGpu(bench, count);
        BenchmarkStridedTransform(bench, count);
    }

    return EXIT_SUCCESS;
}
