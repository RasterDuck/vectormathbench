#pragma once

#include <algorithm>
#include <cmath>
#include <limits>

#include <rtm/vector4d.h>
#include <rtm/vector4f.h>
#include <glm/common.hpp>
#include <glm/geometric.hpp>
#include <glm/vec3.hpp>
#include <move/math/vec3.hpp>

#include <DirectXMath.h>

#include "simplemath.hpp"
#include "vectormath_compat.hpp"

namespace mathbench::geometry
{
    template <typename T>
    struct intersection_result
    {
        T distance{};
        bool intersects{};
    };

    template <typename Ops>
    intersection_result<typename Ops::scalar> intersect_ray_aabb(
        const typename Ops::vector& origin,
        const typename Ops::vector& direction,
        const typename Ops::vector& boundsMin,
        const typename Ops::vector& boundsMax)
    {
        using scalar = typename Ops::scalar;

        const auto inverseDirection = Ops::reciprocal(direction);
        const auto t0 =
            Ops::multiply(Ops::subtract(boundsMin, origin), inverseDirection);
        const auto t1 =
            Ops::multiply(Ops::subtract(boundsMax, origin), inverseDirection);
        const auto nearValues = Ops::minimum(t0, t1);
        const auto farValues = Ops::maximum(t0, t1);

        const scalar entry = std::max(Ops::x(nearValues),
            std::max(Ops::y(nearValues), Ops::z(nearValues)));
        const scalar exit = std::min(
            Ops::x(farValues), std::min(Ops::y(farValues), Ops::z(farValues)));
        const scalar distance = std::max(entry, scalar(0));
        return {distance, exit >= distance};
    }

    template <typename Ops>
    intersection_result<typename Ops::scalar> intersect_ray_triangle(
        const typename Ops::vector& origin,
        const typename Ops::vector& direction,
        const typename Ops::vector& vertex0,
        const typename Ops::vector& vertex1,
        const typename Ops::vector& vertex2)
    {
        using scalar = typename Ops::scalar;
        constexpr scalar epsilon =
            std::numeric_limits<scalar>::epsilon() * scalar(8);

        const auto edge1 = Ops::subtract(vertex1, vertex0);
        const auto edge2 = Ops::subtract(vertex2, vertex0);
        const auto determinantVector = Ops::cross(direction, edge2);
        const scalar determinant = Ops::dot(edge1, determinantVector);
        if (std::abs(determinant) <= epsilon)
        {
            return {};
        }

        const scalar inverseDeterminant = scalar(1) / determinant;
        const auto vertexToOrigin = Ops::subtract(origin, vertex0);
        const scalar u =
            Ops::dot(vertexToOrigin, determinantVector) * inverseDeterminant;
        if (u < scalar(0) || u > scalar(1))
        {
            return {};
        }

        const auto barycentricVector = Ops::cross(vertexToOrigin, edge1);
        const scalar v =
            Ops::dot(direction, barycentricVector) * inverseDeterminant;
        if (v < scalar(0) || u + v > scalar(1))
        {
            return {};
        }

        const scalar distance =
            Ops::dot(edge2, barycentricVector) * inverseDeterminant;
        return {distance, distance >= scalar(0)};
    }

    struct simplemath_ops
    {
        using scalar = float;
        using vector = DirectX::SimpleMath::Vector3;

        static vector make(scalar x, scalar y, scalar z)
        {
            return {x, y, z};
        }

        static vector subtract(const vector& lhs, const vector& rhs)
        {
            return lhs - rhs;
        }

        static vector multiply(const vector& lhs, const vector& rhs)
        {
            return lhs * rhs;
        }

        static vector reciprocal(const vector& value)
        {
            return {1.0f / value.x, 1.0f / value.y, 1.0f / value.z};
        }

        static vector minimum(const vector& lhs, const vector& rhs)
        {
            return vector::Min(lhs, rhs);
        }

        static vector maximum(const vector& lhs, const vector& rhs)
        {
            return vector::Max(lhs, rhs);
        }

        static scalar dot(const vector& lhs, const vector& rhs)
        {
            return lhs.Dot(rhs);
        }

