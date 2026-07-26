#include <math.h>
#include "glm/ext/matrix_clip_space.hpp"
#include "rtm/impl/matrix_common.h"
#include "rtm/scalard.h"
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

#include "sse/internal.hpp"

#include <DirectXMath.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <move/math/mat4x4.hpp>
#include <move/math/quat.hpp>
#include <move/math/vec2.hpp>
#include <move/math/vec3.hpp>
#include <move/math/vec4.hpp>
#include "simplemath.hpp"

#include <rtm/macros.h>
#include <rtm/math.h>
#include <rtm/matrix4x4d.h>
#include <rtm/matrix4x4f.h>
#include <rtm/quatd.h>
#include <rtm/quatf.h>
#include <rtm/qvvd.h>
#include <rtm/qvvf.h>
#include <rtm/vector4d.h>
#include <rtm/vector4f.h>

#include "intersections.hpp"

#include "vectormath_compat.hpp"

namespace Vectormath
{
    using Vector3 = Vectormath::SSE::Vector3;
    using Vector4 = Vectormath::SSE::Vector4;
    using Quat = Vectormath::SSE::Quat;
    using Matrix4 = Vectormath::SSE::Matrix4;
}  // namespace Vectormath

namespace rtm
{
    namespace camera
    {
        template <typename mat_type = rtm::matrix4x4f,
            typename vec_type = rtm::vector4f>
        RTM_DISABLE_SECURITY_COOKIE_CHECK RTM_FORCE_INLINE constexpr mat_type
        look_at_rh(
            const vec_type& eye, const vec_type& center, const vec_type& up)
        {
            // Largely taken from GLM's implementation
            using namespace rtm;
            const vec_type lookDir(vector_normalize3(vector_sub(eye, center)));
            const vec_type rightDir(
                vector_normalize3(vector_mul(vector_cross3(up, lookDir), 1)));
            const vec_type actualUpDir(vector_cross3(lookDir, rightDir));

            mat_type result = matrix_identity();
            result.x_axis = vector_set_w(rightDir, 0);
            result.y_axis = vector_set_w(actualUpDir, 0);
            result.z_axis = vector_set_w(lookDir, 0);
            result = matrix_transpose(result);

            if constexpr (std::is_same_v<mat_type, matrix4x4d>)
            {
                result.w_axis =
                    vector_set_w(vector_set(-double(vector_dot3(rightDir, eye)),
                                     -double(vector_dot3(actualUpDir, eye)),
                                     -double(vector_dot3(lookDir, eye))),
                        1);
            }
            else
            {
                result.w_axis =
                    vector_set_w(vector_set(-float(vector_dot3(rightDir, eye)),
                                     -float(vector_dot3(actualUpDir, eye)),
                                     -float(vector_dot3(lookDir, eye))),
                        1);
            }
            return result;
        }

        template <typename mat_type = rtm::matrix4x4f,
            typename vec_type = rtm::vector4f>
        RTM_DISABLE_SECURITY_COOKIE_CHECK RTM_FORCE_INLINE constexpr mat_type
        look_at_lh(
            const vec_type& eye, const vec_type& center, const vec_type& up)
        {
            // Largely taken from GLM's implementation
            using namespace rtm;
            const vec_type lookDir(vector_normalize3(vector_sub(center, eye)));
            const vec_type rightDir(
                vector_normalize3(vector_mul(vector_cross3(up, lookDir), 1)));
            const vec_type actualUpDir(vector_cross3(lookDir, rightDir));

            mat_type result = matrix_identity();
            result.x_axis = vector_set_w(rightDir, 0);
            result.y_axis = vector_set_w(actualUpDir, 0);
            result.z_axis = vector_set_w(lookDir, 0);
            result = matrix_transpose(result);

            if constexpr (std::is_same_v<mat_type, matrix4x4d>)
            {
                result.w_axis =
                    vector_set_w(vector_set(-double(vector_dot3(rightDir, eye)),
                                     -double(vector_dot3(actualUpDir, eye)),
                                     -double(vector_dot3(lookDir, eye))),
                        1);
            }
            else
            {
                result.w_axis =
                    vector_set_w(vector_set(-float(vector_dot3(rightDir, eye)),
                                     -float(vector_dot3(actualUpDir, eye)),
                                     -float(vector_dot3(lookDir, eye))),
                        1);
            }
            return result;
        }

        // template <typename mat_type = rtm::matrix4x4f>
        RTM_DISABLE_SECURITY_COOKIE_CHECK
        RTM_FORCE_INLINE rtm::matrix4x4f perspective_fov_rh(
            float fovY, float aspectRatio, float near, float far)
        {
            using mat_type = rtm::matrix4x4f;
            using value_type = float;

            assert(near > 0.f && far > 0.f);
            assert(!scalar_near_equal(fovY, 0.0f, 0.00001f * 2.0f));
            assert(!scalar_near_equal(aspectRatio, 0.0f, 0.00001f));
            assert(!scalar_near_equal(far, near, 0.00001f));

            value_type SinFov;
            value_type CosFov;
            rtm::scalar_sincos(0.5f * fovY, SinFov, CosFov);

            value_type Height = CosFov / SinFov;
            value_type Width = Height / aspectRatio;
            value_type fRange = far / (near - far);

            mat_type result;
            result.x_axis = vector_set(Width, 0.0f, 0.0f, 0.0f);
            result.y_axis = vector_set(0.0f, Height, 0.0f, 0.0f);
            result.z_axis = vector_set(0.0f, 0.0f, fRange, -1.0f);
            result.w_axis = vector_set(0.0f, 0.0f, fRange * near, 0.0f);
            return result;
        }

        RTM_DISABLE_SECURITY_COOKIE_CHECK
        RTM_FORCE_INLINE rtm::matrix4x4d perspective_fov_rh(
            double fovY, double aspectRatio, double near, double far)
        {
            using mat_type = rtm::matrix4x4d;
            using value_type = double;

            assert(near > 0. && far > 0.);
            assert(!scalar_near_equal(fovY, 0.0, 0.00001 * 2.0));
            assert(!scalar_near_equal(aspectRatio, 0.0, 0.00001));
            assert(!scalar_near_equal(far, near, 0.00001));

            value_type SinFov;
            value_type CosFov;
            rtm::scalar_sincos(0.5f * fovY, SinFov, CosFov);

            value_type Height = CosFov / SinFov;
            value_type Width = Height / aspectRatio;
            value_type fRange = far / (near - far);

            mat_type result;
            result.x_axis = vector_set(Width, 0.0, 0.0, 0.0);
            result.y_axis = vector_set(0.0, Height, 0.0, 0.0);
            result.z_axis = vector_set(0.0, 0.0, fRange, -1.0);
            result.w_axis = vector_set(0.0, 0.0, fRange * near, 0.0);
            return result;
        }

        RTM_DISABLE_SECURITY_COOKIE_CHECK
        RTM_FORCE_INLINE rtm::matrix4x4f ortho_rh(
            float width, float height, float near, float far)
        {
            using mat_type = rtm::matrix4x4f;
            using value_type = float;

            assert(!scalar_near_equal(width, 0.0f, 0.00001f));
            assert(!scalar_near_equal(height, 0.0f, 0.00001f));
            assert(!scalar_near_equal(far, near, 0.00001f));

            value_type fRange = 1.0f / (near - far);
            mat_type result;
            result.x_axis = vector_set(2.0f / width, 0.0f, 0.0f, 0.0f);
            result.y_axis = vector_set(0.0f, 2.0f / height, 0.0f, 0.0f);
            result.z_axis = vector_set(0.0f, 0.0f, fRange, 0.0f);
            result.w_axis = vector_set(0.0f, 0.0f, fRange * near, 1.0f);
            return result;
        }

        RTM_DISABLE_SECURITY_COOKIE_CHECK
        RTM_FORCE_INLINE rtm::matrix4x4d ortho_rh(
            double width, double height, double near, double far)
        {
            using mat_type = rtm::matrix4x4d;
            using value_type = double;

            assert(!scalar_near_equal(width, 0.0, 0.00001));
            assert(!scalar_near_equal(height, 0.0, 0.00001));
            assert(!scalar_near_equal(far, near, 0.00001));

            value_type fRange = 1.0f / (near - far);
            mat_type result;
            result.x_axis = vector_set(2.0 / width, 0.0, 0.0, 0.0);
            result.y_axis = vector_set(0.0, 2.0 / height, 0.0, 0.0);
            result.z_axis = vector_set(0.0, 0.0, fRange, 0.0);
            result.w_axis = vector_set(0.0, 0.0, fRange * near, 1.0);
            return result;
        }
    }  // namespace camera
}  // namespace rtm

namespace mathbench
{
    static struct result_t
    {
        result_t()
        {
        }

        ~result_t()
        {
        }

        int intnum;
        float floatnum;

        // SimpleMath
        DirectX::SimpleMath::Vector2 smVec2;
        DirectX::SimpleMath::Vector3 smVec3;
        DirectX::SimpleMath::Vector4 smVec4;
        DirectX::SimpleMath::Matrix smMat4a;
        DirectX::SimpleMath::Matrix smMat4b;
        DirectX::SimpleMath::Matrix smMat4c;

        // glm
        glm::vec2 glmVec2;
        glm::vec3 glmVec3;
        glm::vec4 glmVec4;
        glm::mat4 glmMat4a;
        glm::mat4 glmMat4b;
        glm::mat4 glmMat4c;

        // DirectXMath
        DirectX::XMFLOAT2 dxVec2;
        DirectX::XMFLOAT3 dxVec3;
        DirectX::XMFLOAT4 dxVec4;
        DirectX::XMFLOAT4X4 dxMat4a;
        DirectX::XMFLOAT4X4 dxMat4b;
        DirectX::XMFLOAT4X4 dxMat4c;

        // Sony vectormath
        Vectormath::Vector2 sonyVec2;
        Vectormath::Vector3 sonyVec3;
        Vectormath::Vector4 sonyVec4;
        Vectormath::Matrix4 sonyMat4a;
        Vectormath::Matrix4 sonyMat4b;
        Vectormath::Matrix4 sonyMat4c;

        // move::math (float)
        move::math::float2 mvVec2f;
        move::math::fast_float3 mvVec3f;
        move::math::fast_float4 mvVec4f;
        move::math::float4x4 mvMat4x4fa;
        move::math::float4x4 mvMat4x4fb;
        move::math::float4x4 mvMat4x4fc;

        // move::math (double)
        move::math::double2 mvVec2d;
        move::math::fast_double3 mvVec3d;
        move::math::fast_double4 mvVec4d;
        move::math::double4x4 mvMat4x4da;
        move::math::double4x4 mvMat4x4db;
        move::math::double4x4 mvMat4x4dc;

        // rtm
        rtm::vector4f rtmVec4f;
        rtm::vector4f rtmVec4fa;
        rtm::vector4f rtmVec4fb;
        rtm::vector4d rtmVec4d;
        rtm::vector4d rtmVec4da;
        rtm::vector4d rtmVec4db;

        rtm::matrix3x4f rtmMat3x4f;
        rtm::matrix3x4f rtmMat3x4fa;
        rtm::matrix3x4d rtmMat3x4d;
        rtm::matrix3x4d rtmMat3x4da;

        rtm::matrix4x4f rtmMat4x4f;
        rtm::matrix4x4f rtmMat4x4fa;
        rtm::matrix4x4f rtmMat4x4fb;
        rtm::matrix4x4f rtmMat4x4fc;
        rtm::matrix4x4d rtmMat4x4d;
        rtm::matrix4x4d rtmMat4x4da;
        rtm::matrix4x4d rtmMat4x4db;
        rtm::matrix4x4d rtmMat4x4dc;
        rtm::qvvf rtmQvvf;
        rtm::qvvf rtmQvvfa;
        rtm::qvvf rtmQvvfb;
        rtm::qvvf rtmQvvfc;
        rtm::qvvd rtmQvvd;
        rtm::qvvd rtmQvvda;
        rtm::qvvd rtmQvvdb;
        rtm::qvvd rtmQvvdc;

        // DirectXMath
        DirectX::XMVECTOR dxVecA;
        DirectX::XMVECTOR dxVecB;
        DirectX::XMVECTOR dxVecC;
        DirectX::XMMATRIX dxMatA;
        DirectX::XMMATRIX dxMatB;
        DirectX::XMMATRIX dxMatC;
    } results;

    namespace vectors
    {
        void addition(ankerl::nanobench::Bench& bench)
        {
            bench.run("Store int (reference 'no-op')",
                [&]
                {
                    results.intnum = 0;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.intnum));
                });

