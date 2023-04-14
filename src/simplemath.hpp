// Based on
// https://raw.githubusercontent.com/microsoft/DirectXTK/main/Inc/SimpleMath.h
// and
// https://raw.githubusercontent.com/microsoft/DirectXTK/main/Inc/SimpleMath.inl

#pragma once

#include <stdint.h>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <functional>

#if (__cplusplus >= 202002L)
#include <compare>
#endif

#include <DirectXCollision.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#endif

namespace DirectX
{
    namespace SimpleMath
    {
        struct RECT
        {
            long left;
            long top;
            long right;
            long bottom;
        };

        struct Vector2;
        struct Vector4;
        struct Matrix;
        struct Quaternion;
        struct Plane;

        //------------------------------------------------------------------------------
        // 2D rectangle
        struct Rectangle
        {
            long x;
            long y;
            long width;
            long height;

            // Creators
            Rectangle() noexcept : x(0), y(0), width(0), height(0)
            {
            }
            constexpr Rectangle(long ix, long iy, long iw, long ih) noexcept
                : x(ix), y(iy), width(iw), height(ih)
            {
            }
            explicit Rectangle(const RECT& rct) noexcept
                : x(rct.left),
                  y(rct.top),
                  width(rct.right - rct.left),
                  height(rct.bottom - rct.top)
            {
            }

            Rectangle(const Rectangle&) = default;
            Rectangle& operator=(const Rectangle&) = default;

            Rectangle(Rectangle&&) = default;
            Rectangle& operator=(Rectangle&&) = default;

            operator RECT() noexcept
            {
                RECT rct;
                rct.left = x;
                rct.top = y;
                rct.right = (x + width);
                rct.bottom = (y + height);
                return rct;
            }
#ifdef __cplusplus_winrt
            operator Windows::Foundation::Rect() noexcept
            {
                return Windows::Foundation::Rect(
                    float(x), float(y), float(width), float(height));
            }
#endif

            // Comparison operators
#if (__cplusplus >= 202002L)
            bool operator==(const Rectangle&) const = default;
            auto operator<=>(const Rectangle&) const = default;
#else
            bool operator==(const Rectangle& r) const noexcept
            {
                return (x == r.x) && (y == r.y) && (width == r.width) &&
                       (height == r.height);
            }
            bool operator!=(const Rectangle& r) const noexcept
            {
                return (x != r.x) || (y != r.y) || (width != r.width) ||
                       (height != r.height);
            }
#endif
            bool operator==(const RECT& rct) const noexcept
            {
                return (x == rct.left) && (y == rct.top) &&
                       (width == (rct.right - rct.left)) &&
                       (height == (rct.bottom - rct.top));
            }
            bool operator!=(const RECT& rct) const noexcept
            {
                return (x != rct.left) || (y != rct.top) ||
                       (width != (rct.right - rct.left)) ||
                       (height != (rct.bottom - rct.top));
            }

            // Assignment operators
            Rectangle& operator=(_In_ const RECT& rct) noexcept
            {
                x = rct.left;
                y = rct.top;
                width = (rct.right - rct.left);
                height = (rct.bottom - rct.top);
                return *this;
            }

            // Rectangle operations
            Vector2 Location() const noexcept;
            Vector2 Center() const noexcept;

            bool IsEmpty() const noexcept
            {
                return (width == 0 && height == 0 && x == 0 && y == 0);
            }

            bool Contains(long ix, long iy) const noexcept
            {
                return (x <= ix) && (ix < (x + width)) && (y <= iy) &&
                       (iy < (y + height));
            }
            bool Contains(const Vector2& point) const noexcept;
            bool Contains(const Rectangle& r) const noexcept
            {
                return (x <= r.x) && ((r.x + r.width) <= (x + width)) &&
                       (y <= r.y) && ((r.y + r.height) <= (y + height));
            }
            bool Contains(const RECT& rct) const noexcept
            {
                return (x <= rct.left) && (rct.right <= (x + width)) &&
                       (y <= rct.top) && (rct.bottom <= (y + height));
            }

            void Inflate(long horizAmount, long vertAmount) noexcept;

            bool Intersects(const Rectangle& r) const noexcept
            {
                return (r.x < (x + width)) && (x < (r.x + r.width)) &&
                       (r.y < (y + height)) && (y < (r.y + r.height));
            }
            bool Intersects(const RECT& rct) const noexcept
            {
                return (rct.left < (x + width)) && (x < rct.right) &&
                       (rct.top < (y + height)) && (y < rct.bottom);
            }

            void Offset(long ox, long oy) noexcept
            {
                x += ox;
                y += oy;
            }

            // Static functions
            static Rectangle Intersect(
                const Rectangle& ra, const Rectangle& rb) noexcept;
            static RECT Intersect(const RECT& rcta, const RECT& rctb) noexcept;

            static Rectangle Union(
                const Rectangle& ra, const Rectangle& rb) noexcept;
            static RECT Union(const RECT& rcta, const RECT& rctb) noexcept;
        };

        //------------------------------------------------------------------------------
        // 2D vector
        struct Vector2 : public XMFLOAT2
        {
            Vector2() noexcept : XMFLOAT2(0.f, 0.f)
            {
            }
            constexpr explicit Vector2(float ix) noexcept : XMFLOAT2(ix, ix)
            {
            }
            constexpr Vector2(float ix, float iy) noexcept : XMFLOAT2(ix, iy)
            {
            }
            explicit Vector2(_In_reads_(2) const float* pArray) noexcept
                : XMFLOAT2(pArray)
            {
            }
            Vector2(FXMVECTOR V) noexcept
            {
                XMStoreFloat2(this, V);
            }
            Vector2(const XMFLOAT2& V) noexcept
            {
                this->x = V.x;
                this->y = V.y;
            }
            explicit Vector2(const XMVECTORF32& F) noexcept
            {
                this->x = F.f[0];
                this->y = F.f[1];
            }

            Vector2(const Vector2&) = default;
            Vector2& operator=(const Vector2&) = default;

            Vector2(Vector2&&) = default;
            Vector2& operator=(Vector2&&) = default;

            operator XMVECTOR() const noexcept
            {
                return XMLoadFloat2(this);
            }

            // Comparison operators
            bool operator==(const Vector2& V) const noexcept;
            bool operator!=(const Vector2& V) const noexcept;

            // Assignment operators
            Vector2& operator=(const XMVECTORF32& F) noexcept
            {
                x = F.f[0];
                y = F.f[1];
                return *this;
            }
            Vector2& operator+=(const Vector2& V) noexcept;
            Vector2& operator-=(const Vector2& V) noexcept;
            Vector2& operator*=(const Vector2& V) noexcept;
            Vector2& operator*=(float S) noexcept;
            Vector2& operator/=(float S) noexcept;

            // Unary operators
            Vector2 operator+() const noexcept
            {
                return *this;
            }
            Vector2 operator-() const noexcept
            {
                return Vector2(-x, -y);
            }

            // Vector operations
            bool InBounds(const Vector2& Bounds) const noexcept;

            float Length() const noexcept;
            float LengthSquared() const noexcept;

            float Dot(const Vector2& V) const noexcept;
            void Cross(const Vector2& V, Vector2& result) const noexcept;
            Vector2 Cross(const Vector2& V) const noexcept;

            void Normalize() noexcept;
            void Normalize(Vector2& result) const noexcept;

            void Clamp(const Vector2& vmin, const Vector2& vmax) noexcept;
            void Clamp(const Vector2& vmin, const Vector2& vmax,
                Vector2& result) const noexcept;

            // Static functions
            static float Distance(
                const Vector2& v1, const Vector2& v2) noexcept;
            static float DistanceSquared(
                const Vector2& v1, const Vector2& v2) noexcept;

            static void Min(
                const Vector2& v1, const Vector2& v2, Vector2& result) noexcept;
            static Vector2 Min(const Vector2& v1, const Vector2& v2) noexcept;

            static void Max(
                const Vector2& v1, const Vector2& v2, Vector2& result) noexcept;
            static Vector2 Max(const Vector2& v1, const Vector2& v2) noexcept;

            static void Lerp(const Vector2& v1, const Vector2& v2, float t,
                Vector2& result) noexcept;
            static Vector2 Lerp(
                const Vector2& v1, const Vector2& v2, float t) noexcept;

            static void SmoothStep(const Vector2& v1, const Vector2& v2,
                float t, Vector2& result) noexcept;
            static Vector2 SmoothStep(
                const Vector2& v1, const Vector2& v2, float t) noexcept;

            static void Barycentric(const Vector2& v1, const Vector2& v2,
                const Vector2& v3, float f, float g, Vector2& result) noexcept;
            static Vector2 Barycentric(const Vector2& v1, const Vector2& v2,
                const Vector2& v3, float f, float g) noexcept;

            static void CatmullRom(const Vector2& v1, const Vector2& v2,
                const Vector2& v3, const Vector2& v4, float t,
                Vector2& result) noexcept;
            static Vector2 CatmullRom(const Vector2& v1, const Vector2& v2,
                const Vector2& v3, const Vector2& v4, float t) noexcept;

            static void Hermite(const Vector2& v1, const Vector2& t1,
                const Vector2& v2, const Vector2& t2, float t,
                Vector2& result) noexcept;
            static Vector2 Hermite(const Vector2& v1, const Vector2& t1,
                const Vector2& v2, const Vector2& t2, float t) noexcept;

            static void Reflect(const Vector2& ivec, const Vector2& nvec,
                Vector2& result) noexcept;
            static Vector2 Reflect(
                const Vector2& ivec, const Vector2& nvec) noexcept;

            static void Refract(const Vector2& ivec, const Vector2& nvec,
                float refractionIndex, Vector2& result) noexcept;
            static Vector2 Refract(const Vector2& ivec, const Vector2& nvec,
                float refractionIndex) noexcept;

            static void Transform(const Vector2& v, const Quaternion& quat,
                Vector2& result) noexcept;
            static Vector2 Transform(
                const Vector2& v, const Quaternion& quat) noexcept;

            static void Transform(
                const Vector2& v, const Matrix& m, Vector2& result) noexcept;
            static Vector2 Transform(
                const Vector2& v, const Matrix& m) noexcept;
            static void Transform(_In_reads_(count) const Vector2* varray,
                size_t count, const Matrix& m,
                _Out_writes_(count) Vector2* resultArray) noexcept;

            static void Transform(
                const Vector2& v, const Matrix& m, Vector4& result) noexcept;
            static void Transform(_In_reads_(count) const Vector2* varray,
                size_t count, const Matrix& m,
                _Out_writes_(count) Vector4* resultArray) noexcept;

            static void TransformNormal(
                const Vector2& v, const Matrix& m, Vector2& result) noexcept;
            static Vector2 TransformNormal(
                const Vector2& v, const Matrix& m) noexcept;
            static void TransformNormal(_In_reads_(count) const Vector2* varray,
                size_t count, const Matrix& m,
                _Out_writes_(count) Vector2* resultArray) noexcept;

            // Constants
            static const Vector2 Zero;
            static const Vector2 One;
            static const Vector2 UnitX;
            static const Vector2 UnitY;
        };

        // Binary operators
        Vector2 operator+(const Vector2& V1, const Vector2& V2) noexcept;
        Vector2 operator-(const Vector2& V1, const Vector2& V2) noexcept;
        Vector2 operator*(const Vector2& V1, const Vector2& V2) noexcept;
        Vector2 operator*(const Vector2& V, float S) noexcept;
        Vector2 operator/(const Vector2& V1, const Vector2& V2) noexcept;
        Vector2 operator/(const Vector2& V, float S) noexcept;
        Vector2 operator*(float S, const Vector2& V) noexcept;

        //------------------------------------------------------------------------------
        // 3D vector
        struct Vector3 : public XMFLOAT3
        {
            Vector3() noexcept : XMFLOAT3(0.f, 0.f, 0.f)
            {
            }
            constexpr explicit Vector3(float ix) noexcept : XMFLOAT3(ix, ix, ix)
            {
            }
            constexpr Vector3(float ix, float iy, float iz) noexcept
                : XMFLOAT3(ix, iy, iz)
            {
            }
            explicit Vector3(_In_reads_(3) const float* pArray) noexcept
                : XMFLOAT3(pArray)
            {
            }
            Vector3(FXMVECTOR V) noexcept
            {
                XMStoreFloat3(this, V);
            }
            Vector3(const XMFLOAT3& V) noexcept
            {
                this->x = V.x;
                this->y = V.y;
                this->z = V.z;
            }
            explicit Vector3(const XMVECTORF32& F) noexcept
            {
                this->x = F.f[0];
                this->y = F.f[1];
                this->z = F.f[2];
            }

            Vector3(const Vector3&) = default;
            Vector3& operator=(const Vector3&) = default;

            Vector3(Vector3&&) = default;
            Vector3& operator=(Vector3&&) = default;

            operator XMVECTOR() const noexcept
            {
                return XMLoadFloat3(this);
            }

            // Comparison operators
            bool operator==(const Vector3& V) const noexcept;
            bool operator!=(const Vector3& V) const noexcept;

            // Assignment operators
            Vector3& operator=(const XMVECTORF32& F) noexcept
            {
                x = F.f[0];
                y = F.f[1];
                z = F.f[2];
                return *this;
            }
            Vector3& operator+=(const Vector3& V) noexcept;
            Vector3& operator-=(const Vector3& V) noexcept;
            Vector3& operator*=(const Vector3& V) noexcept;
            Vector3& operator*=(float S) noexcept;
            Vector3& operator/=(float S) noexcept;

            // Unary operators
            Vector3 operator+() const noexcept
            {
                return *this;
            }
            Vector3 operator-() const noexcept;

            // Vector operations
            bool InBounds(const Vector3& Bounds) const noexcept;

            float Length() const noexcept;
            float LengthSquared() const noexcept;

            float Dot(const Vector3& V) const noexcept;
            void Cross(const Vector3& V, Vector3& result) const noexcept;
            Vector3 Cross(const Vector3& V) const noexcept;

            void Normalize() noexcept;
            void Normalize(Vector3& result) const noexcept;

            void Clamp(const Vector3& vmin, const Vector3& vmax) noexcept;
            void Clamp(const Vector3& vmin, const Vector3& vmax,
                Vector3& result) const noexcept;

            // Static functions
            static float Distance(
                const Vector3& v1, const Vector3& v2) noexcept;
            static float DistanceSquared(
                const Vector3& v1, const Vector3& v2) noexcept;

            static void Min(
                const Vector3& v1, const Vector3& v2, Vector3& result) noexcept;
            static Vector3 Min(const Vector3& v1, const Vector3& v2) noexcept;

            static void Max(
                const Vector3& v1, const Vector3& v2, Vector3& result) noexcept;
            static Vector3 Max(const Vector3& v1, const Vector3& v2) noexcept;

            static void Lerp(const Vector3& v1, const Vector3& v2, float t,
                Vector3& result) noexcept;
            static Vector3 Lerp(
                const Vector3& v1, const Vector3& v2, float t) noexcept;

            static void SmoothStep(const Vector3& v1, const Vector3& v2,
                float t, Vector3& result) noexcept;
            static Vector3 SmoothStep(
                const Vector3& v1, const Vector3& v2, float t) noexcept;

            static void Barycentric(const Vector3& v1, const Vector3& v2,
                const Vector3& v3, float f, float g, Vector3& result) noexcept;
            static Vector3 Barycentric(const Vector3& v1, const Vector3& v2,
                const Vector3& v3, float f, float g) noexcept;

            static void CatmullRom(const Vector3& v1, const Vector3& v2,
                const Vector3& v3, const Vector3& v4, float t,
                Vector3& result) noexcept;
            static Vector3 CatmullRom(const Vector3& v1, const Vector3& v2,
                const Vector3& v3, const Vector3& v4, float t) noexcept;

            static void Hermite(const Vector3& v1, const Vector3& t1,
                const Vector3& v2, const Vector3& t2, float t,
                Vector3& result) noexcept;
            static Vector3 Hermite(const Vector3& v1, const Vector3& t1,
                const Vector3& v2, const Vector3& t2, float t) noexcept;

            static void Reflect(const Vector3& ivec, const Vector3& nvec,
                Vector3& result) noexcept;
            static Vector3 Reflect(
                const Vector3& ivec, const Vector3& nvec) noexcept;

            static void Refract(const Vector3& ivec, const Vector3& nvec,
                float refractionIndex, Vector3& result) noexcept;
            static Vector3 Refract(const Vector3& ivec, const Vector3& nvec,
                float refractionIndex) noexcept;

            static void Transform(const Vector3& v, const Quaternion& quat,
                Vector3& result) noexcept;
            static Vector3 Transform(
                const Vector3& v, const Quaternion& quat) noexcept;

            static void Transform(
                const Vector3& v, const Matrix& m, Vector3& result) noexcept;
            static Vector3 Transform(
                const Vector3& v, const Matrix& m) noexcept;
            static void Transform(_In_reads_(count) const Vector3* varray,
                size_t count, const Matrix& m,
                _Out_writes_(count) Vector3* resultArray) noexcept;

            static void Transform(
                const Vector3& v, const Matrix& m, Vector4& result) noexcept;
            static void Transform(_In_reads_(count) const Vector3* varray,
                size_t count, const Matrix& m,
                _Out_writes_(count) Vector4* resultArray) noexcept;

            static void TransformNormal(
                const Vector3& v, const Matrix& m, Vector3& result) noexcept;
            static Vector3 TransformNormal(
                const Vector3& v, const Matrix& m) noexcept;
            static void TransformNormal(_In_reads_(count) const Vector3* varray,
                size_t count, const Matrix& m,
                _Out_writes_(count) Vector3* resultArray) noexcept;

            // Constants
            static const Vector3 Zero;
            static const Vector3 One;
            static const Vector3 UnitX;
            static const Vector3 UnitY;
            static const Vector3 UnitZ;
            static const Vector3 Up;
            static const Vector3 Down;
            static const Vector3 Right;
            static const Vector3 Left;
            static const Vector3 Forward;
            static const Vector3 Backward;
        };

