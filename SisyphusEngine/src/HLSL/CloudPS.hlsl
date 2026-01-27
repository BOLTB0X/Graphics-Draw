#define MAX_STEPS 100
#define MARCH_SIZE 0.08
#include "Common.hlsli"


Texture2D iNoise : register(t0); // 노이즈 텍스처
Texture2D iBlueNoise : register(t1); // 블루 노이즈
SamplerState iSampler : register(s0); // 샘플러 상태


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


float4 raymarch(float3 rayOrigin, float3 rayDirection, float2 uv)
{
    float4 res = float4(0, 0, 0, 0);
    
    float blueNoise = iBlueNoise.Sample(iSampler, uv * (iResolution / 1024.0f)).r;
    float offset = frac(blueNoise + float(iFrame % 32) / sqrt(0.5));
    
    float depth = MARCH_SIZE * offset;
    float3 p = rayOrigin + depth * rayDirection;

    float3 sunDirection = normalize(iLightPos);

    for (int i = 0; i < MAX_STEPS; i++)
    {
        //float density = randomScene(p);
        float density = scene(p);

        // 밀도가 0보다 클 경우에만 밀도를 draw
        if (density > 0.0f)
        {
            float shadowDist = 0.3f;
            //float densityNearSun = randomScene(p + shadowDist * sunDirection);
            float densityNearSun = scene(p + shadowDist * sunDirection);
            float diffuse = clamp((density - densityNearSun) / shadowDist, 0.0f, 1.0f);

            // 구름의 기본 색상 (밀도에 따른 보간)
            float3 baseColor = lerp(float3(1.0f, 1.0f, 1.0f), float3(0.5f, 0.5f, 0.5f), density);

            // 조명 색상 조합
            float3 ambient = float3(0.6f, 0.6f, 0.75f) * 1.1f;
            float3 lightEffect = iLightColor.rgb * iIntensity * diffuse;
            float3 finalColor = baseColor * (ambient + lightEffect);

            float4 color = float4(finalColor, density);

            // 알파 블렌딩 (Front-to-Back)
            color.rgb *= color.a;
            res += color * (1.0f - res.a);
            
            if (res.a > 0.95f)
                break;
        }

        depth += MARCH_SIZE;

        p = rayOrigin + depth * rayDirection;
    } // for (int i = 0; i < MAX_STEPS; i++)

    return res;
} // raymarch


float4 main(PixelInput input) : SV_TARGET
{
    // Ray Origin 및 Ray Direction
    float3 ro = iCameraPos;
    float3 rd = normalize(input.worldPos - iCameraPos);
    float4 res = raymarch(ro, rd, input.tex);
    
    return res;
    //return float4(res.rgb, 1.0f);
} // main