            bench.run("SimpleMath::Vector2 addition",
                [&]
                {
                    results.smVec2 = DirectX::SimpleMath::Vector2(1.0f, 2.0f) +
                                     DirectX::SimpleMath::Vector2(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec2));
                });

            bench.run("SimpleMath::Vector3 addition",
                [&]
                {
                    results.smVec3 =
                        DirectX::SimpleMath::Vector3(1.0f, 2.0f, 3.0f) +
                        DirectX::SimpleMath::Vector3(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec3));
                });

            bench.run("SimpleMath::Vector4 addition",
                [&]
                {
                    results.smVec4 =
                        DirectX::SimpleMath::Vector4(1.0f, 2.0f, 3.0f, 4.0f) +
                        DirectX::SimpleMath::Vector4(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec4));
                });

            bench.run("glm::vec2 addition",
                [&]
                {
                    results.glmVec2 =
                        glm::vec2(1.0f, 2.0f) + glm::vec2(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec2));
                });

            bench.run("glm::vec3 addition",
                [&]
                {
                    results.glmVec3 = glm::vec3(1.0f, 2.0f, 3.0f) +
                                      glm::vec3(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec3));
                });

            bench.run("glm::vec4 addition",
                [&]
                {
                    results.glmVec4 = glm::vec4(1.0f, 2.0f, 3.0f, 4.0f) +
                                      glm::vec4(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec4));
                });

            bench.run("DirectX::XMFLOAT2 addition",
                [&]
                {
                    DirectX::XMFLOAT2 lhs(1.0f, 2.0f);
                    DirectX::XMFLOAT2 rhs(3.0f, 4.0f);
                    DirectX::XMVECTOR lhsVec = DirectX::XMLoadFloat2(&lhs);
                    DirectX::XMVECTOR rhsVec = DirectX::XMLoadFloat2(&rhs);
                    DirectX::XMVECTOR result =
                        DirectX::XMVectorAdd(lhsVec, rhsVec);

                    DirectX::XMStoreFloat2(&results.dxVec2, result);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVec2));
                });

            bench.run("DirectX::XMFLOAT3 addition",
                [&]
                {
                    DirectX::XMFLOAT3 lhs(1.0f, 2.0f, 3.0f);
                    DirectX::XMFLOAT3 rhs(3.0f, 4.0f, 5.0f);
                    DirectX::XMVECTOR lhsVec = DirectX::XMLoadFloat3(&lhs);
                    DirectX::XMVECTOR rhsVec = DirectX::XMLoadFloat3(&rhs);
                    DirectX::XMVECTOR result =
                        DirectX::XMVectorAdd(lhsVec, rhsVec);

                    DirectX::XMStoreFloat3(&results.dxVec3, result);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVec3));
                });

            bench.run("DirectX::XMFLOAT4 addition without Loads",
                [&]
                {
                    results.dxVecC = DirectX::XMVectorAdd(
                        DirectX::XMVECTOR{}, DirectX::XMVECTOR{});
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVecC));
                });

            bench.run("Vectormath::Vector2 addition",
                [&]
                {
                    results.sonyVec2 = Vectormath::Vector2(1.0f, 2.0f) +
                                       Vectormath::Vector2(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec2));
                });

            bench.run("Vectormath::Vector3 addition",
                [&]
                {
                    results.sonyVec3 = Vectormath::Vector3(1.0f, 2.0f, 3.0f) +
                                       Vectormath::Vector3(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec3));
                });

            bench.run("Vectormath::Vector4 addition",
                [&]
                {
                    results.sonyVec4 =
                        Vectormath::Vector4(1.0f, 2.0f, 3.0f, 4.0f) +
                        Vectormath::Vector4(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec4));
                });

            bench.run("move::vec2f addition (float)",
                [&]
                {
                    using namespace move::math;
                    results.mvVec2f = vec2f(1.0f, 2.0f) + vec2f(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec2f));
                });

            bench.run("move::vec3f addition (float)",
                [&]
                {
                    using namespace move::math;
                    results.mvVec3f =
                        vec3f(1.0f, 2.0f, 3.0f) + vec3f(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3f));
                });
            bench.run("move::vec4f addition (float)",
                [&]
                {
                    using namespace move::math;
                    results.mvVec4f = vec4f(1.0f, 2.0f, 3.0f, 4.0f) +
                                      vec4f(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4f));
                });

            bench.run("move::vec2d addition (double)",
                [&]
                {
                    using namespace move::math;
                    results.mvVec2d = vec2d(1.0f, 2.0f) + vec2d(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec2d));
                });

            bench.run("move::vec3d addition (double)",
                [&]
                {
                    using namespace move::math;
                    results.mvVec3d =
                        vec3d(1.0f, 2.0f, 3.0f) + vec3d(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3d));
                });
            bench.run("move::vec4d addition (double)",
                [&]
                {
                    using namespace move::math;
                    results.mvVec4d = vec4d(1.0f, 2.0f, 3.0f, 4.0f) +
                                      vec4d(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4d));
                });

            bench.run("rtm::vector4f addition",
                [&]
                {
                    using namespace rtm;
                    results.rtmVec4f =
                        rtm::vector_add(vector_set(1.0f, 2.0f, 3.0f, 4.0f),
                            vector_set(3.0f, 4.0f, 5.0f, 6.0f));

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            bench.run("rtm::vector4d addition",
                [&]
                {
                    using namespace rtm;
                    results.rtmVec4d =
                        rtm::vector_add(vector_set(1.0, 2.0, 3.0, 4.0),
                            vector_set(3.0, 4.0, 5.0, 6.0));

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }

        void complex1(ankerl::nanobench::Bench& bench)
        {
            bench.run("Complex operation 1 with SimpleMath::Vector*",
                [&]
                {
                    DirectX::SimpleMath::Vector2 vec2a(1.0f, 2.0f);
                    DirectX::SimpleMath::Vector2 vec2b(3.0f, 4.0f);
                    DirectX::SimpleMath::Vector3 vec3a(1.0f, 2.0f, 3.0f);
                    DirectX::SimpleMath::Vector3 vec3b(3.0f, 4.0f, 5.0f);
                    DirectX::SimpleMath::Vector4 vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    DirectX::SimpleMath::Vector4 vec4b(3.0f, 4.0f, 5.0f, 6.0f);

                    auto x = vec2a.Dot(vec2b);
                    auto y = vec3a.Cross(vec3b).Dot(vec3b);
                    auto z = vec4a.Dot(vec4b);
                    auto w = (vec4a + vec4b).Dot(vec4b);

                    results.smVec4 = DirectX::SimpleMath::Vector4(x, y, z, w);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec4));
                });

            bench.run("Complex operation 1 with glm::vec*",
                [&]
                {
                    glm::vec2 vec2a(1.0f, 2.0f);
                    glm::vec2 vec2b(3.0f, 4.0f);
                    glm::vec3 vec3a(1.0f, 2.0f, 3.0f);
                    glm::vec3 vec3b(3.0f, 4.0f, 5.0f);
                    glm::vec4 vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    glm::vec4 vec4b(3.0f, 4.0f, 5.0f, 6.0f);

                    auto x = glm::dot(vec2a, vec2b);
                    auto y = glm::dot(glm::cross(vec3a, vec3b), vec3b);
                    auto z = glm::dot(vec4a, vec4b);
                    auto w = glm::dot(vec4a + vec4b, vec4b);

                    results.glmVec4 = glm::vec4(x, y, z, w);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec4));
                });

            bench.run("Complex operation 1 with DXM",
                [&]
                {
                    using namespace DirectX;
                    XMFLOAT2 vec2a(1.0f, 2.0f);
                    XMFLOAT2 vec2b(3.0f, 4.0f);
                    XMFLOAT3 vec3a(1.0f, 2.0f, 3.0f);
                    XMFLOAT3 vec3b(3.0f, 4.0f, 5.0f);
                    XMFLOAT4 vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    XMFLOAT4 vec4b(3.0f, 4.0f, 5.0f, 6.0f);

                    XMVECTOR vec2aVec = XMLoadFloat2(&vec2a);
                    XMVECTOR vec2bVec = XMLoadFloat2(&vec2b);
                    XMVECTOR vec3aVec = XMLoadFloat3(&vec3a);
                    XMVECTOR vec3bVec = XMLoadFloat3(&vec3b);
                    XMVECTOR vec4aVec = XMLoadFloat4(&vec4a);
                    XMVECTOR vec4bVec = XMLoadFloat4(&vec4b);

                    auto x = XMVectorGetX(XMVector2Dot(vec2aVec, vec2bVec));
                    auto y = XMVectorGetX(XMVector3Dot(
                        XMVector3Cross(vec3aVec, vec3bVec), vec3bVec));
                    auto z = XMVectorGetX(XMVector4Dot(vec4aVec, vec4bVec));
                    auto w = XMVectorGetX(XMVector4Dot(
                        XMVectorAdd(vec4aVec, vec4bVec), vec4bVec));

                    XMStoreFloat4(&results.dxVec4, XMVectorSet(x, y, z, w));
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVec4));
                });

            {
                using namespace DirectX;
                XMFLOAT2 vec2a(1.0f, 2.0f);
                XMFLOAT2 vec2b(3.0f, 4.0f);
                XMFLOAT3 vec3a(1.0f, 2.0f, 3.0f);
                XMFLOAT3 vec3b(3.0f, 4.0f, 5.0f);
                XMFLOAT4 vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                XMFLOAT4 vec4b(3.0f, 4.0f, 5.0f, 6.0f);
                bench.run("Complex operation 1 with DXM w/out loads",
                    [&]
                    {
                        XMVECTOR vec2aVec = XMLoadFloat2(&vec2a);
                        XMVECTOR vec2bVec = XMLoadFloat2(&vec2b);
                        XMVECTOR vec3aVec = XMLoadFloat3(&vec3a);
                        XMVECTOR vec3bVec = XMLoadFloat3(&vec3b);
                        XMVECTOR vec4aVec = XMLoadFloat4(&vec4a);
                        XMVECTOR vec4bVec = XMLoadFloat4(&vec4b);

                        auto x = XMVectorGetX(XMVector2Dot(vec2aVec, vec2bVec));
                        auto y = XMVectorGetX(XMVector3Dot(
                            XMVector3Cross(vec3aVec, vec3bVec), vec3bVec));
                        auto z = XMVectorGetX(XMVector4Dot(vec4aVec, vec4bVec));
                        auto w = XMVectorGetX(XMVector4Dot(
                            XMVectorAdd(vec4aVec, vec4bVec), vec4bVec));

                        XMStoreFloat4(&results.dxVec4, XMVectorSet(x, y, z, w));
                        ankerl::nanobench::doNotOptimizeAway(
                            std::as_const(results.dxVec4));
                    });
            }

            bench.run("Complex operation 1 with Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    Vector2 vec2a(1.0f, 2.0f);
                    Vector2 vec2b(3.0f, 4.0f);
                    Vector3 vec3a(1.0f, 2.0f, 3.0f);
                    Vector3 vec3b(3.0f, 4.0f, 5.0f);
                    Vector4 vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    Vector4 vec4b(3.0f, 4.0f, 5.0f, 6.0f);

                    auto x = dot(vec2a, vec2b);
                    auto y = dot(cross(vec3a, vec3b), vec3b);
                    auto z = dot(vec4a, vec4b);
                    auto w = dot(vec4a + vec4b, vec4b);

                    results.sonyVec4 = Vector4(x, y, z, w);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec4));
                });

            bench.run("Complex operation 1 with move::math::* (float)",
                [&]
                {
                    using namespace move::math;
                    vec2f vec2a(1.0f, 2.0f);
                    vec2f vec2b(3.0f, 4.0f);
                    vec3f vec3a(1.0f, 2.0f, 3.0f);
                    vec3f vec3b(3.0f, 4.0f, 5.0f);
                    vec4f vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    vec4f vec4b(3.0f, 4.0f, 5.0f, 6.0f);

                    auto x = vec2f::dot(vec2a, vec2b);
                    auto y = vec3f::dot(vec3f::cross(vec3a, vec3b), vec3b);
                    auto z = vec4f::dot(vec4a, vec4b);
                    auto w = vec4f::dot((vec4a + vec4b), vec4b);

                    results.mvVec4f = move::math::vec4f(x, y, z, w);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4f));
                });

            bench.run("Complex operation 1 with move::math::* (double)",
                [&]
                {
                    using namespace move::math;
                    vec2d vec2a(1.0f, 2.0f);
                    vec2d vec2b(3.0f, 4.0f);
                    vec3d vec3a(1.0f, 2.0f, 3.0f);
                    vec3d vec3b(3.0f, 4.0f, 5.0f);
                    vec4d vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    vec4d vec4b(3.0f, 4.0f, 5.0f, 6.0f);

                    auto x = vec2d::dot(vec2a, vec2b);
                    auto y = vec3d::dot(vec3d::cross(vec3a, vec3b), vec3b);
                    auto z = vec4d::dot(vec4a, vec4b);
                    auto w = vec4d::dot((vec4a + vec4b), vec4b);

                    results.mvVec4d = move::math::vec4d(x, y, z, w);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4d));
                });
        }

        void complex2vec3(ankerl::nanobench::Bench& bench)
        {
            bench.run("Complex operation 2 with SimpleMath::Vector3",
                [&]
                {
                    DirectX::SimpleMath::Vector3 vec3a(1.0f, 2.0f, 3.0f);
                    DirectX::SimpleMath::Vector3 vec3b(3.0f, 4.0f, 5.0f);
                    DirectX::SimpleMath::Vector3 vec3c(5.0f, 6.0f, 7.0f);
                    DirectX::SimpleMath::Vector3 vec3d(7.0f, 8.0f, 9.0f);

                    results.smVec3 =
                        ((vec3a + vec3b) * vec3c - vec3d).Cross(vec3a);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec3));
                });

            bench.run("Complex operation 2 with glm::vec3",
                [&]
                {
                    glm::vec3 vec3a(1.0f, 2.0f, 3.0f);
                    glm::vec3 vec3b(3.0f, 4.0f, 5.0f);
                    glm::vec3 vec3c(5.0f, 6.0f, 7.0f);
                    glm::vec3 vec3d(7.0f, 8.0f, 9.0f);

                    results.glmVec3 =
                        glm::cross((vec3a + vec3b) * vec3c - vec3d, vec3a);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec3));
                });

            bench.run("Complex operation 2 with DXM",
                [&]
                {
                    using namespace DirectX;
                    XMFLOAT3 vec3a(1.0f, 2.0f, 3.0f);
                    XMFLOAT3 vec3b(3.0f, 4.0f, 5.0f);
                    XMFLOAT3 vec3c(5.0f, 6.0f, 7.0f);
                    XMFLOAT3 vec3d(7.0f, 8.0f, 9.0f);

                    XMVECTOR vec3aVec = XMLoadFloat3(&vec3a);
                    XMVECTOR vec3bVec = XMLoadFloat3(&vec3b);
                    XMVECTOR vec3cVec = XMLoadFloat3(&vec3c);
                    XMVECTOR vec3dVec = XMLoadFloat3(&vec3d);

                    XMVECTOR vec3 = XMVector3Cross(
                        XMVectorSubtract(
                            XMVectorMultiply(
                                XMVectorAdd(vec3aVec, vec3bVec), vec3cVec),
                            vec3dVec),
                        vec3aVec);

                    XMStoreFloat3(&results.dxVec3, vec3);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVec3));
                });

            {
                using namespace DirectX;

                XMFLOAT3 vec3a(1.0f, 2.0f, 3.0f);
                XMFLOAT3 vec3b(3.0f, 4.0f, 5.0f);
                XMFLOAT3 vec3c(5.0f, 6.0f, 7.0f);
                XMFLOAT3 vec3d(7.0f, 8.0f, 9.0f);
                XMVECTOR vec3aVec = XMLoadFloat3(&vec3a);
                XMVECTOR vec3bVec = XMLoadFloat3(&vec3b);
                XMVECTOR vec3cVec = XMLoadFloat3(&vec3c);
                XMVECTOR vec3dVec = XMLoadFloat3(&vec3d);
                bench.run("Complex operation 2 with DXM w/out loads",
                    [&]
                    {
                        XMVECTOR vec3 = XMVector3Cross(
                            XMVectorSubtract(
                                XMVectorMultiply(
                                    XMVectorAdd(vec3aVec, vec3bVec), vec3cVec),
                                vec3dVec),
                            vec3aVec);

                        XMStoreFloat3(&results.dxVec3, vec3);
                        ankerl::nanobench::doNotOptimizeAway(
                            std::as_const(results.dxVec3));
                    });
            }

            bench.run("Complex operation 2 with Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    Vector3 vec3a(1.0f, 2.0f, 3.0f);
                    Vector3 vec3b(3.0f, 4.0f, 5.0f);
                    Vector3 vec3c(5.0f, 6.0f, 7.0f);
                    Vector3 vec3d(7.0f, 8.0f, 9.0f);

                    auto stepOne = (vec3a + vec3b);
                    auto stepTwo = vec3c - vec3d;

                    // Vectormath does not have a memberwise multiplication
                    results.sonyVec3 =
                        cross(Vector3(stepOne.getX() * stepTwo.getX(),
                                  stepOne.getY() * stepTwo.getY(),
                                  stepOne.getZ() * stepTwo.getZ()),
                            vec3a);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec3));
                });

            bench.run("Complex operation 2 with move::math (float)",
                [&]
                {
                    using namespace move::math;
                    vec3f vec3a(1.0f, 2.0f, 3.0f);
                    vec3f vec3b(3.0f, 4.0f, 5.0f);
                    vec3f vec3c(5.0f, 6.0f, 7.0f);
                    vec3f vec3d(7.0f, 8.0f, 9.0f);

                    auto stepOne = (vec3a + vec3b);
                    auto stepTwo = vec3c - vec3d;

                    results.mvVec3f = vec3f::cross((stepOne * stepTwo), vec3a);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3f));
                });

            bench.run("Complex operation 2 with move::math (double)",
                [&]
                {
                    using namespace move::math;
                    vec3d vec3a(1.0f, 2.0f, 3.0f);
                    vec3d vec3b(3.0f, 4.0f, 5.0f);
                    vec3d vec3c(5.0f, 6.0f, 7.0f);
                    vec3d vec3d(7.0f, 8.0f, 9.0f);

                    auto stepOne = (vec3a + vec3b);
                    auto stepTwo = vec3c - vec3d;

                    results.mvVec3d = vec3d::cross((stepOne * stepTwo), vec3a);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3d));
                });
        }

        void complex3vec4(ankerl::nanobench::Bench& bench)
        {
            bench.run("Complex operation 3 with SimpleMath::Vector4",
                [&]
                {
                    DirectX::SimpleMath::Vector4 vec3a(1.0f, 2.0f, 3.0f, 4.0f);
                    DirectX::SimpleMath::Vector4 vec3b(3.0f, 4.0f, 5.0f, 6.0f);
                    DirectX::SimpleMath::Vector4 vec3c(5.0f, 6.0f, 7.0f, 8.0f);
                    DirectX::SimpleMath::Vector4 vec3d(7.0f, 8.0f, 9.0f, 10.0f);

                    results.smVec4 = ((vec3a + vec3b) * vec3c - vec3d);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec4));
                });

            bench.run("Complex operation 3 with glm::vec4",
                [&]
                {
                    glm::vec4 vec3a(1.0f, 2.0f, 3.0f, 4.0f);
                    glm::vec4 vec3b(3.0f, 4.0f, 5.0f, 6.0f);
                    glm::vec4 vec3c(5.0f, 6.0f, 7.0f, 8.0f);
                    glm::vec4 vec3d(7.0f, 8.0f, 9.0f, 10.0f);

                    results.glmVec4 = (vec3a + vec3b) * vec3c - vec3d;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec4));
                });

            bench.run("Complex operation 3 with DXM",
                [&]
                {
                    using namespace DirectX;
                    XMFLOAT4 vec3a(1.0f, 2.0f, 3.0f, 4.0f);
                    XMFLOAT4 vec3b(3.0f, 4.0f, 5.0f, 6.0f);
                    XMFLOAT4 vec3c(5.0f, 6.0f, 7.0f, 8.0f);
                    XMFLOAT4 vec3d(7.0f, 8.0f, 9.0f, 10.0f);

                    XMVECTOR vec3aVec = XMLoadFloat4(&vec3a);
                    XMVECTOR vec3bVec = XMLoadFloat4(&vec3b);
                    XMVECTOR vec3cVec = XMLoadFloat4(&vec3c);
                    XMVECTOR vec3dVec = XMLoadFloat4(&vec3d);

                    XMVECTOR vec3 = XMVectorSubtract(
                        XMVectorMultiply(
                            XMVectorAdd(vec3aVec, vec3bVec), vec3cVec),
                        vec3dVec);

                    XMStoreFloat4(&results.dxVec4, vec3);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVec4));
                });

            {
                using namespace DirectX;

                XMFLOAT4 vec3a(1.0f, 2.0f, 3.0f, 4.0f);
                XMFLOAT4 vec3b(3.0f, 4.0f, 5.0f, 6.0f);
                XMFLOAT4 vec3c(5.0f, 6.0f, 7.0f, 8.0f);
                XMFLOAT4 vec3d(7.0f, 8.0f, 9.0f, 10.0f);
                XMVECTOR vec3aVec = XMLoadFloat4(&vec3a);
                XMVECTOR vec3bVec = XMLoadFloat4(&vec3b);
                XMVECTOR vec3cVec = XMLoadFloat4(&vec3c);
                XMVECTOR vec3dVec = XMLoadFloat4(&vec3d);
                bench.run("Complex operation 3 with DXM w/out loads",
                    [&]
                    {
                        XMVECTOR vec3 = XMVectorSubtract(
                            XMVectorMultiply(
                                XMVectorAdd(vec3aVec, vec3bVec), vec3cVec),
                            vec3dVec);

                        XMStoreFloat4(&results.dxVec4, vec3);
                        ankerl::nanobench::doNotOptimizeAway(
                            std::as_const(results.dxVec4));
                    });
            }

            bench.run("Complex operation 3 with Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    Vector4 vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    Vector4 vec4b(3.0f, 4.0f, 5.0f, 6.0f);
                    Vector4 vec4c(5.0f, 6.0f, 7.0f, 8.0f);
                    Vector4 vec4d(7.0f, 8.0f, 9.0f, 10.0f);

                    Vector4 stepOne = (vec4a + vec4b);
                    Vector4 stepTwo = Vector4(stepOne.getX() * vec4c.getX(),
                        stepOne.getY() * vec4c.getY(),
                        stepOne.getZ() * vec4c.getZ(),
                        stepOne.getW() * vec4c.getW());

                    results.sonyVec4 = stepTwo - vec4d;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec4));
                });

            bench.run("Complex operation 3 with move::math (float)",
                [&]
                {
                    using namespace move::math;
                    vec4f vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    vec4f vec4b(3.0f, 4.0f, 5.0f, 6.0f);
                    vec4f vec4c(5.0f, 6.0f, 7.0f, 8.0f);
                    vec4f vec4dd(7.0f, 8.0f, 9.0f, 10.0f);

                    results.mvVec4f = (vec4a + vec4b) * vec4c - vec4dd;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4f));
                });

            bench.run("Complex operation 3 with move::math (double)",
                [&]
                {
                    using namespace move::math;
                    vec4d vec4a(1.0f, 2.0f, 3.0f, 4.0f);
                    vec4d vec4b(3.0f, 4.0f, 5.0f, 6.0f);
                    vec4d vec4c(5.0f, 6.0f, 7.0f, 8.0f);
                    vec4d vec4dd(7.0f, 8.0f, 9.0f, 10.0f);

                    results.mvVec4d = (vec4a + vec4b) * vec4c - vec4dd;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4d));
                });

            bench.run("Complex operation 3 with rtm::vector4f",
                [&]
                {
                    using namespace rtm;
                    vector4f vec4a = vector_set(1.0f, 2.0f, 3.0f, 4.0f);
                    vector4f vec4b = vector_set(3.0f, 4.0f, 5.0f, 6.0f);
                    vector4f vec4c = vector_set(5.0f, 6.0f, 7.0f, 8.0f);
                    vector4f vec4d = vector_set(7.0f, 8.0f, 9.0f, 10.0f);

                    vector4f stepOne =
                        vector_mul(vector_add(vec4a, vec4b), vec4c);
                    vector4f stepTwo = vector_sub(stepOne, vec4d);

                    results.rtmVec4f = stepTwo;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            bench.run("Complex operation 3 with rtm::vector4d",
                [&]
                {
                    using namespace rtm;
                    vector4d vec4a = vector_set(1.0, 2.0, 3.0, 4.0);
                    vector4d vec4b = vector_set(3.0, 4.0, 5.0, 6.0);
                    vector4d vec4c = vector_set(5.0, 6.0, 7.0, 8.0);
                    vector4d vec4d = vector_set(7.0, 8.0, 9.0, 10.0);

                    vector4d stepOne =
                        vector_mul(vector_add(vec4a, vec4b), vec4c);
                    vector4d stepTwo = vector_sub(stepOne, vec4d);

                    results.rtmVec4d = stepTwo;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }
    }  // namespace vectors

    namespace workloads
    {
        template <typename Ops>
        void benchmark_ray_aabb_case(ankerl::nanobench::Bench& bench,
            const char* name, typename Ops::vector origin,
            typename Ops::vector direction, typename Ops::vector boundsMin,
            typename Ops::vector boundsMax)
        {
            bench.run(name,
                [=]() mutable
                {
                    ankerl::nanobench::doNotOptimizeAway(origin);
                    ankerl::nanobench::doNotOptimizeAway(direction);
                    ankerl::nanobench::doNotOptimizeAway(boundsMin);
                    ankerl::nanobench::doNotOptimizeAway(boundsMax);
                    auto result = geometry::intersect_ray_aabb<Ops>(
                        origin, direction, boundsMin, boundsMax);
                    ankerl::nanobench::doNotOptimizeAway(result);
                });
        }

        template <typename Ops>
        void benchmark_ray_triangle_case(ankerl::nanobench::Bench& bench,
            const char* name, typename Ops::vector origin,
            typename Ops::vector direction, typename Ops::vector vertex0,
            typename Ops::vector vertex1, typename Ops::vector vertex2)
        {
            bench.run(name,
                [=]() mutable
                {
                    ankerl::nanobench::doNotOptimizeAway(origin);
                    ankerl::nanobench::doNotOptimizeAway(direction);
                    ankerl::nanobench::doNotOptimizeAway(vertex0);
                    ankerl::nanobench::doNotOptimizeAway(vertex1);
                    ankerl::nanobench::doNotOptimizeAway(vertex2);
                    auto result = geometry::intersect_ray_triangle<Ops>(
                        origin, direction, vertex0, vertex1, vertex2);
                    ankerl::nanobench::doNotOptimizeAway(result);
                });
        }

        void ray_aabb_intersection(ankerl::nanobench::Bench& bench)
        {
            using sm_ops = geometry::simplemath_ops;
            using sm_vec = sm_ops::vector;
            benchmark_ray_aabb_case<sm_ops>(bench, "Ray-AABB hit SimpleMath",
                sm_vec(0.25f, 0.1f, -4.0f), sm_vec(0.1f, 0.05f, 1.0f),
                sm_vec(-1.0f, -1.0f, -1.0f), sm_vec(1.0f, 1.0f, 1.0f));
            benchmark_ray_aabb_case<sm_ops>(bench, "Ray-AABB miss SimpleMath",
                sm_vec(4.0f, 4.0f, -4.0f), sm_vec(0.1f, 0.05f, 1.0f),
                sm_vec(-1.0f, -1.0f, -1.0f), sm_vec(1.0f, 1.0f, 1.0f));

            using glm_ops = geometry::glm_ops;
            using glm_vec = glm_ops::vector;
            benchmark_ray_aabb_case<glm_ops>(bench, "Ray-AABB hit glm",
                glm_vec(0.25f, 0.1f, -4.0f), glm_vec(0.1f, 0.05f, 1.0f),
                glm_vec(-1.0f), glm_vec(1.0f));
            benchmark_ray_aabb_case<glm_ops>(bench, "Ray-AABB miss glm",
                glm_vec(4.0f, 4.0f, -4.0f), glm_vec(0.1f, 0.05f, 1.0f),
                glm_vec(-1.0f), glm_vec(1.0f));

            using dx_ops = geometry::directxmath_ops;
            benchmark_ray_aabb_case<dx_ops>(bench, "Ray-AABB hit DXM",
                DirectX::XMVectorSet(0.25f, 0.1f, -4.0f, 0.0f),
                DirectX::XMVectorSet(0.1f, 0.05f, 1.0f, 0.0f),
                DirectX::XMVectorReplicate(-1.0f),
                DirectX::XMVectorReplicate(1.0f));
            benchmark_ray_aabb_case<dx_ops>(bench, "Ray-AABB miss DXM",
                DirectX::XMVectorSet(4.0f, 4.0f, -4.0f, 0.0f),
                DirectX::XMVectorSet(0.1f, 0.05f, 1.0f, 0.0f),
                DirectX::XMVectorReplicate(-1.0f),
                DirectX::XMVectorReplicate(1.0f));

            using sony_ops = geometry::vectormath_ops;
            using sony_vec = sony_ops::vector;
            benchmark_ray_aabb_case<sony_ops>(bench, "Ray-AABB hit Vectormath",
                sony_vec(0.25f, 0.1f, -4.0f), sony_vec(0.1f, 0.05f, 1.0f),
                sony_vec(-1.0f), sony_vec(1.0f));
            benchmark_ray_aabb_case<sony_ops>(bench, "Ray-AABB miss Vectormath",
                sony_vec(4.0f, 4.0f, -4.0f), sony_vec(0.1f, 0.05f, 1.0f),
                sony_vec(-1.0f), sony_vec(1.0f));

            using mvf_ops = geometry::move_ops<float>;
            using mvf_vec = mvf_ops::vector;
            benchmark_ray_aabb_case<mvf_ops>(bench, "Ray-AABB hit move::float3",
                mvf_vec(0.25f, 0.1f, -4.0f), mvf_vec(0.1f, 0.05f, 1.0f),
                mvf_vec(-1.0f, -1.0f, -1.0f), mvf_vec(1.0f, 1.0f, 1.0f));
            benchmark_ray_aabb_case<mvf_ops>(bench,
                "Ray-AABB miss move::float3", mvf_vec(4.0f, 4.0f, -4.0f),
                mvf_vec(0.1f, 0.05f, 1.0f), mvf_vec(-1.0f, -1.0f, -1.0f),
                mvf_vec(1.0f, 1.0f, 1.0f));

            using mvd_ops = geometry::move_ops<double>;
            using mvd_vec = mvd_ops::vector;
            benchmark_ray_aabb_case<mvd_ops>(bench,
                "Ray-AABB hit move::double3", mvd_vec(0.25, 0.1, -4.0),
                mvd_vec(0.1, 0.05, 1.0), mvd_vec(-1.0, -1.0, -1.0),
                mvd_vec(1.0, 1.0, 1.0));
            benchmark_ray_aabb_case<mvd_ops>(bench,
                "Ray-AABB miss move::double3", mvd_vec(4.0, 4.0, -4.0),
                mvd_vec(0.1, 0.05, 1.0), mvd_vec(-1.0, -1.0, -1.0),
                mvd_vec(1.0, 1.0, 1.0));

            using rtmf_ops = geometry::rtm_ops<float>;
            benchmark_ray_aabb_case<rtmf_ops>(bench,
                "Ray-AABB hit rtm::vector4f",
                rtm::vector_set(0.25f, 0.1f, -4.0f, 0.0f),
                rtm::vector_set(0.1f, 0.05f, 1.0f, 0.0f),
                rtm::vector_set(-1.0f), rtm::vector_set(1.0f));
            benchmark_ray_aabb_case<rtmf_ops>(bench,
                "Ray-AABB miss rtm::vector4f",
                rtm::vector_set(4.0f, 4.0f, -4.0f, 0.0f),
                rtm::vector_set(0.1f, 0.05f, 1.0f, 0.0f),
                rtm::vector_set(-1.0f), rtm::vector_set(1.0f));

            using rtmd_ops = geometry::rtm_ops<double>;
            benchmark_ray_aabb_case<rtmd_ops>(bench,
                "Ray-AABB hit rtm::vector4d",
                rtm::vector_set(0.25, 0.1, -4.0, 0.0),
                rtm::vector_set(0.1, 0.05, 1.0, 0.0), rtm::vector_set(-1.0),
                rtm::vector_set(1.0));
            benchmark_ray_aabb_case<rtmd_ops>(bench,
                "Ray-AABB miss rtm::vector4d",
                rtm::vector_set(4.0, 4.0, -4.0, 0.0),
                rtm::vector_set(0.1, 0.05, 1.0, 0.0), rtm::vector_set(-1.0),
                rtm::vector_set(1.0));
        }

        void ray_triangle_intersection(ankerl::nanobench::Bench& bench)
        {
            using sm_ops = geometry::simplemath_ops;
            using sm_vec = sm_ops::vector;
            benchmark_ray_triangle_case<sm_ops>(bench,
                "Ray-triangle hit SimpleMath", sm_vec(0.25f, 0.25f, -2.0f),
                sm_vec(0.0f, 0.0f, 1.0f), sm_vec(0.0f, 0.0f, 0.0f),
                sm_vec(1.0f, 0.0f, 0.0f), sm_vec(0.0f, 1.0f, 0.0f));
            benchmark_ray_triangle_case<sm_ops>(bench,
                "Ray-triangle miss SimpleMath", sm_vec(1.5f, 1.5f, -2.0f),
                sm_vec(0.0f, 0.0f, 1.0f), sm_vec(0.0f, 0.0f, 0.0f),
                sm_vec(1.0f, 0.0f, 0.0f), sm_vec(0.0f, 1.0f, 0.0f));

            using glm_ops = geometry::glm_ops;
            using glm_vec = glm_ops::vector;
            benchmark_ray_triangle_case<glm_ops>(bench, "Ray-triangle hit glm",
                glm_vec(0.25f, 0.25f, -2.0f), glm_vec(0.0f, 0.0f, 1.0f),
                glm_vec(0.0f), glm_vec(1.0f, 0.0f, 0.0f),
                glm_vec(0.0f, 1.0f, 0.0f));
            benchmark_ray_triangle_case<glm_ops>(bench, "Ray-triangle miss glm",
                glm_vec(1.5f, 1.5f, -2.0f), glm_vec(0.0f, 0.0f, 1.0f),
                glm_vec(0.0f), glm_vec(1.0f, 0.0f, 0.0f),
                glm_vec(0.0f, 1.0f, 0.0f));

            using dx_ops = geometry::directxmath_ops;
            benchmark_ray_triangle_case<dx_ops>(bench, "Ray-triangle hit DXM",
                DirectX::XMVectorSet(0.25f, 0.25f, -2.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
                DirectX::XMVectorZero(),
                DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
            benchmark_ray_triangle_case<dx_ops>(bench, "Ray-triangle miss DXM",
                DirectX::XMVectorSet(1.5f, 1.5f, -2.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
                DirectX::XMVectorZero(),
                DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f),
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

            using sony_ops = geometry::vectormath_ops;
            using sony_vec = sony_ops::vector;
            benchmark_ray_triangle_case<sony_ops>(bench,
                "Ray-triangle hit Vectormath", sony_vec(0.25f, 0.25f, -2.0f),
                sony_vec(0.0f, 0.0f, 1.0f), sony_vec(0.0f),
                sony_vec(1.0f, 0.0f, 0.0f), sony_vec(0.0f, 1.0f, 0.0f));
            benchmark_ray_triangle_case<sony_ops>(bench,
                "Ray-triangle miss Vectormath", sony_vec(1.5f, 1.5f, -2.0f),
                sony_vec(0.0f, 0.0f, 1.0f), sony_vec(0.0f),
                sony_vec(1.0f, 0.0f, 0.0f), sony_vec(0.0f, 1.0f, 0.0f));

            using mvf_ops = geometry::move_ops<float>;
            using mvf_vec = mvf_ops::vector;
            benchmark_ray_triangle_case<mvf_ops>(bench,
                "Ray-triangle hit move::float3", mvf_vec(0.25f, 0.25f, -2.0f),
                mvf_vec(0.0f, 0.0f, 1.0f), mvf_vec(0.0f, 0.0f, 0.0f),
                mvf_vec(1.0f, 0.0f, 0.0f), mvf_vec(0.0f, 1.0f, 0.0f));
            benchmark_ray_triangle_case<mvf_ops>(bench,
                "Ray-triangle miss move::float3", mvf_vec(1.5f, 1.5f, -2.0f),
                mvf_vec(0.0f, 0.0f, 1.0f), mvf_vec(0.0f, 0.0f, 0.0f),
                mvf_vec(1.0f, 0.0f, 0.0f), mvf_vec(0.0f, 1.0f, 0.0f));

            using mvd_ops = geometry::move_ops<double>;
            using mvd_vec = mvd_ops::vector;
            benchmark_ray_triangle_case<mvd_ops>(bench,
                "Ray-triangle hit move::double3", mvd_vec(0.25, 0.25, -2.0),
                mvd_vec(0.0, 0.0, 1.0), mvd_vec(0.0, 0.0, 0.0),
                mvd_vec(1.0, 0.0, 0.0), mvd_vec(0.0, 1.0, 0.0));
            benchmark_ray_triangle_case<mvd_ops>(bench,
                "Ray-triangle miss move::double3", mvd_vec(1.5, 1.5, -2.0),
                mvd_vec(0.0, 0.0, 1.0), mvd_vec(0.0, 0.0, 0.0),
                mvd_vec(1.0, 0.0, 0.0), mvd_vec(0.0, 1.0, 0.0));

            using rtmf_ops = geometry::rtm_ops<float>;
            benchmark_ray_triangle_case<rtmf_ops>(bench,
                "Ray-triangle hit rtm::vector4f",
                rtm::vector_set(0.25f, 0.25f, -2.0f, 0.0f),
                rtm::vector_set(0.0f, 0.0f, 1.0f, 0.0f), rtm::vector_zero(),
                rtm::vector_set(1.0f, 0.0f, 0.0f, 0.0f),
                rtm::vector_set(0.0f, 1.0f, 0.0f, 0.0f));
            benchmark_ray_triangle_case<rtmf_ops>(bench,
                "Ray-triangle miss rtm::vector4f",
                rtm::vector_set(1.5f, 1.5f, -2.0f, 0.0f),
                rtm::vector_set(0.0f, 0.0f, 1.0f, 0.0f), rtm::vector_zero(),
                rtm::vector_set(1.0f, 0.0f, 0.0f, 0.0f),
                rtm::vector_set(0.0f, 1.0f, 0.0f, 0.0f));

            using rtmd_ops = geometry::rtm_ops<double>;
            benchmark_ray_triangle_case<rtmd_ops>(bench,
                "Ray-triangle hit rtm::vector4d",
                rtm::vector_set(0.25, 0.25, -2.0, 0.0),
                rtm::vector_set(0.0, 0.0, 1.0, 0.0), rtm::vector_zero(),
                rtm::vector_set(1.0, 0.0, 0.0, 0.0),
                rtm::vector_set(0.0, 1.0, 0.0, 0.0));
            benchmark_ray_triangle_case<rtmd_ops>(bench,
                "Ray-triangle miss rtm::vector4d",
                rtm::vector_set(1.5, 1.5, -2.0, 0.0),
                rtm::vector_set(0.0, 0.0, 1.0, 0.0), rtm::vector_zero(),
                rtm::vector_set(1.0, 0.0, 0.0, 0.0),
                rtm::vector_set(0.0, 1.0, 0.0, 0.0));
        }

        void normalize_direction(ankerl::nanobench::Bench& bench)
        {
            DirectX::SimpleMath::Vector3 smDirection(3.0f, -2.0f, 7.0f);
            bench.run("Normalize direction SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(smDirection);
                    results.smVec3 = smDirection;
                    results.smVec3.Normalize();
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec3));
                });

            glm::vec3 glmDirection(3.0f, -2.0f, 7.0f);
            bench.run("Normalize direction glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(glmDirection);
                    results.glmVec3 = glm::normalize(glmDirection);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec3));
                });

            DirectX::XMVECTOR dxDirection =
                DirectX::XMVectorSet(3.0f, -2.0f, 7.0f, 0.0f);
            bench.run("Normalize direction DXM",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(dxDirection);
                    results.dxVecA = DirectX::XMVector3Normalize(dxDirection);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVecA));
                });

            Vectormath::Vector3 sonyDirection(3.0f, -2.0f, 7.0f);
            bench.run("Normalize direction Vectormath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(sonyDirection);
                    results.sonyVec3 = normalize(sonyDirection);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec3));
                });

            move::math::float3 mvDirection(3.0f, -2.0f, 7.0f);
            bench.run("Normalize direction move::float3",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirection);
                    results.mvVec3f = mvDirection.normalized();
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3f));
                });

            move::math::double3 mvDirectionD(3.0, -2.0, 7.0);
            bench.run("Normalize direction move::double3",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirectionD);
                    results.mvVec3d = mvDirectionD.normalized();
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3d));
                });

            rtm::vector4f rtmDirection =
                rtm::vector_set(3.0f, -2.0f, 7.0f, 0.0f);
            bench.run("Normalize direction rtm::vector4f",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmDirection);
                    results.rtmVec4f = rtm::vector_normalize3(rtmDirection);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            rtm::vector4d rtmDirectionD = rtm::vector_set(3.0, -2.0, 7.0, 0.0);
            bench.run("Normalize direction rtm::vector4d",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmDirectionD);
                    results.rtmVec4d = rtm::vector_normalize3(rtmDirectionD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }

        void particle_integration(ankerl::nanobench::Bench& bench)
        {
            constexpr float deltaTime = 1.0f / 60.0f;
            constexpr double deltaTimeD = 1.0 / 60.0;

            DirectX::SimpleMath::Vector3 smPosition(12.0f, 8.0f, -4.0f);
            DirectX::SimpleMath::Vector3 smVelocity(2.0f, 5.0f, -1.0f);
            DirectX::SimpleMath::Vector3 smAcceleration(0.0f, -9.81f, 0.0f);
            bench.run("Particle integration SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(smPosition);
                    ankerl::nanobench::doNotOptimizeAway(smVelocity);
                    ankerl::nanobench::doNotOptimizeAway(smAcceleration);
                    const auto velocity =
                        smVelocity + smAcceleration * deltaTime;
                    results.smVec3 = smPosition + velocity * deltaTime;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec3));
                });

            glm::vec3 glmPosition(12.0f, 8.0f, -4.0f);
            glm::vec3 glmVelocity(2.0f, 5.0f, -1.0f);
            glm::vec3 glmAcceleration(0.0f, -9.81f, 0.0f);
            bench.run("Particle integration glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(glmPosition);
                    ankerl::nanobench::doNotOptimizeAway(glmVelocity);
                    ankerl::nanobench::doNotOptimizeAway(glmAcceleration);
                    const auto velocity =
                        glmVelocity + glmAcceleration * deltaTime;
                    results.glmVec3 = glmPosition + velocity * deltaTime;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec3));
                });

            DirectX::XMVECTOR dxPosition =
                DirectX::XMVectorSet(12.0f, 8.0f, -4.0f, 0.0f);
            DirectX::XMVECTOR dxVelocity =
                DirectX::XMVectorSet(2.0f, 5.0f, -1.0f, 0.0f);
            DirectX::XMVECTOR dxAcceleration =
                DirectX::XMVectorSet(0.0f, -9.81f, 0.0f, 0.0f);
            const DirectX::XMVECTOR dxDeltaTime =
                DirectX::XMVectorReplicate(deltaTime);
            bench.run("Particle integration DXM",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(dxPosition);
                    ankerl::nanobench::doNotOptimizeAway(dxVelocity);
                    ankerl::nanobench::doNotOptimizeAway(dxAcceleration);
                    const auto velocity = DirectX::XMVectorMultiplyAdd(
                        dxAcceleration, dxDeltaTime, dxVelocity);
                    results.dxVecA = DirectX::XMVectorMultiplyAdd(
                        velocity, dxDeltaTime, dxPosition);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVecA));
                });

            Vectormath::Vector3 sonyPosition(12.0f, 8.0f, -4.0f);
            Vectormath::Vector3 sonyVelocity(2.0f, 5.0f, -1.0f);
            Vectormath::Vector3 sonyAcceleration(0.0f, -9.81f, 0.0f);
            bench.run("Particle integration Vectormath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(sonyPosition);
                    ankerl::nanobench::doNotOptimizeAway(sonyVelocity);
                    ankerl::nanobench::doNotOptimizeAway(sonyAcceleration);
                    const auto velocity =
                        sonyVelocity + sonyAcceleration * deltaTime;
                    results.sonyVec3 = sonyPosition + velocity * deltaTime;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec3));
                });

            move::math::float3 mvPosition(12.0f, 8.0f, -4.0f);
            move::math::float3 mvVelocity(2.0f, 5.0f, -1.0f);
            move::math::float3 mvAcceleration(0.0f, -9.81f, 0.0f);
            bench.run("Particle integration move::float3",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvPosition);
                    ankerl::nanobench::doNotOptimizeAway(mvVelocity);
                    ankerl::nanobench::doNotOptimizeAway(mvAcceleration);
                    const auto velocity =
                        mvVelocity + mvAcceleration * deltaTime;
                    results.mvVec3f = mvPosition + velocity * deltaTime;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3f));
                });

            move::math::double3 mvPositionD(12.0, 8.0, -4.0);
            move::math::double3 mvVelocityD(2.0, 5.0, -1.0);
            move::math::double3 mvAccelerationD(0.0, -9.81, 0.0);
            bench.run("Particle integration move::double3",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvPositionD);
                    ankerl::nanobench::doNotOptimizeAway(mvVelocityD);
                    ankerl::nanobench::doNotOptimizeAway(mvAccelerationD);
                    const auto velocity =
                        mvVelocityD + mvAccelerationD * deltaTimeD;
                    results.mvVec3d = mvPositionD + velocity * deltaTimeD;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3d));
                });

            rtm::vector4f rtmPosition =
                rtm::vector_set(12.0f, 8.0f, -4.0f, 0.0f);
            rtm::vector4f rtmVelocity =
                rtm::vector_set(2.0f, 5.0f, -1.0f, 0.0f);
            rtm::vector4f rtmAcceleration =
                rtm::vector_set(0.0f, -9.81f, 0.0f, 0.0f);
            bench.run("Particle integration rtm::vector4f",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmPosition);
                    ankerl::nanobench::doNotOptimizeAway(rtmVelocity);
                    ankerl::nanobench::doNotOptimizeAway(rtmAcceleration);
                    const auto velocity = rtm::vector_add(rtmVelocity,
                        rtm::vector_mul(rtmAcceleration, deltaTime));
                    results.rtmVec4f = rtm::vector_add(
                        rtmPosition, rtm::vector_mul(velocity, deltaTime));
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            rtm::vector4d rtmPositionD = rtm::vector_set(12.0, 8.0, -4.0, 0.0);
            rtm::vector4d rtmVelocityD = rtm::vector_set(2.0, 5.0, -1.0, 0.0);
            rtm::vector4d rtmAccelerationD =
                rtm::vector_set(0.0, -9.81, 0.0, 0.0);
            bench.run("Particle integration rtm::vector4d",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmPositionD);
                    ankerl::nanobench::doNotOptimizeAway(rtmVelocityD);
                    ankerl::nanobench::doNotOptimizeAway(rtmAccelerationD);
                    const auto velocity = rtm::vector_add(rtmVelocityD,
                        rtm::vector_mul(rtmAccelerationD, deltaTimeD));
                    results.rtmVec4d = rtm::vector_add(
                        rtmPositionD, rtm::vector_mul(velocity, deltaTimeD));
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }

        void camera_basis(ankerl::nanobench::Bench& bench)
        {
            DirectX::SimpleMath::Vector3 smEye(4.0f, 2.0f, -8.0f);
            DirectX::SimpleMath::Vector3 smTarget(1.0f, 3.0f, 2.0f);
            DirectX::SimpleMath::Vector3 smUp(0.0f, 1.0f, 0.0f);
            bench.run("Camera basis SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(smEye);
                    ankerl::nanobench::doNotOptimizeAway(smTarget);
                    ankerl::nanobench::doNotOptimizeAway(smUp);
                    auto forward = smTarget - smEye;
                    forward.Normalize();
                    auto right = smUp.Cross(forward);
                    right.Normalize();
                    const auto correctedUp = forward.Cross(right);
                    results.smVec3 = right + correctedUp;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec3));
                });

            glm::vec3 glmEye(4.0f, 2.0f, -8.0f);
            glm::vec3 glmTarget(1.0f, 3.0f, 2.0f);
            glm::vec3 glmUp(0.0f, 1.0f, 0.0f);
            bench.run("Camera basis glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(glmEye);
                    ankerl::nanobench::doNotOptimizeAway(glmTarget);
                    ankerl::nanobench::doNotOptimizeAway(glmUp);
                    const auto forward = glm::normalize(glmTarget - glmEye);
                    const auto right =
                        glm::normalize(glm::cross(glmUp, forward));
                    const auto correctedUp = glm::cross(forward, right);
                    results.glmVec3 = right + correctedUp;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec3));
                });

            DirectX::XMVECTOR dxEye =
                DirectX::XMVectorSet(4.0f, 2.0f, -8.0f, 0.0f);
            DirectX::XMVECTOR dxTarget =
                DirectX::XMVectorSet(1.0f, 3.0f, 2.0f, 0.0f);
            DirectX::XMVECTOR dxUp =
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            bench.run("Camera basis DXM",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(dxEye);
                    ankerl::nanobench::doNotOptimizeAway(dxTarget);
                    ankerl::nanobench::doNotOptimizeAway(dxUp);
                    const auto forward = DirectX::XMVector3Normalize(
                        DirectX::XMVectorSubtract(dxTarget, dxEye));
                    const auto right = DirectX::XMVector3Normalize(
                        DirectX::XMVector3Cross(dxUp, forward));
                    const auto correctedUp =
                        DirectX::XMVector3Cross(forward, right);
                    results.dxVecA = DirectX::XMVectorAdd(right, correctedUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVecA));
                });

            Vectormath::Vector3 sonyEye(4.0f, 2.0f, -8.0f);
            Vectormath::Vector3 sonyTarget(1.0f, 3.0f, 2.0f);
            Vectormath::Vector3 sonyUp(0.0f, 1.0f, 0.0f);
            bench.run("Camera basis Vectormath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(sonyEye);
                    ankerl::nanobench::doNotOptimizeAway(sonyTarget);
                    ankerl::nanobench::doNotOptimizeAway(sonyUp);
                    const auto forward = normalize(sonyTarget - sonyEye);
                    const auto right = normalize(cross(sonyUp, forward));
                    const auto correctedUp = cross(forward, right);
                    results.sonyVec3 = right + correctedUp;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec3));
                });

            move::math::float3 mvEye(4.0f, 2.0f, -8.0f);
            move::math::float3 mvTarget(1.0f, 3.0f, 2.0f);
            move::math::float3 mvUp(0.0f, 1.0f, 0.0f);
            bench.run("Camera basis move::float3",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvEye);
                    ankerl::nanobench::doNotOptimizeAway(mvTarget);
                    ankerl::nanobench::doNotOptimizeAway(mvUp);
                    const auto forward = (mvTarget - mvEye).normalized();
                    const auto right =
                        move::math::float3::cross(mvUp, forward).normalized();
                    const auto correctedUp =
                        move::math::float3::cross(forward, right);
                    results.mvVec3f = right + correctedUp;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3f));
                });

            move::math::double3 mvEyeD(4.0, 2.0, -8.0);
            move::math::double3 mvTargetD(1.0, 3.0, 2.0);
            move::math::double3 mvUpD(0.0, 1.0, 0.0);
            bench.run("Camera basis move::double3",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvEyeD);
                    ankerl::nanobench::doNotOptimizeAway(mvTargetD);
                    ankerl::nanobench::doNotOptimizeAway(mvUpD);
                    const auto forward = (mvTargetD - mvEyeD).normalized();
                    const auto right =
                        move::math::double3::cross(mvUpD, forward).normalized();
                    const auto correctedUp =
                        move::math::double3::cross(forward, right);
                    results.mvVec3d = right + correctedUp;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3d));
                });

            rtm::vector4f rtmEye = rtm::vector_set(4.0f, 2.0f, -8.0f, 0.0f);
            rtm::vector4f rtmTarget = rtm::vector_set(1.0f, 3.0f, 2.0f, 0.0f);
            rtm::vector4f rtmUp = rtm::vector_set(0.0f, 1.0f, 0.0f, 0.0f);
            bench.run("Camera basis rtm::vector4f",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmEye);
                    ankerl::nanobench::doNotOptimizeAway(rtmTarget);
                    ankerl::nanobench::doNotOptimizeAway(rtmUp);
                    const auto forward = rtm::vector_normalize3(
                        rtm::vector_sub(rtmTarget, rtmEye));
                    const auto right = rtm::vector_normalize3(
                        rtm::vector_cross3(rtmUp, forward));
                    const auto correctedUp = rtm::vector_cross3(forward, right);
                    results.rtmVec4f = rtm::vector_add(right, correctedUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            rtm::vector4d rtmEyeD = rtm::vector_set(4.0, 2.0, -8.0, 0.0);
            rtm::vector4d rtmTargetD = rtm::vector_set(1.0, 3.0, 2.0, 0.0);
            rtm::vector4d rtmUpD = rtm::vector_set(0.0, 1.0, 0.0, 0.0);
            bench.run("Camera basis rtm::vector4d",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmEyeD);
                    ankerl::nanobench::doNotOptimizeAway(rtmTargetD);
                    ankerl::nanobench::doNotOptimizeAway(rtmUpD);
                    const auto forward = rtm::vector_normalize3(
                        rtm::vector_sub(rtmTargetD, rtmEyeD));
                    const auto right = rtm::vector_normalize3(
                        rtm::vector_cross3(rtmUpD, forward));
                    const auto correctedUp = rtm::vector_cross3(forward, right);
                    results.rtmVec4d = rtm::vector_add(right, correctedUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }

        void rotate_direction(ankerl::nanobench::Bench& bench)
        {
            DirectX::SimpleMath::Vector3 smDirection(0.25f, 0.5f, 1.0f);
            auto smRotation =
                DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
                    DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), 0.35f);
            bench.run("Rotate direction SimpleMath quaternion",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(smDirection);
                    ankerl::nanobench::doNotOptimizeAway(smRotation);
                    results.smVec3 = DirectX::SimpleMath::Vector3::Transform(
                        smDirection, smRotation);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec3));
                });

            glm::vec3 glmDirection(0.25f, 0.5f, 1.0f);
            glm::quat glmRotation =
                glm::angleAxis(0.35f, glm::vec3(0.0f, 1.0f, 0.0f));
            bench.run("Rotate direction glm quaternion",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(glmDirection);
                    ankerl::nanobench::doNotOptimizeAway(glmRotation);
                    results.glmVec3 = glmRotation * glmDirection;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec3));
                });

            DirectX::XMVECTOR dxDirection =
                DirectX::XMVectorSet(0.25f, 0.5f, 1.0f, 0.0f);
            DirectX::XMVECTOR dxRotation = DirectX::XMQuaternionRotationAxis(
                DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.35f);
            bench.run("Rotate direction DXM quaternion",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(dxDirection);
                    ankerl::nanobench::doNotOptimizeAway(dxRotation);
                    results.dxVecA =
                        DirectX::XMVector3Rotate(dxDirection, dxRotation);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVecA));
                });

            Vectormath::Vector3 sonyDirection(0.25f, 0.5f, 1.0f);
            auto sonyRotation =
                Vectormath::Quat::rotation(0.35f, Vectormath::Vector3::yAxis());
            bench.run("Rotate direction Vectormath quaternion",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(sonyDirection);
                    ankerl::nanobench::doNotOptimizeAway(sonyRotation);
                    results.sonyVec3 =
                        Vectormath::SSE::rotate(sonyRotation, sonyDirection);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec3));
                });

            move::math::float3 mvDirection(0.25f, 0.5f, 1.0f);
            auto mvRotation = move::math::quatf::rotation_y(0.35f);
            bench.run("Rotate direction move::math quaternion (float)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirection);
                    ankerl::nanobench::doNotOptimizeAway(mvRotation);
                    results.mvVec3f = mvDirection * mvRotation;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3f));
                });

            move::math::double3 mvDirectionD(0.25, 0.5, 1.0);
            auto mvRotationD = move::math::quatd::rotation_y(0.35);
            bench.run("Rotate direction move::math quaternion (double)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirectionD);
                    ankerl::nanobench::doNotOptimizeAway(mvRotationD);
                    results.mvVec3d = mvDirectionD * mvRotationD;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec3d));
                });

            rtm::vector4f rtmDirection =
                rtm::vector_set(0.25f, 0.5f, 1.0f, 0.0f);
            rtm::quatf rtmRotation = rtm::quat_from_axis_angle(
                rtm::vector_set(0.0f, 1.0f, 0.0f, 0.0f), 0.35f);
            bench.run("Rotate direction rtm::quatf",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmDirection);
                    ankerl::nanobench::doNotOptimizeAway(rtmRotation);
                    results.rtmVec4f =
                        rtm::quat_mul_vector3(rtmDirection, rtmRotation);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            rtm::vector4d rtmDirectionD = rtm::vector_set(0.25, 0.5, 1.0, 0.0);
            rtm::quatd rtmRotationD = rtm::quat_from_axis_angle(
                rtm::vector_set(0.0, 1.0, 0.0, 0.0), 0.35);
            bench.run("Rotate direction rtm::quatd",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(rtmDirectionD);
                    ankerl::nanobench::doNotOptimizeAway(rtmRotationD);
                    results.rtmVec4d =
                        rtm::quat_mul_vector3(rtmDirectionD, rtmRotationD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }
    }  // namespace workloads

    namespace matrices
    {
        void construct_model_matrix(ankerl::nanobench::Bench& bench)
        {
            float tx = 1.0f;
            float ty = 2.0f;
            float tz = 3.0f;
            float angle = 0.5f;
            float sx = 1.0f;
            float sy = 2.0f;
            float sz = 3.0f;
            double txD = 1.0;
            double tyD = 2.0;
            double tzD = 3.0;
            double angleD = 0.5;
            double sxD = 1.0;
            double syD = 2.0;
            double szD = 3.0;

            const auto makeFloatInputsOpaque = [&]
            {
                ankerl::nanobench::doNotOptimizeAway(tx);
                ankerl::nanobench::doNotOptimizeAway(ty);
                ankerl::nanobench::doNotOptimizeAway(tz);
                ankerl::nanobench::doNotOptimizeAway(angle);
                ankerl::nanobench::doNotOptimizeAway(sx);
                ankerl::nanobench::doNotOptimizeAway(sy);
                ankerl::nanobench::doNotOptimizeAway(sz);
            };
            const auto makeDoubleInputsOpaque = [&]
            {
                ankerl::nanobench::doNotOptimizeAway(txD);
                ankerl::nanobench::doNotOptimizeAway(tyD);
                ankerl::nanobench::doNotOptimizeAway(tzD);
                ankerl::nanobench::doNotOptimizeAway(angleD);
                ankerl::nanobench::doNotOptimizeAway(sxD);
                ankerl::nanobench::doNotOptimizeAway(syD);
                ankerl::nanobench::doNotOptimizeAway(szD);
            };

            bench.run("Construct model matrix SimpleMath",
                [&]
                {
                    makeFloatInputsOpaque();
                    results.smMat4a =
                        DirectX::SimpleMath::Matrix::CreateTranslation(
                            tx, ty, tz) *
                        DirectX::SimpleMath::Matrix::CreateRotationX(angle) *
                        DirectX::SimpleMath::Matrix::CreateRotationY(angle) *
                        DirectX::SimpleMath::Matrix::CreateRotationZ(angle) *
                        DirectX::SimpleMath::Matrix::CreateScale(sx, sy, sz);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smMat4a));
                });

            bench.run("Construct model matrix glm",
                [&]
                {
                    makeFloatInputsOpaque();
                    results.glmMat4a =
                        glm::translate(glm::mat4(1.0f), glm::vec3(tx, ty, tz)) *
                        glm::rotate(glm::mat4(1.0f), angle,
                            glm::vec3(1.0f, 0.0f, 0.0f)) *
                        glm::rotate(glm::mat4(1.0f), angle,
                            glm::vec3(0.0f, 1.0f, 0.0f)) *
                        glm::rotate(glm::mat4(1.0f), angle,
                            glm::vec3(0.0f, 0.0f, 1.0f)) *
                        glm::scale(glm::mat4(1.0f), glm::vec3(sx, sy, sz));
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmMat4a));
                });

            bench.run("Construct model matrix DXM (manual)",
                [&]
                {
                    using namespace DirectX;
                    makeFloatInputsOpaque();

                    results.dxMatA = XMMatrixMultiply(
                        XMMatrixMultiply(
                            XMMatrixMultiply(
                                XMMatrixMultiply(
                                    XMMatrixTranslation(tx, ty, tz),
                                    XMMatrixRotationX(angle)),
                                XMMatrixRotationY(angle)),
                            XMMatrixRotationZ(angle)),
                        XMMatrixScaling(sx, sy, sz));
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxMatA));
                });

            bench.run("Construct model matrix DXM (builtin)",
                [&]
                {
                    using namespace DirectX;
                    makeFloatInputsOpaque();

                    XMVECTOR ScalingOrigin = XMVectorZero();
                    XMVECTOR ScalingOrientationQuaternion =
                        XMQuaternionIdentity();
                    XMVECTOR Scaling = XMVectorSet(sx, sy, sz, 0.0f);
                    XMVECTOR RotationOrigin = XMVectorZero();
                    XMVECTOR RotationQuaternion =
                        XMQuaternionRotationRollPitchYaw(angle, angle, angle);
                    XMVECTOR Translation = XMVectorSet(tx, ty, tz, 0.0f);

                    results.dxMatA = XMMatrixTransformation(ScalingOrigin,
                        ScalingOrientationQuaternion, Scaling, RotationOrigin,
                        RotationQuaternion, Translation);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxMatA));
                });

            bench.run("Construct model matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    makeFloatInputsOpaque();
                    results.sonyMat4a =
                        Matrix4::translation(Vector3(tx, ty, tz)) *
                        Matrix4::rotationX(angle) * Matrix4::rotationY(angle) *
                        Matrix4::rotationZ(angle) *
                        Matrix4::scale(Vector3(sx, sy, sz));
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyMat4a));
                });

            bench.run("Construct model matrix move::math (manual, float)",
                [&]
                {
                    using namespace move::math;
                    makeFloatInputsOpaque();
                    results.mvMat4x4fa = mat4x4f::translation({tx, ty, tz}) *
                                         mat4x4f::rotation_x(angle) *
                                         mat4x4f::rotation_y(angle) *
                                         mat4x4f::rotation_z(angle) *
                                         mat4x4f::scale(sx, sy, sz);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4fa));
                });

            bench.run("Construct model matrix move::math (builtin, float)",
                [&]
                {
                    using namespace move::math;
                    makeFloatInputsOpaque();
                    results.mvMat4x4fa = mat4x4f::trs({tx, ty, tz},
                        quatf::euler(angle, angle, angle), {sx, sy, sz});

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4fa));
                });

            bench.run("Construct model matrix move::math (manual, double)",
                [&]
                {
                    using namespace move::math;
                    makeDoubleInputsOpaque();
                    results.mvMat4x4da = mat4x4d::translation({txD, tyD, tzD}) *
                                         mat4x4d::rotation(quatd::euler(
                                             angleD, angleD, angleD)) *
                                         mat4x4d::scale(sxD, syD, szD);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4da));
                });

            bench.run("Construct model matrix move::math (builtin, double)",
                [&]
                {
                    using namespace move::math;
                    makeDoubleInputsOpaque();
                    results.mvMat4x4da = mat4x4d::trs(vec3d(txD, tyD, tzD),
                        quatd::euler(angleD, angleD, angleD),
                        vec3d(sxD, syD, szD));

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4da));
                });

            bench.run("Construct rtm::qvvf (~transformation matrix)",
                [&]
                {
                    using namespace rtm;
                    makeFloatInputsOpaque();
                    results.rtmQvvf =
                        qvv_set(quat_set(angle, angle, angle, 1.0f),
                            vector_set(tx, ty, tz, 0.0f),
                            vector_set(sx, sy, sz, 0.0f));

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmQvvf));
                });

            bench.run("Construct rtm::qvvd (~transformation matrix)",
                [&]
                {
                    using namespace rtm;
                    makeDoubleInputsOpaque();
                    results.rtmQvvd =
                        qvv_set(quat_set(angleD, angleD, angleD, 1.0),
                            vector_set(txD, tyD, tzD, 0.0),
                            vector_set(sxD, syD, szD, 0.0));

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmQvvd));
                });

            bench.run("Construct model matrix rtm::matrix3x4f",
                [&]
                {
                    using namespace rtm;
                    makeFloatInputsOpaque();
                    auto qvv = qvv_set(quat_set(angle, angle, angle, 1.0f),
                        vector_set(tx, ty, tz, 0.0f),
                        vector_set(sx, sy, sz, 0.0f));

                    results.rtmMat3x4fa = matrix_from_qvv(qvv);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat3x4fa));
                });

            bench.run("Construct model matrix rtm::matrix3x4d",
                [&]
                {
                    using namespace rtm;
                    makeDoubleInputsOpaque();
                    auto qvv = qvv_set(quat_set(angleD, angleD, angleD, 1.0),
                        vector_set(txD, tyD, tzD, 0.0),
                        vector_set(sxD, syD, szD, 0.0));

                    results.rtmMat3x4da = matrix_from_qvv(qvv);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat3x4da));
                });

            bench.run("Construct model matrix rtm::matrix4x4f",
                [&]
                {
                    using namespace rtm;
                    makeFloatInputsOpaque();
                    auto qvv = qvv_set(quat_set(angle, angle, angle, 1.0f),
                        vector_set(tx, ty, tz, 0.0f),
                        vector_set(sx, sy, sz, 0.0f));

                    results.rtmMat4x4fa = matrix_cast(matrix_from_qvv(qvv));
                    results.rtmMat4x4fa.z_axis =
                        vector_set_w(results.rtmMat4x4fa.z_axis, 1);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4fa));
                });

            bench.run("Construct model matrix rtm::matrix4x4d",
                [&]
                {
                    using namespace rtm;
                    makeDoubleInputsOpaque();
                    auto qvv = qvv_set(quat_set(angleD, angleD, angleD, 1.0),
                        vector_set(txD, tyD, tzD, 0.0),
                        vector_set(sxD, syD, szD, 0.0));

                    results.rtmMat4x4da = matrix_cast(matrix_from_qvv(qvv));
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4da));
                });
        }

        void construct_view_matrix(ankerl::nanobench::Bench& bench)
        {
            const DirectX::SimpleMath::Vector3 smEye(1.0f, 2.0f, 3.0f);
            const DirectX::SimpleMath::Vector3 smTarget(4.0f, 5.0f, 6.0f);
            const DirectX::SimpleMath::Vector3 smUp(7.0f, 8.0f, 9.0f);
            const glm::vec3 glmEye(1.0f, 2.0f, 3.0f);
            const glm::vec3 glmTarget(4.0f, 5.0f, 6.0f);
            const glm::vec3 glmUp(7.0f, 8.0f, 9.0f);
            const DirectX::XMVECTOR dxEye =
                DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
            const DirectX::XMVECTOR dxTarget =
                DirectX::XMVectorSet(4.0f, 5.0f, 6.0f, 0.0f);
            const DirectX::XMVECTOR dxUp =
                DirectX::XMVectorSet(7.0f, 8.0f, 9.0f, 0.0f);
            const Vectormath::SSE::Point3 sonyEye(1.0f, 2.0f, 3.0f);
            const Vectormath::SSE::Point3 sonyTarget(4.0f, 5.0f, 6.0f);
            const Vectormath::Vector3 sonyUp(7.0f, 8.0f, 9.0f);
            const move::math::fast_float3 mvEye(1.0f, 2.0f, 3.0f);
            const move::math::fast_float3 mvTarget(4.0f, 5.0f, 6.0f);
            const move::math::fast_float3 mvUp(7.0f, 8.0f, 9.0f);
            const move::math::fast_double3 mvEyeD(1.0, 2.0, 3.0);
            const move::math::fast_double3 mvTargetD(4.0, 5.0, 6.0);
            const move::math::fast_double3 mvUpD(7.0, 8.0, 9.0);
            const rtm::vector4f rtmEye =
                rtm::vector_set(1.0f, 2.0f, 3.0f, 0.0f);
            const rtm::vector4f rtmTarget =
                rtm::vector_set(4.0f, 5.0f, 6.0f, 0.0f);
            const rtm::vector4f rtmUp = rtm::vector_set(7.0f, 8.0f, 9.0f, 0.0f);
            const rtm::vector4d rtmEyeD = rtm::vector_set(1.0, 2.0, 3.0, 0.0);
            const rtm::vector4d rtmTargetD =
                rtm::vector_set(4.0, 5.0, 6.0, 0.0);
            const rtm::vector4d rtmUpD = rtm::vector_set(7.0, 8.0, 9.0, 0.0);

            bench.run("Construct view matrix SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(smEye);
                    ankerl::nanobench::doNotOptimizeAway(smTarget);
                    ankerl::nanobench::doNotOptimizeAway(smUp);
                    results.smMat4a = DirectX::SimpleMath::Matrix::CreateLookAt(
                        smEye, smTarget, smUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smMat4a));
                });

            bench.run("Construct view matrix glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(glmEye);
                    ankerl::nanobench::doNotOptimizeAway(glmTarget);
                    ankerl::nanobench::doNotOptimizeAway(glmUp);
                    results.glmMat4a = glm::lookAt(glmEye, glmTarget, glmUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmMat4a));
                });

            bench.run("Construct view matrix DXM",
                [&]
                {
                    using namespace DirectX;
                    ankerl::nanobench::doNotOptimizeAway(dxEye);
                    ankerl::nanobench::doNotOptimizeAway(dxTarget);
                    ankerl::nanobench::doNotOptimizeAway(dxUp);
                    results.dxMatA = XMMatrixLookAtLH(dxEye, dxTarget, dxUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxMatA));
                });

            bench.run("Construct view matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    ankerl::nanobench::doNotOptimizeAway(sonyEye);
                    ankerl::nanobench::doNotOptimizeAway(sonyTarget);
                    ankerl::nanobench::doNotOptimizeAway(sonyUp);
                    results.sonyMat4a =
                        Matrix4::lookAt(sonyEye, sonyTarget, sonyUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyMat4a));
                });

            bench.run("Construct view matrix move::math (float)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(mvEye);
                    ankerl::nanobench::doNotOptimizeAway(mvTarget);
                    ankerl::nanobench::doNotOptimizeAway(mvUp);
                    results.mvMat4x4fa =
                        mat4x4f::look_at(mvEye, mvTarget, mvUp);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4fa));
                });

            bench.run("Construct view matrix move::math (double)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(mvEyeD);
                    ankerl::nanobench::doNotOptimizeAway(mvTargetD);
                    ankerl::nanobench::doNotOptimizeAway(mvUpD);
                    results.mvMat4x4da =
                        mat4x4d::look_at(mvEyeD, mvTargetD, mvUpD);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4da));
                });

            bench.run("Construct view matrix rtm::matrix4x4f",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(rtmEye);
                    ankerl::nanobench::doNotOptimizeAway(rtmTarget);
                    ankerl::nanobench::doNotOptimizeAway(rtmUp);
                    results.rtmMat4x4fa =
                        rtm::camera::look_at_rh<matrix4x4f, vector4f>(
                            rtmEye, rtmTarget, rtmUp);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4fa));
                });

            bench.run("Construct view matrix rtm::matrix4x4d",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(rtmEyeD);
                    ankerl::nanobench::doNotOptimizeAway(rtmTargetD);
                    ankerl::nanobench::doNotOptimizeAway(rtmUpD);
                    results.rtmMat4x4da =
                        rtm::camera::look_at_rh<matrix4x4d, vector4d>(
                            rtmEyeD, rtmTargetD, rtmUpD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4da));
                });
        }

        void construct_perspective_projection_matrix(
            ankerl::nanobench::Bench& bench)
        {
            float fov = 0.5f;
            float aspect = 1.0f;
            float nearPlane = 0.1f;
            float farPlane = 100.0f;
            double fovD = 0.5;
            double aspectD = 1.0;
            double nearPlaneD = 0.1;
            double farPlaneD = 100.0;

            bench.run("Construct perspective matrix SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(fov);
                    ankerl::nanobench::doNotOptimizeAway(aspect);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.smMat4a = DirectX::SimpleMath::Matrix::
                        CreatePerspectiveFieldOfView(
                            fov, aspect, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smMat4a));
                });

            bench.run("Construct perspective matrix glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(fov);
                    ankerl::nanobench::doNotOptimizeAway(aspect);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.glmMat4a =
                        glm::perspective(fov, aspect, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmMat4a));
                });

            bench.run("Construct perspective matrix DXM",
                [&]
                {
                    using namespace DirectX;
                    ankerl::nanobench::doNotOptimizeAway(fov);
                    ankerl::nanobench::doNotOptimizeAway(aspect);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.dxMatA = XMMatrixPerspectiveFovLH(
                        fov, aspect, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxMatA));
                });

            bench.run("Construct perspective matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    ankerl::nanobench::doNotOptimizeAway(fov);
                    ankerl::nanobench::doNotOptimizeAway(aspect);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.sonyMat4a =
                        Matrix4::perspective(fov, aspect, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyMat4a));
                });

            bench.run("Construct perspective matrix move::math (float)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(fov);
                    ankerl::nanobench::doNotOptimizeAway(aspect);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.mvMat4x4fa =
                        mat4x4f::perspective(fov, aspect, nearPlane, farPlane);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4fa));
                });

            bench.run("Construct perspective matrix move::math (double)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(fovD);
                    ankerl::nanobench::doNotOptimizeAway(aspectD);
                    ankerl::nanobench::doNotOptimizeAway(nearPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(farPlaneD);
                    results.mvMat4x4da = mat4x4d::perspective(
                        fovD, aspectD, nearPlaneD, farPlaneD);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4da));
                });

            bench.run("Construct perspective matrix rtm::matrix4x4f",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(fov);
                    ankerl::nanobench::doNotOptimizeAway(aspect);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.rtmMat4x4fa = rtm::camera::perspective_fov_rh(
                        fov, aspect, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4fa));
                });

            bench.run("Construct perspective matrix rtm::matrix4x4d",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(fovD);
                    ankerl::nanobench::doNotOptimizeAway(aspectD);
                    ankerl::nanobench::doNotOptimizeAway(nearPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(farPlaneD);
                    results.rtmMat4x4da = rtm::camera::perspective_fov_rh(
                        fovD, aspectD, nearPlaneD, farPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4da));
                });
        }

        void ortho_projection_matrix(ankerl::nanobench::Bench& bench)
        {
            float width = 1280.0f;
            float height = 720.0f;
            float nearPlane = 0.1f;
            float farPlane = 100.0f;
            double widthD = 1280.0;
            double heightD = 720.0;
            double nearPlaneD = 0.1;
            double farPlaneD = 100.0;

            bench.run("Construct ortho matrix SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(width);
                    ankerl::nanobench::doNotOptimizeAway(height);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.smMat4a =
                        DirectX::SimpleMath::Matrix::CreateOrthographic(
                            width, height, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smMat4a));
                });

            bench.run("Construct ortho matrix glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(width);
                    ankerl::nanobench::doNotOptimizeAway(height);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.glmMat4a = glm::ortho<float>(
                        0, width, 0, height, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmMat4a));
                });

            bench.run("Construct ortho matrix DXM",
                [&]
                {
                    using namespace DirectX;
                    ankerl::nanobench::doNotOptimizeAway(width);
                    ankerl::nanobench::doNotOptimizeAway(height);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.dxMatA = XMMatrixOrthographicLH(
                        width, height, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxMatA));
                });

            bench.run("Construct ortho matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    ankerl::nanobench::doNotOptimizeAway(width);
                    ankerl::nanobench::doNotOptimizeAway(height);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.sonyMat4a = Matrix4::orthographic(
                        0, width, 0, height, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyMat4a));
                });

            bench.run("Construct ortho matrix move::math (float)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(width);
                    ankerl::nanobench::doNotOptimizeAway(height);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.mvMat4x4fa = mat4x4f::orthographic(
                        width, height, nearPlane, farPlane);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4fa));
                });

            bench.run("Construct ortho matrix move::math (double)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(widthD);
                    ankerl::nanobench::doNotOptimizeAway(heightD);
                    ankerl::nanobench::doNotOptimizeAway(nearPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(farPlaneD);
                    results.mvMat4x4da = mat4x4d::orthographic(
                        widthD, heightD, nearPlaneD, farPlaneD);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4da));
                });

            bench.run("Construct ortho matrix rtm::matrix4x4f",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(width);
                    ankerl::nanobench::doNotOptimizeAway(height);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    results.rtmMat4x4fa = rtm::camera::ortho_rh(
                        width, height, nearPlane, farPlane);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4fa));
                });

            bench.run("Construct ortho matrix rtm::matrix4x4d",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(widthD);
                    ankerl::nanobench::doNotOptimizeAway(heightD);
                    ankerl::nanobench::doNotOptimizeAway(nearPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(farPlaneD);
                    results.rtmMat4x4da = rtm::camera::ortho_rh(
                        widthD, heightD, nearPlaneD, farPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4da));
                });
        }

        void vector_matrix_multiply(ankerl::nanobench::Bench& bench)
        {
            const DirectX::SimpleMath::Vector4 smInput(1.0f, 2.0f, 3.0f, 1.0f);
            const DirectX::SimpleMath::Matrix smMatrix =
                DirectX::SimpleMath::Matrix::CreateRotationX(0.35f);
            const glm::vec4 glmInput(1.0f, 2.0f, 3.0f, 1.0f);
            const glm::mat4 glmMatrix =
                glm::rotate(glm::mat4(1.0f), 0.35f, glm::vec3(1, 0, 0));
            const DirectX::XMVECTOR dxInput =
                DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 1.0f);
            const DirectX::XMMATRIX dxMatrix =
                DirectX::XMMatrixRotationX(0.35f);
            const Vectormath::Vector4 sonyInput(1.0f, 2.0f, 3.0f, 1.0f);
            const Vectormath::Matrix4 sonyMatrix =
                Vectormath::Matrix4::rotationX(0.35f);
            const move::math::fast_float4 mvInput(1.0f, 2.0f, 3.0f, 1.0f);
            const move::math::float4x4 mvMatrix =
                move::math::float4x4::rotation_x(0.35f);
            const move::math::fast_double4 mvInputD(1.0, 2.0, 3.0, 1.0);
            const move::math::double4x4 mvMatrixD =
                move::math::double4x4::rotation_x(0.35);
            const rtm::vector4f rtmInput =
                rtm::vector_set(1.0f, 2.0f, 3.0f, 1.0f);
            const rtm::matrix4x4f rtmMatrix = mvMatrix.to_rtm();
            const rtm::vector4d rtmInputD = rtm::vector_set(1.0, 2.0, 3.0, 1.0);
            const rtm::matrix4x4d rtmMatrixD = mvMatrixD.to_rtm();

            bench.run("Vector matrix multiply SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(smInput);
                    ankerl::nanobench::doNotOptimizeAway(smMatrix);
                    results.smVec4 = DirectX::SimpleMath::Vector4::Transform(
                        smInput, smMatrix);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smVec4));
                });

            bench.run("Vector matrix multiply glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(glmInput);
                    ankerl::nanobench::doNotOptimizeAway(glmMatrix);
                    results.glmVec4 = glmMatrix * glmInput;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmVec4));
                });

            bench.run("Vector matrix multiply DXM",
                [&]
                {
                    using namespace DirectX;
                    ankerl::nanobench::doNotOptimizeAway(dxInput);
                    ankerl::nanobench::doNotOptimizeAway(dxMatrix);
                    results.dxVecA = XMVector4Transform(dxInput, dxMatrix);
                    XMStoreFloat4(&results.dxVec4, results.dxVecA);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxVec4));
                });

            bench.run("Vector matrix multiply Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    ankerl::nanobench::doNotOptimizeAway(sonyInput);
                    ankerl::nanobench::doNotOptimizeAway(sonyMatrix);
                    results.sonyVec4 = sonyMatrix * sonyInput;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyVec4));
                });

            bench.run("Vector matrix multiply move::math (float)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(mvInput);
                    ankerl::nanobench::doNotOptimizeAway(mvMatrix);
                    results.mvVec4f = mvInput * mvMatrix;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4f));
                });

            bench.run("Vector matrix multiply move::math (double)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(mvInputD);
                    ankerl::nanobench::doNotOptimizeAway(mvMatrixD);
                    results.mvVec4d = mvInputD * mvMatrixD;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvVec4d));
                });

            bench.run("Vector matrix multiply rtm::matrix4x4f",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(rtmInput);
                    ankerl::nanobench::doNotOptimizeAway(rtmMatrix);
                    results.rtmVec4f = matrix_mul_vector(rtmInput, rtmMatrix);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            bench.run("Vector matrix multiply rtm::matrix4x4d",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(rtmInputD);
                    ankerl::nanobench::doNotOptimizeAway(rtmMatrixD);
                    results.rtmVec4d = matrix_mul_vector(rtmInputD, rtmMatrixD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }

        void qvv_point_multiply(ankerl::nanobench::Bench& bench)
        {
            const rtm::vector4f point = rtm::vector_set(1.0f, 2.0f, 3.0f, 1.0f);
            const rtm::qvvf transform =
                rtm::qvv_set(rtm::quat_set(0.0f, 0.0f, 0.174108f, 0.984727f),
                    rtm::vector_set(4.0f, 5.0f, 6.0f, 0.0f),
                    rtm::vector_set(1.5f, 0.75f, 2.0f, 0.0f));
            const rtm::vector4d pointD = rtm::vector_set(1.0, 2.0, 3.0, 1.0);
            const rtm::qvvd transformD =
                rtm::qvv_set(rtm::quat_set(0.0, 0.0, 0.174108, 0.984727),
                    rtm::vector_set(4.0, 5.0, 6.0, 0.0),
                    rtm::vector_set(1.5, 0.75, 2.0, 0.0));

            bench.run("Point QVV multiply rtm::qvvf",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(point);
                    ankerl::nanobench::doNotOptimizeAway(transform);
                    results.rtmVec4f = qvv_mul_point3(point, transform);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4f));
                });

            bench.run("Point QVV multiply rtm::qvvd",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(pointD);
                    ankerl::nanobench::doNotOptimizeAway(transformD);
                    results.rtmVec4d = qvv_mul_point3(pointD, transformD);

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmVec4d));
                });
        }

        void matrix_matrix_multiply(ankerl::nanobench::Bench& bench)
        {
            const DirectX::SimpleMath::Matrix smLeft =
                DirectX::SimpleMath::Matrix::CreateRotationX(0.35f);
            const DirectX::SimpleMath::Matrix smRight =
                DirectX::SimpleMath::Matrix::CreateTranslation(
                    1.0f, 2.0f, 3.0f);
            const glm::mat4 glmLeft =
                glm::rotate(glm::mat4(1.0f), 0.35f, glm::vec3(1, 0, 0));
            const glm::mat4 glmRight =
                glm::translate(glm::mat4(1.0f), glm::vec3(1, 2, 3));
            const DirectX::XMMATRIX dxLeft = DirectX::XMMatrixRotationX(0.35f);
            const DirectX::XMMATRIX dxRight =
                DirectX::XMMatrixTranslation(1.0f, 2.0f, 3.0f);
            const Vectormath::Matrix4 sonyLeft =
                Vectormath::Matrix4::rotationX(0.35f);
            const Vectormath::Matrix4 sonyRight =
                Vectormath::Matrix4::translation(
                    Vectormath::Vector3(1.0f, 2.0f, 3.0f));
            const move::math::float4x4 mvLeft =
                move::math::float4x4::rotation_x(0.35f);
            const move::math::float4x4 mvRight =
                move::math::float4x4::translation({1.0f, 2.0f, 3.0f});
            const move::math::double4x4 mvLeftD =
                move::math::double4x4::rotation_x(0.35);
            const move::math::double4x4 mvRightD =
                move::math::double4x4::translation({1.0, 2.0, 3.0});
            const rtm::matrix4x4f rtmLeft = mvLeft.to_rtm();
            const rtm::matrix4x4f rtmRight = mvRight.to_rtm();
            const rtm::matrix4x4d rtmLeftD = mvLeftD.to_rtm();
            const rtm::matrix4x4d rtmRightD = mvRightD.to_rtm();

            bench.run("Matrix matrix multiply SimpleMath",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(smLeft);
                    ankerl::nanobench::doNotOptimizeAway(smRight);
                    results.smMat4a = smLeft * smRight;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.smMat4a));
                });

            bench.run("Matrix matrix multiply glm",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(glmLeft);
                    ankerl::nanobench::doNotOptimizeAway(glmRight);
                    results.glmMat4a = glmLeft * glmRight;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.glmMat4a));
                });

            bench.run("Matrix matrix multiply DXM",
                [&]
                {
                    using namespace DirectX;
                    ankerl::nanobench::doNotOptimizeAway(dxLeft);
                    ankerl::nanobench::doNotOptimizeAway(dxRight);
                    results.dxMatA = XMMatrixMultiply(dxLeft, dxRight);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.dxMatA));
                });

            bench.run("Matrix matrix multiply Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    ankerl::nanobench::doNotOptimizeAway(sonyLeft);
                    ankerl::nanobench::doNotOptimizeAway(sonyRight);
                    results.sonyMat4a = sonyLeft * sonyRight;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.sonyMat4a));
                });

            bench.run("Matrix matrix multiply move::math (float)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(mvLeft);
                    ankerl::nanobench::doNotOptimizeAway(mvRight);
                    results.mvMat4x4fa = mvLeft * mvRight;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4fa));
                });

            bench.run("Matrix matrix multiply move::math (double)",
                [&]
                {
                    using namespace move::math;
                    ankerl::nanobench::doNotOptimizeAway(mvLeftD);
                    ankerl::nanobench::doNotOptimizeAway(mvRightD);
                    results.mvMat4x4da = mvLeftD * mvRightD;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvMat4x4da));
                });

            bench.run("Matrix matrix multiply rtm::matrix4x4f",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(rtmLeft);
                    ankerl::nanobench::doNotOptimizeAway(rtmRight);
                    results.rtmMat4x4fa = matrix_mul(rtmLeft, rtmRight);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4fa));
                });

            bench.run("Matrix matrix multiply rtm::matrix4x4d",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(rtmLeftD);
                    ankerl::nanobench::doNotOptimizeAway(rtmRightD);
                    results.rtmMat4x4da = matrix_mul(rtmLeftD, rtmRightD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmMat4x4da));
                });
        }

        void qvv_multiply(ankerl::nanobench::Bench& bench)
        {
            const rtm::qvvf left =
                rtm::qvv_set(rtm::quat_set(0.0f, 0.0f, 0.174108f, 0.984727f),
                    rtm::vector_set(1.0f, 2.0f, 3.0f, 0.0f),
                    rtm::vector_set(1.5f, 0.75f, 2.0f, 0.0f));
            const rtm::qvvf right =
                rtm::qvv_set(rtm::quat_set(0.0f, 0.247404f, 0.0f, 0.968912f),
                    rtm::vector_set(4.0f, 5.0f, 6.0f, 0.0f),
                    rtm::vector_set(0.5f, 1.25f, 1.0f, 0.0f));
            const rtm::qvvd leftD =
                rtm::qvv_set(rtm::quat_set(0.0, 0.0, 0.174108, 0.984727),
                    rtm::vector_set(1.0, 2.0, 3.0, 0.0),
                    rtm::vector_set(1.5, 0.75, 2.0, 0.0));
            const rtm::qvvd rightD =
                rtm::qvv_set(rtm::quat_set(0.0, 0.247404, 0.0, 0.968912),
                    rtm::vector_set(4.0, 5.0, 6.0, 0.0),
                    rtm::vector_set(0.5, 1.25, 1.0, 0.0));

            bench.run("QVV QVV multiply rtm::qvvf",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(left);
                    ankerl::nanobench::doNotOptimizeAway(right);
                    results.rtmQvvfa = qvv_mul(left, right);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmQvvfa));
                });

            bench.run("QVV QVV multiply rtm::qvvd",
                [&]
                {
                    using namespace rtm;
                    ankerl::nanobench::doNotOptimizeAway(leftD);
                    ankerl::nanobench::doNotOptimizeAway(rightD);
                    results.rtmQvvda = qvv_mul(leftD, rightD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.rtmQvvda));
                });
        }
    }  // namespace matrices
}  // namespace mathbench