        // Binary operators
        Vector3 operator+(const Vector3& V1, const Vector3& V2) noexcept;
        Vector3 operator-(const Vector3& V1, const Vector3& V2) noexcept;
        Vector3 operator*(const Vector3& V1, const Vector3& V2) noexcept;
        Vector3 operator*(const Vector3& V, float S) noexcept;
        Vector3 operator/(const Vector3& V1, const Vector3& V2) noexcept;
        Vector3 operator/(const Vector3& V, float S) noexcept;
        Vector3 operator*(float S, const Vector3& V) noexcept;

        //------------------------------------------------------------------------------
        // 4D vector
        struct Vector4 : public XMFLOAT4
        {
            Vector4() noexcept : XMFLOAT4(0.f, 0.f, 0.f, 0.f)
            {
            }
            constexpr explicit Vector4(float ix) noexcept
                : XMFLOAT4(ix, ix, ix, ix)
            {
            }
            constexpr Vector4(float ix, float iy, float iz, float iw) noexcept
                : XMFLOAT4(ix, iy, iz, iw)
            {
            }
            explicit Vector4(_In_reads_(4) const float* pArray) noexcept
                : XMFLOAT4(pArray)
            {
            }
            Vector4(FXMVECTOR V) noexcept
            {
                XMStoreFloat4(this, V);
            }
            Vector4(const XMFLOAT4& V) noexcept
            {
                this->x = V.x;
                this->y = V.y;
                this->z = V.z;
                this->w = V.w;
            }
            explicit Vector4(const XMVECTORF32& F) noexcept
            {
                this->x = F.f[0];
                this->y = F.f[1];
                this->z = F.f[2];
                this->w = F.f[3];
            }

            Vector4(const Vector4&) = default;
            Vector4& operator=(const Vector4&) = default;

            Vector4(Vector4&&) = default;
            Vector4& operator=(Vector4&&) = default;

            operator XMVECTOR() const noexcept
            {
                return XMLoadFloat4(this);
            }

            // Comparison operators
            bool operator==(const Vector4& V) const noexcept;
            bool operator!=(const Vector4& V) const noexcept;

            // Assignment operators
            Vector4& operator=(const XMVECTORF32& F) noexcept
            {
                x = F.f[0];
                y = F.f[1];
                z = F.f[2];
                w = F.f[3];
                return *this;
            }
            Vector4& operator+=(const Vector4& V) noexcept;
            Vector4& operator-=(const Vector4& V) noexcept;
            Vector4& operator*=(const Vector4& V) noexcept;
            Vector4& operator*=(float S) noexcept;
            Vector4& operator/=(float S) noexcept;

            // Unary operators
            Vector4 operator+() const noexcept
            {
                return *this;
            }
            Vector4 operator-() const noexcept;

            // Vector operations
            bool InBounds(const Vector4& Bounds) const noexcept;

            float Length() const noexcept;
            float LengthSquared() const noexcept;

            float Dot(const Vector4& V) const noexcept;
            void Cross(const Vector4& v1, const Vector4& v2,
                Vector4& result) const noexcept;
            Vector4 Cross(const Vector4& v1, const Vector4& v2) const noexcept;

            void Normalize() noexcept;
            void Normalize(Vector4& result) const noexcept;

            void Clamp(const Vector4& vmin, const Vector4& vmax) noexcept;
            void Clamp(const Vector4& vmin, const Vector4& vmax,
                Vector4& result) const noexcept;

            // Static functions
            static float Distance(
                const Vector4& v1, const Vector4& v2) noexcept;
            static float DistanceSquared(
                const Vector4& v1, const Vector4& v2) noexcept;

            static void Min(
                const Vector4& v1, const Vector4& v2, Vector4& result) noexcept;
            static Vector4 Min(const Vector4& v1, const Vector4& v2) noexcept;

            static void Max(
                const Vector4& v1, const Vector4& v2, Vector4& result) noexcept;
            static Vector4 Max(const Vector4& v1, const Vector4& v2) noexcept;

            static void Lerp(const Vector4& v1, const Vector4& v2, float t,
                Vector4& result) noexcept;
            static Vector4 Lerp(
                const Vector4& v1, const Vector4& v2, float t) noexcept;

            static void SmoothStep(const Vector4& v1, const Vector4& v2,
                float t, Vector4& result) noexcept;
            static Vector4 SmoothStep(
                const Vector4& v1, const Vector4& v2, float t) noexcept;

            static void Barycentric(const Vector4& v1, const Vector4& v2,
                const Vector4& v3, float f, float g, Vector4& result) noexcept;
            static Vector4 Barycentric(const Vector4& v1, const Vector4& v2,
                const Vector4& v3, float f, float g) noexcept;

            static void CatmullRom(const Vector4& v1, const Vector4& v2,
                const Vector4& v3, const Vector4& v4, float t,
                Vector4& result) noexcept;
            static Vector4 CatmullRom(const Vector4& v1, const Vector4& v2,
                const Vector4& v3, const Vector4& v4, float t) noexcept;

            static void Hermite(const Vector4& v1, const Vector4& t1,
                const Vector4& v2, const Vector4& t2, float t,
                Vector4& result) noexcept;
            static Vector4 Hermite(const Vector4& v1, const Vector4& t1,
                const Vector4& v2, const Vector4& t2, float t) noexcept;

            static void Reflect(const Vector4& ivec, const Vector4& nvec,
                Vector4& result) noexcept;
            static Vector4 Reflect(
                const Vector4& ivec, const Vector4& nvec) noexcept;

            static void Refract(const Vector4& ivec, const Vector4& nvec,
                float refractionIndex, Vector4& result) noexcept;
            static Vector4 Refract(const Vector4& ivec, const Vector4& nvec,
                float refractionIndex) noexcept;

            static void Transform(const Vector2& v, const Quaternion& quat,
                Vector4& result) noexcept;
            static Vector4 Transform(
                const Vector2& v, const Quaternion& quat) noexcept;

            static void Transform(const Vector3& v, const Quaternion& quat,
                Vector4& result) noexcept;
            static Vector4 Transform(
                const Vector3& v, const Quaternion& quat) noexcept;

            static void Transform(const Vector4& v, const Quaternion& quat,
                Vector4& result) noexcept;
            static Vector4 Transform(
                const Vector4& v, const Quaternion& quat) noexcept;

            static void Transform(
                const Vector4& v, const Matrix& m, Vector4& result) noexcept;
            static Vector4 Transform(
                const Vector4& v, const Matrix& m) noexcept;
            static void Transform(_In_reads_(count) const Vector4* varray,
                size_t count, const Matrix& m,
                _Out_writes_(count) Vector4* resultArray) noexcept;

            // Constants
            static const Vector4 Zero;
            static const Vector4 One;
            static const Vector4 UnitX;
            static const Vector4 UnitY;
            static const Vector4 UnitZ;
            static const Vector4 UnitW;
        };

        // Binary operators
        Vector4 operator+(const Vector4& V1, const Vector4& V2) noexcept;
        Vector4 operator-(const Vector4& V1, const Vector4& V2) noexcept;
        Vector4 operator*(const Vector4& V1, const Vector4& V2) noexcept;
        Vector4 operator*(const Vector4& V, float S) noexcept;
        Vector4 operator/(const Vector4& V1, const Vector4& V2) noexcept;
        Vector4 operator/(const Vector4& V, float S) noexcept;
        Vector4 operator*(float S, const Vector4& V) noexcept;

        //------------------------------------------------------------------------------
        // 4x4 Matrix (assumes right-handed cooordinates)
        struct Matrix : public XMFLOAT4X4
        {
            Matrix() noexcept
                : XMFLOAT4X4(
                      1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f)
            {
            }
            constexpr Matrix(float m00, float m01, float m02, float m03,
                float m10, float m11, float m12, float m13, float m20,
                float m21, float m22, float m23, float m30, float m31,
                float m32, float m33) noexcept
                : XMFLOAT4X4(m00, m01, m02, m03, m10, m11, m12, m13, m20, m21,
                      m22, m23, m30, m31, m32, m33)
            {
            }
            explicit Matrix(const Vector3& r0, const Vector3& r1,
                const Vector3& r2) noexcept
                : XMFLOAT4X4(r0.x, r0.y, r0.z, 0, r1.x, r1.y, r1.z, 0, r2.x,
                      r2.y, r2.z, 0, 0, 0, 0, 1.f)
            {
            }
            explicit Matrix(const Vector4& r0, const Vector4& r1,
                const Vector4& r2, const Vector4& r3) noexcept
                : XMFLOAT4X4(r0.x, r0.y, r0.z, r0.w, r1.x, r1.y, r1.z, r1.w,
                      r2.x, r2.y, r2.z, r2.w, r3.x, r3.y, r3.z, r3.w)
            {
            }
            Matrix(const XMFLOAT4X4& M) noexcept
            {
                memcpy(this, &M, sizeof(XMFLOAT4X4));
            }
            Matrix(const XMFLOAT3X3& M) noexcept;
            Matrix(const XMFLOAT4X3& M) noexcept;

            explicit Matrix(_In_reads_(16) const float* pArray) noexcept
                : XMFLOAT4X4(pArray)
            {
            }
            Matrix(CXMMATRIX M) noexcept
            {
                XMStoreFloat4x4(this, M);
            }

            Matrix(const Matrix&) = default;
            Matrix& operator=(const Matrix&) = default;

            Matrix(Matrix&&) = default;
            Matrix& operator=(Matrix&&) = default;

            operator XMMATRIX() const noexcept
            {
                return XMLoadFloat4x4(this);
            }

            // Comparison operators
            bool operator==(const Matrix& M) const noexcept;
            bool operator!=(const Matrix& M) const noexcept;

            // Assignment operators
            Matrix& operator=(const XMFLOAT3X3& M) noexcept;
            Matrix& operator=(const XMFLOAT4X3& M) noexcept;
            Matrix& operator+=(const Matrix& M) noexcept;
            Matrix& operator-=(const Matrix& M) noexcept;
            Matrix& operator*=(const Matrix& M) noexcept;
            Matrix& operator*=(float S) noexcept;
            Matrix& operator/=(float S) noexcept;

            Matrix& operator/=(const Matrix& M) noexcept;
            // Element-wise divide

            // Unary operators
            Matrix operator+() const noexcept
            {
                return *this;
            }
            Matrix operator-() const noexcept;

            // Properties
            Vector3 Up() const noexcept
            {
                return Vector3(_21, _22, _23);
            }
            void Up(const Vector3& v) noexcept
            {
                _21 = v.x;
                _22 = v.y;
                _23 = v.z;
            }

            Vector3 Down() const noexcept
            {
                return Vector3(-_21, -_22, -_23);
            }
            void Down(const Vector3& v) noexcept
            {
                _21 = -v.x;
                _22 = -v.y;
                _23 = -v.z;
            }

            Vector3 Right() const noexcept
            {
                return Vector3(_11, _12, _13);
            }
            void Right(const Vector3& v) noexcept
            {
                _11 = v.x;
                _12 = v.y;
                _13 = v.z;
            }

            Vector3 Left() const noexcept
            {
                return Vector3(-_11, -_12, -_13);
            }
            void Left(const Vector3& v) noexcept
            {
                _11 = -v.x;
                _12 = -v.y;
                _13 = -v.z;
            }

            Vector3 Forward() const noexcept
            {
                return Vector3(-_31, -_32, -_33);
            }
            void Forward(const Vector3& v) noexcept
            {
                _31 = -v.x;
                _32 = -v.y;
                _33 = -v.z;
            }

            Vector3 Backward() const noexcept
            {
                return Vector3(_31, _32, _33);
            }
            void Backward(const Vector3& v) noexcept
            {
                _31 = v.x;
                _32 = v.y;
                _33 = v.z;
            }

            Vector3 Translation() const noexcept
            {
                return Vector3(_41, _42, _43);
            }
            void Translation(const Vector3& v) noexcept
            {
                _41 = v.x;
                _42 = v.y;
                _43 = v.z;
            }

            // Matrix operations
            bool Decompose(Vector3& scale, Quaternion& rotation,
                Vector3& translation) noexcept;

            Matrix Transpose() const noexcept;
            void Transpose(Matrix& result) const noexcept;

            Matrix Invert() const noexcept;
            void Invert(Matrix& result) const noexcept;

            float Determinant() const noexcept;

            // Computes rotation about y-axis (y), then x-axis (x), then z-axis
            // (z)
            Vector3 ToEuler() const noexcept;

            // Static functions
            static Matrix CreateBillboard(const Vector3& object,
                const Vector3& cameraPosition, const Vector3& cameraUp,
                _In_opt_ const Vector3* cameraForward = nullptr) noexcept;

            static Matrix CreateConstrainedBillboard(const Vector3& object,
                const Vector3& cameraPosition, const Vector3& rotateAxis,
                _In_opt_ const Vector3* cameraForward = nullptr,
                _In_opt_ const Vector3* objectForward = nullptr) noexcept;

            static Matrix CreateTranslation(const Vector3& position) noexcept;
            static Matrix CreateTranslation(float x, float y, float z) noexcept;

            static Matrix CreateScale(const Vector3& scales) noexcept;
            static Matrix CreateScale(float xs, float ys, float zs) noexcept;
            static Matrix CreateScale(float scale) noexcept;

            static Matrix CreateRotationX(float radians) noexcept;
            static Matrix CreateRotationY(float radians) noexcept;
            static Matrix CreateRotationZ(float radians) noexcept;

            static Matrix CreateFromAxisAngle(
                const Vector3& axis, float angle) noexcept;

            static Matrix CreatePerspectiveFieldOfView(float fov,
                float aspectRatio, float nearPlane, float farPlane) noexcept;
            static Matrix CreatePerspective(float width, float height,
                float nearPlane, float farPlane) noexcept;
            static Matrix CreatePerspectiveOffCenter(float left, float right,
                float bottom, float top, float nearPlane,
                float farPlane) noexcept;
            static Matrix CreateOrthographic(float width, float height,
                float zNearPlane, float zFarPlane) noexcept;
            static Matrix CreateOrthographicOffCenter(float left, float right,
                float bottom, float top, float zNearPlane,
                float zFarPlane) noexcept;

            static Matrix CreateLookAt(const Vector3& position,
                const Vector3& target, const Vector3& up) noexcept;
            static Matrix CreateWorld(const Vector3& position,
                const Vector3& forward, const Vector3& up) noexcept;

            static Matrix CreateFromQuaternion(const Quaternion& quat) noexcept;

            // Rotates about y-axis (yaw), then x-axis (pitch), then z-axis
            // (roll)
            static Matrix CreateFromYawPitchRoll(
                float yaw, float pitch, float roll) noexcept;

            // Rotates about y-axis (angles.y), then x-axis (angles.x), then
            // z-axis (angles.z)
            static Matrix CreateFromYawPitchRoll(
                const Vector3& angles) noexcept;

            static Matrix CreateShadow(
                const Vector3& lightDir, const Plane& plane) noexcept;

            static Matrix CreateReflection(const Plane& plane) noexcept;

            static void Lerp(const Matrix& M1, const Matrix& M2, float t,
                Matrix& result) noexcept;
            static Matrix Lerp(
                const Matrix& M1, const Matrix& M2, float t) noexcept;

            static void Transform(const Matrix& M, const Quaternion& rotation,
                Matrix& result) noexcept;
            static Matrix Transform(
                const Matrix& M, const Quaternion& rotation) noexcept;

            // Constants
            static const Matrix Identity;
        };

        // Binary operators
        Matrix operator+(const Matrix& M1, const Matrix& M2) noexcept;
        Matrix operator-(const Matrix& M1, const Matrix& M2) noexcept;
        Matrix operator*(const Matrix& M1, const Matrix& M2) noexcept;
        Matrix operator*(const Matrix& M, float S) noexcept;
        Matrix operator/(const Matrix& M, float S) noexcept;
        Matrix operator/(const Matrix& M1, const Matrix& M2) noexcept;
        // Element-wise divide
        Matrix operator*(float S, const Matrix& M) noexcept;

        //-----------------------------------------------------------------------------
        // Plane
        struct Plane : public XMFLOAT4
        {
            Plane() noexcept : XMFLOAT4(0.f, 1.f, 0.f, 0.f)
            {
            }
            constexpr Plane(float ix, float iy, float iz, float iw) noexcept
                : XMFLOAT4(ix, iy, iz, iw)
            {
            }
            Plane(const Vector3& normal, float d) noexcept
                : XMFLOAT4(normal.x, normal.y, normal.z, d)
            {
            }
            Plane(const Vector3& point1, const Vector3& point2,
                const Vector3& point3) noexcept;
            Plane(const Vector3& point, const Vector3& normal) noexcept;
            explicit Plane(const Vector4& v) noexcept
                : XMFLOAT4(v.x, v.y, v.z, v.w)
            {
            }
            explicit Plane(_In_reads_(4) const float* pArray) noexcept
                : XMFLOAT4(pArray)
            {
            }
            Plane(FXMVECTOR V) noexcept
            {
                XMStoreFloat4(this, V);
            }
            Plane(const XMFLOAT4& p) noexcept
            {
                this->x = p.x;
                this->y = p.y;
                this->z = p.z;
                this->w = p.w;
            }
            explicit Plane(const XMVECTORF32& F) noexcept
            {
                this->x = F.f[0];
                this->y = F.f[1];
                this->z = F.f[2];
                this->w = F.f[3];
            }

            Plane(const Plane&) = default;
            Plane& operator=(const Plane&) = default;

            Plane(Plane&&) = default;
            Plane& operator=(Plane&&) = default;

            operator XMVECTOR() const noexcept
            {
                return XMLoadFloat4(this);
            }

            // Comparison operators
            bool operator==(const Plane& p) const noexcept;
            bool operator!=(const Plane& p) const noexcept;

            // Assignment operators
            Plane& operator=(const XMVECTORF32& F) noexcept
            {
                x = F.f[0];
                y = F.f[1];
                z = F.f[2];
                w = F.f[3];
                return *this;
            }

            // Properties
            Vector3 Normal() const noexcept
            {
                return Vector3(x, y, z);
            }
            void Normal(const Vector3& normal) noexcept
            {
                x = normal.x;
                y = normal.y;
                z = normal.z;
            }

            float D() const noexcept
            {
                return w;
            }
            void D(float d) noexcept
            {
                w = d;
            }

            // Plane operations
            void Normalize() noexcept;
            void Normalize(Plane& result) const noexcept;

