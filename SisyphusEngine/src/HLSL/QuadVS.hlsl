cbuffer MatrixBuffer : register(b0)
{
    matrix modelMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
}; // MatrixBuffer


struct QuadInput
{
    float4 position : POSITION;
    float2 tex : TEXCOORD0;
}; // VertexInput


struct PixelInput
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
}; // PixelInput


PixelInput main(QuadInput input)
{
    PixelInput output;
    
    input.position.w = 1.0f;
    
    float4 worldPos = mul(input.position, modelMatrix);
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    output.tex = input.tex;
    
    return output;
} // main