inline void test_camera_matrix_funcs()
{
    auto smMat = DirectX::SimpleMath::Matrix::CreateOrthographic(
        0.5f, 1.0f, 0.1f, 100.0f);
    auto glmMat = glm::ortho(0.5f, 1.0f, 0.1f, 100.0f);
    auto rtmMat = rtm::camera::ortho_rh(0.5f, 1.0f, 0.1f, 100.0f);

    printf("GLM: \n");
    for (int x = 0; x < 4; ++x)
    {
        for (int y = 0; y < 4; ++y)
        {
            printf("%f ", glmMat[x][y]);
        }
        printf("\n");
    }

    printf("SimpleMath:\n");
    printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
        float(smMat._11), float(smMat._12), float(smMat._13), float(smMat._14),
        float(smMat._21), float(smMat._22), float(smMat._23), float(smMat._24),
        float(smMat._31), float(smMat._32), float(smMat._33), float(smMat._34),
        float(smMat._41), float(smMat._42), float(smMat._43), float(smMat._44));

    printf("RTM: \n");
    printf("%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n%f %f %f %f\n",
        float(rtm::vector_get_x(rtmMat.x_axis)),
        float(rtm::vector_get_y(rtmMat.x_axis)),
        float(rtm::vector_get_z(rtmMat.x_axis)),
        float(rtm::vector_get_w(rtmMat.x_axis)),
        float(rtm::vector_get_x(rtmMat.y_axis)),
        float(rtm::vector_get_y(rtmMat.y_axis)),
        float(rtm::vector_get_z(rtmMat.y_axis)),
        float(rtm::vector_get_w(rtmMat.y_axis)),
        float(rtm::vector_get_x(rtmMat.z_axis)),
        float(rtm::vector_get_y(rtmMat.z_axis)),
        float(rtm::vector_get_z(rtmMat.z_axis)),
        float(rtm::vector_get_w(rtmMat.z_axis)),
        float(rtm::vector_get_x(rtmMat.w_axis)),
        float(rtm::vector_get_y(rtmMat.w_axis)),
        float(rtm::vector_get_z(rtmMat.w_axis)),
        float(rtm::vector_get_w(rtmMat.w_axis)));
}