            float Dot(const Vector4& v) const noexcept;
            float DotCoordinate(const Vector3& position) const noexcept;
            float DotNormal(const Vector3& normal) const noexcept;

            // Static functions
            static void Transform(
                const Plane& plane, const Matrix& M, Plane& result) noexcept;
            static Plane Transform(
                const Plane& plane, const Matrix& M) noexcept;

            static void Transform(const Plane& plane,
                const Quaternion& rotation, Plane& result) noexcept;
            static Plane Transform(
                const Plane& plane, const Quaternion& rotation) noexcept;
            // Input quaternion must be the inverse transpose of the
            // transformation
        };

        //------------------------------------------------------------------------------
        // Quaternion
        struct Quaternion : public XMFLOAT4
        {
            Quaternion() noexcept : XMFLOAT4(0, 0, 0, 1.f)
            {
            }
            constexpr Quaternion(
                float ix, float iy, float iz, float iw) noexcept
                : XMFLOAT4(ix, iy, iz, iw)
            {
            }
            Quaternion(const Vector3& v, float scalar) noexcept
                : XMFLOAT4(v.x, v.y, v.z, scalar)
            {
            }
            explicit Quaternion(const Vector4& v) noexcept
                : XMFLOAT4(v.x, v.y, v.z, v.w)
            {
            }
            explicit Quaternion(_In_reads_(4) const float* pArray) noexcept
                : XMFLOAT4(pArray)
            {
            }
            Quaternion(FXMVECTOR V) noexcept
            {
                XMStoreFloat4(this, V);
            }
            Quaternion(const XMFLOAT4& q) noexcept
            {
                this->x = q.x;
                this->y = q.y;
                this->z = q.z;
                this->w = q.w;
            }
            explicit Quaternion(const XMVECTORF32& F) noexcept
            {
                this->x = F.f[0];
                this->y = F.f[1];
                this->z = F.f[2];
                this->w = F.f[3];
            }

            Quaternion(const Quaternion&) = default;
            Quaternion& operator=(const Quaternion&) = default;

            Quaternion(Quaternion&&) = default;
            Quaternion& operator=(Quaternion&&) = default;

            operator XMVECTOR() const noexcept
            {
                return XMLoadFloat4(this);
            }

            // Comparison operators
            bool operator==(const Quaternion& q) const noexcept;
            bool operator!=(const Quaternion& q) const noexcept;

            // Assignment operators
            Quaternion& operator=(const XMVECTORF32& F) noexcept
            {
                x = F.f[0];
                y = F.f[1];
                z = F.f[2];
                w = F.f[3];
                return *this;
            }
            Quaternion& operator+=(const Quaternion& q) noexcept;
            Quaternion& operator-=(const Quaternion& q) noexcept;
            Quaternion& operator*=(const Quaternion& q) noexcept;
            Quaternion& operator*=(float S) noexcept;
            Quaternion& operator/=(const Quaternion& q) noexcept;

            // Unary operators
            Quaternion operator+() const noexcept
            {
                return *this;
            }
            Quaternion operator-() const noexcept;

            // Quaternion operations
            float Length() const noexcept;
            float LengthSquared() const noexcept;

            void Normalize() noexcept;
            void Normalize(Quaternion& result) const noexcept;

            void Conjugate() noexcept;
            void Conjugate(Quaternion& result) const noexcept;

            void Inverse(Quaternion& result) const noexcept;

            float Dot(const Quaternion& Q) const noexcept;

            void RotateTowards(
                const Quaternion& target, float maxAngle) noexcept;

            void RotateTowards(const Quaternion& target, float maxAngle,
                Quaternion& result) const noexcept;

            // Computes rotation about y-axis (y), then x-axis (x), then z-axis
            // (z)
            Vector3 ToEuler() const noexcept;

            // Static functions
            static Quaternion CreateFromAxisAngle(
                const Vector3& axis, float angle) noexcept;

            // Rotates about y-axis (yaw), then x-axis (pitch), then z-axis
            // (roll)
            static Quaternion CreateFromYawPitchRoll(
                float yaw, float pitch, float roll) noexcept;

            // Rotates about y-axis (angles.y), then x-axis (angles.x), then
            // z-axis (angles.z)
            static Quaternion CreateFromYawPitchRoll(
                const Vector3& angles) noexcept;

            static Quaternion CreateFromRotationMatrix(
                const Matrix& M) noexcept;

            static void Lerp(const Quaternion& q1, const Quaternion& q2,
                float t, Quaternion& result) noexcept;
            static Quaternion Lerp(
                const Quaternion& q1, const Quaternion& q2, float t) noexcept;

            static void Slerp(const Quaternion& q1, const Quaternion& q2,
                float t, Quaternion& result) noexcept;
            static Quaternion Slerp(
                const Quaternion& q1, const Quaternion& q2, float t) noexcept;

            static void Concatenate(const Quaternion& q1, const Quaternion& q2,
                Quaternion& result) noexcept;
            static Quaternion Concatenate(
                const Quaternion& q1, const Quaternion& q2) noexcept;

            static void FromToRotation(const Vector3& fromDir,
                const Vector3& toDir, Quaternion& result) noexcept;
            static Quaternion FromToRotation(
                const Vector3& fromDir, const Vector3& toDir) noexcept;

            static void LookRotation(const Vector3& forward, const Vector3& up,
                Quaternion& result) noexcept;
            static Quaternion LookRotation(
                const Vector3& forward, const Vector3& up) noexcept;

            static float Angle(
                const Quaternion& q1, const Quaternion& q2) noexcept;

            // Constants
            static const Quaternion Identity;
        };

        // Binary operators
        Quaternion operator+(
            const Quaternion& Q1, const Quaternion& Q2) noexcept;
        Quaternion operator-(
            const Quaternion& Q1, const Quaternion& Q2) noexcept;
        Quaternion operator*(
            const Quaternion& Q1, const Quaternion& Q2) noexcept;
        Quaternion operator*(const Quaternion& Q, float S) noexcept;
        Quaternion operator/(
            const Quaternion& Q1, const Quaternion& Q2) noexcept;
        Quaternion operator*(float S, const Quaternion& Q) noexcept;

        //------------------------------------------------------------------------------
        // Color
        struct Color : public XMFLOAT4
        {
            Color() noexcept : XMFLOAT4(0, 0, 0, 1.f)
            {
            }
            constexpr Color(float _r, float _g, float _b) noexcept
                : XMFLOAT4(_r, _g, _b, 1.f)
            {
            }
            constexpr Color(float _r, float _g, float _b, float _a) noexcept
                : XMFLOAT4(_r, _g, _b, _a)
            {
            }
            explicit Color(const Vector3& clr) noexcept
                : XMFLOAT4(clr.x, clr.y, clr.z, 1.f)
            {
            }
            explicit Color(const Vector4& clr) noexcept
                : XMFLOAT4(clr.x, clr.y, clr.z, clr.w)
            {
            }
            explicit Color(_In_reads_(4) const float* pArray) noexcept
                : XMFLOAT4(pArray)
            {
            }
            Color(FXMVECTOR V) noexcept
            {
                XMStoreFloat4(this, V);
            }
            Color(const XMFLOAT4& c) noexcept
            {
                this->x = c.x;
                this->y = c.y;
                this->z = c.z;
                this->w = c.w;
            }
            explicit Color(const XMVECTORF32& F) noexcept
            {
                this->x = F.f[0];
                this->y = F.f[1];
                this->z = F.f[2];
                this->w = F.f[3];
            }

            // BGRA Direct3D 9 D3DCOLOR packed color
            explicit Color(
                const DirectX::PackedVector::XMCOLOR& Packed) noexcept;

