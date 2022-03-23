#define MAX_STEPS 256
#define EPSILON 0.00001f
#define MAX_DISTANCE 100.0f

#include"Essentials.hlsli"

struct Pixel
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD;
};

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
    
    float dist = march(view, dir, 0.01f, 100.0f);
    if (dist > MAX_DISTANCE - EPSILON)
    {
        return float4(0, 0, 0, 0);
    }
   
    return float4(1, 1, 1, 0);
}