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
}; // VertexInput


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
}; // PixelInput


PixelInput main(VertexInput input)
{
    PixelInput output;
    
    input.position.w = 1.0f;
    //output.position = input.position;
    output.position = mul(input.position, modelMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    
    output.tex = input.tex;
    
    return output;
} // main