#define MAX_STEPS 256
#define EPSILON 0.00001f
#define MAX_DISTANCE 100.0f

#define WIDTH 1280
#define HEIGHT 720

struct Vertex
{
    float3 Position : POSITION;
    float2 Texcoord : TEXCOORD;
};

struct Pixel
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD;
};

Pixel vert(Vertex input)
{
    Pixel output = (Pixel) 0;

    output.Position = float4(input.Position, 1.0f);
    output.Texcoord = input.Texcoord;

	return output;
}

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

float march(float3 view, float3 uvw, float near, float far)
{
    float depth = near;

    for (float i = 0; i < MAX_STEPS; i += 1.0f)
    {
        float distance = sdfSphere(view + (depth * uvw), 1.0f);

        if (distance < EPSILON)
        {
            return depth;
        }
        
        depth += distance;
        
        if (depth >= far)
        {
            return far;
        }
    }
    
    return far;
}

float4 frag(Pixel input) : SV_Target0
{
    float3 view = float3(0, 0, 2);
    float2 fragCoord = input.Texcoord * float2(WIDTH, HEIGHT);
    float3 dir = GetRayDirection(view, 60.0f, fragCoord);
    //float3 dir = GetRayDirection(view, 60.0f, uv);
    
    float dist = march(view, dir, 0.01f, 100.0f);
    
  //  return normalize(float4(uvw, 0.0f));
    
    if (dist > MAX_DISTANCE - EPSILON)
    {
        return float4(0, 0, 0, 0);
    }
    
    return float4(1, 0, 0, 0);

    //return float4(normalize(uv), 0.0f, 0.0f);

}