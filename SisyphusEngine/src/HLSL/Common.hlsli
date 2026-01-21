// Common.hlsli

// 기본 SDF 구체
float sdSphere(float3 p, float radius)
{
    return length(p) - radius;
} // sdSphere


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