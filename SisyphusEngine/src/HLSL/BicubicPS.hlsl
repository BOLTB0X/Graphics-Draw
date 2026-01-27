#include "Common.hlsli"

Texture2D iLowResTexture : register(t0);
SamplerState iSampler : register(s0);

// 2차 가중 함수
float w0(float a)
{
    return (1.0f / 6.0f) * (a * (a * (-a + 3.0f) - 3.0f) + 1.0f);
}  // w0


float w1(float a)
{
    return (1.0f / 6.0f) * (a * a * (3.0f * a - 6.0f) + 4.0f);
} // w1


float w2(float a)
{
    return (1.0f / 6.0f) * (a * (a * (-3.0f * a + 3.0f) + 3.0f) + 1.0f);
} // w2


float w3(float a)
{
    return (1.0f / 6.0f) * (a * a * a);
} // w3


// g0와 g1은 진폭 함수
float g0(float a)
{
    return w0(a) + w1(a);
} // g0


float g1(float a)
{
    return w2(a) + w3(a);
} // g1


// h0와 h1은 하드웨어 가속 이중선형 필터링을 위한 오프셋 함수
float h0(float a)
{
    return -1.0f + w1(a) / (w0(a) + w1(a));
} // h0


float h1(float a)
{
    return 1.0f + w3(a) / (w2(a) + w3(a));
} //h1


float4 main(PixelInput input) : SV_TARGET
{
    float2 uv = input.tex;
    
    float2 texSize;
    iLowResTexture.GetDimensions(texSize.x, texSize.y);
    float2 invTexSize = 1.0f / texSize;

    uv = uv * texSize + 0.5f;
    float2 iuv = floor(uv);
    float2 fuv = frac(uv);

    // 가중치 및 오프셋 계산
    float g0x = g0(fuv.x);
    float g1x = g1(fuv.x);
    float h0x = h0(fuv.x);
    float h1x = h1(fuv.x);
    float h0y = h0(fuv.y);
    float h1y = h1(fuv.y);

    // 주변 4개 지점 샘플링 좌표 계산
    float2 p0 = (float2(iuv.x + h0x, iuv.y + h0y) - 0.5f) * invTexSize;
    float2 p1 = (float2(iuv.x + h1x, iuv.y + h0y) - 0.5f) * invTexSize;
    float2 p2 = (float2(iuv.x + h0x, iuv.y + h1y) - 0.5f) * invTexSize;
    float2 p3 = (float2(iuv.x + h1x, iuv.y + h1y) - 0.5f) * invTexSize;

    return g0(fuv.y) * (g0x * iLowResTexture.Sample(iSampler, p0)
                            + g1x * iLowResTexture.Sample(iSampler, p1))
         + g1(fuv.y) * (g0x * iLowResTexture.Sample(iSampler, p2)
                            + g1x * iLowResTexture.Sample(iSampler, p3));
} // main