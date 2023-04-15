#include <math.h>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <type_traits>

#if __unix
namespace std
{
    inline float fabsf(float x)
    {
        return ::abs(x);
    }

    inline float sqrtf(float x)
    {
        return ::sqrtf(x);
    }

    inline float tanf(float x)
    {
        return ::tanf(x);
    }
}  // namespace std
#endif

// Sony vectormath
#include <vectormath.hpp>

#include "sse/internal.hpp"
#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <DirectXMath.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "simplemath.hpp"

namespace Vectormath
{
    using Vector3 = Vectormath::SSE::Vector3;
    using Vector4 = Vectormath::SSE::Vector4;
    using Matrix4 = Vectormath::SSE::Matrix4;
}  // namespace Vectormath

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
        DirectX::SimpleMath::Vector2 smVec2;
        DirectX::SimpleMath::Vector3 smVec3;
        DirectX::SimpleMath::Vector4 smVec4;
        DirectX::SimpleMath::Matrix smMat4;
        glm::vec2 glmVec2;
        glm::vec3 glmVec3;
        glm::vec4 glmVec4;
        glm::mat4 glmMat4;
        DirectX::XMFLOAT2 dxVec2;
        DirectX::XMFLOAT3 dxVec3;
        DirectX::XMFLOAT4 dxVec4;
        DirectX::XMFLOAT4X4 dxMat4;
        Vectormath::Vector2 sonyVec2;
        Vectormath::Vector3 sonyVec3;
        Vectormath::Vector4 sonyVec4;
        Vectormath::Matrix4 sonyMat4;

        DirectX::XMVECTOR dxVecA;
        DirectX::XMVECTOR dxVecB;
        DirectX::XMVECTOR dxVecC;
        DirectX::XMMATRIX dxMatA;
    } results;

    namespace vectors
    {
        void addition(ankerl::nanobench::Bench& bench)
        {
            bench.run("Store int (reference 'no-op')",
                [&]
                {
                    results.intnum = 0;
                    ankerl::nanobench::doNotOptimizeAway(results.intnum);
                });

            bench.run("SimpleMath::Vector2 addition",
                [&]
                {
                    results.smVec2 = DirectX::SimpleMath::Vector2(1.0f, 2.0f) +
                                     DirectX::SimpleMath::Vector2(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.smVec2);
                });

            bench.run("SimpleMath::Vector3 addition",
                [&]
                {
                    results.smVec3 =
                        DirectX::SimpleMath::Vector3(1.0f, 2.0f, 3.0f) +
                        DirectX::SimpleMath::Vector3(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.smVec3);
                });

            bench.run("SimpleMath::Vector4 addition",
                [&]
                {
                    results.smVec4 =
                        DirectX::SimpleMath::Vector4(1.0f, 2.0f, 3.0f, 4.0f) +
                        DirectX::SimpleMath::Vector4(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.smVec4);
                });

            bench.run("glm::vec2 addition",
                [&]
                {
                    results.glmVec2 =
                        glm::vec2(1.0f, 2.0f) + glm::vec2(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.glmVec2);
                });

            bench.run("glm::vec3 addition",
                [&]
                {
                    results.glmVec3 = glm::vec3(1.0f, 2.0f, 3.0f) +
                                      glm::vec3(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.glmVec3);
                });

            bench.run("glm::vec4 addition",
                [&]
                {
                    results.glmVec4 = glm::vec4(1.0f, 2.0f, 3.0f, 4.0f) +
                                      glm::vec4(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.glmVec4);
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
                    ankerl::nanobench::doNotOptimizeAway(results.dxVec2);
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
                    ankerl::nanobench::doNotOptimizeAway(results.dxVec3);
                });

            bench.run("DirectX::XMFLOAT4 addition without Loads",
                [&]
                {
                    results.dxVecC = DirectX::XMVectorAdd(
                        DirectX::XMVECTOR{}, DirectX::XMVECTOR{});
                    ankerl::nanobench::doNotOptimizeAway(results.dxVecC);
                });

            bench.run("Vectormath::Vector2 addition",
                [&]
                {
                    results.sonyVec2 = Vectormath::Vector2(1.0f, 2.0f) +
                                       Vectormath::Vector2(3.0f, 4.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.sonyVec2);
                });

            bench.run("Vectormath::Vector3 addition",
                [&]
                {
                    results.sonyVec3 = Vectormath::Vector3(1.0f, 2.0f, 3.0f) +
                                       Vectormath::Vector3(3.0f, 4.0f, 5.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.sonyVec3);
                });

            bench.run("Vectormath::Vector4 addition",
                [&]
                {
                    results.sonyVec4 =
                        Vectormath::Vector4(1.0f, 2.0f, 3.0f, 4.0f) +
                        Vectormath::Vector4(3.0f, 4.0f, 5.0f, 6.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.sonyVec4);
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
                    ankerl::nanobench::doNotOptimizeAway(results.smVec4);
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
                    ankerl::nanobench::doNotOptimizeAway(results.glmVec4);
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
                    ankerl::nanobench::doNotOptimizeAway(results.dxVec4);
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
                        ankerl::nanobench::doNotOptimizeAway(results.dxVec4);
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
                    ankerl::nanobench::doNotOptimizeAway(results.sonyVec4);
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
                    ankerl::nanobench::doNotOptimizeAway(results.smVec3);
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
                    ankerl::nanobench::doNotOptimizeAway(results.glmVec3);
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
                    ankerl::nanobench::doNotOptimizeAway(results.dxVec3);
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
                        ankerl::nanobench::doNotOptimizeAway(results.dxVec3);
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

                    ankerl::nanobench::doNotOptimizeAway(results.sonyVec3);
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
                    ankerl::nanobench::doNotOptimizeAway(results.smVec4);
                });

            bench.run("Complex operation 3 with glm::vec4",
                [&]
                {
                    glm::vec4 vec3a(1.0f, 2.0f, 3.0f, 4.0f);
                    glm::vec4 vec3b(3.0f, 4.0f, 5.0f, 6.0f);
                    glm::vec4 vec3c(5.0f, 6.0f, 7.0f, 8.0f);
                    glm::vec4 vec3d(7.0f, 8.0f, 9.0f, 10.0f);

                    results.glmVec4 = (vec3a + vec3b) * vec3c - vec3d;
                    ankerl::nanobench::doNotOptimizeAway(results.glmVec4);
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
                    ankerl::nanobench::doNotOptimizeAway(results.dxVec4);
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
                        ankerl::nanobench::doNotOptimizeAway(results.dxVec4);
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
                    Vector4 stepTwo = vec4c - vec4d;
                    results.sonyVec4 = Vector4(stepOne.getX() * stepTwo.getX(),
                        stepOne.getY() * stepTwo.getY(),
                        stepOne.getZ() * stepTwo.getZ(),
                        stepOne.getW() * stepTwo.getW());
                    ankerl::nanobench::doNotOptimizeAway(results.sonyVec4);
                });
        }
    }  // namespace vectors

    namespace matrices
    {
        void construct_model_matrix(ankerl::nanobench::Bench& bench)
        {
            bench.run("Construct model matrix SimpleMath",
                [&]
                {
                    results.smMat4 =
                        DirectX::SimpleMath::Matrix::CreateTranslation(
                            1.0f, 2.0f, 3.0f) *
                        DirectX::SimpleMath::Matrix::CreateRotationX(0.5f) *
                        DirectX::SimpleMath::Matrix::CreateRotationY(0.5f) *
                        DirectX::SimpleMath::Matrix::CreateRotationZ(0.5f) *
                        DirectX::SimpleMath::Matrix::CreateScale(
                            1.0f, 2.0f, 3.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.smMat4);
                });

            bench.run("Construct model matrix glm",
                [&]
                {
                    results.glmMat4 = glm::translate(glm::mat4(1.0f),
                                          glm::vec3(1.0f, 2.0f, 3.0f)) *
                                      glm::rotate(glm::mat4(1.0f), 0.5f,
                                          glm::vec3(1.0f, 0.0f, 0.0f)) *
                                      glm::rotate(glm::mat4(1.0f), 0.5f,
                                          glm::vec3(0.0f, 1.0f, 0.0f)) *
                                      glm::rotate(glm::mat4(1.0f), 0.5f,
                                          glm::vec3(0.0f, 0.0f, 1.0f)) *
                                      glm::scale(glm::mat4(1.0f),
                                          glm::vec3(1.0f, 2.0f, 3.0f));
                    ankerl::nanobench::doNotOptimizeAway(results.glmMat4);
                });

            bench.run("Construct model matrix DXM (manual)",
                [&]
                {
                    using namespace DirectX;
                    // Use XMMatrixMultiply

                    results.dxMatA = XMMatrixMultiply(
                        XMMatrixMultiply(
                            XMMatrixMultiply(
                                XMMatrixMultiply(
                                    XMMatrixTranslation(1.0f, 2.0f, 3.0f),
                                    XMMatrixRotationX(0.5f)),
                                XMMatrixRotationY(0.5f)),
                            XMMatrixRotationZ(0.5f)),
                        XMMatrixScaling(1.0f, 2.0f, 3.0f));
                    ankerl::nanobench::doNotOptimizeAway(results.dxMatA);
                });

            bench.run("Construct model matrix DXM (builtin)",
                [&]
                {
                    using namespace DirectX;

                    XMVECTOR ScalingOrigin = XMVectorZero();
                    XMVECTOR ScalingOrientationQuaternion =
                        XMQuaternionIdentity();
                    XMVECTOR Scaling = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
                    XMVECTOR RotationOrigin = XMVectorZero();
                    XMVECTOR RotationQuaternion =
                        XMQuaternionRotationRollPitchYaw(0.5f, 0.5f, 0.5f);
                    XMVECTOR Translation = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
                    XMVECTOR Scale = XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
                    XMVECTOR Rotation =
                        XMQuaternionRotationRollPitchYaw(0.5f, 0.5f, 0.5f);

                    results.dxMatA = XMMatrixTransformation(ScalingOrigin,
                        ScalingOrientationQuaternion, Scaling, RotationOrigin,
                        RotationQuaternion, Translation);
                    ankerl::nanobench::doNotOptimizeAway(results.dxMatA);
                });

            bench.run("Construct model matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    results.sonyMat4 =
                        Matrix4::translation(Vector3(1.0f, 2.0f, 3.0f)) *
                        Matrix4::rotationX(0.5f) * Matrix4::rotationY(0.5f) *
                        Matrix4::rotationZ(0.5f) *
                        Matrix4::scale(Vector3(1.0f, 2.0f, 3.0f));
                    ankerl::nanobench::doNotOptimizeAway(results.sonyMat4);
                });
        }

        void construct_view_matrix(ankerl::nanobench::Bench& bench)
        {
            bench.run("Construct view matrix SimpleMath",
                [&]
                {
                    results.smMat4 = DirectX::SimpleMath::Matrix::CreateLookAt(
                        DirectX::SimpleMath::Vector3(1.0f, 2.0f, 3.0f),
                        DirectX::SimpleMath::Vector3(4.0f, 5.0f, 6.0f),
                        DirectX::SimpleMath::Vector3(7.0f, 8.0f, 9.0f));
                    ankerl::nanobench::doNotOptimizeAway(results.smMat4);
                });

            bench.run("Construct view matrix glm",
                [&]
                {
                    results.glmMat4 = glm::lookAt(glm::vec3(1.0f, 2.0f, 3.0f),
                        glm::vec3(4.0f, 5.0f, 6.0f),
                        glm::vec3(7.0f, 8.0f, 9.0f));
                    ankerl::nanobench::doNotOptimizeAway(results.glmMat4);
                });

            bench.run("Construct view matrix DXM",
                [&]
                {
                    using namespace DirectX;
                    results.dxMatA =
                        XMMatrixLookAtLH(XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f),
                            XMVectorSet(4.0f, 5.0f, 6.0f, 0.0f),
                            XMVectorSet(7.0f, 8.0f, 9.0f, 0.0f));
                    ankerl::nanobench::doNotOptimizeAway(results.dxMatA);
                });

            bench.run("Construct view matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    results.sonyMat4 = Matrix4::lookAt(Point3(1.0f, 2.0f, 3.0f),
                        Point3(4.0f, 5.0f, 6.0f), Vector3(7.0f, 8.0f, 9.0f));
                    ankerl::nanobench::doNotOptimizeAway(results.sonyMat4);
                });
        }

        void construct_perspective_projection_matrix(
            ankerl::nanobench::Bench& bench)
        {
            bench.run("Construct perspective matrix SimpleMath",
                [&]
                {
                    results.smMat4 = DirectX::SimpleMath::Matrix::
                        CreatePerspectiveFieldOfView(0.5f, 1.0f, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.smMat4);
                });

            bench.run("Construct perspective matrix glm",
                [&]
                {
                    results.glmMat4 =
                        glm::perspective(0.5f, 1.0f, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.glmMat4);
                });

            bench.run("Construct perspective matrix DXM",
                [&]
                {
                    using namespace DirectX;
                    results.dxMatA =
                        XMMatrixPerspectiveFovLH(0.5f, 1.0f, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.dxMatA);
                });

            bench.run("Construct perspective matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    results.sonyMat4 =
                        Matrix4::perspective(0.5f, 1.0f, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.sonyMat4);
                });
        }

        void create_ortho_projection_matrix(ankerl::nanobench::Bench& bench)
        {
            bench.run("Construct ortho matrix SimpleMath",
                [&]
                {
                    results.smMat4 =
                        DirectX::SimpleMath::Matrix::CreateOrthographic(
                            1280, 720, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.smMat4);
                });

            bench.run("Construct ortho matrix glm",
                [&]
                {
                    results.glmMat4 =
                        glm::ortho<float>(0, 1280, 0, 720, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.glmMat4);
                });

            bench.run("Construct ortho matrix DXM",
                [&]
                {
                    using namespace DirectX;
                    results.dxMatA =
                        XMMatrixOrthographicLH(1280, 720, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.dxMatA);
                });

            bench.run("Construct ortho matrix Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    results.sonyMat4 =
                        Matrix4::orthographic(0, 1280, 0, 720, 0.1f, 100.0f);
                    ankerl::nanobench::doNotOptimizeAway(results.sonyMat4);
                });
        }

        void vector_matrix_multiply(ankerl::nanobench::Bench& bench)
        {
            bench.run("Vector matrix multiply SimpleMath",
                [&]
                {
                    results.smVec4 = DirectX::SimpleMath::Vector4::Transform(
                        results.smVec4, results.smMat4);
                    ankerl::nanobench::doNotOptimizeAway(results.smVec4);
                });

            bench.run("Vector matrix multiply glm",
                [&]
                {
                    results.glmVec4 = results.glmMat4 * results.glmVec4;
                    ankerl::nanobench::doNotOptimizeAway(results.glmVec4);
                });

            bench.run("Vector matrix multiply DXM",
                [&]
                {
                    using namespace DirectX;
                    results.dxVecA =
                        XMVector4Transform(results.dxVecA, results.dxMatA);
                    XMStoreFloat4(&results.dxVec4, results.dxVecA);
                    ankerl::nanobench::doNotOptimizeAway(results.dxVec4);
                });

            bench.run("Vector matrix multiply Vectormath",
                [&]
                {
                    using namespace Vectormath;
                    results.sonyVec4 = results.sonyMat4 * results.sonyVec4;
                    ankerl::nanobench::doNotOptimizeAway(results.sonyVec4);
                });
        }
    }  // namespace matrices
}  // namespace mathbench

int main(int argc, char** argv)
{
    {
        ankerl::nanobench::Bench vectorBench;
        vectorBench.name("Vectors");
        vectorBench.minEpochIterations(114336014);
        mathbench::vectors::addition(vectorBench);
        mathbench::vectors::complex1(vectorBench);
        mathbench::vectors::complex2vec3(vectorBench);
        mathbench::vectors::complex3vec4(vectorBench);
    }

    {
        ankerl::nanobench::Bench matrixBench;
        // matrixBench.minEpochIterations(10533656);
        matrixBench.name("Matrices");
        mathbench::matrices::construct_model_matrix(matrixBench);
        mathbench::matrices::construct_view_matrix(matrixBench);
        mathbench::matrices::construct_perspective_projection_matrix(
            matrixBench);
        mathbench::matrices::create_ortho_projection_matrix(matrixBench);
        mathbench::matrices::vector_matrix_multiply(matrixBench);
    }
    return 0;
}