        static vector cross(const vector& lhs, const vector& rhs)
        {
            return lhs.Cross(rhs);
        }

        static scalar x(const vector& value)
        {
            return value.x;
        }

        static scalar y(const vector& value)
        {
            return value.y;
        }

        static scalar z(const vector& value)
        {
            return value.z;
        }
    };

    struct glm_ops
    {
        using scalar = float;
        using vector = glm::vec3;

        static vector make(scalar x, scalar y, scalar z)
        {
            return {x, y, z};
        }

        static vector subtract(const vector& lhs, const vector& rhs)
        {
            return lhs - rhs;
        }

        static vector multiply(const vector& lhs, const vector& rhs)
        {
            return lhs * rhs;
        }

        static vector reciprocal(const vector& value)
        {
            return scalar(1) / value;
        }

        static vector minimum(const vector& lhs, const vector& rhs)
        {
            return glm::min(lhs, rhs);
        }

        static vector maximum(const vector& lhs, const vector& rhs)
        {
            return glm::max(lhs, rhs);
        }

        static scalar dot(const vector& lhs, const vector& rhs)
        {
            return glm::dot(lhs, rhs);
        }

        static vector cross(const vector& lhs, const vector& rhs)
        {
            return glm::cross(lhs, rhs);
        }

        static scalar x(const vector& value)
        {
            return value.x;
        }

        static scalar y(const vector& value)
        {
            return value.y;
        }

        static scalar z(const vector& value)
        {
            return value.z;
        }
    };

    struct directxmath_ops
    {
        using scalar = float;
        using vector = DirectX::XMVECTOR;

        static vector make(scalar x, scalar y, scalar z)
        {
            return DirectX::XMVectorSet(x, y, z, 0.0f);
        }

        static vector subtract(const vector& lhs, const vector& rhs)
        {
            return DirectX::XMVectorSubtract(lhs, rhs);
        }

        static vector multiply(const vector& lhs, const vector& rhs)
        {
            return DirectX::XMVectorMultiply(lhs, rhs);
        }

        static vector reciprocal(const vector& value)
        {
            return DirectX::XMVectorReciprocal(value);
        }

        static vector minimum(const vector& lhs, const vector& rhs)
        {
            return DirectX::XMVectorMin(lhs, rhs);
        }

        static vector maximum(const vector& lhs, const vector& rhs)
        {
            return DirectX::XMVectorMax(lhs, rhs);
        }

        static scalar dot(const vector& lhs, const vector& rhs)
        {
            return DirectX::XMVectorGetX(DirectX::XMVector3Dot(lhs, rhs));
        }

        static vector cross(const vector& lhs, const vector& rhs)
        {
            return DirectX::XMVector3Cross(lhs, rhs);
        }

        static scalar x(const vector& value)
        {
            return DirectX::XMVectorGetX(value);
        }

        static scalar y(const vector& value)
        {
            return DirectX::XMVectorGetY(value);
        }

        static scalar z(const vector& value)
        {
            return DirectX::XMVectorGetZ(value);
        }
    };

    struct vectormath_ops
    {
        using scalar = float;
        using vector = Vectormath::SSE::Vector3;

        static vector make(scalar x, scalar y, scalar z)
        {
            return {x, y, z};
        }

        static vector subtract(const vector& lhs, const vector& rhs)
        {
            return lhs - rhs;
        }

        static vector multiply(const vector& lhs, const vector& rhs)
        {
            return Vectormath::SSE::mulPerElem(lhs, rhs);
        }

        static vector reciprocal(const vector& value)
        {
            return Vectormath::SSE::recipPerElem(value);
        }

        static vector minimum(const vector& lhs, const vector& rhs)
        {
            return Vectormath::SSE::minPerElem(lhs, rhs);
        }

        static vector maximum(const vector& lhs, const vector& rhs)
        {
            return Vectormath::SSE::maxPerElem(lhs, rhs);
        }

        static scalar dot(const vector& lhs, const vector& rhs)
        {
            return float(Vectormath::SSE::dot(lhs, rhs));
        }