namespace mathbench::report
{
    struct ranked_result
    {
        std::string capability;
        std::string winner;
        double winnerNanoseconds{};
        double moveNanoseconds{};
        std::size_t moveRank{};
    };

    std::vector<ranked_result> rankings;

    std::string library_name(std::string_view benchmarkName)
    {
        if (benchmarkName.find("move::") != std::string_view::npos)
        {
            return "move::math";
        }
        if (benchmarkName.find("SimpleMath") != std::string_view::npos)
        {
            return "SimpleMath";
        }
        if (benchmarkName.find("glm") != std::string_view::npos)
        {
            return "GLM";
        }
        if (benchmarkName.find("DXM") != std::string_view::npos ||
            benchmarkName.find("DirectX") != std::string_view::npos)
        {
            return "DirectXMath";
        }
        if (benchmarkName.find("Vectormath") != std::string_view::npos)
        {
            return "Sony Vectormath";
        }
        if (benchmarkName.find("rtm::") != std::string_view::npos)
        {
            return "RTM";
        }
        return {};
    }

    bool is_double_precision(std::string_view benchmarkName)
    {
        return benchmarkName.find("double") != std::string_view::npos ||
               benchmarkName.find("vec2d") != std::string_view::npos ||
               benchmarkName.find("vec3d") != std::string_view::npos ||
               benchmarkName.find("vec4d") != std::string_view::npos ||
               benchmarkName.find("vector4d") != std::string_view::npos ||
               benchmarkName.find("quatd") != std::string_view::npos ||
               benchmarkName.find("qvvd") != std::string_view::npos ||
               benchmarkName.find("matrix3x4d") != std::string_view::npos ||
               benchmarkName.find("matrix4x4d") != std::string_view::npos ||
               benchmarkName.find("Mat4x4d") != std::string_view::npos;
    }

