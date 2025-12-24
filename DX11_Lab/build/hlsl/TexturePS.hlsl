Texture2D shaderTexture : register(t0);
SamplerState sampleState : register(s0);

struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 PS_Main(PixelInput input) : SV_TARGET
{
    float4 textureColor = shaderTexture.Sample(sampleState, input.tex);

    float3 lightDir = normalize(float3(0.5f, -1.0f, 0.5f));
    float lightIntensity = saturate(dot(input.normal, -lightDir));
    
    float4 color = textureColor * (lightIntensity + 0.2f); 
    
    color.a = 1.0f;
    return color;
}