        static vector cross(const vector& lhs, const vector& rhs)
        {
            return Vectormath::SSE::cross(lhs, rhs);
        }

        static scalar x(const vector& value)
        {
            return float(value.getX());
        }

        static scalar y(const vector& value)
        {
            return float(value.getY());
        }

        static scalar z(const vector& value)
        {
            return float(value.getZ());
        }
    };

    template <typename T>
    struct move_ops
    {
        using scalar = T;
        using vector = move::math::vec3<T, move::math::Acceleration::RTM>;

        static vector make(scalar x, scalar y, scalar z)
        {
            return {x, y, z};
        }

        static vector subtract(const vector& lhs, const vector& rhs)
        {
            return lhs - rhs;
        }

        static vector multiply(const vector& lhs, const vector& rhs)
        {
            return lhs * rhs;
        }

        static vector reciprocal(const vector& value)
        {
            return vector(T(1), T(1), T(1)) / value;
        }

        static vector minimum(const vector& lhs, const vector& rhs)
        {
            return vector::min(lhs, rhs);
        }

        static vector maximum(const vector& lhs, const vector& rhs)
        {
            return vector::max(lhs, rhs);
        }

        static scalar dot(const vector& lhs, const vector& rhs)
        {
            return vector::dot(lhs, rhs);
        }

        static vector cross(const vector& lhs, const vector& rhs)
        {
            return vector::cross(lhs, rhs);
        }

        static scalar x(const vector& value)
        {
            return value.get_x();
        }

        static scalar y(const vector& value)
        {
            return value.get_y();
        }

        static scalar z(const vector& value)
        {
            return value.get_z();
        }
    };

    template <typename T>
    struct rtm_ops;

    template <>
    struct rtm_ops<float>
    {
        using scalar = float;
        using vector = rtm::vector4f;

        static vector make(scalar x, scalar y, scalar z)
        {
            return rtm::vector_set(x, y, z, 0.0f);
        }

        static vector subtract(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_sub(lhs, rhs);
        }

        static vector multiply(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_mul(lhs, rhs);
        }

        static vector reciprocal(const vector& value)
        {
            return rtm::vector_reciprocal(value);
        }

        static vector minimum(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_min(lhs, rhs);
        }

        static vector maximum(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_max(lhs, rhs);
        }

        static scalar dot(const vector& lhs, const vector& rhs)
        {
            return rtm::scalar_cast(rtm::vector_dot3_as_scalar(lhs, rhs));
        }

        static vector cross(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_cross3(lhs, rhs);
        }

        static scalar x(const vector& value)
        {
            return rtm::vector_get_x(value);
        }

        static scalar y(const vector& value)
        {
            return rtm::vector_get_y(value);
        }

        static scalar z(const vector& value)
        {
            return rtm::vector_get_z(value);
        }
    };

    template <>
    struct rtm_ops<double>
    {
        using scalar = double;
        using vector = rtm::vector4d;

        static vector make(scalar x, scalar y, scalar z)
        {
            return rtm::vector_set(x, y, z, 0.0);
        }

        static vector subtract(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_sub(lhs, rhs);
        }

        static vector multiply(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_mul(lhs, rhs);
        }

        static vector reciprocal(const vector& value)
        {
            return rtm::vector_reciprocal(value);
        }

        static vector minimum(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_min(lhs, rhs);
        }

        static vector maximum(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_max(lhs, rhs);
        }

        static scalar dot(const vector& lhs, const vector& rhs)
        {
            return rtm::scalar_cast(rtm::vector_dot3_as_scalar(lhs, rhs));
        }

        static vector cross(const vector& lhs, const vector& rhs)
        {
            return rtm::vector_cross3(lhs, rhs);
        }

        static scalar x(const vector& value)
        {
            return rtm::vector_get_x(value);
        }

        static scalar y(const vector& value)
        {
            return rtm::vector_get_y(value);
        }

        static scalar z(const vector& value)
        {
            return rtm::vector_get_z(value);
        }
    };
}  // namespace mathbench::geometry
