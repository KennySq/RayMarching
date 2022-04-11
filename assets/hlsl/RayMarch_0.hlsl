#define MAX_STEPS 64
#define EPSILON 0.001f
#define MAX_DISTANCE 100.0f

#include "Essentials.hlsli"

cbuffer Constants : register(b0)
{
	float4 Padding;
    float EyeX, EyeY, EyeZ;
    float Fov;
    float2 UV;
    float PosX, PosY, PosZ;
}

static const float3 gLightPosition = float3(0, 100, 0);

SamplerState DefaultSamplerState : register(s0);
Texture3D<unorm float4> mCloudTexture : register(t0);

struct Pixel
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD;
};

float sdfSmooth(float a, float b, float k)
{
	float res = exp(-k * a) + exp(-k * b);
	return -log(max(0.0001, res)) / k;
}

float sdfScene(float3 samplePoint)
{
    float cube0 = sdfCube(samplePoint - float3(PosX, PosY, PosZ), 1.0f);
    return cube0;
}

float3 ApproximateNormal(float3 samplePoint, float cloudSample)
{
	float x = sdfScene(float3(samplePoint.x + EPSILON, samplePoint.y, samplePoint.z))
						 - sdfScene(float3(samplePoint.x - EPSILON, samplePoint.y, samplePoint.z));
	float y = sdfScene(float3(samplePoint.x, samplePoint.y + EPSILON, samplePoint.z))
						 - sdfScene(float3(samplePoint.x, samplePoint.y - EPSILON, samplePoint.z));
	float z = sdfScene(float3(samplePoint.x, samplePoint.y, samplePoint.z + EPSILON))
						 - sdfScene(float3(samplePoint.x, samplePoint.y, samplePoint.z - EPSILON));

    return float3(x, y, z) * cloudSample;
}

float4x4 generateView(float3 eye, float3 at, float3 up)
{
	float3 f = normalize(at - eye);
	float3 s = normalize(cross(f, up));
	float3 u = cross(s, f);
	
	return float4x4(
		float4(s,0),
		float4(u,0),
		float4(-f,0),
		float4(0,0,0,1));
}

float march(float3 view, float3 uvw, float near, float far, float3 dir)
{
    float depth = near;

    int totalSlice = 64;
    float accumulation = 0.0f;
	float3 lightDir = gLightPosition - float3(0,0,0);
    
	[unroll(64)]
    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 samplePoint = float3(uvw.xy * uvw.z, depth / (far - near));
        samplePoint.x += UV.x;
        samplePoint.y += UV.y;
        
        float cloudSample = mCloudTexture.Sample(DefaultSamplerState, samplePoint).x;
		
		float3 viewDir = view + (depth * uvw);
        float distance = sdfScene(viewDir);
		float3 normal = ApproximateNormal(viewDir, cloudSample);
        float diffuse = dot(-lightDir, normal) + 0.5f * 0.5f;
		
        float discrimination = distance * cloudSample; 
        if (discrimination < EPSILON) // where pass cube sdf and cloud texture sampling
        {
            accumulation += discrimination;
        }
        
        depth += distance/cloudSample;
    
        if (depth > far)
        {
            return far;
        }
    }
    
    return (accumulation / totalSlice) + 0.5f * 0.5f;
}

float4 frag(Pixel input) : SV_Target0
{
    float3 eye = float3(EyeX, EyeY, EyeZ);
	float4x4 viewMat = generateView(eye, float3(0,0,0), float3(0,1,0));
    float2 fragCoord = input.Texcoord * float2(WIDTH, HEIGHT);
    float3 dir = GetRayDirection(eye, Fov, fragCoord);
    float3 worldDir = mul(dir, (float3x3) viewMat);
    
    //float4 cloudSample = mCloudTexture.Sample(DefaultSamplerState, worldDir);
	
    float dist = march(eye, worldDir, 0.01f, 100.0f, dir);
    if (dist < EPSILON + 0.5f * 0.5f)
    {
        return float4(0.3, 0.5, 1.0, 0);
    }
	
   
    return dist;
}