    std::string scenario_name(
        std::string_view capability, std::string_view benchmarkName)
    {
        if (benchmarkName.find(" hit ") != std::string_view::npos)
        {
            return std::string(capability) + " — hit";
        }
        if (benchmarkName.find(" miss ") != std::string_view::npos)
        {
            return std::string(capability) + " — miss";
        }
        return std::string(capability);
    }

    void collect_rankings(
        std::string_view capability, const ankerl::nanobench::Bench& bench)
    {
        using measure = ankerl::nanobench::Result::Measure;
        std::map<std::string, std::map<std::string, double>> groupedResults;

        for (const auto& result : bench.results())
        {
            const auto& benchmarkName = result.config().mBenchmarkName;
            if (is_double_precision(benchmarkName))
            {
                continue;
            }

            const auto library = library_name(benchmarkName);
            if (library.empty())
            {
                continue;
            }

            const auto scenario = scenario_name(capability, benchmarkName);
            const double nanoseconds = result.median(measure::elapsed) * 1.0e9;
            auto [entry, inserted] =
                groupedResults[scenario].try_emplace(library, nanoseconds);
            if (!inserted)
            {
                entry->second = std::min(entry->second, nanoseconds);
            }
        }

        for (const auto& [scenario, libraries] : groupedResults)
        {
            std::vector<std::pair<std::string, double>> ordered(
                libraries.begin(), libraries.end());
            std::sort(ordered.begin(), ordered.end(),
                [](const auto& lhs, const auto& rhs)
                {
                    return lhs.second < rhs.second;
                });

            const auto move = std::find_if(ordered.begin(), ordered.end(),
                [](const auto& entry)
                {
                    return entry.first == "move::math";
                });
            if (ordered.empty() || move == ordered.end())
            {
                continue;
            }

            rankings.push_back({scenario, ordered.front().first,
                ordered.front().second, move->second,
                static_cast<std::size_t>(
                    std::distance(ordered.begin(), move) + 1)});
        }
    }

