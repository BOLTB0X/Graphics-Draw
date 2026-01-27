#include "Common.hlsli"

cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}; // MatrixBuffer


struct VertexInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
}; // VertexInput


PixelInput main(VertexInput input)
{
    PixelInput output;
    
    input.position.w = 1.0f;
    
    float4 worldPos = mul(input.position, modelMatrix);
    output.worldPos = worldPos.xyz;
    output.worldNormal = normalize(mul(input.normal, (float3x3) modelMatrix));
    
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input.tex;
    
    return output;
} // main