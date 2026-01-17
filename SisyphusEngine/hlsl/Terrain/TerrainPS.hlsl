// hlsl/Terrain/TerrainPS.hlsl
Texture2D NormalMap : register(t0);
SamplerState sampleType : register(s0);

cbuffer MaterialBuffer : register(b2)
{
    int type;
    float gTime;
    float3 padding;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float3 worldPos : TEXCOORD1;
};


float4 main(PS_INPUT input) : SV_TARGET
{
    // 노말맵에서 미세 굴곡 가져오기
    float2 scrollUV = input.texCoord * 10.0f + float2(gTime * 0.01f, gTime * 0.01f);
    float3 normalSample = NormalMap.Sample(sampleType, scrollUV).rgb * 2.0f - 1.0f;
    
    // TBN 연산으로 월드 공간 노말 생성
    float3x3 TBN = float3x3(input.tangent, input.binormal, input.normal);
    float3 worldNormal = normalize(mul(normalSample, TBN));

    // 낮은 곳: 청회, 높은 곳: 흰색
    float heightFactor = saturate(input.worldPos.y / 15.0f);
    float3 baseColor = lerp(float3(0.4f, 0.5f, 0.6f), float3(1.0f, 1.0f, 1.0f), heightFactor);

    // 간단한 조명
    float3 lightDir = normalize(float3(0.5f, 1.0f, 0.5f));
    float diff = saturate(dot(worldNormal, lightDir));
    
    // 최종 색상 조합
    float3 finalColor = baseColor * (diff + 0.3f);

    return float4(finalColor, 1.0f);
} // main