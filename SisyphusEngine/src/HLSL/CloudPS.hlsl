#define MAX_STEPS 100
#define MARCH_SIZE 0.08
#include "Common.hlsli"

Texture2D uNoise : register(t0); // 노이즈 텍스처
SamplerState uSampler : register(s0); // 샘플러 상태

cbuffer GlobalBuffer : register(b1)
{
    float uTime;
    float3 padding;
    
    float2 uResolution;
    float uNoiseRes;
    float padding2;
}; // GlobalBuffer


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
}; // PixelInput


// FBM (Fractal BrownianMotion)
// 여러 옥타브의 노이즈를 중첩
float fbm(float3 p, float time)
{
    // 시간에 따른 흐름 추가
    float3 q = p + time * 0.5f * float3(1.0f, -0.2f, -1.0f);
    
    float f = 0.0f;
    float scale = 0.5f;
    float factor = 2.02f;

    for (int i = 0; i < 6; i++)
    {
        f += scale * GetNoise(q, uNoise, uSampler, 128.0f);
        q *= factor;
        factor += 0.21f;
        scale *= 0.5f;
    }

    return f;
} // fbm


// 밀도 정의
float scene(float3 p)
{
    float distance = sdSphere(p, 1.0f); // 기본 뼈대
    float f = fbm(p, uTime); // 노이즈 추가
    
    // 구 안쪽(음수 거리)일수록 밀도가 높아지도록 반전시키고 노이즈를 더함
    return -distance + f;
} // scene


float4 raymarch(float3 rayOrigin, float3 rayDirection)
{
    float4 res = float4(0, 0, 0, 0);
    float depth = 0.0f;
    float3 p = rayOrigin + depth * rayDirection;

    for (int i = 0; i < MAX_STEPS; i++)
    {
        float density = scene(p);

        // 밀도가 0보다 클 경우에만 밀도를 draw
        if (density > 0.0f)
        {
            float3 baseColor = lerp(float3(1, 1, 1), float3(0, 0, 0), density);
            float4 color = float4(baseColor, density);
            
            color.rgb *= color.a;
            res += color * (1.0f - res.a);
        }

        depth += MARCH_SIZE;
        p = rayOrigin + depth * rayDirection;
        
    } // for (int i = 0; i < MAX_STEPS; i++)

    return res;
} // raymarch


float4 main(PixelInput input) : SV_TARGET
{
    // 화면 좌표 정규화
    float2 uv = input.position.xy / uResolution.xy;
    uv -= 0.5f;
    uv.y *= -1.0f; // DX 좌표계 보정
    uv.x *= (uResolution.x / uResolution.y);

    // Ray Origin 및 Ray Direction
    float3 ro = float3(0.0f, 0.0f, 5.0f);
    float3 rd = normalize(float3(uv, -1.0f));

    float4 res = raymarch(ro, rd);
    return float4(res.rgb, res.a);
} // main