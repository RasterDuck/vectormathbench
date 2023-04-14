#define ANKERL_NANOBENCH_IMPLEMENT
#include "nanobench.h"

#include <chrono>
#include <cstdlib>
#include <type_traits>

#include <DirectXMath.h>
#include "simplemath.hpp"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

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
        glm::vec2 glmVec2;
        glm::vec3 glmVec3;
        glm::vec4 glmVec4;
        DirectX::XMFLOAT2 dxVec2;
        DirectX::XMFLOAT3 dxVec3;
        DirectX::XMFLOAT4 dxVec4;

        DirectX::XMVECTOR dxVecA;
        DirectX::XMVECTOR dxVecB;
        DirectX::XMVECTOR dxVecC;
    } results;

    void addition(ankerl::nanobench::Bench& bench)
    {
        bench.run("Store int (reference 'no-op')",
            [&]
            {
                results.intnum = 0;
                ankerl::nanobench::doNotOptimizeAway(results.intnum);
            });

        bench.run("DirectX::SimpleMath::Vector2 addition",
            [&]
            {
                results.smVec2 = DirectX::SimpleMath::Vector2(1.0f, 2.0f) +
                                 DirectX::SimpleMath::Vector2(3.0f, 4.0f);
                ankerl::nanobench::doNotOptimizeAway(results.smVec2);
            });

        bench.run("DirectX::SimpleMath::Vector3 addition",
            [&]
            {
                results.smVec3 =
                    DirectX::SimpleMath::Vector3(1.0f, 2.0f, 3.0f) +
                    DirectX::SimpleMath::Vector3(3.0f, 4.0f, 5.0f);
                ankerl::nanobench::doNotOptimizeAway(results.smVec3);
            });

        bench.run("DirectX::SimpleMath::Vector4 addition",
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
                results.glmVec2 = glm::vec2(1.0f, 2.0f) + glm::vec2(3.0f, 4.0f);
                ankerl::nanobench::doNotOptimizeAway(results.glmVec2);
            });

        bench.run("glm::vec3 addition",
            [&]
            {
                results.glmVec3 =
                    glm::vec3(1.0f, 2.0f, 3.0f) + glm::vec3(3.0f, 4.0f, 5.0f);
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
                DirectX::XMVECTOR result = DirectX::XMVectorAdd(lhsVec, rhsVec);

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
                DirectX::XMVECTOR result = DirectX::XMVectorAdd(lhsVec, rhsVec);

                DirectX::XMStoreFloat3(&results.dxVec3, result);
                ankerl::nanobench::doNotOptimizeAway(results.dxVec3);
            });

        bench.run("DirectX::XMFLOAT4 addition",
            [&]
            {
                DirectX::XMFLOAT4 lhs(1.0f, 2.0f, 3.0f, 4.0f);
                DirectX::XMFLOAT4 rhs(3.0f, 4.0f, 5.0f, 6.0f);
                DirectX::XMVECTOR lhsVec = DirectX::XMLoadFloat4(&lhs);
                DirectX::XMVECTOR rhsVec = DirectX::XMLoadFloat4(&rhs);
                DirectX::XMVECTOR result = DirectX::XMVectorAdd(lhsVec, rhsVec);

                DirectX::XMStoreFloat4(&results.dxVec4, result);
                ankerl::nanobench::doNotOptimizeAway(results.dxVec4);
            });

        bench.run("DirectX::XMFLOAT2 addition without Loads",
            [&]
            {
                results.dxVecC = DirectX::XMVectorAdd(
                    DirectX::XMVECTOR{}, DirectX::XMVECTOR{});
                ankerl::nanobench::doNotOptimizeAway(results.dxVecC);
            });

        bench.run("DirectX::XMFLOAT3 addition without Loads",
            [&]
            {
                results.dxVecC = DirectX::XMVectorAdd(
                    DirectX::XMVECTOR{}, DirectX::XMVECTOR{});
                ankerl::nanobench::doNotOptimizeAway(results.dxVecC);
            });

        bench.run("DirectX::XMFLOAT4 addition without Loads",
            [&]
            {
                results.dxVecC = DirectX::XMVectorAdd(
                    DirectX::XMVECTOR{}, DirectX::XMVECTOR{});
                ankerl::nanobench::doNotOptimizeAway(results.dxVecC);
            });
    }

    void complex1(ankerl::nanobench::Bench& bench)
    {
        bench.run("Complex operation 1 with DirectX::SimpleMath::Vector*",
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
                auto y = XMVectorGetX(
                    XMVector3Dot(XMVector3Cross(vec3aVec, vec3bVec), vec3bVec));
                auto z = XMVectorGetX(XMVector4Dot(vec4aVec, vec4bVec));
                auto w = XMVectorGetX(
                    XMVector4Dot(XMVectorAdd(vec4aVec, vec4bVec), vec4bVec));

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
    }

    void complex2vec3(ankerl::nanobench::Bench& bench)
    {
        bench.run("Complex operation 2 with DirectX::SimpleMath::Vector3",
            [&]
            {
                DirectX::SimpleMath::Vector3 vec3a(1.0f, 2.0f, 3.0f);
                DirectX::SimpleMath::Vector3 vec3b(3.0f, 4.0f, 5.0f);
                DirectX::SimpleMath::Vector3 vec3c(5.0f, 6.0f, 7.0f);
                DirectX::SimpleMath::Vector3 vec3d(7.0f, 8.0f, 9.0f);

                results.smVec3 = ((vec3a + vec3b) * vec3c - vec3d).Cross(vec3a);
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
    }

    void complex3vec4(ankerl::nanobench::Bench& bench)
    {
        bench.run("Complex operation 3 with DirectX::SimpleMath::Vector4",
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
                    XMVectorMultiply(XMVectorAdd(vec3aVec, vec3bVec), vec3cVec),
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
    }
}  // namespace mathbench

int main(int argc, char** argv)
{
    ankerl::nanobench::Bench bench;
    bench.minEpochIterations(170965776);
    mathbench::addition(bench);
    mathbench::complex1(bench);
    mathbench::complex2vec3(bench);
    mathbench::complex3vec4(bench);
    return 0;
}