            // RGBA XNA Game Studio packed color
            explicit Color(
                const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept;

            Color(const Color&) = default;
            Color& operator=(const Color&) = default;

            Color(Color&&) = default;
            Color& operator=(Color&&) = default;

            operator XMVECTOR() const noexcept
            {
                return XMLoadFloat4(this);
            }
            operator const float*() const noexcept
            {
                return reinterpret_cast<const float*>(this);
            }

            // Comparison operators
            bool operator==(const Color& c) const noexcept;
            bool operator!=(const Color& c) const noexcept;

            // Assignment operators
            Color& operator=(const XMVECTORF32& F) noexcept
            {
                x = F.f[0];
                y = F.f[1];
                z = F.f[2];
                w = F.f[3];
                return *this;
            }
            Color& operator=(
                const DirectX::PackedVector::XMCOLOR& Packed) noexcept;
            Color& operator=(
                const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept;
            Color& operator+=(const Color& c) noexcept;
            Color& operator-=(const Color& c) noexcept;
            Color& operator*=(const Color& c) noexcept;
            Color& operator*=(float S) noexcept;
            Color& operator/=(const Color& c) noexcept;

            // Unary operators
            Color operator+() const noexcept
            {
                return *this;
            }
            Color operator-() const noexcept;

            // Properties
            float R() const noexcept
            {
                return x;
            }
            void R(float r) noexcept
            {
                x = r;
            }

            float G() const noexcept
            {
                return y;
            }
            void G(float g) noexcept
            {
                y = g;
            }

            float B() const noexcept
            {
                return z;
            }
            void B(float b) noexcept
            {
                z = b;
            }

            float A() const noexcept
            {
                return w;
            }
            void A(float a) noexcept
            {
                w = a;
            }

            // Color operations
            DirectX::PackedVector::XMCOLOR BGRA() const noexcept;
            DirectX::PackedVector::XMUBYTEN4 RGBA() const noexcept;

            Vector3 ToVector3() const noexcept;
            Vector4 ToVector4() const noexcept;

            void Negate() noexcept;
            void Negate(Color& result) const noexcept;

            void Saturate() noexcept;
            void Saturate(Color& result) const noexcept;

            void Premultiply() noexcept;
            void Premultiply(Color& result) const noexcept;

            void AdjustSaturation(float sat) noexcept;
            void AdjustSaturation(float sat, Color& result) const noexcept;

            void AdjustContrast(float contrast) noexcept;
            void AdjustContrast(float contrast, Color& result) const noexcept;

            // Static functions
            static void Modulate(
                const Color& c1, const Color& c2, Color& result) noexcept;
            static Color Modulate(const Color& c1, const Color& c2) noexcept;

            static void Lerp(const Color& c1, const Color& c2, float t,
                Color& result) noexcept;
            static Color Lerp(
                const Color& c1, const Color& c2, float t) noexcept;
        };

        // Binary operators
        Color operator+(const Color& C1, const Color& C2) noexcept;
        Color operator-(const Color& C1, const Color& C2) noexcept;
        Color operator*(const Color& C1, const Color& C2) noexcept;
        Color operator*(const Color& C, float S) noexcept;
        Color operator/(const Color& C1, const Color& C2) noexcept;
        Color operator*(float S, const Color& C) noexcept;

        //------------------------------------------------------------------------------
        // Ray
        class Ray
        {
        public:
            Vector3 position;
            Vector3 direction;

            Ray() noexcept : position(0, 0, 0), direction(0, 0, 1)
            {
            }
            Ray(const Vector3& pos, const Vector3& dir) noexcept
                : position(pos), direction(dir)
            {
            }

            Ray(const Ray&) = default;
            Ray& operator=(const Ray&) = default;

            Ray(Ray&&) = default;
            Ray& operator=(Ray&&) = default;

            // Comparison operators
            bool operator==(const Ray& r) const noexcept;
            bool operator!=(const Ray& r) const noexcept;

            // Ray operations
            bool Intersects(
                const BoundingSphere& sphere, _Out_ float& Dist) const noexcept;
            bool Intersects(
                const BoundingBox& box, _Out_ float& Dist) const noexcept;
            bool Intersects(const Vector3& tri0, const Vector3& tri1,
                const Vector3& tri2, _Out_ float& Dist) const noexcept;
            bool Intersects(
                const Plane& plane, _Out_ float& Dist) const noexcept;
        };

        //------------------------------------------------------------------------------
        // Viewport
        class Viewport
        {
        public:
            float x;
            float y;
            float width;
            float height;
            float minDepth;
            float maxDepth;

            Viewport() noexcept
                : x(0.f),
                  y(0.f),
                  width(0.f),
                  height(0.f),
                  minDepth(0.f),
                  maxDepth(1.f)
            {
            }
            constexpr Viewport(float ix, float iy, float iw, float ih,
                float iminz = 0.f, float imaxz = 1.f) noexcept
                : x(ix),
                  y(iy),
                  width(iw),
                  height(ih),
                  minDepth(iminz),
                  maxDepth(imaxz)
            {
            }
            explicit Viewport(const RECT& rct) noexcept
                : x(float(rct.left)),
                  y(float(rct.top)),
                  width(float(rct.right - rct.left)),
                  height(float(rct.bottom - rct.top)),
                  minDepth(0.f),
                  maxDepth(1.f)
            {
            }

#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
            // Direct3D 11 interop
            explicit Viewport(const D3D11_VIEWPORT& vp) noexcept
                : x(vp.TopLeftX),
                  y(vp.TopLeftY),
                  width(vp.Width),
                  height(vp.Height),
                  minDepth(vp.MinDepth),
                  maxDepth(vp.MaxDepth)
            {
            }

            operator D3D11_VIEWPORT() noexcept
            {
                return *reinterpret_cast<const D3D11_VIEWPORT*>(this);
            }
            const D3D11_VIEWPORT* Get11() const noexcept
            {
                return reinterpret_cast<const D3D11_VIEWPORT*>(this);
            }
            Viewport& operator=(const D3D11_VIEWPORT& vp) noexcept;
#endif

#if defined(__d3d12_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
            // Direct3D 12 interop
            explicit Viewport(const D3D12_VIEWPORT& vp) noexcept
                : x(vp.TopLeftX),
                  y(vp.TopLeftY),
                  width(vp.Width),
                  height(vp.Height),
                  minDepth(vp.MinDepth),
                  maxDepth(vp.MaxDepth)
            {
            }

            operator D3D12_VIEWPORT() noexcept
            {
                return *reinterpret_cast<const D3D12_VIEWPORT*>(this);
            }
            const D3D12_VIEWPORT* Get12() const noexcept
            {
                return reinterpret_cast<const D3D12_VIEWPORT*>(this);
            }
            Viewport& operator=(const D3D12_VIEWPORT& vp) noexcept;
#endif

            Viewport(const Viewport&) = default;
            Viewport& operator=(const Viewport&) = default;

            Viewport(Viewport&&) = default;
            Viewport& operator=(Viewport&&) = default;

            // Comparison operators
#if (__cplusplus >= 202002L)
            bool operator==(const Viewport&) const = default;
            auto operator<=>(const Viewport&) const = default;
#else
            bool operator==(const Viewport& vp) const noexcept;
            bool operator!=(const Viewport& vp) const noexcept;
#endif

            // Assignment operators
            Viewport& operator=(const RECT& rct) noexcept;

            // Viewport operations
            float AspectRatio() const noexcept;

            Vector3 Project(const Vector3& p, const Matrix& proj,
                const Matrix& view, const Matrix& world) const noexcept;
            void Project(const Vector3& p, const Matrix& proj,
                const Matrix& view, const Matrix& world,
                Vector3& result) const noexcept;

            Vector3 Unproject(const Vector3& p, const Matrix& proj,
                const Matrix& view, const Matrix& world) const noexcept;
            void Unproject(const Vector3& p, const Matrix& proj,
                const Matrix& view, const Matrix& world,
                Vector3& result) const noexcept;

            // Static methods
#if defined(__dxgi1_2_h__) || defined(__d3d11_x_h__) || \
    defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
            static RECT ComputeDisplayArea(DXGI_SCALING scaling,
                uint32_t backBufferWidth, uint32_t backBufferHeight,
                int outputWidth, int outputHeight) noexcept;
#endif
        };
    }  // namespace SimpleMath

}  // namespace DirectX

//------------------------------------------------------------------------------
// Support for SimpleMath and Standard C++ Library containers
namespace std
{
    template <>
    struct less<DirectX::SimpleMath::Rectangle>
    {
        bool operator()(const DirectX::SimpleMath::Rectangle& r1,
            const DirectX::SimpleMath::Rectangle& r2) const noexcept
        {
            return (
                (r1.x < r2.x) || ((r1.x == r2.x) && (r1.y < r2.y)) ||
                ((r1.x == r2.x) && (r1.y == r2.y) && (r1.width < r2.width)) ||
                ((r1.x == r2.x) && (r1.y == r2.y) && (r1.width == r2.width) &&
                    (r1.height < r2.height)));
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Vector2>
    {
        bool operator()(const DirectX::SimpleMath::Vector2& V1,
            const DirectX::SimpleMath::Vector2& V2) const noexcept
        {
            return ((V1.x < V2.x) || ((V1.x == V2.x) && (V1.y < V2.y)));
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Vector3>
    {
        bool operator()(const DirectX::SimpleMath::Vector3& V1,
            const DirectX::SimpleMath::Vector3& V2) const noexcept
        {
            return ((V1.x < V2.x) || ((V1.x == V2.x) && (V1.y < V2.y)) ||
                    ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z)));
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Vector4>
    {
        bool operator()(const DirectX::SimpleMath::Vector4& V1,
            const DirectX::SimpleMath::Vector4& V2) const noexcept
        {
            return ((V1.x < V2.x) || ((V1.x == V2.x) && (V1.y < V2.y)) ||
                    ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z < V2.z)) ||
                    ((V1.x == V2.x) && (V1.y == V2.y) && (V1.z == V2.z) &&
                        (V1.w < V2.w)));
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Matrix>
    {
        bool operator()(const DirectX::SimpleMath::Matrix& M1,
            const DirectX::SimpleMath::Matrix& M2) const noexcept
        {
            if (M1._11 != M2._11) return M1._11 < M2._11;
            if (M1._12 != M2._12) return M1._12 < M2._12;
            if (M1._13 != M2._13) return M1._13 < M2._13;
            if (M1._14 != M2._14) return M1._14 < M2._14;
            if (M1._21 != M2._21) return M1._21 < M2._21;
            if (M1._22 != M2._22) return M1._22 < M2._22;
            if (M1._23 != M2._23) return M1._23 < M2._23;
            if (M1._24 != M2._24) return M1._24 < M2._24;
            if (M1._31 != M2._31) return M1._31 < M2._31;
            if (M1._32 != M2._32) return M1._32 < M2._32;
            if (M1._33 != M2._33) return M1._33 < M2._33;
            if (M1._34 != M2._34) return M1._34 < M2._34;
            if (M1._41 != M2._41) return M1._41 < M2._41;
            if (M1._42 != M2._42) return M1._42 < M2._42;
            if (M1._43 != M2._43) return M1._43 < M2._43;
            if (M1._44 != M2._44) return M1._44 < M2._44;

            return false;
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Plane>
    {
        bool operator()(const DirectX::SimpleMath::Plane& P1,
            const DirectX::SimpleMath::Plane& P2) const noexcept
        {
            return ((P1.x < P2.x) || ((P1.x == P2.x) && (P1.y < P2.y)) ||
                    ((P1.x == P2.x) && (P1.y == P2.y) && (P1.z < P2.z)) ||
                    ((P1.x == P2.x) && (P1.y == P2.y) && (P1.z == P2.z) &&
                        (P1.w < P2.w)));
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Quaternion>
    {
        bool operator()(const DirectX::SimpleMath::Quaternion& Q1,
            const DirectX::SimpleMath::Quaternion& Q2) const noexcept
        {
            return ((Q1.x < Q2.x) || ((Q1.x == Q2.x) && (Q1.y < Q2.y)) ||
                    ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z < Q2.z)) ||
                    ((Q1.x == Q2.x) && (Q1.y == Q2.y) && (Q1.z == Q2.z) &&
                        (Q1.w < Q2.w)));
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Color>
    {
        bool operator()(const DirectX::SimpleMath::Color& C1,
            const DirectX::SimpleMath::Color& C2) const noexcept
        {
            return ((C1.x < C2.x) || ((C1.x == C2.x) && (C1.y < C2.y)) ||
                    ((C1.x == C2.x) && (C1.y == C2.y) && (C1.z < C2.z)) ||
                    ((C1.x == C2.x) && (C1.y == C2.y) && (C1.z == C2.z) &&
                        (C1.w < C2.w)));
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Ray>
    {
        bool operator()(const DirectX::SimpleMath::Ray& R1,
            const DirectX::SimpleMath::Ray& R2) const noexcept
        {
            if (R1.position.x != R2.position.x)
                return R1.position.x < R2.position.x;
            if (R1.position.y != R2.position.y)
                return R1.position.y < R2.position.y;
            if (R1.position.z != R2.position.z)
                return R1.position.z < R2.position.z;

            if (R1.direction.x != R2.direction.x)
                return R1.direction.x < R2.direction.x;
            if (R1.direction.y != R2.direction.y)
                return R1.direction.y < R2.direction.y;
            if (R1.direction.z != R2.direction.z)
                return R1.direction.z < R2.direction.z;

            return false;
        }
    };

    template <>
    struct less<DirectX::SimpleMath::Viewport>
    {
        bool operator()(const DirectX::SimpleMath::Viewport& vp1,
            const DirectX::SimpleMath::Viewport& vp2) const noexcept
        {
            if (vp1.x != vp2.x) return (vp1.x < vp2.x);
            if (vp1.y != vp2.y) return (vp1.y < vp2.y);

            if (vp1.width != vp2.width) return (vp1.width < vp2.width);
            if (vp1.height != vp2.height) return (vp1.height < vp2.height);

            if (vp1.minDepth != vp2.minDepth)
                return (vp1.minDepth < vp2.minDepth);
            if (vp1.maxDepth != vp2.maxDepth)
                return (vp1.maxDepth < vp2.maxDepth);

            return false;
        }
    };

}  // namespace std

#ifdef __clang__
#pragma clang diagnostic pop
#endif

//-------------------------------------------------------------------------------------
// SimpleMath.inl -- Simplified C++ Math wrapper for DirectXMath
//
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
//
// http://go.microsoft.com/fwlink/?LinkId=248929
// http://go.microsoft.com/fwlink/?LinkID=615561
//-------------------------------------------------------------------------------------

namespace DirectX::SimpleMath
{
    /****************************************************************************
     *
     * Rectangle
     *
     ****************************************************************************/

    //------------------------------------------------------------------------------
    // Rectangle operations
    //------------------------------------------------------------------------------
    inline Vector2 Rectangle::Location() const noexcept
    {
        return Vector2(float(x), float(y));
    }

    inline Vector2 Rectangle::Center() const noexcept
    {
        return Vector2(
            float(x) + (float(width) / 2.f), float(y) + (float(height) / 2.f));
    }

    inline bool Rectangle::Contains(const Vector2& point) const noexcept
    {
        return (float(x) <= point.x) && (point.x < float(x + width)) &&
               (float(y) <= point.y) && (point.y < float(y + height));
    }

    inline void Rectangle::Inflate(long horizAmount, long vertAmount) noexcept
    {
        x -= horizAmount;
        y -= vertAmount;
        width += horizAmount;
        height += vertAmount;
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    inline Rectangle Rectangle::Intersect(
        const Rectangle& ra, const Rectangle& rb) noexcept
    {
        const long righta = ra.x + ra.width;
        const long rightb = rb.x + rb.width;

        const long bottoma = ra.y + ra.height;
        const long bottomb = rb.y + rb.height;

        const long maxX = ra.x > rb.x ? ra.x : rb.x;
        const long maxY = ra.y > rb.y ? ra.y : rb.y;

        const long minRight = righta < rightb ? righta : rightb;
        const long minBottom = bottoma < bottomb ? bottoma : bottomb;

        Rectangle result;

        if ((minRight > maxX) && (minBottom > maxY))
        {
            result.x = maxX;
            result.y = maxY;
            result.width = minRight - maxX;
            result.height = minBottom - maxY;
        }
        else
        {
            result.x = 0;
            result.y = 0;
            result.width = 0;
            result.height = 0;
        }

        return result;
    }

    inline RECT Rectangle::Intersect(
        const RECT& rcta, const RECT& rctb) noexcept
    {
        const long maxX = rcta.left > rctb.left ? rcta.left : rctb.left;
        const long maxY = rcta.top > rctb.top ? rcta.top : rctb.top;

        const long minRight = rcta.right < rctb.right ? rcta.right : rctb.right;
        const long minBottom =
            rcta.bottom < rctb.bottom ? rcta.bottom : rctb.bottom;

        RECT result;

        if ((minRight > maxX) && (minBottom > maxY))
        {
            result.left = maxX;
            result.top = maxY;
            result.right = minRight;
            result.bottom = minBottom;
        }
        else
        {
            result.left = 0;
            result.top = 0;
            result.right = 0;
            result.bottom = 0;
        }

        return result;
    }

    inline Rectangle Rectangle::Union(
        const Rectangle& ra, const Rectangle& rb) noexcept
    {
        const long righta = ra.x + ra.width;
        const long rightb = rb.x + rb.width;

        const long bottoma = ra.y + ra.height;
        const long bottomb = rb.y + rb.height;

        const int minX = ra.x < rb.x ? ra.x : rb.x;
        const int minY = ra.y < rb.y ? ra.y : rb.y;

        const int maxRight = righta > rightb ? righta : rightb;
        const int maxBottom = bottoma > bottomb ? bottoma : bottomb;

        Rectangle result;
        result.x = minX;
        result.y = minY;
        result.width = maxRight - minX;
        result.height = maxBottom - minY;
        return result;
    }

    inline RECT Rectangle::Union(const RECT& rcta, const RECT& rctb) noexcept
    {
        RECT result;
        result.left = rcta.left < rctb.left ? rcta.left : rctb.left;
        result.top = rcta.top < rctb.top ? rcta.top : rctb.top;
        result.right = rcta.right > rctb.right ? rcta.right : rctb.right;
        result.bottom = rcta.bottom > rctb.bottom ? rcta.bottom : rctb.bottom;
        return result;
    }

    /****************************************************************************
     *
     * Vector2
     *
     ****************************************************************************/

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------

    inline bool Vector2::operator==(const Vector2& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        return XMVector2Equal(v1, v2);
    }

    inline bool Vector2::operator!=(const Vector2& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        return XMVector2NotEqual(v1, v2);
    }

    //------------------------------------------------------------------------------
    // Assignment operators
    //------------------------------------------------------------------------------

    inline Vector2& Vector2::operator+=(const Vector2& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        const XMVECTOR X = XMVectorAdd(v1, v2);
        XMStoreFloat2(this, X);
        return *this;
    }

    inline Vector2& Vector2::operator-=(const Vector2& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        const XMVECTOR X = XMVectorSubtract(v1, v2);
        XMStoreFloat2(this, X);
        return *this;
    }

    inline Vector2& Vector2::operator*=(const Vector2& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        const XMVECTOR X = XMVectorMultiply(v1, v2);
        XMStoreFloat2(this, X);
        return *this;
    }

    inline Vector2& Vector2::operator*=(float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR X = XMVectorScale(v1, S);
        XMStoreFloat2(this, X);
        return *this;
    }

    inline Vector2& Vector2::operator/=(float S) noexcept
    {
        using namespace DirectX;
        assert(S != 0.0f);
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR X = XMVectorScale(v1, 1.f / S);
        XMStoreFloat2(this, X);
        return *this;
    }

    //------------------------------------------------------------------------------
    // Binary operators
    //------------------------------------------------------------------------------

    inline Vector2 operator+(const Vector2& V1, const Vector2& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&V1);
        const XMVECTOR v2 = XMLoadFloat2(&V2);
        const XMVECTOR X = XMVectorAdd(v1, v2);
        Vector2 R;
        XMStoreFloat2(&R, X);
        return R;
    }

    inline Vector2 operator-(const Vector2& V1, const Vector2& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&V1);
        const XMVECTOR v2 = XMLoadFloat2(&V2);
        const XMVECTOR X = XMVectorSubtract(v1, v2);
        Vector2 R;
        XMStoreFloat2(&R, X);
        return R;
    }

    inline Vector2 operator*(const Vector2& V1, const Vector2& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&V1);
        const XMVECTOR v2 = XMLoadFloat2(&V2);
        const XMVECTOR X = XMVectorMultiply(v1, v2);
        Vector2 R;
        XMStoreFloat2(&R, X);
        return R;
    }

    inline Vector2 operator*(const Vector2& V, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&V);
        const XMVECTOR X = XMVectorScale(v1, S);
        Vector2 R;
        XMStoreFloat2(&R, X);
        return R;
    }

    inline Vector2 operator/(const Vector2& V1, const Vector2& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&V1);
        const XMVECTOR v2 = XMLoadFloat2(&V2);
        const XMVECTOR X = XMVectorDivide(v1, v2);
        Vector2 R;
        XMStoreFloat2(&R, X);
        return R;
    }

    inline Vector2 operator/(const Vector2& V, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&V);
        const XMVECTOR X = XMVectorScale(v1, 1.f / S);
        Vector2 R;
        XMStoreFloat2(&R, X);
        return R;
    }

    inline Vector2 operator*(float S, const Vector2& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&V);
        const XMVECTOR X = XMVectorScale(v1, S);
        Vector2 R;
        XMStoreFloat2(&R, X);
        return R;
    }

    //------------------------------------------------------------------------------
    // Vector operations
    //------------------------------------------------------------------------------

    inline bool Vector2::InBounds(const Vector2& Bounds) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&Bounds);
        return XMVector2InBounds(v1, v2);
    }

    inline float Vector2::Length() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR X = XMVector2Length(v1);
        return XMVectorGetX(X);
    }

    inline float Vector2::LengthSquared() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR X = XMVector2LengthSq(v1);
        return XMVectorGetX(X);
    }

    inline float Vector2::Dot(const Vector2& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        const XMVECTOR X = XMVector2Dot(v1, v2);
        return XMVectorGetX(X);
    }

    inline void Vector2::Cross(const Vector2& V, Vector2& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        const XMVECTOR R = XMVector2Cross(v1, v2);
        XMStoreFloat2(&result, R);
    }

    inline Vector2 Vector2::Cross(const Vector2& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&V);
        const XMVECTOR R = XMVector2Cross(v1, v2);

        Vector2 result;
        XMStoreFloat2(&result, R);
        return result;
    }

    inline void Vector2::Normalize() noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR X = XMVector2Normalize(v1);
        XMStoreFloat2(this, X);
    }

    inline void Vector2::Normalize(Vector2& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR X = XMVector2Normalize(v1);
        XMStoreFloat2(&result, X);
    }

