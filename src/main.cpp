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
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <optional>
#include <stdexcept>
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
#include <mv/math/Math.hpp>
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

        // mv::math current API
        mv::math::Vec3f mvCurrentVec3f;
        mv::math::Vec3d mvCurrentVec3d;
        mv::math::Vec4f mvCurrentVec4f;
        mv::math::Vec4d mvCurrentVec4d;
        mv::math::Mat4f mvCurrentMat4f;
        mv::math::Mat4d mvCurrentMat4d;

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

    namespace workloads
    {
        constexpr std::size_t intersectionBatchSize = 256;

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
            benchmark_ray_aabb_case<mvf_ops>(bench,
                "Ray-AABB hit mv::math::Vec3f", mvf_vec(0.25f, 0.1f, -4.0f),
                mvf_vec(0.1f, 0.05f, 1.0f), mvf_vec(-1.0f, -1.0f, -1.0f),
                mvf_vec(1.0f, 1.0f, 1.0f));
            benchmark_ray_aabb_case<mvf_ops>(bench,
                "Ray-AABB miss mv::math::Vec3f", mvf_vec(4.0f, 4.0f, -4.0f),
                mvf_vec(0.1f, 0.05f, 1.0f), mvf_vec(-1.0f, -1.0f, -1.0f),
                mvf_vec(1.0f, 1.0f, 1.0f));

            using mvd_ops = geometry::move_ops<double>;
            using mvd_vec = mvd_ops::vector;
            benchmark_ray_aabb_case<mvd_ops>(bench,
                "Ray-AABB hit mv::math::Vec3d", mvd_vec(0.25, 0.1, -4.0),
                mvd_vec(0.1, 0.05, 1.0), mvd_vec(-1.0, -1.0, -1.0),
                mvd_vec(1.0, 1.0, 1.0));
            benchmark_ray_aabb_case<mvd_ops>(bench,
                "Ray-AABB miss mv::math::Vec3d", mvd_vec(4.0, 4.0, -4.0),
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
                "Ray-triangle hit mv::math::Vec3f",
                mvf_vec(0.25f, 0.25f, -2.0f), mvf_vec(0.0f, 0.0f, 1.0f),
                mvf_vec(0.0f, 0.0f, 0.0f), mvf_vec(1.0f, 0.0f, 0.0f),
                mvf_vec(0.0f, 1.0f, 0.0f));
            benchmark_ray_triangle_case<mvf_ops>(bench,
                "Ray-triangle miss mv::math::Vec3f", mvf_vec(1.5f, 1.5f, -2.0f),
                mvf_vec(0.0f, 0.0f, 1.0f), mvf_vec(0.0f, 0.0f, 0.0f),
                mvf_vec(1.0f, 0.0f, 0.0f), mvf_vec(0.0f, 1.0f, 0.0f));

            using mvd_ops = geometry::move_ops<double>;
            using mvd_vec = mvd_ops::vector;
            benchmark_ray_triangle_case<mvd_ops>(bench,
                "Ray-triangle hit mv::math::Vec3d", mvd_vec(0.25, 0.25, -2.0),
                mvd_vec(0.0, 0.0, 1.0), mvd_vec(0.0, 0.0, 0.0),
                mvd_vec(1.0, 0.0, 0.0), mvd_vec(0.0, 1.0, 0.0));
            benchmark_ray_triangle_case<mvd_ops>(bench,
                "Ray-triangle miss mv::math::Vec3d", mvd_vec(1.5, 1.5, -2.0),
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

        template <typename Ops>
        void benchmark_ray_aabb_batch(
            ankerl::nanobench::Bench& bench, const char* name)
        {
            using scalar = typename Ops::scalar;
            using vector = typename Ops::vector;

            struct ray_case
            {
                vector origin;
                vector direction;
            };

            std::vector<ray_case> rays;
            rays.reserve(intersectionBatchSize);
            for (std::size_t index = 0; index < intersectionBatchSize; ++index)
            {
                const bool shouldHit = index % 3 != 0;
                const scalar x =
                    shouldHit ? (scalar(index % 11) - scalar(5)) * scalar(0.08)
                              : scalar(3.0) + scalar(index % 7) * scalar(0.15);
                const scalar y =
                    (scalar((index * 3) % 9) - scalar(4)) * scalar(0.07);
                const scalar z =
                    -(scalar(2.0) + scalar(index % 13) * scalar(0.2));
                const scalar dx =
                    (scalar(index % 5) - scalar(2)) * scalar(0.015);
                rays.push_back({Ops::make(x, y, z),
                    Ops::make(dx, scalar(0.025), scalar(1))});
            }

            const vector boundsMin =
                Ops::make(-scalar(1), -scalar(1), -scalar(1));
            const vector boundsMax = Ops::make(scalar(1), scalar(1), scalar(1));
            bench.run(name,
                [rays = std::move(rays), boundsMin, boundsMax]() mutable
                {
                    const auto* const rayData = rays.data();
                    ankerl::nanobench::doNotOptimizeAway(rayData);
                    scalar distanceSum{};
                    std::size_t hitCount{};
                    for (std::size_t index = 0; index < rays.size(); ++index)
                    {
                        const auto hit = geometry::intersect_ray_aabb<Ops>(
                            rayData[index].origin, rayData[index].direction,
                            boundsMin, boundsMax);
                        distanceSum += hit.distance;
                        hitCount += hit.intersects ? 1U : 0U;
                    }
                    ankerl::nanobench::doNotOptimizeAway(distanceSum);
                    ankerl::nanobench::doNotOptimizeAway(hitCount);
                });
        }

        void ray_aabb_batch(ankerl::nanobench::Bench& bench)
        {
            bench.batch(intersectionBatchSize);
            benchmark_ray_aabb_batch<geometry::simplemath_ops>(
                bench, "Ray-AABB mixed batch SimpleMath");
            benchmark_ray_aabb_batch<geometry::glm_ops>(
                bench, "Ray-AABB mixed batch glm");
            benchmark_ray_aabb_batch<geometry::directxmath_ops>(
                bench, "Ray-AABB mixed batch DXM");
            benchmark_ray_aabb_batch<geometry::vectormath_ops>(
                bench, "Ray-AABB mixed batch Vectormath");
            benchmark_ray_aabb_batch<geometry::move_ops<float>>(
                bench, "Ray-AABB mixed batch mv::math::Vec3f");
            benchmark_ray_aabb_batch<geometry::move_ops<double>>(
                bench, "Ray-AABB mixed batch mv::math::Vec3d");
            benchmark_ray_aabb_batch<geometry::rtm_ops<float>>(
                bench, "Ray-AABB mixed batch rtm::vector4f");
            benchmark_ray_aabb_batch<geometry::rtm_ops<double>>(
                bench, "Ray-AABB mixed batch rtm::vector4d");
        }

        template <typename Ops>
        void benchmark_ray_triangle_batch(
            ankerl::nanobench::Bench& bench, const char* name)
        {
            using scalar = typename Ops::scalar;
            using vector = typename Ops::vector;

            struct ray_case
            {
                vector origin;
                vector direction;
            };

            std::vector<ray_case> rays;
            rays.reserve(intersectionBatchSize);
            for (std::size_t index = 0; index < intersectionBatchSize; ++index)
            {
                const bool shouldHit = index % 3 != 0;
                const scalar x =
                    shouldHit ? scalar(0.1) + scalar(index % 5) * scalar(0.08)
                              : scalar(1.2);
                const scalar y =
                    shouldHit ? scalar(0.1) + scalar(index % 3) * scalar(0.06)
                              : scalar(1.1);
                const scalar z =
                    -(scalar(1.0) + scalar(index % 17) * scalar(0.15));
                rays.push_back({Ops::make(x, y, z),
                    Ops::make(scalar(0), scalar(0), scalar(1))});
            }

            const vector vertex0 = Ops::make(scalar(0), scalar(0), scalar(0));
            const vector vertex1 = Ops::make(scalar(1), scalar(0), scalar(0));
            const vector vertex2 = Ops::make(scalar(0), scalar(1), scalar(0));
            bench.run(name,
                [rays = std::move(rays), vertex0, vertex1, vertex2]() mutable
                {
                    const auto* const rayData = rays.data();
                    ankerl::nanobench::doNotOptimizeAway(rayData);
                    scalar distanceSum{};
                    std::size_t hitCount{};
                    for (std::size_t index = 0; index < rays.size(); ++index)
                    {
                        const auto hit = geometry::intersect_ray_triangle<Ops>(
                            rayData[index].origin, rayData[index].direction,
                            vertex0, vertex1, vertex2);
                        distanceSum += hit.distance;
                        hitCount += hit.intersects ? 1U : 0U;
                    }
                    ankerl::nanobench::doNotOptimizeAway(distanceSum);
                    ankerl::nanobench::doNotOptimizeAway(hitCount);
                });
        }

        void ray_triangle_batch(ankerl::nanobench::Bench& bench)
        {
            bench.batch(intersectionBatchSize);
            benchmark_ray_triangle_batch<geometry::simplemath_ops>(
                bench, "Ray-triangle mixed batch SimpleMath");
            benchmark_ray_triangle_batch<geometry::glm_ops>(
                bench, "Ray-triangle mixed batch glm");
            benchmark_ray_triangle_batch<geometry::directxmath_ops>(
                bench, "Ray-triangle mixed batch DXM");
            benchmark_ray_triangle_batch<geometry::vectormath_ops>(
                bench, "Ray-triangle mixed batch Vectormath");
            benchmark_ray_triangle_batch<geometry::move_ops<float>>(
                bench, "Ray-triangle mixed batch mv::math::Vec3f");
            benchmark_ray_triangle_batch<geometry::move_ops<double>>(
                bench, "Ray-triangle mixed batch mv::math::Vec3d");
            benchmark_ray_triangle_batch<geometry::rtm_ops<float>>(
                bench, "Ray-triangle mixed batch rtm::vector4f");
            benchmark_ray_triangle_batch<geometry::rtm_ops<double>>(
                bench, "Ray-triangle mixed batch rtm::vector4d");
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

            mv::math::Vec3f mvDirection(3.0f, -2.0f, 7.0f);
            bench.run("Normalize direction mv::math::Vec3f checked",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirection);
                    const auto direction = mv::math::TryNormalize(mvDirection);
                    if (!direction)
                    {
                        std::abort();
                    }
                    results.mvCurrentVec3f = direction->Vector();
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3f));
                });

            mv::math::Vec3d mvDirectionD(3.0, -2.0, 7.0);
            bench.run("Normalize direction mv::math::Vec3d checked",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirectionD);
                    const auto direction = mv::math::TryNormalize(mvDirectionD);
                    if (!direction)
                    {
                        std::abort();
                    }
                    results.mvCurrentVec3d = direction->Vector();
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3d));
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

            mv::math::Vec3f mvPosition(12.0f, 8.0f, -4.0f);
            mv::math::Vec3f mvVelocity(2.0f, 5.0f, -1.0f);
            mv::math::Vec3f mvAcceleration(0.0f, -9.81f, 0.0f);
            bench.run("Particle integration mv::math::Vec3f",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvPosition);
                    ankerl::nanobench::doNotOptimizeAway(mvVelocity);
                    ankerl::nanobench::doNotOptimizeAway(mvAcceleration);
                    const auto velocity =
                        mvVelocity + mvAcceleration * deltaTime;
                    results.mvCurrentVec3f = mvPosition + velocity * deltaTime;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3f));
                });

            mv::math::Vec3d mvPositionD(12.0, 8.0, -4.0);
            mv::math::Vec3d mvVelocityD(2.0, 5.0, -1.0);
            mv::math::Vec3d mvAccelerationD(0.0, -9.81, 0.0);
            bench.run("Particle integration mv::math::Vec3d",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvPositionD);
                    ankerl::nanobench::doNotOptimizeAway(mvVelocityD);
                    ankerl::nanobench::doNotOptimizeAway(mvAccelerationD);
                    const auto velocity =
                        mvVelocityD + mvAccelerationD * deltaTimeD;
                    results.mvCurrentVec3d =
                        mvPositionD + velocity * deltaTimeD;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3d));
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

            mv::math::Vec3f mvEye(4.0f, 2.0f, -8.0f);
            mv::math::Vec3f mvTarget(1.0f, 3.0f, 2.0f);
            const mv::math::Direction3f mvUp = mv::math::Direction3f::AxisY();
            bench.run("Camera basis mv::math::Vec3f checked",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvEye);
                    ankerl::nanobench::doNotOptimizeAway(mvTarget);
                    ankerl::nanobench::doNotOptimizeAway(mvUp);
                    const auto forward =
                        mv::math::TryNormalize(mvTarget - mvEye);
                    if (!forward)
                    {
                        std::abort();
                    }
                    const auto right = mv::math::TryNormalize(
                        mv::math::Cross(mvUp.Vector(), forward->Vector()));
                    if (!right)
                    {
                        std::abort();
                    }
                    const auto correctedUp =
                        mv::math::Cross(forward->Vector(), right->Vector());
                    results.mvCurrentVec3f = right->Vector() + correctedUp;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3f));
                });

            mv::math::Vec3d mvEyeD(4.0, 2.0, -8.0);
            mv::math::Vec3d mvTargetD(1.0, 3.0, 2.0);
            const mv::math::Direction3d mvUpD = mv::math::Direction3d::AxisY();
            bench.run("Camera basis mv::math::Vec3d checked",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvEyeD);
                    ankerl::nanobench::doNotOptimizeAway(mvTargetD);
                    ankerl::nanobench::doNotOptimizeAway(mvUpD);
                    const auto forward =
                        mv::math::TryNormalize(mvTargetD - mvEyeD);
                    if (!forward)
                    {
                        std::abort();
                    }
                    const auto right = mv::math::TryNormalize(
                        mv::math::Cross(mvUpD.Vector(), forward->Vector()));
                    if (!right)
                    {
                        std::abort();
                    }
                    const auto correctedUp =
                        mv::math::Cross(forward->Vector(), right->Vector());
                    results.mvCurrentVec3d = right->Vector() + correctedUp;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3d));
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

            mv::math::Vec3f mvDirection(0.25f, 0.5f, 1.0f);
            const auto mvRotation = mv::math::Rotation3f::FromAxisAngle(
                mv::math::Direction3f::AxisY(), mv::math::Radiansf(0.35f));
            bench.run("Rotate direction mv::math::Rotation3f",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirection);
                    ankerl::nanobench::doNotOptimizeAway(mvRotation);
                    results.mvCurrentVec3f =
                        mv::math::Rotate(mvRotation, mvDirection);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3f));
                });

            mv::math::Vec3d mvDirectionD(0.25, 0.5, 1.0);
            const auto mvRotationD = mv::math::Rotation3d::FromAxisAngle(
                mv::math::Direction3d::AxisY(), mv::math::Radiansd(0.35));
            bench.run("Rotate direction mv::math::Rotation3d",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvDirectionD);
                    ankerl::nanobench::doNotOptimizeAway(mvRotationD);
                    results.mvCurrentVec3d =
                        mv::math::Rotate(mvRotationD, mvDirectionD);
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec3d));
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

            bench.run("Construct model matrix mv::math::TrsTransform3f",
                [&]
                {
                    makeFloatInputsOpaque();
                    results.mvCurrentMat4f = mv::math::ToMat4(
                        mv::math::TrsTransform3f(mv::math::Vec3f(tx, ty, tz),
                            mv::math::Rotation3f::FromEuler(
                                {mv::math::Radiansf(angle),
                                    mv::math::Radiansf(angle),
                                    mv::math::Radiansf(angle),
                                    mv::math::EulerOrder::XYZ}),
                            mv::math::Vec3f(sx, sy, sz)));

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4f));
                });

            bench.run("Construct model matrix mv::math::TrsTransform3d",
                [&]
                {
                    makeDoubleInputsOpaque();
                    results.mvCurrentMat4d = mv::math::ToMat4(
                        mv::math::TrsTransform3d(mv::math::Vec3d(txD, tyD, tzD),
                            mv::math::Rotation3d::FromEuler(
                                {mv::math::Radiansd(angleD),
                                    mv::math::Radiansd(angleD),
                                    mv::math::Radiansd(angleD),
                                    mv::math::EulerOrder::XYZ}),
                            mv::math::Vec3d(sxD, syD, szD)));

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4d));
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
            const mv::math::Point3f mvEye(1.0f, 2.0f, 3.0f);
            const mv::math::Point3f mvTarget(4.0f, 5.0f, 6.0f);
            const mv::math::Direction3f mvUp =
                mv::math::Direction3f::FromOr(mv::math::Vec3f(7.0f, 8.0f, 9.0f),
                    mv::math::Direction3f::AxisY());
            const mv::math::Point3d mvEyeD(1.0, 2.0, 3.0);
            const mv::math::Point3d mvTargetD(4.0, 5.0, 6.0);
            const mv::math::Direction3d mvUpD = mv::math::Direction3d::FromOr(
                mv::math::Vec3d(7.0, 8.0, 9.0), mv::math::Direction3d::AxisY());
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

            bench.run("Construct view matrix mv::math checked (float)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvEye);
                    ankerl::nanobench::doNotOptimizeAway(mvTarget);
                    ankerl::nanobench::doNotOptimizeAway(mvUp);
                    const auto view =
                        mv::math::TryLookAtLH(mvEye, mvTarget, mvUp);
                    if (!view)
                    {
                        std::abort();
                    }
                    results.mvCurrentMat4f = *view;

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4f));
                });

            bench.run("Construct view matrix mv::math checked (double)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvEyeD);
                    ankerl::nanobench::doNotOptimizeAway(mvTargetD);
                    ankerl::nanobench::doNotOptimizeAway(mvUpD);
                    const auto view =
                        mv::math::TryLookAtLH(mvEyeD, mvTargetD, mvUpD);
                    if (!view)
                    {
                        std::abort();
                    }
                    results.mvCurrentMat4d = *view;

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4d));
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

            bench.run("Construct perspective matrix mv::math checked (float)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(fov);
                    ankerl::nanobench::doNotOptimizeAway(aspect);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    const auto projection = mv::math::TryPerspectiveFov(
                        mv::math::Radiansf(fov), aspect, nearPlane, farPlane,
                        mv::math::Handedness::Left,
                        mv::math::ClipDepth::ZeroToOne,
                        mv::math::DepthDirection::Forward);
                    if (!projection)
                    {
                        std::abort();
                    }
                    results.mvCurrentMat4f = *projection;

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4f));
                });

            bench.run("Construct perspective matrix mv::math checked (double)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(fovD);
                    ankerl::nanobench::doNotOptimizeAway(aspectD);
                    ankerl::nanobench::doNotOptimizeAway(nearPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(farPlaneD);
                    const auto projection = mv::math::TryPerspectiveFov(
                        mv::math::Radiansd(fovD), aspectD, nearPlaneD,
                        farPlaneD, mv::math::Handedness::Left,
                        mv::math::ClipDepth::ZeroToOne,
                        mv::math::DepthDirection::Forward);
                    if (!projection)
                    {
                        std::abort();
                    }
                    results.mvCurrentMat4d = *projection;

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4d));
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

            bench.run("Construct ortho matrix mv::math checked (float)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(width);
                    ankerl::nanobench::doNotOptimizeAway(height);
                    ankerl::nanobench::doNotOptimizeAway(nearPlane);
                    ankerl::nanobench::doNotOptimizeAway(farPlane);
                    const auto projection =
                        mv::math::TryOrthographicOffCenter(-width * 0.5f,
                            width * 0.5f, -height * 0.5f, height * 0.5f,
                            nearPlane, farPlane, mv::math::Handedness::Left,
                            mv::math::ClipDepth::ZeroToOne,
                            mv::math::DepthDirection::Forward);
                    if (!projection)
                    {
                        std::abort();
                    }
                    results.mvCurrentMat4f = *projection;

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4f));
                });

            bench.run("Construct ortho matrix mv::math checked (double)",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(widthD);
                    ankerl::nanobench::doNotOptimizeAway(heightD);
                    ankerl::nanobench::doNotOptimizeAway(nearPlaneD);
                    ankerl::nanobench::doNotOptimizeAway(farPlaneD);
                    const auto projection =
                        mv::math::TryOrthographicOffCenter(-widthD * 0.5,
                            widthD * 0.5, -heightD * 0.5, heightD * 0.5,
                            nearPlaneD, farPlaneD, mv::math::Handedness::Left,
                            mv::math::ClipDepth::ZeroToOne,
                            mv::math::DepthDirection::Forward);
                    if (!projection)
                    {
                        std::abort();
                    }
                    results.mvCurrentMat4d = *projection;

                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4d));
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
            const mv::math::Vec4f mvInput(1.0f, 2.0f, 3.0f, 1.0f);
            const mv::math::Mat4f mvMatrix = mv::math::ToMat4(
                mv::math::RigidTransform3f(mv::math::Rotation3f::FromAxisAngle(
                                               mv::math::Direction3f::AxisX(),
                                               mv::math::Radiansf(0.35f)),
                    mv::math::Vec3f::Zero()));
            const mv::math::Vec4d mvInputD(1.0, 2.0, 3.0, 1.0);
            const mv::math::Mat4d mvMatrixD = mv::math::ToMat4(
                mv::math::RigidTransform3d(mv::math::Rotation3d::FromAxisAngle(
                                               mv::math::Direction3d::AxisX(),
                                               mv::math::Radiansd(0.35)),
                    mv::math::Vec3d::Zero()));
            const rtm::vector4f rtmInput =
                rtm::vector_set(1.0f, 2.0f, 3.0f, 1.0f);
            const rtm::matrix4x4f rtmMatrix =
                rtm::matrix_cast(rtm::matrix_from_qvv(
                    rtm::quat_from_axis_angle(
                        rtm::vector_set(1.0f, 0.0f, 0.0f, 0.0f), 0.35f),
                    rtm::vector_zero(), rtm::vector_set(1.0f)));
            const rtm::vector4d rtmInputD = rtm::vector_set(1.0, 2.0, 3.0, 1.0);
            const rtm::matrix4x4d rtmMatrixD =
                rtm::matrix_cast(rtm::matrix_from_qvv(
                    rtm::quat_from_axis_angle(
                        rtm::vector_set(1.0, 0.0, 0.0, 0.0), 0.35),
                    rtm::vector_zero(), rtm::vector_set(1.0)));

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

            bench.run("Vector matrix multiply mv::math::Mat4f",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvInput);
                    ankerl::nanobench::doNotOptimizeAway(mvMatrix);
                    results.mvCurrentVec4f = mvInput * mvMatrix;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec4f));
                });

            bench.run("Vector matrix multiply mv::math::Mat4d",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvInputD);
                    ankerl::nanobench::doNotOptimizeAway(mvMatrixD);
                    results.mvCurrentVec4d = mvInputD * mvMatrixD;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentVec4d));
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
            const mv::math::Mat4f mvLeft = mv::math::ToMat4(
                mv::math::RigidTransform3f(mv::math::Rotation3f::FromAxisAngle(
                                               mv::math::Direction3f::AxisX(),
                                               mv::math::Radiansf(0.35f)),
                    mv::math::Vec3f::Zero()));
            const mv::math::Mat4f mvRight = mv::math::ToMat4(
                mv::math::RigidTransform3f(mv::math::Rotation3f::Identity(),
                    mv::math::Vec3f(1.0f, 2.0f, 3.0f)));
            const mv::math::Mat4d mvLeftD = mv::math::ToMat4(
                mv::math::RigidTransform3d(mv::math::Rotation3d::FromAxisAngle(
                                               mv::math::Direction3d::AxisX(),
                                               mv::math::Radiansd(0.35)),
                    mv::math::Vec3d::Zero()));
            const mv::math::Mat4d mvRightD = mv::math::ToMat4(
                mv::math::RigidTransform3d(mv::math::Rotation3d::Identity(),
                    mv::math::Vec3d(1.0, 2.0, 3.0)));
            const rtm::matrix4x4f rtmLeft =
                rtm::matrix_cast(rtm::matrix_from_qvv(
                    rtm::quat_from_axis_angle(
                        rtm::vector_set(1.0f, 0.0f, 0.0f, 0.0f), 0.35f),
                    rtm::vector_zero(), rtm::vector_set(1.0f)));
            const rtm::matrix4x4f rtmRight =
                rtm::matrix_cast(rtm::matrix_from_qvv(rtm::quat_identity(),
                    rtm::vector_set(1.0f, 2.0f, 3.0f, 0.0f),
                    rtm::vector_set(1.0f)));
            const rtm::matrix4x4d rtmLeftD =
                rtm::matrix_cast(rtm::matrix_from_qvv(
                    rtm::quat_from_axis_angle(
                        rtm::vector_set(1.0, 0.0, 0.0, 0.0), 0.35),
                    rtm::vector_zero(), rtm::vector_set(1.0)));
            const rtm::matrix4x4d rtmRightD =
                rtm::matrix_cast(rtm::matrix_from_qvv(rtm::quat_identity(),
                    rtm::vector_set(1.0, 2.0, 3.0, 0.0), rtm::vector_set(1.0)));

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

            bench.run("Matrix matrix multiply mv::math::Mat4f",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvLeft);
                    ankerl::nanobench::doNotOptimizeAway(mvRight);
                    results.mvCurrentMat4f = mvLeft * mvRight;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4f));
                });

            bench.run("Matrix matrix multiply mv::math::Mat4d",
                [&]
                {
                    ankerl::nanobench::doNotOptimizeAway(mvLeftD);
                    ankerl::nanobench::doNotOptimizeAway(mvRightD);
                    results.mvCurrentMat4d = mvLeftD * mvRightD;
                    ankerl::nanobench::doNotOptimizeAway(
                        std::as_const(results.mvCurrentMat4d));
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

namespace mathbench::verification
{
    struct vector3_value
    {
        double x;
        double y;
        double z;
    };

    void require_vector(std::string_view label, vector3_value actual,
        vector3_value expected, double tolerance = 1.0e-5)
    {
        const auto close = [tolerance](double lhs, double rhs)
        {
            return std::abs(lhs - rhs) <= tolerance;
        };
        if (!close(actual.x, expected.x) || !close(actual.y, expected.y) ||
            !close(actual.z, expected.z))
        {
            throw std::runtime_error(
                std::string(label) +
                " failed semantic parity validation: actual=(" +
                std::to_string(actual.x) + ", " + std::to_string(actual.y) +
                ", " + std::to_string(actual.z) + "), expected=(" +
                std::to_string(expected.x) + ", " + std::to_string(expected.y) +
                ", " + std::to_string(expected.z) +
                "), tolerance=" + std::to_string(tolerance));
        }
    }

    template <typename Ops>
    void verify_intersections(std::string_view label)
    {
        using scalar = typename Ops::scalar;
        const auto zero = Ops::make(scalar(0), scalar(0), scalar(0));
        const auto one = Ops::make(scalar(1), scalar(1), scalar(1));
        const auto negativeOne = Ops::make(-scalar(1), -scalar(1), -scalar(1));
        const auto forward = Ops::make(scalar(0), scalar(0), scalar(1));

        const auto boxHit = geometry::intersect_ray_aabb<Ops>(
            Ops::make(scalar(0.25), scalar(0.1), -scalar(4)), forward,
            negativeOne, one);
        const auto boxMiss = geometry::intersect_ray_aabb<Ops>(
            Ops::make(scalar(4), scalar(4), -scalar(4)), forward, negativeOne,
            one);
        const auto boxInside =
            geometry::intersect_ray_aabb<Ops>(zero, forward, negativeOne, one);
        const auto triangleHit = geometry::intersect_ray_triangle<Ops>(
            Ops::make(scalar(0.25), scalar(0.25), -scalar(2)), forward, zero,
            Ops::make(scalar(1), scalar(0), scalar(0)),
            Ops::make(scalar(0), scalar(1), scalar(0)));
        const auto triangleMiss = geometry::intersect_ray_triangle<Ops>(
            Ops::make(scalar(1.5), scalar(1.5), -scalar(2)), forward, zero,
            Ops::make(scalar(1), scalar(0), scalar(0)),
            Ops::make(scalar(0), scalar(1), scalar(0)));
        const auto triangleParallel = geometry::intersect_ray_triangle<Ops>(
            Ops::make(scalar(0.25), scalar(0.25), -scalar(2)),
            Ops::make(scalar(1), scalar(0), scalar(0)), zero,
            Ops::make(scalar(1), scalar(0), scalar(0)),
            Ops::make(scalar(0), scalar(1), scalar(0)));

        const double tolerance =
            std::is_same_v<scalar, float> ? 1.0e-3 : 1.0e-10;
        const auto near = [tolerance](scalar lhs, double rhs)
        {
            return std::abs(static_cast<double>(lhs) - rhs) <= tolerance;
        };
        if (!boxHit.intersects || !near(boxHit.distance, 3.0) ||
            boxMiss.intersects || !boxInside.intersects ||
            !near(boxInside.distance, 0.0) || !triangleHit.intersects ||
            !near(triangleHit.distance, 2.0) || triangleMiss.intersects ||
            triangleParallel.intersects)
        {
            throw std::runtime_error(
                std::string(label) +
                " failed intersection validation: box "
                "hit=" +
                std::to_string(boxHit.intersects) +
                " distance=" + std::to_string(boxHit.distance) +
                ", box miss=" + std::to_string(boxMiss.intersects) +
                ", box inside=" + std::to_string(boxInside.intersects) +
                ", triangle hit=" + std::to_string(triangleHit.intersects) +
                " distance=" + std::to_string(triangleHit.distance) +
                ", triangle miss=" + std::to_string(triangleMiss.intersects) +
                ", triangle parallel=" +
                std::to_string(triangleParallel.intersects));
        }
    }

    void verify_normalization()
    {
        const double inverseLength = 1.0 / std::sqrt(62.0);
        const vector3_value expected{
            3.0 * inverseLength, -2.0 * inverseLength, 7.0 * inverseLength};

        DirectX::SimpleMath::Vector3 simple(3.0f, -2.0f, 7.0f);
        simple.Normalize();
        require_vector("SimpleMath normalization",
            {simple.x, simple.y, simple.z}, expected);

        const auto glmValue = glm::normalize(glm::vec3(3.0f, -2.0f, 7.0f));
        require_vector("GLM normalization",
            {glmValue.x, glmValue.y, glmValue.z}, expected);

        const auto dxValue = DirectX::XMVector3Normalize(
            DirectX::XMVectorSet(3.0f, -2.0f, 7.0f, 0.0f));
        require_vector("DirectXMath normalization",
            {DirectX::XMVectorGetX(dxValue), DirectX::XMVectorGetY(dxValue),
                DirectX::XMVectorGetZ(dxValue)},
            expected);

        const auto sonyValue =
            normalize(Vectormath::Vector3(3.0f, -2.0f, 7.0f));
        require_vector("Sony Vectormath normalization",
            {float(sonyValue.getX()), float(sonyValue.getY()),
                float(sonyValue.getZ())},
            expected);

        const auto moveValue =
            mv::math::TryNormalize(mv::math::Vec3f(3.0f, -2.0f, 7.0f));
        if (!moveValue)
        {
            throw std::runtime_error(
                "mv::math normalization rejected a finite non-zero vector");
        }
        require_vector("mv::math normalization",
            {moveValue->Vector().X(), moveValue->Vector().Y(),
                moveValue->Vector().Z()},
            expected);

        const auto rtmValue =
            rtm::vector_normalize3(rtm::vector_set(3.0f, -2.0f, 7.0f, 0.0f));
        require_vector("RTM normalization",
            {rtm::vector_get_x(rtmValue), rtm::vector_get_y(rtmValue),
                rtm::vector_get_z(rtmValue)},
            expected);
    }

    void verify_particle_integration()
    {
        constexpr double deltaTime = 1.0 / 60.0;
        const vector3_value expected{12.0 + 2.0 * deltaTime,
            8.0 + (5.0 - 9.81 * deltaTime) * deltaTime, -4.0 - deltaTime};

        const DirectX::SimpleMath::Vector3 smPosition(12.0f, 8.0f, -4.0f);
        const DirectX::SimpleMath::Vector3 smVelocity(2.0f, 5.0f, -1.0f);
        const DirectX::SimpleMath::Vector3 smAcceleration(0.0f, -9.81f, 0.0f);
        const auto smIntegrated =
            smPosition +
            (smVelocity + smAcceleration * float(deltaTime)) * float(deltaTime);
        require_vector("SimpleMath particle integration",
            {smIntegrated.x, smIntegrated.y, smIntegrated.z}, expected);

        const glm::vec3 glmPosition(12.0f, 8.0f, -4.0f);
        const glm::vec3 glmVelocity(2.0f, 5.0f, -1.0f);
        const glm::vec3 glmAcceleration(0.0f, -9.81f, 0.0f);
        const auto glmIntegrated =
            glmPosition + (glmVelocity + glmAcceleration * float(deltaTime)) *
                              float(deltaTime);
        require_vector("GLM particle integration",
            {glmIntegrated.x, glmIntegrated.y, glmIntegrated.z}, expected);

        const auto dxPosition = DirectX::XMVectorSet(12.0f, 8.0f, -4.0f, 0.0f);
        const auto dxVelocity = DirectX::XMVectorSet(2.0f, 5.0f, -1.0f, 0.0f);
        const auto dxAcceleration =
            DirectX::XMVectorSet(0.0f, -9.81f, 0.0f, 0.0f);
        const auto dxTime = DirectX::XMVectorReplicate(float(deltaTime));
        const auto dxIntegrated = DirectX::XMVectorMultiplyAdd(
            DirectX::XMVectorMultiplyAdd(dxAcceleration, dxTime, dxVelocity),
            dxTime, dxPosition);
        require_vector("DirectXMath particle integration",
            {DirectX::XMVectorGetX(dxIntegrated),
                DirectX::XMVectorGetY(dxIntegrated),
                DirectX::XMVectorGetZ(dxIntegrated)},
            expected);

        const Vectormath::Vector3 sonyPosition(12.0f, 8.0f, -4.0f);
        const Vectormath::Vector3 sonyVelocity(2.0f, 5.0f, -1.0f);
        const Vectormath::Vector3 sonyAcceleration(0.0f, -9.81f, 0.0f);
        const auto sonyIntegrated =
            sonyPosition +
            (sonyVelocity + sonyAcceleration * float(deltaTime)) *
                float(deltaTime);
        require_vector("Sony Vectormath particle integration",
            {float(sonyIntegrated.getX()), float(sonyIntegrated.getY()),
                float(sonyIntegrated.getZ())},
            expected);

        const mv::math::Vec3f movePosition(12.0f, 8.0f, -4.0f);
        const mv::math::Vec3f moveVelocity(2.0f, 5.0f, -1.0f);
        const mv::math::Vec3f moveAcceleration(0.0f, -9.81f, 0.0f);
        const auto moveIntegrated =
            movePosition +
            (moveVelocity + moveAcceleration * float(deltaTime)) *
                float(deltaTime);
        require_vector("mv::math particle integration",
            {moveIntegrated.X(), moveIntegrated.Y(), moveIntegrated.Z()},
            expected);

        const auto rtmPosition = rtm::vector_set(12.0f, 8.0f, -4.0f, 0.0f);
        const auto rtmVelocity = rtm::vector_set(2.0f, 5.0f, -1.0f, 0.0f);
        const auto rtmAcceleration = rtm::vector_set(0.0f, -9.81f, 0.0f, 0.0f);
        const auto rtmIntegrated = rtm::vector_add(rtmPosition,
            rtm::vector_mul(
                rtm::vector_add(rtmVelocity,
                    rtm::vector_mul(rtmAcceleration, float(deltaTime))),
                float(deltaTime)));
        require_vector("RTM particle integration",
            {rtm::vector_get_x(rtmIntegrated), rtm::vector_get_y(rtmIntegrated),
                rtm::vector_get_z(rtmIntegrated)},
            expected);
    }

    void verify_camera_basis()
    {
        const glm::vec3 glmEye(4.0f, 2.0f, -8.0f);
        const glm::vec3 glmTarget(1.0f, 3.0f, 2.0f);
        const glm::vec3 glmUp(0.0f, 1.0f, 0.0f);
        const auto glmForward = glm::normalize(glmTarget - glmEye);
        const auto glmRight = glm::normalize(glm::cross(glmUp, glmForward));
        const auto glmResult = glmRight + glm::cross(glmForward, glmRight);
        const vector3_value expected{glmResult.x, glmResult.y, glmResult.z};

        const DirectX::SimpleMath::Vector3 smEye(4.0f, 2.0f, -8.0f);
        const DirectX::SimpleMath::Vector3 smTarget(1.0f, 3.0f, 2.0f);
        const DirectX::SimpleMath::Vector3 smUp(0.0f, 1.0f, 0.0f);
        auto smForward = smTarget - smEye;
        smForward.Normalize();
        auto smRight = smUp.Cross(smForward);
        smRight.Normalize();
        const auto smResult = smRight + smForward.Cross(smRight);
        require_vector("SimpleMath camera basis",
            {smResult.x, smResult.y, smResult.z}, expected);

        const auto dxEye = DirectX::XMVectorSet(4.0f, 2.0f, -8.0f, 0.0f);
        const auto dxTarget = DirectX::XMVectorSet(1.0f, 3.0f, 2.0f, 0.0f);
        const auto dxUp = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        const auto dxForward = DirectX::XMVector3Normalize(
            DirectX::XMVectorSubtract(dxTarget, dxEye));
        const auto dxRight = DirectX::XMVector3Normalize(
            DirectX::XMVector3Cross(dxUp, dxForward));
        const auto dxResult = DirectX::XMVectorAdd(
            dxRight, DirectX::XMVector3Cross(dxForward, dxRight));
        require_vector("DirectXMath camera basis",
            {DirectX::XMVectorGetX(dxResult), DirectX::XMVectorGetY(dxResult),
                DirectX::XMVectorGetZ(dxResult)},
            expected);

        const Vectormath::Vector3 sonyEye(4.0f, 2.0f, -8.0f);
        const Vectormath::Vector3 sonyTarget(1.0f, 3.0f, 2.0f);
        const Vectormath::Vector3 sonyUp(0.0f, 1.0f, 0.0f);
        const auto sonyForward = normalize(sonyTarget - sonyEye);
        const auto sonyRight = normalize(cross(sonyUp, sonyForward));
        const auto sonyResult = sonyRight + cross(sonyForward, sonyRight);
        require_vector("Sony Vectormath camera basis",
            {float(sonyResult.getX()), float(sonyResult.getY()),
                float(sonyResult.getZ())},
            expected);

        const mv::math::Vec3f moveEye(4.0f, 2.0f, -8.0f);
        const mv::math::Vec3f moveTarget(1.0f, 3.0f, 2.0f);
        const auto moveForward = mv::math::TryNormalize(moveTarget - moveEye);
        const auto moveRight =
            moveForward ? mv::math::TryNormalize(mv::math::Cross(
                              mv::math::Direction3f::AxisY().Vector(),
                              moveForward->Vector()))
                        : std::nullopt;
        if (!moveForward || !moveRight)
        {
            throw std::runtime_error(
                "mv::math camera basis rejected valid inputs");
        }
        const auto moveResult =
            moveRight->Vector() +
            mv::math::Cross(moveForward->Vector(), moveRight->Vector());
        require_vector("mv::math camera basis",
            {moveResult.X(), moveResult.Y(), moveResult.Z()}, expected);

        const auto rtmEye = rtm::vector_set(4.0f, 2.0f, -8.0f, 0.0f);
        const auto rtmTarget = rtm::vector_set(1.0f, 3.0f, 2.0f, 0.0f);
        const auto rtmUp = rtm::vector_set(0.0f, 1.0f, 0.0f, 0.0f);
        const auto rtmForward =
            rtm::vector_normalize3(rtm::vector_sub(rtmTarget, rtmEye));
        const auto rtmRight =
            rtm::vector_normalize3(rtm::vector_cross3(rtmUp, rtmForward));
        const auto rtmResult =
            rtm::vector_add(rtmRight, rtm::vector_cross3(rtmForward, rtmRight));
        require_vector("RTM camera basis",
            {rtm::vector_get_x(rtmResult), rtm::vector_get_y(rtmResult),
                rtm::vector_get_z(rtmResult)},
            expected);
    }

    void verify_quaternion_rotation()
    {
        const glm::vec3 glmDirection(0.25f, 0.5f, 1.0f);
        const auto glmRotation =
            glm::angleAxis(0.35f, glm::vec3(0.0f, 1.0f, 0.0f));
        const auto glmResult = glmRotation * glmDirection;
        const vector3_value expected{glmResult.x, glmResult.y, glmResult.z};

        const auto smRotation =
            DirectX::SimpleMath::Quaternion::CreateFromAxisAngle(
                DirectX::SimpleMath::Vector3(0.0f, 1.0f, 0.0f), 0.35f);
        const auto smResult = DirectX::SimpleMath::Vector3::Transform(
            DirectX::SimpleMath::Vector3(0.25f, 0.5f, 1.0f), smRotation);
        require_vector("SimpleMath quaternion rotation",
            {smResult.x, smResult.y, smResult.z}, expected);

        const auto dxRotation = DirectX::XMQuaternionRotationAxis(
            DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), 0.35f);
        const auto dxResult = DirectX::XMVector3Rotate(
            DirectX::XMVectorSet(0.25f, 0.5f, 1.0f, 0.0f), dxRotation);
        require_vector("DirectXMath quaternion rotation",
            {DirectX::XMVectorGetX(dxResult), DirectX::XMVectorGetY(dxResult),
                DirectX::XMVectorGetZ(dxResult)},
            expected);

        const auto sonyRotation =
            Vectormath::Quat::rotation(0.35f, Vectormath::Vector3::yAxis());
        const auto sonyResult = Vectormath::SSE::rotate(
            sonyRotation, Vectormath::Vector3(0.25f, 0.5f, 1.0f));
        require_vector("Sony Vectormath quaternion rotation",
            {float(sonyResult.getX()), float(sonyResult.getY()),
                float(sonyResult.getZ())},
            expected);

        const auto moveRotation = mv::math::Rotation3f::FromAxisAngle(
            mv::math::Direction3f::AxisY(), mv::math::Radiansf(0.35f));
        const auto moveResult =
            mv::math::Rotate(moveRotation, mv::math::Vec3f(0.25f, 0.5f, 1.0f));
        require_vector("mv::math quaternion rotation",
            {moveResult.X(), moveResult.Y(), moveResult.Z()}, expected);

        const auto rtmRotation = rtm::quat_from_axis_angle(
            rtm::vector_set(0.0f, 1.0f, 0.0f, 0.0f), 0.35f);
        const auto rtmResult = rtm::quat_mul_vector3(
            rtm::vector_set(0.25f, 0.5f, 1.0f, 0.0f), rtmRotation);
        require_vector("RTM quaternion rotation",
            {rtm::vector_get_x(rtmResult), rtm::vector_get_y(rtmResult),
                rtm::vector_get_z(rtmResult)},
            expected);
    }

    void run_all()
    {
        verify_normalization();
        verify_particle_integration();
        verify_camera_basis();
        verify_quaternion_rotation();
        verify_intersections<geometry::simplemath_ops>("SimpleMath");
        verify_intersections<geometry::glm_ops>("GLM");
        verify_intersections<geometry::directxmath_ops>("DirectXMath");
        verify_intersections<geometry::vectormath_ops>("Sony Vectormath");
        verify_intersections<geometry::move_ops<float>>("mv::math float");
        verify_intersections<geometry::move_ops<double>>("mv::math double");
        verify_intersections<geometry::rtm_ops<float>>("RTM float");
        verify_intersections<geometry::rtm_ops<double>>("RTM double");
    }
}  // namespace mathbench::verification

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
        std::string comparisonBasis;
        std::string winner;
        double winnerNanoseconds{};
        double moveNanoseconds{};
        std::size_t moveRank{};
    };

    std::vector<ranked_result> rankings;
    std::vector<ankerl::nanobench::Result> allResults;

    std::string library_name(std::string_view benchmarkName)
    {
        if (benchmarkName.find("mv::math") != std::string_view::npos)
        {
            return "Move current";
        }
        if (benchmarkName.find("move::") != std::string_view::npos)
        {
            return "Move legacy";
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
               benchmarkName.find("Mat4x4d") != std::string_view::npos ||
               benchmarkName.find("Vec3d") != std::string_view::npos ||
               benchmarkName.find("Rotation3d") != std::string_view::npos ||
               benchmarkName.find("Mat4d") != std::string_view::npos;
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

    void collect_rankings(std::string_view capability,
        std::string_view comparisonBasis, const ankerl::nanobench::Bench& bench)
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
            const double nanoseconds = result.median(measure::elapsed) * 1.0e9 /
                                       result.config().mBatch;
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

            const auto moveImplementation =
                std::find_if(ordered.begin(), ordered.end(),
                    [](const auto& entry)
                    {
                        return entry.first == "Move current" ||
                               entry.first == "Move legacy";
                    });
            if (ordered.empty() || moveImplementation == ordered.end())
            {
                continue;
            }

            rankings.push_back({scenario, std::string(comparisonBasis),
                ordered.front().first, ordered.front().second,
                moveImplementation->second,
                static_cast<std::size_t>(
                    std::distance(ordered.begin(), moveImplementation) + 1)});
        }
    }

    template <typename Function>
    void run_capability(std::string_view title, bool summarize,
        Function&& function,
        std::string_view comparisonBasis = "Matching operation")
    {
        ankerl::nanobench::Bench bench;
        bench.title(std::string(title))
            .unit("op")
            .epochs(15)
            .warmup(1000)
            .minEpochTime(std::chrono::milliseconds(5));
        function(bench);
        allResults.insert(
            allResults.end(), bench.results().begin(), bench.results().end());
        if (summarize)
        {
            collect_rankings(title, comparisonBasis, bench);
        }
    }

    void print_rankings()
    {
        std::cout
            << "\n## Capability winners (single-precision implementations)\n\n"
            << "| Capability | Comparison basis | Fastest library | "
               "Fastest ns/op | Move ns/op | Move rank | Move gap |\n"
            << "|:--|:--|:--|--:|--:|--:|--:|\n";

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
            std::cout << "| " << result.capability << " | "
                      << result.comparisonBasis << " | " << result.winner
                      << " | " << std::fixed << std::setprecision(2)
                      << result.winnerNanoseconds << " | "
                      << result.moveNanoseconds << " | " << result.moveRank
                      << " | " << gap << "% |\n";
        }
    }

    void write_results(const std::optional<std::string>& jsonPath,
        const std::optional<std::string>& csvPath)
    {
        const auto render = [](std::string_view path, const char* format)
        {
            std::ofstream output{std::string(path)};
            if (!output)
            {
                throw std::runtime_error(
                    "Unable to open benchmark output file: " +
                    std::string(path));
            }
            ankerl::nanobench::render(format, allResults, output);
        };

        if (jsonPath)
        {
            render(*jsonPath, ankerl::nanobench::templates::json());
        }
        if (csvPath)
        {
            render(*csvPath, ankerl::nanobench::templates::csv());
        }
    }
}  // namespace mathbench::report

