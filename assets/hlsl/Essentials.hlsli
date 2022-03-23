#define WIDTH 1280
#define HEIGHT 720

float sdfSphere(float3 center, float radius)
{
    return length(center) - radius;
}

float3 GetRayDirection(float3 view, float fov, float2 uv)
{
    float2 xy = uv - float2(WIDTH, HEIGHT) / 2.0f;
    float focalLength = HEIGHT / tan(radians(fov)) / 2.0f;
    
    return normalize(float3(xy, -focalLength));
}