    inline void Vector2::Clamp(
        const Vector2& vmin, const Vector2& vmax) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&vmin);
        const XMVECTOR v3 = XMLoadFloat2(&vmax);
        const XMVECTOR X = XMVectorClamp(v1, v2, v3);
        XMStoreFloat2(this, X);
    }

    inline void Vector2::Clamp(const Vector2& vmin, const Vector2& vmax,
        Vector2& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(this);
        const XMVECTOR v2 = XMLoadFloat2(&vmin);
        const XMVECTOR v3 = XMLoadFloat2(&vmax);
        const XMVECTOR X = XMVectorClamp(v1, v2, v3);
        XMStoreFloat2(&result, X);
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    inline float Vector2::Distance(
        const Vector2& v1, const Vector2& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR V = XMVectorSubtract(x2, x1);
        const XMVECTOR X = XMVector2Length(V);
        return XMVectorGetX(X);
    }

    inline float Vector2::DistanceSquared(
        const Vector2& v1, const Vector2& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR V = XMVectorSubtract(x2, x1);
        const XMVECTOR X = XMVector2LengthSq(V);
        return XMVectorGetX(X);
    }

    inline void Vector2::Min(
        const Vector2& v1, const Vector2& v2, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorMin(x1, x2);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Min(const Vector2& v1, const Vector2& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorMin(x1, x2);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Max(
        const Vector2& v1, const Vector2& v2, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorMax(x1, x2);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Max(const Vector2& v1, const Vector2& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorMax(x1, x2);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Lerp(
        const Vector2& v1, const Vector2& v2, float t, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Lerp(
        const Vector2& v1, const Vector2& v2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::SmoothStep(
        const Vector2& v1, const Vector2& v2, float t, Vector2& result) noexcept
    {
        using namespace DirectX;
        t = (t > 1.0f) ? 1.0f
                       : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t * t * (3.f - 2.f * t);
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::SmoothStep(
        const Vector2& v1, const Vector2& v2, float t) noexcept
    {
        using namespace DirectX;
        t = (t > 1.0f) ? 1.0f
                       : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t * t * (3.f - 2.f * t);
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Barycentric(const Vector2& v1, const Vector2& v2,
        const Vector2& v3, float f, float g, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR x3 = XMLoadFloat2(&v3);
        const XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Barycentric(const Vector2& v1, const Vector2& v2,
        const Vector2& v3, float f, float g) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR x3 = XMLoadFloat2(&v3);
        const XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::CatmullRom(const Vector2& v1, const Vector2& v2,
        const Vector2& v3, const Vector2& v4, float t, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR x3 = XMLoadFloat2(&v3);
        const XMVECTOR x4 = XMLoadFloat2(&v4);
        const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::CatmullRom(const Vector2& v1, const Vector2& v2,
        const Vector2& v3, const Vector2& v4, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&v2);
        const XMVECTOR x3 = XMLoadFloat2(&v3);
        const XMVECTOR x4 = XMLoadFloat2(&v4);
        const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Hermite(const Vector2& v1, const Vector2& t1,
        const Vector2& v2, const Vector2& t2, float t, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&t1);
        const XMVECTOR x3 = XMLoadFloat2(&v2);
        const XMVECTOR x4 = XMLoadFloat2(&t2);
        const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Hermite(const Vector2& v1, const Vector2& t1,
        const Vector2& v2, const Vector2& t2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat2(&v1);
        const XMVECTOR x2 = XMLoadFloat2(&t1);
        const XMVECTOR x3 = XMLoadFloat2(&v2);
        const XMVECTOR x4 = XMLoadFloat2(&t2);
        const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Reflect(
        const Vector2& ivec, const Vector2& nvec, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat2(&ivec);
        const XMVECTOR n = XMLoadFloat2(&nvec);
        const XMVECTOR X = XMVector2Reflect(i, n);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Reflect(
        const Vector2& ivec, const Vector2& nvec) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat2(&ivec);
        const XMVECTOR n = XMLoadFloat2(&nvec);
        const XMVECTOR X = XMVector2Reflect(i, n);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Refract(const Vector2& ivec, const Vector2& nvec,
        float refractionIndex, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat2(&ivec);
        const XMVECTOR n = XMLoadFloat2(&nvec);
        const XMVECTOR X = XMVector2Refract(i, n, refractionIndex);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Refract(const Vector2& ivec, const Vector2& nvec,
        float refractionIndex) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat2(&ivec);
        const XMVECTOR n = XMLoadFloat2(&nvec);
        const XMVECTOR X = XMVector2Refract(i, n, refractionIndex);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Transform(
        const Vector2& v, const Quaternion& quat, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        const XMVECTOR X = XMVector3Rotate(v1, q);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Transform(
        const Vector2& v, const Quaternion& quat) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        const XMVECTOR X = XMVector3Rotate(v1, q);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    inline void Vector2::Transform(
        const Vector2& v, const Matrix& m, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector2TransformCoord(v1, M);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::Transform(
        const Vector2& v, const Matrix& m) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector2TransformCoord(v1, M);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    _Use_decl_annotations_ inline void Vector2::Transform(const Vector2* varray,
        size_t count, const Matrix& m, Vector2* resultArray) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(&m);
        XMVector2TransformCoordStream(
            resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), count, M);
    }

    inline void Vector2::Transform(
        const Vector2& v, const Matrix& m, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector2Transform(v1, M);
        XMStoreFloat4(&result, X);
    }

    _Use_decl_annotations_ inline void Vector2::Transform(const Vector2* varray,
        size_t count, const Matrix& m, Vector4* resultArray) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(&m);
        XMVector2TransformStream(
            resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT2), count, M);
    }

    inline void Vector2::TransformNormal(
        const Vector2& v, const Matrix& m, Vector2& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector2TransformNormal(v1, M);
        XMStoreFloat2(&result, X);
    }

    inline Vector2 Vector2::TransformNormal(
        const Vector2& v, const Matrix& m) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector2TransformNormal(v1, M);

        Vector2 result;
        XMStoreFloat2(&result, X);
        return result;
    }

    _Use_decl_annotations_ inline void Vector2::TransformNormal(
        const Vector2* varray, size_t count, const Matrix& m,
        Vector2* resultArray) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(&m);
        XMVector2TransformNormalStream(
            resultArray, sizeof(XMFLOAT2), varray, sizeof(XMFLOAT2), count, M);
    }

    /****************************************************************************
     *
     * Vector3
     *
     ****************************************************************************/

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------

    inline bool Vector3::operator==(const Vector3& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        return XMVector3Equal(v1, v2);
    }

    inline bool Vector3::operator!=(const Vector3& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        return XMVector3NotEqual(v1, v2);
    }

    //------------------------------------------------------------------------------
    // Assignment operators
    //------------------------------------------------------------------------------

    inline Vector3& Vector3::operator+=(const Vector3& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        const XMVECTOR X = XMVectorAdd(v1, v2);
        XMStoreFloat3(this, X);
        return *this;
    }

    inline Vector3& Vector3::operator-=(const Vector3& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        const XMVECTOR X = XMVectorSubtract(v1, v2);
        XMStoreFloat3(this, X);
        return *this;
    }

    inline Vector3& Vector3::operator*=(const Vector3& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        const XMVECTOR X = XMVectorMultiply(v1, v2);
        XMStoreFloat3(this, X);
        return *this;
    }

    inline Vector3& Vector3::operator*=(float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR X = XMVectorScale(v1, S);
        XMStoreFloat3(this, X);
        return *this;
    }

    inline Vector3& Vector3::operator/=(float S) noexcept
    {
        using namespace DirectX;
        assert(S != 0.0f);
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR X = XMVectorScale(v1, 1.f / S);
        XMStoreFloat3(this, X);
        return *this;
    }

    //------------------------------------------------------------------------------
    // Urnary operators
    //------------------------------------------------------------------------------

    inline Vector3 Vector3::operator-() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR X = XMVectorNegate(v1);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    //------------------------------------------------------------------------------
    // Binary operators
    //------------------------------------------------------------------------------

    inline Vector3 operator+(const Vector3& V1, const Vector3& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&V1);
        const XMVECTOR v2 = XMLoadFloat3(&V2);
        const XMVECTOR X = XMVectorAdd(v1, v2);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    inline Vector3 operator-(const Vector3& V1, const Vector3& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&V1);
        const XMVECTOR v2 = XMLoadFloat3(&V2);
        const XMVECTOR X = XMVectorSubtract(v1, v2);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    inline Vector3 operator*(const Vector3& V1, const Vector3& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&V1);
        const XMVECTOR v2 = XMLoadFloat3(&V2);
        const XMVECTOR X = XMVectorMultiply(v1, v2);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    inline Vector3 operator*(const Vector3& V, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&V);
        const XMVECTOR X = XMVectorScale(v1, S);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    inline Vector3 operator/(const Vector3& V1, const Vector3& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&V1);
        const XMVECTOR v2 = XMLoadFloat3(&V2);
        const XMVECTOR X = XMVectorDivide(v1, v2);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    inline Vector3 operator/(const Vector3& V, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&V);
        const XMVECTOR X = XMVectorScale(v1, 1.f / S);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    inline Vector3 operator*(float S, const Vector3& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&V);
        const XMVECTOR X = XMVectorScale(v1, S);
        Vector3 R;
        XMStoreFloat3(&R, X);
        return R;
    }

    //------------------------------------------------------------------------------
    // Vector operations
    //------------------------------------------------------------------------------

    inline bool Vector3::InBounds(const Vector3& Bounds) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&Bounds);
        return XMVector3InBounds(v1, v2);
    }

    inline float Vector3::Length() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR X = XMVector3Length(v1);
        return XMVectorGetX(X);
    }

    inline float Vector3::LengthSquared() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR X = XMVector3LengthSq(v1);
        return XMVectorGetX(X);
    }

    inline float Vector3::Dot(const Vector3& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        const XMVECTOR X = XMVector3Dot(v1, v2);
        return XMVectorGetX(X);
    }

    inline void Vector3::Cross(const Vector3& V, Vector3& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        const XMVECTOR R = XMVector3Cross(v1, v2);
        XMStoreFloat3(&result, R);
    }

    inline Vector3 Vector3::Cross(const Vector3& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&V);
        const XMVECTOR R = XMVector3Cross(v1, v2);

        Vector3 result;
        XMStoreFloat3(&result, R);
        return result;
    }

    inline void Vector3::Normalize() noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR X = XMVector3Normalize(v1);
        XMStoreFloat3(this, X);
    }

    inline void Vector3::Normalize(Vector3& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR X = XMVector3Normalize(v1);
        XMStoreFloat3(&result, X);
    }

    inline void Vector3::Clamp(
        const Vector3& vmin, const Vector3& vmax) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&vmin);
        const XMVECTOR v3 = XMLoadFloat3(&vmax);
        const XMVECTOR X = XMVectorClamp(v1, v2, v3);
        XMStoreFloat3(this, X);
    }

    inline void Vector3::Clamp(const Vector3& vmin, const Vector3& vmax,
        Vector3& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(this);
        const XMVECTOR v2 = XMLoadFloat3(&vmin);
        const XMVECTOR v3 = XMLoadFloat3(&vmax);
        const XMVECTOR X = XMVectorClamp(v1, v2, v3);
        XMStoreFloat3(&result, X);
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    inline float Vector3::Distance(
        const Vector3& v1, const Vector3& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR V = XMVectorSubtract(x2, x1);
        const XMVECTOR X = XMVector3Length(V);
        return XMVectorGetX(X);
    }

    inline float Vector3::DistanceSquared(
        const Vector3& v1, const Vector3& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR V = XMVectorSubtract(x2, x1);
        const XMVECTOR X = XMVector3LengthSq(V);
        return XMVectorGetX(X);
    }

    inline void Vector3::Min(
        const Vector3& v1, const Vector3& v2, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorMin(x1, x2);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Min(const Vector3& v1, const Vector3& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorMin(x1, x2);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Max(
        const Vector3& v1, const Vector3& v2, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorMax(x1, x2);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Max(const Vector3& v1, const Vector3& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorMax(x1, x2);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Lerp(
        const Vector3& v1, const Vector3& v2, float t, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Lerp(
        const Vector3& v1, const Vector3& v2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::SmoothStep(
        const Vector3& v1, const Vector3& v2, float t, Vector3& result) noexcept
    {
        using namespace DirectX;
        t = (t > 1.0f) ? 1.0f
                       : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t * t * (3.f - 2.f * t);
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::SmoothStep(
        const Vector3& v1, const Vector3& v2, float t) noexcept
    {
        using namespace DirectX;
        t = (t > 1.0f) ? 1.0f
                       : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t * t * (3.f - 2.f * t);
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Barycentric(const Vector3& v1, const Vector3& v2,
        const Vector3& v3, float f, float g, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR x3 = XMLoadFloat3(&v3);
        const XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Barycentric(const Vector3& v1, const Vector3& v2,
        const Vector3& v3, float f, float g) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR x3 = XMLoadFloat3(&v3);
        const XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::CatmullRom(const Vector3& v1, const Vector3& v2,
        const Vector3& v3, const Vector3& v4, float t, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR x3 = XMLoadFloat3(&v3);
        const XMVECTOR x4 = XMLoadFloat3(&v4);
        const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::CatmullRom(const Vector3& v1, const Vector3& v2,
        const Vector3& v3, const Vector3& v4, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&v2);
        const XMVECTOR x3 = XMLoadFloat3(&v3);
        const XMVECTOR x4 = XMLoadFloat3(&v4);
        const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Hermite(const Vector3& v1, const Vector3& t1,
        const Vector3& v2, const Vector3& t2, float t, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&t1);
        const XMVECTOR x3 = XMLoadFloat3(&v2);
        const XMVECTOR x4 = XMLoadFloat3(&t2);
        const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Hermite(const Vector3& v1, const Vector3& t1,
        const Vector3& v2, const Vector3& t2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat3(&v1);
        const XMVECTOR x2 = XMLoadFloat3(&t1);
        const XMVECTOR x3 = XMLoadFloat3(&v2);
        const XMVECTOR x4 = XMLoadFloat3(&t2);
        const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Reflect(
        const Vector3& ivec, const Vector3& nvec, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat3(&ivec);
        const XMVECTOR n = XMLoadFloat3(&nvec);
        const XMVECTOR X = XMVector3Reflect(i, n);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Reflect(
        const Vector3& ivec, const Vector3& nvec) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat3(&ivec);
        const XMVECTOR n = XMLoadFloat3(&nvec);
        const XMVECTOR X = XMVector3Reflect(i, n);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Refract(const Vector3& ivec, const Vector3& nvec,
        float refractionIndex, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat3(&ivec);
        const XMVECTOR n = XMLoadFloat3(&nvec);
        const XMVECTOR X = XMVector3Refract(i, n, refractionIndex);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Refract(const Vector3& ivec, const Vector3& nvec,
        float refractionIndex) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat3(&ivec);
        const XMVECTOR n = XMLoadFloat3(&nvec);
        const XMVECTOR X = XMVector3Refract(i, n, refractionIndex);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Transform(
        const Vector3& v, const Quaternion& quat, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        const XMVECTOR X = XMVector3Rotate(v1, q);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Transform(
        const Vector3& v, const Quaternion& quat) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        const XMVECTOR X = XMVector3Rotate(v1, q);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    inline void Vector3::Transform(
        const Vector3& v, const Matrix& m, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector3TransformCoord(v1, M);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::Transform(
        const Vector3& v, const Matrix& m) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector3TransformCoord(v1, M);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    _Use_decl_annotations_ inline void Vector3::Transform(const Vector3* varray,
        size_t count, const Matrix& m, Vector3* resultArray) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(&m);
        XMVector3TransformCoordStream(
            resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, M);
    }

    inline void Vector3::Transform(
        const Vector3& v, const Matrix& m, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector3Transform(v1, M);
        XMStoreFloat4(&result, X);
    }

    _Use_decl_annotations_ inline void Vector3::Transform(const Vector3* varray,
        size_t count, const Matrix& m, Vector4* resultArray) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(&m);
        XMVector3TransformStream(
            resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT3), count, M);
    }

    inline void Vector3::TransformNormal(
        const Vector3& v, const Matrix& m, Vector3& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector3TransformNormal(v1, M);
        XMStoreFloat3(&result, X);
    }

    inline Vector3 Vector3::TransformNormal(
        const Vector3& v, const Matrix& m) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector3TransformNormal(v1, M);

        Vector3 result;
        XMStoreFloat3(&result, X);
        return result;
    }

    _Use_decl_annotations_ inline void Vector3::TransformNormal(
        const Vector3* varray, size_t count, const Matrix& m,
        Vector3* resultArray) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(&m);
        XMVector3TransformNormalStream(
            resultArray, sizeof(XMFLOAT3), varray, sizeof(XMFLOAT3), count, M);
    }

    /****************************************************************************
     *
     * Vector4
     *
     ****************************************************************************/

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------

    inline bool Vector4::operator==(const Vector4& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&V);
        return XMVector4Equal(v1, v2);
    }

    inline bool Vector4::operator!=(const Vector4& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&V);
        return XMVector4NotEqual(v1, v2);
    }

    //------------------------------------------------------------------------------
    // Assignment operators
    //------------------------------------------------------------------------------

    inline Vector4& Vector4::operator+=(const Vector4& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&V);
        const XMVECTOR X = XMVectorAdd(v1, v2);
        XMStoreFloat4(this, X);
        return *this;
    }

    inline Vector4& Vector4::operator-=(const Vector4& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&V);
        const XMVECTOR X = XMVectorSubtract(v1, v2);
        XMStoreFloat4(this, X);
        return *this;
    }

    inline Vector4& Vector4::operator*=(const Vector4& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&V);
        const XMVECTOR X = XMVectorMultiply(v1, v2);
        XMStoreFloat4(this, X);
        return *this;
    }

    inline Vector4& Vector4::operator*=(float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR X = XMVectorScale(v1, S);
        XMStoreFloat4(this, X);
        return *this;
    }

    inline Vector4& Vector4::operator/=(float S) noexcept
    {
        using namespace DirectX;
        assert(S != 0.0f);
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR X = XMVectorScale(v1, 1.f / S);
        XMStoreFloat4(this, X);
        return *this;
    }

    //------------------------------------------------------------------------------
    // Urnary operators
    //------------------------------------------------------------------------------

    inline Vector4 Vector4::operator-() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR X = XMVectorNegate(v1);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    //------------------------------------------------------------------------------
    // Binary operators
    //------------------------------------------------------------------------------

    inline Vector4 operator+(const Vector4& V1, const Vector4& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&V1);
        const XMVECTOR v2 = XMLoadFloat4(&V2);
        const XMVECTOR X = XMVectorAdd(v1, v2);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    inline Vector4 operator-(const Vector4& V1, const Vector4& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&V1);
        const XMVECTOR v2 = XMLoadFloat4(&V2);
        const XMVECTOR X = XMVectorSubtract(v1, v2);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    inline Vector4 operator*(const Vector4& V1, const Vector4& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&V1);
        const XMVECTOR v2 = XMLoadFloat4(&V2);
        const XMVECTOR X = XMVectorMultiply(v1, v2);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    inline Vector4 operator*(const Vector4& V, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&V);
        const XMVECTOR X = XMVectorScale(v1, S);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    inline Vector4 operator/(const Vector4& V1, const Vector4& V2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&V1);
        const XMVECTOR v2 = XMLoadFloat4(&V2);
        const XMVECTOR X = XMVectorDivide(v1, v2);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    inline Vector4 operator/(const Vector4& V, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&V);
        const XMVECTOR X = XMVectorScale(v1, 1.f / S);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    inline Vector4 operator*(float S, const Vector4& V) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&V);
        const XMVECTOR X = XMVectorScale(v1, S);
        Vector4 R;
        XMStoreFloat4(&R, X);
        return R;
    }

    //------------------------------------------------------------------------------
    // Vector operations
    //------------------------------------------------------------------------------

    inline bool Vector4::InBounds(const Vector4& Bounds) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&Bounds);
        return XMVector4InBounds(v1, v2);
    }

    inline float Vector4::Length() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR X = XMVector4Length(v1);
        return XMVectorGetX(X);
    }

    inline float Vector4::LengthSquared() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR X = XMVector4LengthSq(v1);
        return XMVectorGetX(X);
    }

    inline float Vector4::Dot(const Vector4& V) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&V);
        const XMVECTOR X = XMVector4Dot(v1, v2);
        return XMVectorGetX(X);
    }

    inline void Vector4::Cross(
        const Vector4& v1, const Vector4& v2, Vector4& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(this);
        const XMVECTOR x2 = XMLoadFloat4(&v1);
        const XMVECTOR x3 = XMLoadFloat4(&v2);
        const XMVECTOR R = XMVector4Cross(x1, x2, x3);
        XMStoreFloat4(&result, R);
    }

    inline Vector4 Vector4::Cross(
        const Vector4& v1, const Vector4& v2) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(this);
        const XMVECTOR x2 = XMLoadFloat4(&v1);
        const XMVECTOR x3 = XMLoadFloat4(&v2);
        const XMVECTOR R = XMVector4Cross(x1, x2, x3);

        Vector4 result;
        XMStoreFloat4(&result, R);
        return result;
    }

    inline void Vector4::Normalize() noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR X = XMVector4Normalize(v1);
        XMStoreFloat4(this, X);
    }

    inline void Vector4::Normalize(Vector4& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR X = XMVector4Normalize(v1);
        XMStoreFloat4(&result, X);
    }

    inline void Vector4::Clamp(
        const Vector4& vmin, const Vector4& vmax) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&vmin);
        const XMVECTOR v3 = XMLoadFloat4(&vmax);
        const XMVECTOR X = XMVectorClamp(v1, v2, v3);
        XMStoreFloat4(this, X);
    }

    inline void Vector4::Clamp(const Vector4& vmin, const Vector4& vmax,
        Vector4& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(this);
        const XMVECTOR v2 = XMLoadFloat4(&vmin);
        const XMVECTOR v3 = XMLoadFloat4(&vmax);
        const XMVECTOR X = XMVectorClamp(v1, v2, v3);
        XMStoreFloat4(&result, X);
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    inline float Vector4::Distance(
        const Vector4& v1, const Vector4& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR V = XMVectorSubtract(x2, x1);
        const XMVECTOR X = XMVector4Length(V);
        return XMVectorGetX(X);
    }

    inline float Vector4::DistanceSquared(
        const Vector4& v1, const Vector4& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR V = XMVectorSubtract(x2, x1);
        const XMVECTOR X = XMVector4LengthSq(V);
        return XMVectorGetX(X);
    }

    inline void Vector4::Min(
        const Vector4& v1, const Vector4& v2, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorMin(x1, x2);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Min(const Vector4& v1, const Vector4& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorMin(x1, x2);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Max(
        const Vector4& v1, const Vector4& v2, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorMax(x1, x2);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Max(const Vector4& v1, const Vector4& v2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorMax(x1, x2);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Lerp(
        const Vector4& v1, const Vector4& v2, float t, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Lerp(
        const Vector4& v1, const Vector4& v2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::SmoothStep(
        const Vector4& v1, const Vector4& v2, float t, Vector4& result) noexcept
    {
        using namespace DirectX;
        t = (t > 1.0f) ? 1.0f
                       : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t * t * (3.f - 2.f * t);
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::SmoothStep(
        const Vector4& v1, const Vector4& v2, float t) noexcept
    {
        using namespace DirectX;
        t = (t > 1.0f) ? 1.0f
                       : ((t < 0.0f) ? 0.0f : t);  // Clamp value to 0 to 1
        t = t * t * (3.f - 2.f * t);
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR X = XMVectorLerp(x1, x2, t);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Barycentric(const Vector4& v1, const Vector4& v2,
        const Vector4& v3, float f, float g, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR x3 = XMLoadFloat4(&v3);
        const XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Barycentric(const Vector4& v1, const Vector4& v2,
        const Vector4& v3, float f, float g) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR x3 = XMLoadFloat4(&v3);
        const XMVECTOR X = XMVectorBaryCentric(x1, x2, x3, f, g);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::CatmullRom(const Vector4& v1, const Vector4& v2,
        const Vector4& v3, const Vector4& v4, float t, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR x3 = XMLoadFloat4(&v3);
        const XMVECTOR x4 = XMLoadFloat4(&v4);
        const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::CatmullRom(const Vector4& v1, const Vector4& v2,
        const Vector4& v3, const Vector4& v4, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&v2);
        const XMVECTOR x3 = XMLoadFloat4(&v3);
        const XMVECTOR x4 = XMLoadFloat4(&v4);
        const XMVECTOR X = XMVectorCatmullRom(x1, x2, x3, x4, t);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Hermite(const Vector4& v1, const Vector4& t1,
        const Vector4& v2, const Vector4& t2, float t, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&t1);
        const XMVECTOR x3 = XMLoadFloat4(&v2);
        const XMVECTOR x4 = XMLoadFloat4(&t2);
        const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Hermite(const Vector4& v1, const Vector4& t1,
        const Vector4& v2, const Vector4& t2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 = XMLoadFloat4(&v1);
        const XMVECTOR x2 = XMLoadFloat4(&t1);
        const XMVECTOR x3 = XMLoadFloat4(&v2);
        const XMVECTOR x4 = XMLoadFloat4(&t2);
        const XMVECTOR X = XMVectorHermite(x1, x2, x3, x4, t);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Reflect(
        const Vector4& ivec, const Vector4& nvec, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat4(&ivec);
        const XMVECTOR n = XMLoadFloat4(&nvec);
        const XMVECTOR X = XMVector4Reflect(i, n);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Reflect(
        const Vector4& ivec, const Vector4& nvec) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat4(&ivec);
        const XMVECTOR n = XMLoadFloat4(&nvec);
        const XMVECTOR X = XMVector4Reflect(i, n);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Refract(const Vector4& ivec, const Vector4& nvec,
        float refractionIndex, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat4(&ivec);
        const XMVECTOR n = XMLoadFloat4(&nvec);
        const XMVECTOR X = XMVector4Refract(i, n, refractionIndex);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Refract(const Vector4& ivec, const Vector4& nvec,
        float refractionIndex) noexcept
    {
        using namespace DirectX;
        const XMVECTOR i = XMLoadFloat4(&ivec);
        const XMVECTOR n = XMLoadFloat4(&nvec);
        const XMVECTOR X = XMVector4Refract(i, n, refractionIndex);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Transform(
        const Vector2& v, const Quaternion& quat, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        XMVECTOR X = XMVector3Rotate(v1, q);
        X = XMVectorSelect(
            g_XMIdentityR3, X, g_XMSelect1110);  // result.w = 1.f
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Transform(
        const Vector2& v, const Quaternion& quat) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat2(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        XMVECTOR X = XMVector3Rotate(v1, q);
        X = XMVectorSelect(
            g_XMIdentityR3, X, g_XMSelect1110);  // result.w = 1.f

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Transform(
        const Vector3& v, const Quaternion& quat, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        XMVECTOR X = XMVector3Rotate(v1, q);
        X = XMVectorSelect(
            g_XMIdentityR3, X, g_XMSelect1110);  // result.w = 1.f
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Transform(
        const Vector3& v, const Quaternion& quat) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat3(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        XMVECTOR X = XMVector3Rotate(v1, q);
        X = XMVectorSelect(
            g_XMIdentityR3, X, g_XMSelect1110);  // result.w = 1.f

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Transform(
        const Vector4& v, const Quaternion& quat, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        XMVECTOR X = XMVector3Rotate(v1, q);
        X = XMVectorSelect(v1, X, g_XMSelect1110);  // result.w = v.w
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Transform(
        const Vector4& v, const Quaternion& quat) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&v);
        const XMVECTOR q = XMLoadFloat4(&quat);
        XMVECTOR X = XMVector3Rotate(v1, q);
        X = XMVectorSelect(v1, X, g_XMSelect1110);  // result.w = v.w

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    inline void Vector4::Transform(
        const Vector4& v, const Matrix& m, Vector4& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector4Transform(v1, M);
        XMStoreFloat4(&result, X);
    }

    inline Vector4 Vector4::Transform(
        const Vector4& v, const Matrix& m) noexcept
    {
        using namespace DirectX;
        const XMVECTOR v1 = XMLoadFloat4(&v);
        const XMMATRIX M = XMLoadFloat4x4(&m);
        const XMVECTOR X = XMVector4Transform(v1, M);

        Vector4 result;
        XMStoreFloat4(&result, X);
        return result;
    }

    _Use_decl_annotations_ inline void Vector4::Transform(const Vector4* varray,
        size_t count, const Matrix& m, Vector4* resultArray) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(&m);
        XMVector4TransformStream(
            resultArray, sizeof(XMFLOAT4), varray, sizeof(XMFLOAT4), count, M);
    }

    /****************************************************************************
     *
     * Matrix
     *
     ****************************************************************************/

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------

    inline bool Matrix::operator==(const Matrix& M) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        const XMVECTOR x2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        const XMVECTOR x3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        const XMVECTOR x4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        return (XMVector4Equal(x1, y1) && XMVector4Equal(x2, y2) &&
                   XMVector4Equal(x3, y3) && XMVector4Equal(x4, y4)) != 0;
    }

    inline bool Matrix::operator!=(const Matrix& M) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR x1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        const XMVECTOR x2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        const XMVECTOR x3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        const XMVECTOR x4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        return (XMVector4NotEqual(x1, y1) || XMVector4NotEqual(x2, y2) ||
                   XMVector4NotEqual(x3, y3) || XMVector4NotEqual(x4, y4)) != 0;
    }

    //------------------------------------------------------------------------------
    // Assignment operators
    //------------------------------------------------------------------------------

    inline Matrix::Matrix(const XMFLOAT3X3& M) noexcept
    {
        _11 = M._11;
        _12 = M._12;
        _13 = M._13;
        _14 = 0.f;
        _21 = M._21;
        _22 = M._22;
        _23 = M._23;
        _24 = 0.f;
        _31 = M._31;
        _32 = M._32;
        _33 = M._33;
        _34 = 0.f;
        _41 = 0.f;
        _42 = 0.f;
        _43 = 0.f;
        _44 = 1.f;
    }

    inline Matrix::Matrix(const XMFLOAT4X3& M) noexcept
    {
        _11 = M._11;
        _12 = M._12;
        _13 = M._13;
        _14 = 0.f;
        _21 = M._21;
        _22 = M._22;
        _23 = M._23;
        _24 = 0.f;
        _31 = M._31;
        _32 = M._32;
        _33 = M._33;
        _34 = 0.f;
        _41 = M._41;
        _42 = M._42;
        _43 = M._43;
        _44 = 1.f;
    }

    inline Matrix& Matrix::operator=(const XMFLOAT3X3& M) noexcept
    {
        _11 = M._11;
        _12 = M._12;
        _13 = M._13;
        _14 = 0.f;
        _21 = M._21;
        _22 = M._22;
        _23 = M._23;
        _24 = 0.f;
        _31 = M._31;
        _32 = M._32;
        _33 = M._33;
        _34 = 0.f;
        _41 = 0.f;
        _42 = 0.f;
        _43 = 0.f;
        _44 = 1.f;
        return *this;
    }

    inline Matrix& Matrix::operator=(const XMFLOAT4X3& M) noexcept
    {
        _11 = M._11;
        _12 = M._12;
        _13 = M._13;
        _14 = 0.f;
        _21 = M._21;
        _22 = M._22;
        _23 = M._23;
        _24 = 0.f;
        _31 = M._31;
        _32 = M._32;
        _33 = M._33;
        _34 = 0.f;
        _41 = M._41;
        _42 = M._42;
        _43 = M._43;
        _44 = 1.f;
        return *this;
    }

    inline Matrix& Matrix::operator+=(const Matrix& M) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        x1 = XMVectorAdd(x1, y1);
        x2 = XMVectorAdd(x2, y2);
        x3 = XMVectorAdd(x3, y3);
        x4 = XMVectorAdd(x4, y4);

        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
        return *this;
    }

    inline Matrix& Matrix::operator-=(const Matrix& M) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        x1 = XMVectorSubtract(x1, y1);
        x2 = XMVectorSubtract(x2, y2);
        x3 = XMVectorSubtract(x3, y3);
        x4 = XMVectorSubtract(x4, y4);

        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
        return *this;
    }

    inline Matrix& Matrix::operator*=(const Matrix& M) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M1 = XMLoadFloat4x4(this);
        const XMMATRIX M2 = XMLoadFloat4x4(&M);
        const XMMATRIX X = XMMatrixMultiply(M1, M2);
        XMStoreFloat4x4(this, X);
        return *this;
    }

    inline Matrix& Matrix::operator*=(float S) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        x1 = XMVectorScale(x1, S);
        x2 = XMVectorScale(x2, S);
        x3 = XMVectorScale(x3, S);
        x4 = XMVectorScale(x4, S);

        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
        return *this;
    }

    inline Matrix& Matrix::operator/=(float S) noexcept
    {
        using namespace DirectX;
        assert(S != 0.f);
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        const float rs = 1.f / S;

        x1 = XMVectorScale(x1, rs);
        x2 = XMVectorScale(x2, rs);
        x3 = XMVectorScale(x3, rs);
        x4 = XMVectorScale(x4, rs);

        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
        return *this;
    }

    inline Matrix& Matrix::operator/=(const Matrix& M) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        x1 = XMVectorDivide(x1, y1);
        x2 = XMVectorDivide(x2, y2);
        x3 = XMVectorDivide(x3, y3);
        x4 = XMVectorDivide(x4, y4);

        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&_41), x4);
        return *this;
    }

    //------------------------------------------------------------------------------
    // Urnary operators
    //------------------------------------------------------------------------------

    inline Matrix Matrix::operator-() const noexcept
    {
        using namespace DirectX;
        XMVECTOR v1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_11));
        XMVECTOR v2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_21));
        XMVECTOR v3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_31));
        XMVECTOR v4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&_41));

        v1 = XMVectorNegate(v1);
        v2 = XMVectorNegate(v2);
        v3 = XMVectorNegate(v3);
        v4 = XMVectorNegate(v4);

        Matrix R;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), v1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), v2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), v3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), v4);
        return R;
    }

    //------------------------------------------------------------------------------
    // Binary operators
    //------------------------------------------------------------------------------

    inline Matrix operator+(const Matrix& M1, const Matrix& M2) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

        x1 = XMVectorAdd(x1, y1);
        x2 = XMVectorAdd(x2, y2);
        x3 = XMVectorAdd(x3, y3);
        x4 = XMVectorAdd(x4, y4);

        Matrix R;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
        return R;
    }

    inline Matrix operator-(const Matrix& M1, const Matrix& M2) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

        x1 = XMVectorSubtract(x1, y1);
        x2 = XMVectorSubtract(x2, y2);
        x3 = XMVectorSubtract(x3, y3);
        x4 = XMVectorSubtract(x4, y4);

        Matrix R;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
        return R;
    }

    inline Matrix operator*(const Matrix& M1, const Matrix& M2) noexcept
    {
        using namespace DirectX;
        const XMMATRIX m1 = XMLoadFloat4x4(&M1);
        const XMMATRIX m2 = XMLoadFloat4x4(&M2);
        const XMMATRIX X = XMMatrixMultiply(m1, m2);

        Matrix R;
        XMStoreFloat4x4(&R, X);
        return R;
    }

    inline Matrix operator*(const Matrix& M, float S) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        x1 = XMVectorScale(x1, S);
        x2 = XMVectorScale(x2, S);
        x3 = XMVectorScale(x3, S);
        x4 = XMVectorScale(x4, S);

        Matrix R;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
        return R;
    }

    inline Matrix operator/(const Matrix& M, float S) noexcept
    {
        using namespace DirectX;
        assert(S != 0.f);

        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        const float rs = 1.f / S;

        x1 = XMVectorScale(x1, rs);
        x2 = XMVectorScale(x2, rs);
        x3 = XMVectorScale(x3, rs);
        x4 = XMVectorScale(x4, rs);

        Matrix R;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
        return R;
    }

    inline Matrix operator/(const Matrix& M1, const Matrix& M2) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

        x1 = XMVectorDivide(x1, y1);
        x2 = XMVectorDivide(x2, y2);
        x3 = XMVectorDivide(x3, y3);
        x4 = XMVectorDivide(x4, y4);

        Matrix R;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
        return R;
    }

    inline Matrix operator*(float S, const Matrix& M) noexcept
    {
        using namespace DirectX;

        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M._41));

        x1 = XMVectorScale(x1, S);
        x2 = XMVectorScale(x2, S);
        x3 = XMVectorScale(x3, S);
        x4 = XMVectorScale(x4, S);

        Matrix R;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&R._41), x4);
        return R;
    }

    //------------------------------------------------------------------------------
    // Matrix operations
    //------------------------------------------------------------------------------

    inline bool Matrix::Decompose(
        Vector3& scale, Quaternion& rotation, Vector3& translation) noexcept
    {
        using namespace DirectX;

        XMVECTOR s, r, t;

        if (!XMMatrixDecompose(&s, &r, &t, *this)) return false;

        XMStoreFloat3(&scale, s);
        XMStoreFloat4(&rotation, r);
        XMStoreFloat3(&translation, t);

        return true;
    }

    inline Matrix Matrix::Transpose() const noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(this);
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixTranspose(M));
        return R;
    }

    inline void Matrix::Transpose(Matrix& result) const noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(this);
        XMStoreFloat4x4(&result, XMMatrixTranspose(M));
    }

    inline Matrix Matrix::Invert() const noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(this);
        Matrix R;
        XMVECTOR det;
        XMStoreFloat4x4(&R, XMMatrixInverse(&det, M));
        return R;
    }

    inline void Matrix::Invert(Matrix& result) const noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(this);
        XMVECTOR det;
        XMStoreFloat4x4(&result, XMMatrixInverse(&det, M));
    }

    inline float Matrix::Determinant() const noexcept
    {
        using namespace DirectX;
        const XMMATRIX M = XMLoadFloat4x4(this);
        return XMVectorGetX(XMMatrixDeterminant(M));
    }

    inline Vector3 Matrix::ToEuler() const noexcept
    {
        const float cy = sqrtf(_33 * _33 + _31 * _31);
        const float cx = atan2f(-_32, cy);
        if (cy > 16.f * FLT_EPSILON)
        {
            return Vector3(cx, atan2f(_31, _33), atan2f(_12, _22));
        }
        else
        {
            return Vector3(cx, 0.f, atan2f(-_21, _11));
        }
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    _Use_decl_annotations_ inline Matrix Matrix::CreateBillboard(
        const Vector3& object, const Vector3& cameraPosition,
        const Vector3& cameraUp, const Vector3* cameraForward) noexcept
    {
        using namespace DirectX;
        const XMVECTOR O = XMLoadFloat3(&object);
        const XMVECTOR C = XMLoadFloat3(&cameraPosition);
        XMVECTOR Z = XMVectorSubtract(O, C);

        const XMVECTOR N = XMVector3LengthSq(Z);
        if (XMVector3Less(N, g_XMEpsilon))
        {
            if (cameraForward)
            {
                const XMVECTOR F = XMLoadFloat3(cameraForward);
                Z = XMVectorNegate(F);
            }
            else
                Z = g_XMNegIdentityR2;
        }
        else
        {
            Z = XMVector3Normalize(Z);
        }

        const XMVECTOR up = XMLoadFloat3(&cameraUp);
        XMVECTOR X = XMVector3Cross(up, Z);
        X = XMVector3Normalize(X);

        const XMVECTOR Y = XMVector3Cross(Z, X);

        XMMATRIX M;
        M.r[0] = X;
        M.r[1] = Y;
        M.r[2] = Z;
        M.r[3] = XMVectorSetW(O, 1.f);

        Matrix R;
        XMStoreFloat4x4(&R, M);
        return R;
    }

    _Use_decl_annotations_ inline Matrix Matrix::CreateConstrainedBillboard(
        const Vector3& object, const Vector3& cameraPosition,
        const Vector3& rotateAxis, const Vector3* cameraForward,
        const Vector3* objectForward) noexcept
    {
        using namespace DirectX;

        static const XMVECTORF32 s_minAngle = {
            {{0.99825467075f, 0.99825467075f, 0.99825467075f,
                0.99825467075f}}};  // 1.0 - XMConvertToRadians( 0.1f );

        const XMVECTOR O = XMLoadFloat3(&object);
        const XMVECTOR C = XMLoadFloat3(&cameraPosition);
        XMVECTOR faceDir = XMVectorSubtract(O, C);

        const XMVECTOR N = XMVector3LengthSq(faceDir);
        if (XMVector3Less(N, g_XMEpsilon))
        {
            if (cameraForward)
            {
                const XMVECTOR F = XMLoadFloat3(cameraForward);
                faceDir = XMVectorNegate(F);
            }
            else
                faceDir = g_XMNegIdentityR2;
        }
        else
        {
            faceDir = XMVector3Normalize(faceDir);
        }

        const XMVECTOR Y = XMLoadFloat3(&rotateAxis);
        XMVECTOR X, Z;

        XMVECTOR dot = XMVectorAbs(XMVector3Dot(Y, faceDir));
        if (XMVector3Greater(dot, s_minAngle))
        {
            if (objectForward)
            {
                Z = XMLoadFloat3(objectForward);
                dot = XMVectorAbs(XMVector3Dot(Y, Z));
                if (XMVector3Greater(dot, s_minAngle))
                {
                    dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
                    Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0
                                                            : g_XMNegIdentityR2;
                }
            }
            else
            {
                dot = XMVectorAbs(XMVector3Dot(Y, g_XMNegIdentityR2));
                Z = (XMVector3Greater(dot, s_minAngle)) ? g_XMIdentityR0
                                                        : g_XMNegIdentityR2;
            }

            X = XMVector3Cross(Y, Z);
            X = XMVector3Normalize(X);

            Z = XMVector3Cross(X, Y);
            Z = XMVector3Normalize(Z);
        }
        else
        {
            X = XMVector3Cross(Y, faceDir);
            X = XMVector3Normalize(X);

            Z = XMVector3Cross(X, Y);
            Z = XMVector3Normalize(Z);
        }

        XMMATRIX M;
        M.r[0] = X;
        M.r[1] = Y;
        M.r[2] = Z;
        M.r[3] = XMVectorSetW(O, 1.f);

        Matrix R;
        XMStoreFloat4x4(&R, M);
        return R;
    }

    inline Matrix Matrix::CreateTranslation(const Vector3& position) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(
            &R, XMMatrixTranslation(position.x, position.y, position.z));
        return R;
    }

    inline Matrix Matrix::CreateTranslation(float x, float y, float z) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixTranslation(x, y, z));
        return R;
    }

    inline Matrix Matrix::CreateScale(const Vector3& scales) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixScaling(scales.x, scales.y, scales.z));
        return R;
    }

    inline Matrix Matrix::CreateScale(float xs, float ys, float zs) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixScaling(xs, ys, zs));
        return R;
    }

    inline Matrix Matrix::CreateScale(float scale) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixScaling(scale, scale, scale));
        return R;
    }

    inline Matrix Matrix::CreateRotationX(float radians) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixRotationX(radians));
        return R;
    }

    inline Matrix Matrix::CreateRotationY(float radians) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixRotationY(radians));
        return R;
    }

    inline Matrix Matrix::CreateRotationZ(float radians) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixRotationZ(radians));
        return R;
    }

    inline Matrix Matrix::CreateFromAxisAngle(
        const Vector3& axis, float angle) noexcept
    {
        using namespace DirectX;
        Matrix R;
        const XMVECTOR a = XMLoadFloat3(&axis);
        XMStoreFloat4x4(&R, XMMatrixRotationAxis(a, angle));
        return R;
    }

    inline Matrix Matrix::CreatePerspectiveFieldOfView(
        float fov, float aspectRatio, float nearPlane, float farPlane) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R,
            XMMatrixPerspectiveFovRH(fov, aspectRatio, nearPlane, farPlane));
        return R;
    }

    inline Matrix Matrix::CreatePerspective(
        float width, float height, float nearPlane, float farPlane) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(
            &R, XMMatrixPerspectiveRH(width, height, nearPlane, farPlane));
        return R;
    }

    inline Matrix Matrix::CreatePerspectiveOffCenter(float left, float right,
        float bottom, float top, float nearPlane, float farPlane) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixPerspectiveOffCenterRH(
                                left, right, bottom, top, nearPlane, farPlane));
        return R;
    }

    inline Matrix Matrix::CreateOrthographic(
        float width, float height, float zNearPlane, float zFarPlane) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(
            &R, XMMatrixOrthographicRH(width, height, zNearPlane, zFarPlane));
        return R;
    }

    inline Matrix Matrix::CreateOrthographicOffCenter(float left, float right,
        float bottom, float top, float zNearPlane, float zFarPlane) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixOrthographicOffCenterRH(left, right, bottom,
                                top, zNearPlane, zFarPlane));
        return R;
    }

    inline Matrix Matrix::CreateLookAt(
        const Vector3& eye, const Vector3& target, const Vector3& up) noexcept
    {
        using namespace DirectX;
        Matrix R;
        const XMVECTOR eyev = XMLoadFloat3(&eye);
        const XMVECTOR targetv = XMLoadFloat3(&target);
        const XMVECTOR upv = XMLoadFloat3(&up);
        XMStoreFloat4x4(&R, XMMatrixLookAtRH(eyev, targetv, upv));
        return R;
    }

    inline Matrix Matrix::CreateWorld(const Vector3& position,
        const Vector3& forward, const Vector3& up) noexcept
    {
        using namespace DirectX;
        const XMVECTOR zaxis =
            XMVector3Normalize(XMVectorNegate(XMLoadFloat3(&forward)));
        XMVECTOR yaxis = XMLoadFloat3(&up);
        const XMVECTOR xaxis = XMVector3Normalize(XMVector3Cross(yaxis, zaxis));
        yaxis = XMVector3Cross(zaxis, xaxis);

        Matrix R;
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._11), xaxis);
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._21), yaxis);
        XMStoreFloat3(reinterpret_cast<XMFLOAT3*>(&R._31), zaxis);
        R._14 = R._24 = R._34 = 0.f;
        R._41 = position.x;
        R._42 = position.y;
        R._43 = position.z;
        R._44 = 1.f;
        return R;
    }

    inline Matrix Matrix::CreateFromQuaternion(
        const Quaternion& rotation) noexcept
    {
        using namespace DirectX;
        const XMVECTOR quatv = XMLoadFloat4(&rotation);
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixRotationQuaternion(quatv));
        return R;
    }

    inline Matrix Matrix::CreateFromYawPitchRoll(
        float yaw, float pitch, float roll) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYaw(pitch, yaw, roll));
        return R;
    }

    inline Matrix Matrix::CreateFromYawPitchRoll(const Vector3& angles) noexcept
    {
        using namespace DirectX;
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixRotationRollPitchYawFromVector(angles));
        return R;
    }

    inline Matrix Matrix::CreateShadow(
        const Vector3& lightDir, const Plane& plane) noexcept
    {
        using namespace DirectX;
        const XMVECTOR light = XMLoadFloat3(&lightDir);
        const XMVECTOR planev = XMLoadFloat4(&plane);
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixShadow(planev, light));
        return R;
    }

    inline Matrix Matrix::CreateReflection(const Plane& plane) noexcept
    {
        using namespace DirectX;
        const XMVECTOR planev = XMLoadFloat4(&plane);
        Matrix R;
        XMStoreFloat4x4(&R, XMMatrixReflect(planev));
        return R;
    }

    inline void Matrix::Lerp(
        const Matrix& M1, const Matrix& M2, float t, Matrix& result) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

        x1 = XMVectorLerp(x1, y1, t);
        x2 = XMVectorLerp(x2, y2, t);
        x3 = XMVectorLerp(x3, y3, t);
        x4 = XMVectorLerp(x4, y4, t);

        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
    }

    inline Matrix Matrix::Lerp(
        const Matrix& M1, const Matrix& M2, float t) noexcept
    {
        using namespace DirectX;
        XMVECTOR x1 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._11));
        XMVECTOR x2 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._21));
        XMVECTOR x3 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._31));
        XMVECTOR x4 = XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M1._41));

        const XMVECTOR y1 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._11));
        const XMVECTOR y2 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._21));
        const XMVECTOR y3 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._31));
        const XMVECTOR y4 =
            XMLoadFloat4(reinterpret_cast<const XMFLOAT4*>(&M2._41));

        x1 = XMVectorLerp(x1, y1, t);
        x2 = XMVectorLerp(x2, y2, t);
        x3 = XMVectorLerp(x3, y3, t);
        x4 = XMVectorLerp(x4, y4, t);

        Matrix result;
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._11), x1);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._21), x2);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._31), x3);
        XMStoreFloat4(reinterpret_cast<XMFLOAT4*>(&result._41), x4);
        return result;
    }

    inline void Matrix::Transform(
        const Matrix& M, const Quaternion& rotation, Matrix& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR quatv = XMLoadFloat4(&rotation);

        const XMMATRIX M0 = XMLoadFloat4x4(&M);
        const XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

        XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
    }

    inline Matrix Matrix::Transform(
        const Matrix& M, const Quaternion& rotation) noexcept
    {
        using namespace DirectX;
        const XMVECTOR quatv = XMLoadFloat4(&rotation);

        const XMMATRIX M0 = XMLoadFloat4x4(&M);
        const XMMATRIX M1 = XMMatrixRotationQuaternion(quatv);

        Matrix result;
        XMStoreFloat4x4(&result, XMMatrixMultiply(M0, M1));
        return result;
    }

    /****************************************************************************
     *
     * Plane
     *
     ****************************************************************************/

    inline Plane::Plane(const Vector3& point1, const Vector3& point2,
        const Vector3& point3) noexcept
    {
        using namespace DirectX;
        const XMVECTOR P0 = XMLoadFloat3(&point1);
        const XMVECTOR P1 = XMLoadFloat3(&point2);
        const XMVECTOR P2 = XMLoadFloat3(&point3);
        XMStoreFloat4(this, XMPlaneFromPoints(P0, P1, P2));
    }

    inline Plane::Plane(const Vector3& point, const Vector3& normal) noexcept
    {
        using namespace DirectX;
        const XMVECTOR P = XMLoadFloat3(&point);
        const XMVECTOR N = XMLoadFloat3(&normal);
        XMStoreFloat4(this, XMPlaneFromPointNormal(P, N));
    }

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------

    inline bool Plane::operator==(const Plane& p) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR p1 = XMLoadFloat4(this);
        const XMVECTOR p2 = XMLoadFloat4(&p);
        return XMPlaneEqual(p1, p2);
    }

    inline bool Plane::operator!=(const Plane& p) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR p1 = XMLoadFloat4(this);
        const XMVECTOR p2 = XMLoadFloat4(&p);
        return XMPlaneNotEqual(p1, p2);
    }

    //------------------------------------------------------------------------------
    // Plane operations
    //------------------------------------------------------------------------------

    inline void Plane::Normalize() noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(this);
        XMStoreFloat4(this, XMPlaneNormalize(p));
    }

    inline void Plane::Normalize(Plane& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMPlaneNormalize(p));
    }

    inline float Plane::Dot(const Vector4& v) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(this);
        const XMVECTOR v0 = XMLoadFloat4(&v);
        return XMVectorGetX(XMPlaneDot(p, v0));
    }

    inline float Plane::DotCoordinate(const Vector3& position) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(this);
        const XMVECTOR v0 = XMLoadFloat3(&position);
        return XMVectorGetX(XMPlaneDotCoord(p, v0));
    }

    inline float Plane::DotNormal(const Vector3& normal) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(this);
        const XMVECTOR n0 = XMLoadFloat3(&normal);
        return XMVectorGetX(XMPlaneDotNormal(p, n0));
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    inline void Plane::Transform(
        const Plane& plane, const Matrix& M, Plane& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(&plane);
        const XMMATRIX m0 = XMLoadFloat4x4(&M);
        XMStoreFloat4(&result, XMPlaneTransform(p, m0));
    }

    inline Plane Plane::Transform(const Plane& plane, const Matrix& M) noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(&plane);
        const XMMATRIX m0 = XMLoadFloat4x4(&M);

        Plane result;
        XMStoreFloat4(&result, XMPlaneTransform(p, m0));
        return result;
    }

    inline void Plane::Transform(
        const Plane& plane, const Quaternion& rotation, Plane& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(&plane);
        const XMVECTOR q = XMLoadFloat4(&rotation);
        XMVECTOR X = XMVector3Rotate(p, q);
        X = XMVectorSelect(p, X, g_XMSelect1110);  // result.d = plane.d
        XMStoreFloat4(&result, X);
    }

    inline Plane Plane::Transform(
        const Plane& plane, const Quaternion& rotation) noexcept
    {
        using namespace DirectX;
        const XMVECTOR p = XMLoadFloat4(&plane);
        const XMVECTOR q = XMLoadFloat4(&rotation);
        XMVECTOR X = XMVector3Rotate(p, q);
        X = XMVectorSelect(p, X, g_XMSelect1110);  // result.d = plane.d

        Plane result;
        XMStoreFloat4(&result, X);
        return result;
    }

    /****************************************************************************
     *
     * Quaternion
     *
     ****************************************************************************/

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------

    inline bool Quaternion::operator==(const Quaternion& q) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        return XMQuaternionEqual(q1, q2);
    }

    inline bool Quaternion::operator!=(const Quaternion& q) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        return XMQuaternionNotEqual(q1, q2);
    }

    //------------------------------------------------------------------------------
    // Assignment operators
    //------------------------------------------------------------------------------

    inline Quaternion& Quaternion::operator+=(const Quaternion& q) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        XMStoreFloat4(this, XMVectorAdd(q1, q2));
        return *this;
    }

    inline Quaternion& Quaternion::operator-=(const Quaternion& q) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        XMStoreFloat4(this, XMVectorSubtract(q1, q2));
        return *this;
    }

    inline Quaternion& Quaternion::operator*=(const Quaternion& q) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        XMStoreFloat4(this, XMQuaternionMultiply(q1, q2));
        return *this;
    }

    inline Quaternion& Quaternion::operator*=(float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        XMStoreFloat4(this, XMVectorScale(q, S));
        return *this;
    }

    inline Quaternion& Quaternion::operator/=(const Quaternion& q) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        XMVECTOR q2 = XMLoadFloat4(&q);
        q2 = XMQuaternionInverse(q2);
        XMStoreFloat4(this, XMQuaternionMultiply(q1, q2));
        return *this;
    }

    //------------------------------------------------------------------------------
    // Urnary operators
    //------------------------------------------------------------------------------

    inline Quaternion Quaternion::operator-() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);

        Quaternion R;
        XMStoreFloat4(&R, XMVectorNegate(q));
        return R;
    }

    //------------------------------------------------------------------------------
    // Binary operators
    //------------------------------------------------------------------------------

    inline Quaternion operator+(
        const Quaternion& Q1, const Quaternion& Q2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(&Q1);
        const XMVECTOR q2 = XMLoadFloat4(&Q2);

        Quaternion R;
        XMStoreFloat4(&R, XMVectorAdd(q1, q2));
        return R;
    }

    inline Quaternion operator-(
        const Quaternion& Q1, const Quaternion& Q2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(&Q1);
        const XMVECTOR q2 = XMLoadFloat4(&Q2);

        Quaternion R;
        XMStoreFloat4(&R, XMVectorSubtract(q1, q2));
        return R;
    }

    inline Quaternion operator*(
        const Quaternion& Q1, const Quaternion& Q2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(&Q1);
        const XMVECTOR q2 = XMLoadFloat4(&Q2);

        Quaternion R;
        XMStoreFloat4(&R, XMQuaternionMultiply(q1, q2));
        return R;
    }

    inline Quaternion operator*(const Quaternion& Q, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(&Q);

        Quaternion R;
        XMStoreFloat4(&R, XMVectorScale(q, S));
        return R;
    }

    inline Quaternion operator/(
        const Quaternion& Q1, const Quaternion& Q2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(&Q1);
        XMVECTOR q2 = XMLoadFloat4(&Q2);
        q2 = XMQuaternionInverse(q2);

        Quaternion R;
        XMStoreFloat4(&R, XMQuaternionMultiply(q1, q2));
        return R;
    }

    inline Quaternion operator*(float S, const Quaternion& Q) noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(&Q);

        Quaternion R;
        XMStoreFloat4(&R, XMVectorScale(q1, S));
        return R;
    }

    //------------------------------------------------------------------------------
    // Quaternion operations
    //------------------------------------------------------------------------------

    inline float Quaternion::Length() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        return XMVectorGetX(XMQuaternionLength(q));
    }

    inline float Quaternion::LengthSquared() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        return XMVectorGetX(XMQuaternionLengthSq(q));
    }

    inline void Quaternion::Normalize() noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        XMStoreFloat4(this, XMQuaternionNormalize(q));
    }

    inline void Quaternion::Normalize(Quaternion& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMQuaternionNormalize(q));
    }

    inline void Quaternion::Conjugate() noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        XMStoreFloat4(this, XMQuaternionConjugate(q));
    }

    inline void Quaternion::Conjugate(Quaternion& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMQuaternionConjugate(q));
    }

    inline void Quaternion::Inverse(Quaternion& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMQuaternionInverse(q));
    }

    inline float Quaternion::Dot(const Quaternion& q) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR q1 = XMLoadFloat4(this);
        const XMVECTOR q2 = XMLoadFloat4(&q);
        return XMVectorGetX(XMQuaternionDot(q1, q2));
    }

    inline void Quaternion::RotateTowards(
        const Quaternion& target, float maxAngle) noexcept
    {
        RotateTowards(target, maxAngle, *this);
    }

    inline Vector3 Quaternion::ToEuler() const noexcept
    {
        const float xx = x * x;
        const float yy = y * y;
        const float zz = z * z;

        const float m31 = 2.f * x * z + 2.f * y * w;
        const float m32 = 2.f * y * z - 2.f * x * w;
        const float m33 = 1.f - 2.f * xx - 2.f * yy;

        const float cy = sqrtf(m33 * m33 + m31 * m31);
        const float cx = atan2f(-m32, cy);
        if (cy > 16.f * FLT_EPSILON)
        {
            const float m12 = 2.f * x * y + 2.f * z * w;
            const float m22 = 1.f - 2.f * xx - 2.f * zz;

            return Vector3(cx, atan2f(m31, m33), atan2f(m12, m22));
        }
        else
        {
            const float m11 = 1.f - 2.f * yy - 2.f * zz;
            const float m21 = 2.f * x * y - 2.f * z * w;

            return Vector3(cx, 0.f, atan2f(-m21, m11));
        }
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    inline Quaternion Quaternion::CreateFromAxisAngle(
        const Vector3& axis, float angle) noexcept
    {
        using namespace DirectX;
        const XMVECTOR a = XMLoadFloat3(&axis);

        Quaternion R;
        XMStoreFloat4(&R, XMQuaternionRotationAxis(a, angle));
        return R;
    }

    inline Quaternion Quaternion::CreateFromYawPitchRoll(
        float yaw, float pitch, float roll) noexcept
    {
        using namespace DirectX;
        Quaternion R;
        XMStoreFloat4(&R, XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
        return R;
    }

    inline Quaternion Quaternion::CreateFromYawPitchRoll(
        const Vector3& angles) noexcept
    {
        using namespace DirectX;
        Quaternion R;
        XMStoreFloat4(&R, XMQuaternionRotationRollPitchYawFromVector(angles));
        return R;
    }

    inline Quaternion Quaternion::CreateFromRotationMatrix(
        const Matrix& M) noexcept
    {
        using namespace DirectX;
        const XMMATRIX M0 = XMLoadFloat4x4(&M);

        Quaternion R;
        XMStoreFloat4(&R, XMQuaternionRotationMatrix(M0));
        return R;
    }

    inline void Quaternion::Lerp(const Quaternion& q1, const Quaternion& q2,
        float t, Quaternion& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR Q0 = XMLoadFloat4(&q1);
        const XMVECTOR Q1 = XMLoadFloat4(&q2);

        const XMVECTOR dot = XMVector4Dot(Q0, Q1);

        XMVECTOR R;
        if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
        {
            R = XMVectorLerp(Q0, Q1, t);
        }
        else
        {
            const XMVECTOR tv = XMVectorReplicate(t);
            const XMVECTOR t1v = XMVectorReplicate(1.f - t);
            const XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
            const XMVECTOR X1 = XMVectorMultiply(Q1, tv);
            R = XMVectorSubtract(X0, X1);
        }

        XMStoreFloat4(&result, XMQuaternionNormalize(R));
    }

    inline Quaternion Quaternion::Lerp(
        const Quaternion& q1, const Quaternion& q2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR Q0 = XMLoadFloat4(&q1);
        const XMVECTOR Q1 = XMLoadFloat4(&q2);

        const XMVECTOR dot = XMVector4Dot(Q0, Q1);

        XMVECTOR R;
        if (XMVector4GreaterOrEqual(dot, XMVectorZero()))
        {
            R = XMVectorLerp(Q0, Q1, t);
        }
        else
        {
            const XMVECTOR tv = XMVectorReplicate(t);
            const XMVECTOR t1v = XMVectorReplicate(1.f - t);
            const XMVECTOR X0 = XMVectorMultiply(Q0, t1v);
            const XMVECTOR X1 = XMVectorMultiply(Q1, tv);
            R = XMVectorSubtract(X0, X1);
        }

        Quaternion result;
        XMStoreFloat4(&result, XMQuaternionNormalize(R));
        return result;
    }

    inline void Quaternion::Slerp(const Quaternion& q1, const Quaternion& q2,
        float t, Quaternion& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR Q0 = XMLoadFloat4(&q1);
        const XMVECTOR Q1 = XMLoadFloat4(&q2);
        XMStoreFloat4(&result, XMQuaternionSlerp(Q0, Q1, t));
    }

    inline Quaternion Quaternion::Slerp(
        const Quaternion& q1, const Quaternion& q2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR Q0 = XMLoadFloat4(&q1);
        const XMVECTOR Q1 = XMLoadFloat4(&q2);

        Quaternion result;
        XMStoreFloat4(&result, XMQuaternionSlerp(Q0, Q1, t));
        return result;
    }

    inline void Quaternion::Concatenate(
        const Quaternion& q1, const Quaternion& q2, Quaternion& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR Q0 = XMLoadFloat4(&q1);
        const XMVECTOR Q1 = XMLoadFloat4(&q2);
        XMStoreFloat4(&result, XMQuaternionMultiply(Q1, Q0));
    }

    inline Quaternion Quaternion::Concatenate(
        const Quaternion& q1, const Quaternion& q2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR Q0 = XMLoadFloat4(&q1);
        const XMVECTOR Q1 = XMLoadFloat4(&q2);

        Quaternion result;
        XMStoreFloat4(&result, XMQuaternionMultiply(Q1, Q0));
        return result;
    }

    inline Quaternion Quaternion::FromToRotation(
        const Vector3& fromDir, const Vector3& toDir) noexcept
    {
        Quaternion result;
        FromToRotation(fromDir, toDir, result);
        return result;
    }

    inline Quaternion Quaternion::LookRotation(
        const Vector3& forward, const Vector3& up) noexcept
    {
        Quaternion result;
        LookRotation(forward, up, result);
        return result;
    }

    inline float Quaternion::Angle(
        const Quaternion& q1, const Quaternion& q2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR Q0 = XMLoadFloat4(&q1);
        const XMVECTOR Q1 = XMLoadFloat4(&q2);

        // We can use the conjugate here instead of inverse assuming q1 & q2 are
        // normalized.
        XMVECTOR R = XMQuaternionMultiply(XMQuaternionConjugate(Q0), Q1);

        const float rs = XMVectorGetW(R);
        R = XMVector3Length(R);
        return 2.f * atan2f(XMVectorGetX(R), rs);
    }

    /****************************************************************************
     *
     * Color
     *
     ****************************************************************************/

    inline Color::Color(const DirectX::PackedVector::XMCOLOR& Packed) noexcept
    {
        using namespace DirectX;
        XMStoreFloat4(this, PackedVector::XMLoadColor(&Packed));
    }

    inline Color::Color(const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept
    {
        using namespace DirectX;
        XMStoreFloat4(this, PackedVector::XMLoadUByteN4(&Packed));
    }

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------
    inline bool Color::operator==(const Color& c) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(this);
        const XMVECTOR c2 = XMLoadFloat4(&c);
        return XMColorEqual(c1, c2);
    }

    inline bool Color::operator!=(const Color& c) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(this);
        const XMVECTOR c2 = XMLoadFloat4(&c);
        return XMColorNotEqual(c1, c2);
    }

    //------------------------------------------------------------------------------
    // Assignment operators
    //------------------------------------------------------------------------------

    inline Color& Color::operator=(
        const DirectX::PackedVector::XMCOLOR& Packed) noexcept
    {
        using namespace DirectX;
        XMStoreFloat4(this, PackedVector::XMLoadColor(&Packed));
        return *this;
    }

    inline Color& Color::operator=(
        const DirectX::PackedVector::XMUBYTEN4& Packed) noexcept
    {
        using namespace DirectX;
        XMStoreFloat4(this, PackedVector::XMLoadUByteN4(&Packed));
        return *this;
    }

    inline Color& Color::operator+=(const Color& c) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(this);
        const XMVECTOR c2 = XMLoadFloat4(&c);
        XMStoreFloat4(this, XMVectorAdd(c1, c2));
        return *this;
    }

    inline Color& Color::operator-=(const Color& c) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(this);
        const XMVECTOR c2 = XMLoadFloat4(&c);
        XMStoreFloat4(this, XMVectorSubtract(c1, c2));
        return *this;
    }

    inline Color& Color::operator*=(const Color& c) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(this);
        const XMVECTOR c2 = XMLoadFloat4(&c);
        XMStoreFloat4(this, XMVectorMultiply(c1, c2));
        return *this;
    }

    inline Color& Color::operator*=(float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(this, XMVectorScale(c, S));
        return *this;
    }

    inline Color& Color::operator/=(const Color& c) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(this);
        const XMVECTOR c2 = XMLoadFloat4(&c);
        XMStoreFloat4(this, XMVectorDivide(c1, c2));
        return *this;
    }

    //------------------------------------------------------------------------------
    // Urnary operators
    //------------------------------------------------------------------------------

    inline Color Color::operator-() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        Color R;
        XMStoreFloat4(&R, XMVectorNegate(c));
        return R;
    }

    //------------------------------------------------------------------------------
    // Binary operators
    //------------------------------------------------------------------------------

    inline Color operator+(const Color& C1, const Color& C2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(&C1);
        const XMVECTOR c2 = XMLoadFloat4(&C2);
        Color R;
        XMStoreFloat4(&R, XMVectorAdd(c1, c2));
        return R;
    }

    inline Color operator-(const Color& C1, const Color& C2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(&C1);
        const XMVECTOR c2 = XMLoadFloat4(&C2);
        Color R;
        XMStoreFloat4(&R, XMVectorSubtract(c1, c2));
        return R;
    }

    inline Color operator*(const Color& C1, const Color& C2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(&C1);
        const XMVECTOR c2 = XMLoadFloat4(&C2);
        Color R;
        XMStoreFloat4(&R, XMVectorMultiply(c1, c2));
        return R;
    }

    inline Color operator*(const Color& C, float S) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(&C);
        Color R;
        XMStoreFloat4(&R, XMVectorScale(c, S));
        return R;
    }

    inline Color operator/(const Color& C1, const Color& C2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(&C1);
        const XMVECTOR c2 = XMLoadFloat4(&C2);
        Color R;
        XMStoreFloat4(&R, XMVectorDivide(c1, c2));
        return R;
    }

    inline Color operator*(float S, const Color& C) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c1 = XMLoadFloat4(&C);
        Color R;
        XMStoreFloat4(&R, XMVectorScale(c1, S));
        return R;
    }

    //------------------------------------------------------------------------------
    // Color operations
    //------------------------------------------------------------------------------

    inline DirectX::PackedVector::XMCOLOR Color::BGRA() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR clr = XMLoadFloat4(this);
        PackedVector::XMCOLOR Packed;
        PackedVector::XMStoreColor(&Packed, clr);
        return Packed;
    }

    inline DirectX::PackedVector::XMUBYTEN4 Color::RGBA() const noexcept
    {
        using namespace DirectX;
        const XMVECTOR clr = XMLoadFloat4(this);
        PackedVector::XMUBYTEN4 Packed;
        PackedVector::XMStoreUByteN4(&Packed, clr);
        return Packed;
    }

    inline Vector3 Color::ToVector3() const noexcept
    {
        return Vector3(x, y, z);
    }

    inline Vector4 Color::ToVector4() const noexcept
    {
        return Vector4(x, y, z, w);
    }

    inline void Color::Negate() noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(this, XMColorNegative(c));
    }

    inline void Color::Negate(Color& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMColorNegative(c));
    }

    inline void Color::Saturate() noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(this, XMVectorSaturate(c));
    }

    inline void Color::Saturate(Color& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMVectorSaturate(c));
    }

    inline void Color::Premultiply() noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMVECTOR a = XMVectorSplatW(c);
        a = XMVectorSelect(g_XMIdentityR3, a, g_XMSelect1110);
        XMStoreFloat4(this, XMVectorMultiply(c, a));
    }

    inline void Color::Premultiply(Color& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMVECTOR a = XMVectorSplatW(c);
        a = XMVectorSelect(g_XMIdentityR3, a, g_XMSelect1110);
        XMStoreFloat4(&result, XMVectorMultiply(c, a));
    }

    inline void Color::AdjustSaturation(float sat) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(this, XMColorAdjustSaturation(c, sat));
    }

    inline void Color::AdjustSaturation(float sat, Color& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMColorAdjustSaturation(c, sat));
    }

    inline void Color::AdjustContrast(float contrast) noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(this, XMColorAdjustContrast(c, contrast));
    }

    inline void Color::AdjustContrast(
        float contrast, Color& result) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR c = XMLoadFloat4(this);
        XMStoreFloat4(&result, XMColorAdjustContrast(c, contrast));
    }

    //------------------------------------------------------------------------------
    // Static functions
    //------------------------------------------------------------------------------

    inline void Color::Modulate(
        const Color& c1, const Color& c2, Color& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR C0 = XMLoadFloat4(&c1);
        const XMVECTOR C1 = XMLoadFloat4(&c2);
        XMStoreFloat4(&result, XMColorModulate(C0, C1));
    }

    inline Color Color::Modulate(const Color& c1, const Color& c2) noexcept
    {
        using namespace DirectX;
        const XMVECTOR C0 = XMLoadFloat4(&c1);
        const XMVECTOR C1 = XMLoadFloat4(&c2);

        Color result;
        XMStoreFloat4(&result, XMColorModulate(C0, C1));
        return result;
    }

    inline void Color::Lerp(
        const Color& c1, const Color& c2, float t, Color& result) noexcept
    {
        using namespace DirectX;
        const XMVECTOR C0 = XMLoadFloat4(&c1);
        const XMVECTOR C1 = XMLoadFloat4(&c2);
        XMStoreFloat4(&result, XMVectorLerp(C0, C1, t));
    }

    inline Color Color::Lerp(const Color& c1, const Color& c2, float t) noexcept
    {
        using namespace DirectX;
        const XMVECTOR C0 = XMLoadFloat4(&c1);
        const XMVECTOR C1 = XMLoadFloat4(&c2);

        Color result;
        XMStoreFloat4(&result, XMVectorLerp(C0, C1, t));
        return result;
    }

    /****************************************************************************
     *
     * Ray
     *
     ****************************************************************************/

    //-----------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------
    inline bool Ray::operator==(const Ray& r) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR r1p = XMLoadFloat3(&position);
        const XMVECTOR r2p = XMLoadFloat3(&r.position);
        const XMVECTOR r1d = XMLoadFloat3(&direction);
        const XMVECTOR r2d = XMLoadFloat3(&r.direction);
        return XMVector3Equal(r1p, r2p) && XMVector3Equal(r1d, r2d);
    }

    inline bool Ray::operator!=(const Ray& r) const noexcept
    {
        using namespace DirectX;
        const XMVECTOR r1p = XMLoadFloat3(&position);
        const XMVECTOR r2p = XMLoadFloat3(&r.position);
        const XMVECTOR r1d = XMLoadFloat3(&direction);
        const XMVECTOR r2d = XMLoadFloat3(&r.direction);
        return XMVector3NotEqual(r1p, r2p) && XMVector3NotEqual(r1d, r2d);
    }

    //-----------------------------------------------------------------------------
    // Ray operators
    //------------------------------------------------------------------------------

    inline bool Ray::Intersects(
        const BoundingSphere& sphere, _Out_ float& Dist) const noexcept
    {
        return sphere.Intersects(position, direction, Dist);
    }

    inline bool Ray::Intersects(
        const BoundingBox& box, _Out_ float& Dist) const noexcept
    {
        return box.Intersects(position, direction, Dist);
    }

    inline bool Ray::Intersects(const Vector3& tri0, const Vector3& tri1,
        const Vector3& tri2, _Out_ float& Dist) const noexcept
    {
        return DirectX::TriangleTests::Intersects(
            position, direction, tri0, tri1, tri2, Dist);
    }

    inline bool Ray::Intersects(
        const Plane& plane, _Out_ float& Dist) const noexcept
    {
        using namespace DirectX;

        const XMVECTOR p = XMLoadFloat4(&plane);
        const XMVECTOR dir = XMLoadFloat3(&direction);

        const XMVECTOR nd = XMPlaneDotNormal(p, dir);

        if (XMVector3LessOrEqual(XMVectorAbs(nd), g_RayEpsilon))
        {
            Dist = 0.f;
            return false;
        }
        else
        {
            // t = -(dot(n,origin) + D) / dot(n,dir)
            const XMVECTOR pos = XMLoadFloat3(&position);
            XMVECTOR v = XMPlaneDotNormal(p, pos);
            v = XMVectorAdd(v, XMVectorSplatW(p));
            v = XMVectorDivide(v, nd);
            float dist = -XMVectorGetX(v);
            if (dist < 0)
            {
                Dist = 0.f;
                return false;
            }
            else
            {
                Dist = dist;
                return true;
            }
        }
    }

    /****************************************************************************
     *
     * Viewport
     *
     ****************************************************************************/

    //------------------------------------------------------------------------------
    // Comparision operators
    //------------------------------------------------------------------------------

