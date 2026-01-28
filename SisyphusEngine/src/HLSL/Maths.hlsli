#define PI 3.14159265359


// 일반적인 수학

float3 mod289(float3 x)
{
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
} // mod289


float2 mod289(float2 x)
{
    return x - floor(x * (1.0f / 289.0f)) * 289.0f;
} // mod289


float3 permute(float3 x)
{
    return mod289(((x * 34.0f) + 1.0f) * x);
} // permute


float rand(float3 p)
{
    return frac(sin(dot(p, float3(12.9898, 78.233, 45.164))) * 43758.5453123);
} // rand

//

// SDF(Signed DistanceFunction, or Signed Distance Field)

// 구 SDF
float sdSphere(float3 p, float radius)
{
    return length(p) - radius;
} // sdSphere


// 캡슐 SDF
float sdCapsule(float3 p, float3 a, float3 b, float r)
{
    float3 ab = b - a;
    float3 ap = p - a;
    float t = dot(ab, ap) / dot(ab, ab);
    t = clamp(t, 0.0f, 1.0f);
    float3 c = a + t * ab;
    return length(p - c) - r;
} // sdCapsule


// 도넛 SDF
float sdTorus(float3 p, float2 r)
{
    float x = length(p.xz) - r.x;
    return length(float2(x, p.y)) - r.y;
} // sdTorus


// 십자형 SDF
float sdCross(float3 p, float s)
{
    float da = max(abs(p.x), abs(p.y));
    float db = max(abs(p.y), abs(p.z));
    float dc = max(abs(p.z), abs(p.x));
    return min(da, min(db, dc)) - s;
} // sdCross

// etc

// 2D 회전 행렬 생성
float2x2 rotate2D(float a)
{
    float s = sin(a);
    float c = cos(a);
    return float2x2(c, -s, s, c);
} // rotate2D


// 부드러운 스텝 함수
float nextStep(float t, float len, float smo)
{
    float tt = (t + smo) % len;
    float stp = floor((t + smo) / len) - 1.0f;
    return smoothstep(0.0f, smo, tt) + stp;
} // nextStep

