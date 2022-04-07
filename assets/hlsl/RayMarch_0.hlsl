#define MAX_STEPS 256
#define EPSILON 0.001f
#define MAX_DISTANCE 100.0f

#include "Essentials.hlsli"

cbuffer Constants : register(b0)
{
	float4 Fade;
}

static const float3 gLightPosition = float3(-50, 100, -100);

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
	float cube0 = sdfCube(samplePoint - float3(-Fade.x,0,0), 0.5f);
	float sphere0 = sdfSphere(samplePoint, 1.0f);
	return sdfSmooth(cube0, sphere0, 5.0f);
}

float sdfNoise(float3 samplePoint)
{
    float4 sample = mCloudTexture.Sample(DefaultSamplerState, samplePoint);
	
    return sample.x;
}

float3 ApproximateNormal(float3 samplePoint)
{
	float x = sdfScene(float3(samplePoint.x + EPSILON, samplePoint.y, samplePoint.z))
						 - sdfScene(float3(samplePoint.x - EPSILON, samplePoint.y, samplePoint.z));
	float y = sdfScene(float3(samplePoint.x, samplePoint.y + EPSILON, samplePoint.z))
						 - sdfScene(float3(samplePoint.x, samplePoint.y - EPSILON, samplePoint.z));
	float z = sdfScene(float3(samplePoint.x, samplePoint.y, samplePoint.z + EPSILON))
						 - sdfScene(float3(samplePoint.x, samplePoint.y, samplePoint.z - EPSILON));

	return float3(x, y, z);
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

float march(float3 view, float3 uvw, float near, float far, float cloudDist)
{
    float depth = near;
	float theta = Fade.x;
	float4x4 rotMat = float4x4(
		float4(1,0,0,0),	
		float4(0,cos(theta),-sin(theta),0),
		float4(0,sin(theta),cos(theta),0),
		float4(0,0,0,1)
	);

    for (int i = 0; i < MAX_STEPS; i++)
    {
		float3 viewDir = view + (depth * uvw);
        float distance = sdfNoise(viewDir);
		float3 normal = ApproximateNormal(viewDir);
		float3 lightDir = gLightPosition - float3(0,0,0);
        float diffuse = dot(-lightDir, normal);
		
        if (distance < EPSILON)
        {
            return diffuse;
        }
        
        depth += distance * cloudDist;
        
        if (depth > far)
        {
            return far;
        }
    }
    
    return far;
}

float4 frag(Pixel input) : SV_Target0
{
    float3 eye = float3(0.0f, -0.0f,2);
	float4x4 viewMat = generateView(eye, float3(0,0,0), float3(0,1,0));
    float2 fragCoord = input.Texcoord * float2(WIDTH, HEIGHT);
    float3 dir = GetRayDirection(eye, 50.0f, fragCoord);
    float3 worldDir = mul(dir, (float3x3) viewMat);
    
	float4 cloudSample = mCloudTexture.Sample(DefaultSamplerState, float3(input.Texcoord, 0.0f));
    //float4 cloudSample = mCloudTexture.Sample(DefaultSamplerState, worldDir);
	
    float dist = march(eye, worldDir, 0.01f, 100.0f, cloudSample.x);
    if (dist > MAX_DISTANCE - EPSILON)
    {
        return float4(0, 0, 0, 0);
    }
	
   
    return dist;
}