#if (__cplusplus < 202002L)
    inline bool Viewport::operator==(const Viewport& vp) const noexcept
    {
        return (x == vp.x && y == vp.y && width == vp.width &&
                height == vp.height && minDepth == vp.minDepth &&
                maxDepth == vp.maxDepth);
    }

    inline bool Viewport::operator!=(const Viewport& vp) const noexcept
    {
        return (x != vp.x || y != vp.y || width != vp.width ||
                height != vp.height || minDepth != vp.minDepth ||
                maxDepth != vp.maxDepth);
    }
#endif

    //------------------------------------------------------------------------------
    // Assignment operators
    //------------------------------------------------------------------------------

    inline Viewport& Viewport::operator=(const RECT& rct) noexcept
    {
        x = float(rct.left);
        y = float(rct.top);
        width = float(rct.right - rct.left);
        height = float(rct.bottom - rct.top);
        minDepth = 0.f;
        maxDepth = 1.f;
        return *this;
    }

#if defined(__d3d11_h__) || defined(__d3d11_x_h__)
    inline Viewport& Viewport::operator=(const D3D11_VIEWPORT& vp) noexcept
    {
        x = vp.TopLeftX;
        y = vp.TopLeftY;
        width = vp.Width;
        height = vp.Height;
        minDepth = vp.MinDepth;
        maxDepth = vp.MaxDepth;
        return *this;
    }
