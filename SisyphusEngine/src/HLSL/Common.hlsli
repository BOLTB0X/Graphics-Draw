cbuffer GlobalBuffer : register(b1)
{
    // Row 1
    float iTime;
    float iFrame;
    float2 iResolution;

    // Row 2
    float3 iCameraPos;
    float iNoiseRes;

    // Row 3
    float iCloudType;
    float3 padding; 
};


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

