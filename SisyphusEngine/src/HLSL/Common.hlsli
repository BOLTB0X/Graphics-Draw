// Common.hlsli
#define PI 3.14159265359

cbuffer GlobalBuffer : register(b1)
{
    // 1
    float iTime;
    float iFrame;
    float2 padding0;

    // 2
    float3 iCameraPos;
    float padding1;

    // 3
    float2 iResolution;
    float iNoiseRes;
    float padding2;
}; // GlobalBuffer


cbuffer LightBuffer : register(b2)
{
    float3 iLightPos;
    float iIntensity;
    float4 iLightColor;
}; // LightBuffer


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 worldPos : POSITION;
    float3 worldNormal : NORMAL;
}; // PixelInput


// 2D 회전 행렬 생성
float2x2 rotate2D(float a)
{
    float s = sin(a);
    float c = cos(a);
    return float2x2(c, -s, s, c);
} // rotate2D


// 부드러운 스텝 함수
float nextStep(float t, float len, float smo)
{
    float tt = (t + smo) % len;
    float stp = floor((t + smo) / len) - 1.0f;
    return smoothstep(0.0f, smo, tt) + stp;
} // nextStep


// 구 SDF
float sdSphere(float3 p, float radius)
{
    return length(p) - radius;
} // sdSphere


// 캡슐 SDF
float sdCapsule(float3 p, float3 a, float3 b, float r)
{
    float3 ab = b - a;
    float3 ap = p - a;
    float t = dot(ab, ap) / dot(ab, ab);
    t = clamp(t, 0.0f, 1.0f);
    float3 c = a + t * ab;
    return length(p - c) - r;
} // sdCapsule


// 도넛 SDF
float sdTorus(float3 p, float2 r)
{
    float x = length(p.xz) - r.x;
    return length(float2(x, p.y)) - r.y;
} // sdTorus


// 십자형 SDF
float sdCross(float3 p, float s)
{
    float da = max(abs(p.x), abs(p.y));
    float db = max(abs(p.y), abs(p.z));
    float dc = max(abs(p.z), abs(p.x));
    return min(da, min(db, dc)) - s;
} // sdCross


// Texture 기반
float GetNoise(float3 x, Texture2D tex, SamplerState samp, float resolution)
{
    float3 p = floor(x);
    float3 f = frac(x);
    
    // Quintic 보간
    f = f * f * (3.0f - 2.0f * f);

    // 3D 좌표를 2D 텍스처 좌표로 투영하는 기법
    float2 uv = (p.xy + float2(37.0f, 239.0f) * p.z) + f.xy;
    
    // 인자로 받은 resolution을 사용하여 샘플링 좌표 정규화
    float2 rg = tex.SampleLevel(samp, (uv + 0.5f) / resolution, 0.0f).yx;

    // 0~1 범위를 -1~1 범위로 매핑
    return lerp(rg.x, rg.y, f.z) * 2.0f - 1.0f;
} // GetNoise

float3 mod289(float3 x)
{
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

float2 mod289(float2 x)
{
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
}

float3 permute(float3 x)
{
    return mod289(((x * 34.0f) + 1.0f) * x);
}

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

    // Permutations
    i = mod289(i);
    float3 p = permute(permute(i.y + float3(0.0f, i1.y, 1.0f))
               + i.x + float3(0.0f, i1.x, 1.0f));

    float3 m = max(0.5f - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.0f);
    m = m * m;
    m = m * m;

    // Gradients
    float3 x = 2.0f * frac(p * C.www) - 1.0f;
    float3 h = abs(x) - 0.5f;
    float3 ox = floor(x + 0.5f);
    float3 a0 = x - ox;

    // Normalise gradients
    m *= 1.79284291400159f - 0.85373472095314f * (a0 * a0 + h * h);

    // Final noise value
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    
    return 130.0f * dot(m, g);
}