namespace
{
    struct command_line_options
    {
        bool verifyOnly{};
        std::optional<std::string> jsonPath;
        std::optional<std::string> csvPath;
    };

    command_line_options parse_options(int argc, char** argv)
    {
        command_line_options options;
        for (int index = 1; index < argc; ++index)
        {
            const std::string_view argument(argv[index]);
            if (argument == "--verify-only")
            {
                options.verifyOnly = true;
            }
            else if (argument == "--json" || argument == "--csv")
            {
                if (++index >= argc)
                {
                    throw std::runtime_error(
                        std::string(argument) + " requires an output path");
                }
                auto& path =
                    argument == "--json" ? options.jsonPath : options.csvPath;
                path = argv[index];
            }
            else if (argument == "--help")
            {
                std::cout << "Usage: vectormathbench [--verify-only] "
                             "[--json PATH] [--csv PATH]\n";
                std::exit(EXIT_SUCCESS);
            }
            else
            {
                throw std::runtime_error(
                    "Unknown command-line option: " + std::string(argument));
            }
        }
        return options;
    }
}  // namespace

int main(int argc, char** argv)
{
    try
    {
        const auto options = parse_options(argc, argv);
        mathbench::verification::run_all();
        std::cerr << "Semantic parity checks passed\n";
        if (options.verifyOnly)
        {
            return EXIT_SUCCESS;
        }

        using mathbench::report::run_capability;

        run_capability("Game loop / Normalize direction", true,
            mathbench::workloads::normalize_direction,
            "Move checked; peers unchecked");
        run_capability("Game loop / Particle integration", true,
            mathbench::workloads::particle_integration);
        run_capability("Graphics / Camera basis", true,
            mathbench::workloads::camera_basis,
            "Move checked; peers unchecked");
        run_capability("Graphics / Quaternion direction rotation", true,
            mathbench::workloads::rotate_direction);
        run_capability("Geometry / Ray-AABB intersection latency", true,
            mathbench::workloads::ray_aabb_intersection,
            "Shared full-precision raw-vector kernel");
        run_capability("Geometry / Ray-AABB intersection throughput", true,
            mathbench::workloads::ray_aabb_batch,
            "Shared full-precision raw-vector kernel");
        run_capability("Geometry / Ray-triangle intersection latency", true,
            mathbench::workloads::ray_triangle_intersection,
            "Shared raw-vector kernel");
        run_capability("Geometry / Ray-triangle intersection throughput", true,
            mathbench::workloads::ray_triangle_batch,
            "Shared raw-vector kernel");

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
        mathbench::report::write_results(options.jsonPath, options.csvPath);
        printf("Benchmark complete\n");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
