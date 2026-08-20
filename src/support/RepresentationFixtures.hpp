#pragma once

#include <type_traits>

#include <mv/math/detail/VectorOps.hpp>

// Private benchmark controls for comparing storage and compute representations.
// These are deliberately not Move math API types.
namespace vmb::support
{
    class alignas(16) FixedStorageVec3f
    {
    public:
        constexpr FixedStorageVec3f() noexcept = default;

        constexpr FixedStorageVec3f(float x, float y, float z) noexcept
            : Components_{x, y, z, 0.0F}
        {
        }

        [[nodiscard]] FixedStorageVec3f operator+(
            const FixedStorageVec3f& other) const noexcept
        {
            return FromNative(Ops::Add(
                Ops::Load4(Components_), Ops::Load4(other.Components_)));
        }

        [[nodiscard]] FixedStorageVec3f operator*(float scalar) const noexcept
        {
            return FromNative(Ops::Multiply(Ops::Load4(Components_), scalar));
        }

        void Store(float* destination) const noexcept
        {
            destination[0] = Components_[0];
            destination[1] = Components_[1];
            destination[2] = Components_[2];
        }

    private:
        using Ops = mv::math::detail::SelectedVectorOps<float>;

        [[nodiscard]] static FixedStorageVec3f FromNative(
            typename Ops::Native value) noexcept
        {
            FixedStorageVec3f result;
            Ops::Store4(value, result.Components_);
            return result;
        }

        float Components_[4]{};
    };

    class alignas(16) NativeVec3f
    {
    public:
        NativeVec3f() noexcept : Native_(Ops::Load3(Zero_))
        {
        }

        NativeVec3f(float x, float y, float z) noexcept
        {
            const float values[3] = {x, y, z};
            Native_ = Ops::Load3(values);
        }

        [[nodiscard]] NativeVec3f operator+(
            const NativeVec3f& other) const noexcept
        {
            return NativeVec3f(Ops::Add(Native_, other.Native_));
        }

        [[nodiscard]] NativeVec3f operator-(
            const NativeVec3f& other) const noexcept
        {
            return NativeVec3f(Ops::Subtract(Native_, other.Native_));
        }

        [[nodiscard]] NativeVec3f operator*(float scalar) const noexcept
        {
            return NativeVec3f(Ops::Multiply(Native_, scalar));
        }

        [[nodiscard]] float X() const noexcept
        {
            float values[3]{};
            Ops::Store3(Native_, values);
            return values[0];
        }

        [[nodiscard]] float Y() const noexcept
        {
            float values[3]{};
            Ops::Store3(Native_, values);
            return values[1];
        }

        [[nodiscard]] float Z() const noexcept
        {
            float values[3]{};
            Ops::Store3(Native_, values);
            return values[2];
        }

        void Store(float* destination) const noexcept
        {
            Ops::Store3(Native_, destination);
        }

    private:
        using Ops = mv::math::detail::SelectedVectorOps<float>;

        explicit NativeVec3f(typename Ops::Native native) noexcept
            : Native_(native)
        {
        }

        inline static constexpr float Zero_[3] = {0.0F, 0.0F, 0.0F};
        typename Ops::Native Native_;
    };

    class alignas(16) AlignedVec2f16
    {
    public:
        constexpr AlignedVec2f16() noexcept = default;

        constexpr AlignedVec2f16(float x, float y) noexcept
            : Components_{x, y, 0.0F, 0.0F}
        {
        }

        [[nodiscard]] AlignedVec2f16 operator+(
            const AlignedVec2f16& other) const noexcept
        {
            return FromNative(Ops::Add(
                Ops::Load4(Components_), Ops::Load4(other.Components_)));
        }

        [[nodiscard]] AlignedVec2f16 operator*(float scalar) const noexcept
        {
            return FromNative(Ops::Multiply(Ops::Load4(Components_), scalar));
        }

        [[nodiscard]] constexpr float X() const noexcept
        {
            return Components_[0];
        }

        [[nodiscard]] constexpr float Y() const noexcept
        {
            return Components_[1];
        }

    private:
        using Ops = mv::math::detail::SelectedVectorOps<float>;

        [[nodiscard]] static AlignedVec2f16 FromNative(
            typename Ops::Native value) noexcept
        {
            AlignedVec2f16 result;
            Ops::Store4(value, result.Components_);
            return result;
        }

        float Components_[4]{};
    };
}  // namespace vmb::support

static_assert(sizeof(vmb::support::NativeVec3f) == 16);
static_assert(alignof(vmb::support::NativeVec3f) == 16);
static_assert(std::is_trivially_copyable_v<vmb::support::NativeVec3f>);
static_assert(sizeof(vmb::support::FixedStorageVec3f) == 16);
static_assert(alignof(vmb::support::FixedStorageVec3f) == 16);
static_assert(std::is_trivially_copyable_v<vmb::support::FixedStorageVec3f>);
static_assert(sizeof(vmb::support::AlignedVec2f16) == 16);
static_assert(alignof(vmb::support::AlignedVec2f16) == 16);
static_assert(std::is_trivially_copyable_v<vmb::support::AlignedVec2f16>);