    template <typename Function>
    void run_capability(
        std::string_view title, bool summarize, Function&& function)
    {
        ankerl::nanobench::Bench bench;
        bench.title(std::string(title))
            .unit("op")
            .epochs(15)
            .warmup(1000)
            .minEpochTime(std::chrono::milliseconds(5));
        function(bench);
        if (summarize)
        {
            collect_rankings(title, bench);
        }
    }

    void print_rankings()
    {
        std::cout
            << "\n## Capability winners (single-precision implementations)\n\n"
            << "| Capability | Fastest library | Fastest ns/op | "
               "move::math ns/op | Move rank | Move gap |\n"
            << "|:--|:--|--:|--:|--:|--:|\n";

        std::sort(rankings.begin(), rankings.end(),
            [](const auto& lhs, const auto& rhs)
            {
                return lhs.capability < rhs.capability;
            });
        for (const auto& result : rankings)
        {
            const double gap =
                result.winnerNanoseconds == 0.0
                    ? 0.0
                    : (result.moveNanoseconds / result.winnerNanoseconds -
                          1.0) *
                          100.0;
            std::cout << "| " << result.capability << " | " << result.winner
                      << " | " << std::fixed << std::setprecision(2)
                      << result.winnerNanoseconds << " | "
                      << result.moveNanoseconds << " | " << result.moveRank
                      << " | " << gap << "% |\n";
        }
    }
}  // namespace mathbench::report

