#pragma once
#include <DirectXMath.h>
#include "Model/Mesh/VertexTypes.h"

namespace MathHelper {
    // 수학 상수
    const float PI = 3.1415926535f;
    const float DEG_TO_RAD = PI / 180.0f;
    const float RAD_TO_DEG = 180.0f / PI;

    // 물리/이동 기본 설정
    const float DEFAULT_MOVE_SPEED = 50.0f;
    const float DEFAULT_TURN_SPEED = 150.0f;
    const float DEFAULT_LOOK_SPEED = 75.0f;
    const float FRICTION_RATIO = 0.5f;

    const float MOVE_ACCEL = 1.0f; // 이동 가속도
    const float MOVE_BRAKE = 0.5f; // 이동 감속도 (마찰력)
    const float VERTICAL_MOVE_SPEED = 15.0f;

    const float TURN_ACCEL = 5.0f; // 회전 가속도
    const float TURN_BRAKE = 3.5f; // 회전 감속도
    const float LOOK_ACCEL = 7.5f; // 상하 시선 가속도
    const float LOOK_BRAKE = 2.0f; // 상하 시선 감속도

    // 변환 함수
    inline float ToRadians(float degrees) { return degrees * DEG_TO_RAD; }
    inline float ToDegrees(float radians) { return radians * RAD_TO_DEG; }


    template <typename T>
    T clamp(T value, T min_val, T max_val)
    {
        if (value < min_val)
            return min_val;

        else if (value > max_val)
            return max_val;

        else
            return value;
    } // clamp


    inline void CalculateTangentBinormal(
        const ModelVertex& v1, const ModelVertex& v2, const ModelVertex& v3,
        DirectX::XMFLOAT3& tangent, DirectX::XMFLOAT3& binormal)
    {
        float vector1[3], vector2[3];
        float tuVector[2], tvVector[2];

        // 두 변의 벡터 계산
        vector1[0] = v2.position.x - v1.position.x;
        vector1[1] = v2.position.y - v1.position.y;
        vector1[2] = v2.position.z - v1.position.z;

        vector2[0] = v3.position.x - v1.position.x;
        vector2[1] = v3.position.y - v1.position.y;
        vector2[2] = v3.position.z - v1.position.z;

        // UV 좌표 차이 계산
        tuVector[0] = v2.texture.x - v1.texture.x;
        tvVector[0] = v2.texture.y - v1.texture.y;

        tuVector[1] = v3.texture.x - v1.texture.x;
        tvVector[1] = v3.texture.y - v1.texture.y;

        // 접선/종법선 공식 적용
        float den = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

        tangent.x = (tvVector[1] * vector1[0] - tvVector[0] * vector2[0]) * den;
        tangent.y = (tvVector[1] * vector1[1] - tvVector[0] * vector2[1]) * den;
        tangent.z = (tvVector[1] * vector1[2] - tvVector[0] * vector2[2]) * den;

        binormal.x = (tuVector[0] * vector2[0] - tuVector[1] * vector1[0]) * den;
        binormal.y = (tuVector[0] * vector2[1] - tuVector[1] * vector1[1]) * den;
        binormal.z = (tuVector[0] * vector2[2] - tuVector[1] * vector1[2]) * den;

        // 정규화
        DirectX::XMVECTOR t = DirectX::XMLoadFloat3(&tangent);
        DirectX::XMVECTOR b = DirectX::XMLoadFloat3(&binormal);
        DirectX::XMStoreFloat3(&tangent, DirectX::XMVector3Normalize(t));
        DirectX::XMStoreFloat3(&binormal, DirectX::XMVector3Normalize(b));

        return;
    } // CalculateTangentBinormal


    inline float Frac(float x)
    {
        return x - floorf(x);
    } // Frac


    inline float Lerp(float a, float b, float t)
    {
        return a + t * (b - a);
    } // Lerp


    // frac(sin(dot(p, float2(12.9898, 78.233))) * 43758.5453)
    inline float Hash(DirectX::XMFLOAT2 p)
    {
        float dotProduct = p.x * 12.9898f + p.y * 78.233f;
        return Frac(sinf(dotProduct) * 43758.5453f);
    } // Hash


    // 2D Value Noise (HLSL의 noise 함수 이식)
    inline float Noise(DirectX::XMFLOAT2 p)
    {
        DirectX::XMFLOAT2 i = { floorf(p.x), floorf(p.y) };
        DirectX::XMFLOAT2 f = { p.x - i.x, p.y - i.y };

        float ux = f.x * f.x * (3.0f - 2.0f * f.x);
        float uy = f.y * f.y * (3.0f - 2.0f * f.y);

        float a = Hash({ i.x, i.y });
        float b = Hash({ i.x + 1.0f, i.y });
        float c = Hash({ i.x, i.y + 1.0f });
        float d = Hash({ i.x + 1.0f, i.y + 1.0f });

        return Lerp(Lerp(a, b, ux), Lerp(c, d, ux), uy);
    } // Noise


    // CPU용 FBM 높이 계산 함수
    inline float GetFBMHeight(float x, float z, float time)
    {
        float height = 0.0f;
        float amplitude = 0.5f;
        float frequency = 0.05f;

        for (int i = 0; i < 6; i++)
        {
            height += Noise({ x * frequency + time * 0.1f,  z * frequency + time * 0.1f }) * amplitude;
            amplitude *= 0.45f;
            frequency *= 2.0f;
        }

        return height * 15.0f;
        //return pow(height, 1.2f) * 15.0f;
    } // MathHelper
}