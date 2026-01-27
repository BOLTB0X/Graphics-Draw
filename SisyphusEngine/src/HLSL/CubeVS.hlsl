#include "Common.hlsli"


cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}; // MatrixBuffer


struct VertexInput
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
}; // VertexInput
    

struct PixelSkyInput
{
    float4 position : SV_POSITION;
    float3 localPos : TEXCOORD0;
}; // PixelSkyInput


PixelSkyInput main(VertexInput input)
{
    PixelSkyInput output;
    
    float4 worldPos = mul(float4(input.position, 1.0f), modelMatrix);
    
    // 행렬 곱셈 순서 명확히 분리
    float4 viewPos = mul(worldPos, viewMatrix);
    output.position = mul(viewPos, projectionMatrix);

    // 깊이값 고정
    output.position = output.position.xyww;

    // 로컬 좌표 전달
    output.localPos = input.position;

    return output;
} // main
