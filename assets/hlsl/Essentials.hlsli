#define WIDTH 1280
#define HEIGHT 720

float sdfCube(float3 samplePoint, float3 size)
{
	float3 d = abs(samplePoint) - size;

	float inDistance = min(max(d.x, max(d.y, d.z)), 0.0f);
	float outDistance = length(max(d, 0.0f));

	return inDistance + outDistance;
}

float sdfSphere(float3 viewDir, float3 center, float radius)
{
    return distance(viewDir, center) - radius;
}

float sdfSphere(float3 samplePoint, float radius)
{
	return length(samplePoint) - radius;
}

float3 GetRayDirection(float3 view, float fov, float2 uv)
{
    float2 xy = view.xy + (uv - float2(WIDTH, HEIGHT) / 2.0f);
    float focalLength = view.z + (HEIGHT / tan(radians(fov)) / 2.0f);
    
    return normalize(float3(xy, -focalLength));
}