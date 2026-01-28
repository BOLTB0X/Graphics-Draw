#define MAX_STEPS 100
#define MARCH_SIZE 0.08
#include "Common.hlsli"
#include "Maths.hlsli"


Texture2D iNoise : register(t0); // 노이즈 텍스처
Texture2D iBlueNoise : register(t1); // 블루 노이즈
SamplerState iSampler : register(s0); // 샘플러 상태


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
        f += scale * GetNoise(q, iNoise, iSampler, 256.0f);
        q *= factor;
        factor += 0.21f;
        scale *= 0.5f;
    }

    return f;
} // fbm


// 밀도 정의
float scene(float3 p)
{
    float distance = sdSphere(p, 2.5f);
    float f = fbm(p, iTime); // 노이즈 추가
    
    return -distance + f;
} // scene


float planeScene(float3 p)
{
    float groundHeight = 1.0f;
    float cloudThickness = 2.0f;
    
    // 두께가 있는 층(Slab) 느낌으로 변경
    float distance = abs(p.y - groundHeight) - cloudThickness;
    
    // 노이즈 샘플링 시 카메라 위치를 오프셋
    float f = fbm(p, iTime);
    
    return -distance + f;
} // planeScene


float randomScene(float3 p)
{
    float3 p1 = p;
    
    // 회전
    p1.xz = mul(p1.xz, rotate2D(-PI * 0.1f));
    p1.yz = mul(p1.yz, rotate2D(PI * 0.3f));

    // 각 SDF 계산
    float s1 = sdTorus(p1, float2(1.3f, 0.9f));
    float s2 = sdCross(p1 * 2.0f, 0.6f) / 2.0f; // 스케일링 보정
    float s3 = sdSphere(p, 1.5f);
    float s4 = sdCapsule(p, float3(-2.0f, -1.5f, 0.0f), float3(2.0f, 1.5f, 0.0f), 1.0f);

    // 시간 기반 모핑 상태 계산
    float stepValue = nextStep(iTime, 3.0f, 1.2f);
    float t = stepValue % 4.0f;

    // lerp(mix)를 이용한 부드러운 도형 전환
    float d = lerp(s1, s2, clamp(t, 0.0f, 1.0f));
    d = lerp(d, s3, clamp(t - 1.0f, 0.0f, 1.0f));
    d = lerp(d, s4, clamp(t - 2.0f, 0.0f, 1.0f));
    d = lerp(d, s1, clamp(t - 3.0f, 0.0f, 1.0f));

    // fbm 합성
    float f = fbm(p, iTime);

    return -d + f;
} // randomScene


float4 rayMarch(float3 rayOrigin, float3 rayDirection, float2 uv)
{
    float4 res = float4(0, 0, 0, 0);
    
    // 블루 노이즈 디더링
    float blueNoise = iBlueNoise.Sample(iSampler, uv * (iResolution / 1024.0f)).r;
    float offset = frac(blueNoise + float(iFrame % 32) / sqrt(0.5));
    
    float depth = MARCH_SIZE * offset;
    float3 sunPos = iLightPos;
    float3 sunDirection = normalize(sunPos);

    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 p = rayOrigin + depth * rayDirection;
        float density = 0.0f;
        
        switch (iCloudType)
        {
            case 0:
                density = scene(p);
                break;
            case 1:
                density = planeScene(p);
                break;
            default:
                density = randomScene(p);
                break;
        }
        
        if (density > 0.0f)
        {
            float shadowDist = 0.4f;
            float densityNearSun = 0.0f;
            
            switch (iCloudType)
            {
                case 0:
                    densityNearSun = scene(p + shadowDist * sunDirection);
                    break;
                case 1:
                    densityNearSun = planeScene(p + shadowDist * sunDirection);
                    break;
                default:
                    densityNearSun = randomScene(p + shadowDist * sunDirection);
                    break;
            }
        
            
            // 확산광
            float diffuse = pow(saturate((density - densityNearSun) / shadowDist), 2.0f);

            // 역제곱 법칙(Inverse Square Law) 느낌
            // 중력, 전자기력, 빛의 세기 등 물리적 힘의 강도가 원천으로부터 거리의 제곱에 반비례하여 급격히 줄어드는 원리(위키백과)
            // 태양과 현재 구름 입자(p) 사이의 거리 계산
            float distToSun = length(sunPos - p);
            float falloff = 1.0f / (1.0f + distToSun * 0.1f); // 거리에 따른 감쇄 수치

            // Mie Scattering (태양을 바라볼 때 구름 경계가 밝아짐)
            float cosTheta = dot(rayDirection, sunDirection);
            float mie = 0.5f + 0.5f * pow(saturate(cosTheta), 8.0f);

            // 최종 조명 색상 조합
            float3 cloudAmbient = float3(0.2f, 0.15f, 0.3f);
            float3 lightEffect = iLightColor.rgb * iIntensity * diffuse * falloff * mie * 3.0f;
            
            float3 baseColor = lerp(float3(1.0f, 1.0f, 1.0f), float3(0.4f, 0.4f, 0.5f), density);
            float3 finalColor = baseColor * (cloudAmbient + lightEffect);

            // 알파 블렌딩
            float alpha = density * 0.4f; // 부드러운 중첩을 위해 밀도 조절
            res.rgb += (1.0f - res.a) * finalColor * alpha;
            res.a += (1.0f - res.a) * alpha;
            
            if (res.a > 0.95f)
                break;
        }

        depth += MARCH_SIZE;
        if (depth > 50.0f)
            break;
    }

    return res;
} // rayMarch


float4 main(PixelInput input) : SV_TARGET
{
    // Ray Origin 및 Ray Direction
    float3 ro = iCameraPos;
    float3 rd = normalize(input.worldPos - iCameraPos);
    float4 res = rayMarch(ro, rd, input.tex);
    
    return res;
} // main