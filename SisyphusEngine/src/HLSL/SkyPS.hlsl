// https://www.shadertoy.com/view/4dl3zr, 배경과 태양부분 참고
#include "Common.hlsli"
#include "Maths.hlsli"

struct PixelSkyInput
{
    float4 position : SV_POSITION;
    float3 localPos : TEXCOORD0;
}; // PixelSkyInput


// 2D Simplex Noise 구현
float snoise(float2 v)
{
    const float4 C = float4(0.211324865405187f, // (3.0-sqrt(3.0))/6.0
                            0.366025403784439f, // 0.5*(sqrt(3.0)-1.0)
                           -0.577350269189626f, // -1.0 + 2.0 * C.x
                            0.024390243902439f); // 1.0 / 41.0

    // First corner
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);

    // Other corners
    float2 i1;
    i1 = (x0.x > x0.y) ? float2(1.0f, 0.0f) : float2(0.0f, 1.0f);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;

    // 순열
    i = mod289(i);
    float3 p = permute(permute(i.y + float3(0.0f, i1.y, 1.0f))
               + i.x + float3(0.0f, i1.x, 1.0f));

    float3 m = max(0.5f - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0f);
    m = m * m;
    m = m * m;

    // 그레디언트
    float3 x = 2.0f * frac(p * C.www) - 1.0f;
    float3 h = abs(x) - 0.5f;
    float3 ox = floor(x + 0.5f);
    float3 a0 = x - ox;

    // 정규화 그레디언트
    m *= 1.79284291400159f - 0.85373472095314f * (a0 * a0 + h * h);

    float3 g;
    
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    
    return 130.0f * dot(m, g);
} // snoise


float4 main(PixelSkyInput input) : SV_TARGET
{
    float3 rd = normalize(input.localPos);
    float3 sd = normalize(iLightPos);
    
    float3 skyColor;
    
    if (rd.y > 0)
        skyColor = lerp(float3(0.5f, 0.2f, 0.4f), float3(0.05f, 0.1f, 0.3f), pow(rd.y, 0.3f));
    else
        skyColor = lerp(float3(0.5f, 0.2f, 0.4f), float3(0.05f, 0.02f, 0.1f), pow(abs(rd.y), 0.3f));

    // sunDot이 1에 가까울수록 태양 중심
    float sunDot = dot(rd, sd);
    float sunDist = saturate(1.0f - sunDot);

    // 태양 핵
    float core = smoothstep(0.005f, 0.002f, sunDist);
    
    // 가우시안 블룸
    float bloom = exp(-sunDist * 80.0f) * 2.5f;
    float wideGlow = exp(-sunDist * 10.0f) * 0.6f;

    // RayGlow
    float angle = atan2(rd.z, rd.x);
    float rays = snoise(float2(angle * 3.0f, iTime * 0.15f)) * 0.5f + 0.5f;
    float3 rayEffect = float3(1.0f, 0.8f, 0.5f) * pow(rays, 5.0f) * exp(-sunDist * 15.0f) * 0.4f;

    // 최종
    float3 sunColor = iLightColor.rgb * (core * 6.0f + bloom + wideGlow);    
    return float4(skyColor + sunColor + rayEffect * iIntensity, 0.8f);
} // main