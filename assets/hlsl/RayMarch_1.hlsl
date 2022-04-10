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
	float AppTime;
}

static const float3 gLightPosition = float3(0, 100, 0);

SamplerState DefaultSamplerState : register(s0);
Texture3D<unorm float4> mCloudTexture : register(t0);

struct Pixel
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD;
};

float sdfScene(float3 samplePoint)
{
    float cube0 = sdfCube(samplePoint - float3(PosX, PosY, PosZ), 1.0f);
    return cube0;
}

float sdfSmooth(float a, float b, float k)
{
	float res = exp(-k * a) + exp(-k * b);
	return -log(max(0.0001, res)) / k;
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

float Cloud(float3 rayDir, float3 viewPos, float3 lightDir, float near, float far)
{
	float depth = near;
	float attenuation = 0.0f;
	float diffuse = 1.0f;


	[unroll(MAX_STEPS)]
	for(int i = 0; i < MAX_STEPS; i++)
	{
	//	float3 texSamplePoint = float3(rayDir.xy * rayDir.z, depth / (far - near));
		float3 texSamplePoint = float3(rayDir.xy, rayDir.z) + viewPos;
		texSamplePoint.x += UV.x * AppTime;
		texSamplePoint.y += UV.y;

		float cloudSample = mCloudTexture.Sample(DefaultSamplerState, texSamplePoint).x;
		float3 samplePoint = viewPos + (depth * rayDir);
		float sceneDist = sdfScene(samplePoint);


		attenuation += cloudSample;
	
		if(sceneDist < EPSILON) // within unit box
		{
			return diffuse / attenuation;//sdfSmooth(0.12, 0.35, cloudSample);
		}

		depth += sceneDist;
	
		if(depth > far)
		{
			return far;
		}
	}
	return far;
}

float4 frag(Pixel input) : SV_Target0
{
    float3 eye = float3(EyeX, EyeY, EyeZ);
	float4x4 viewMat = generateView(eye, float3(0,0,0), float3(0,1,0));
    float2 fragCoord = input.Texcoord * float2(WIDTH, HEIGHT);
    float3 dir = GetRayDirection(eye, Fov, fragCoord);
    float3 worldDir = mul(dir, (float3x3) viewMat);

	float3 lightDir = normalize(gLightPosition - float3(PosX, PosY, PosZ));

	float dist = Cloud(worldDir, eye, lightDir, 0.01f, 1000.0f);
	
	float4 background = float4(0.3, 0.5, 1.0f, 1.0f);
	
	if(dist > MAX_DISTANCE - EPSILON)
	{
		return background;
	}
	float4 retColor = max(dist.xxxx, background);
	//float4 retColor = dist;
	//float4 retColor = dist;

    return retColor;
}