#endif

#if defined(__d3d12_h__) || defined(__d3d12_x_h__) || defined(__XBOX_D3D12_X__)
    inline Viewport& Viewport::operator=(const D3D12_VIEWPORT& vp) noexcept
    {
        x = vp.TopLeftX;
        y = vp.TopLeftY;
        width = vp.Width;
        height = vp.Height;
        minDepth = vp.MinDepth;
        maxDepth = vp.MaxDepth;
        return *this;
    }
#endif

    //------------------------------------------------------------------------------
    // Viewport operations
    //------------------------------------------------------------------------------

    inline float Viewport::AspectRatio() const noexcept
    {
        if (width == 0.f || height == 0.f) return 0.f;

        return (width / height);
    }

    inline Vector3 Viewport::Project(const Vector3& p, const Matrix& proj,
        const Matrix& view, const Matrix& world) const noexcept
    {
        using namespace DirectX;
        XMVECTOR v = XMLoadFloat3(&p);
        const XMMATRIX projection = XMLoadFloat4x4(&proj);
        v = XMVector3Project(v, x, y, width, height, minDepth, maxDepth,
            projection, view, world);
        Vector3 result;
        XMStoreFloat3(&result, v);
        return result;
    }

    inline void Viewport::Project(const Vector3& p, const Matrix& proj,
        const Matrix& view, const Matrix& world, Vector3& result) const noexcept
    {
        using namespace DirectX;
        XMVECTOR v = XMLoadFloat3(&p);
        const XMMATRIX projection = XMLoadFloat4x4(&proj);
        v = XMVector3Project(v, x, y, width, height, minDepth, maxDepth,
            projection, view, world);
        XMStoreFloat3(&result, v);
    }

    inline Vector3 Viewport::Unproject(const Vector3& p, const Matrix& proj,
        const Matrix& view, const Matrix& world) const noexcept
    {
        using namespace DirectX;
        XMVECTOR v = XMLoadFloat3(&p);
        const XMMATRIX projection = XMLoadFloat4x4(&proj);
        v = XMVector3Unproject(v, x, y, width, height, minDepth, maxDepth,
            projection, view, world);
        Vector3 result;
        XMStoreFloat3(&result, v);
        return result;
    }

    inline void Viewport::Unproject(const Vector3& p, const Matrix& proj,
        const Matrix& view, const Matrix& world, Vector3& result) const noexcept
    {
        using namespace DirectX;
        XMVECTOR v = XMLoadFloat3(&p);
        const XMMATRIX projection = XMLoadFloat4x4(&proj);
        v = XMVector3Unproject(v, x, y, width, height, minDepth, maxDepth,
            projection, view, world);
        XMStoreFloat3(&result, v);
    }
}  // namespace DirectX::SimpleMath