int main()
{
    try
    {
        using mathbench::report::run_capability;

        run_capability("Game loop / Normalize direction", true,
            mathbench::workloads::normalize_direction);
        run_capability("Game loop / Particle integration", true,
            mathbench::workloads::particle_integration);
        run_capability("Graphics / Camera basis", true,
            mathbench::workloads::camera_basis);
        run_capability("Graphics / Quaternion direction rotation", true,
            mathbench::workloads::rotate_direction);
        run_capability("Geometry / Ray-AABB intersection", true,
            mathbench::workloads::ray_aabb_intersection);
        run_capability("Geometry / Ray-triangle intersection", true,
            mathbench::workloads::ray_triangle_intersection);

        run_capability("Transforms / Model-matrix construction", false,
            mathbench::matrices::construct_model_matrix);
        run_capability("Camera / View-matrix construction", false,
            mathbench::matrices::construct_view_matrix);
        run_capability("Camera / Perspective-matrix construction", false,
            mathbench::matrices::construct_perspective_projection_matrix);
        run_capability("Camera / Orthographic-matrix construction", false,
            mathbench::matrices::ortho_projection_matrix);
        run_capability("Transforms / Vector-matrix multiply throughput", true,
            mathbench::matrices::vector_matrix_multiply);
        run_capability("Transforms / QVV point transform", false,
            mathbench::matrices::qvv_point_multiply);
        run_capability("Transforms / Matrix-matrix multiply throughput", true,
            mathbench::matrices::matrix_matrix_multiply);
        run_capability("Transforms / QVV composition", false,
            mathbench::matrices::qvv_multiply);

        mathbench::report::print_rankings();
        printf("Benchmark complete\n");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
