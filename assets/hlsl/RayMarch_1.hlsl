#define MAX_STEPS 16
#define EPSILON 0.001f
#define MAX_DISTANCE 10000.0f

#include "Essentials.hlsli"

cbuffer Constants : register(b0)
{
    float4 Padding;
    float EyeX, EyeY, EyeZ;
    float Fov;
    float2 UV;
    float PosX, PosY, PosZ;
    float AnimateSpeed;
    float Density;
    float AppTime;
    float Size;
    float Mask;
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

float sdfScene(float3 samplePoint, float size)
{
    float cube0 = sdfCube(samplePoint - float3(PosX, PosY, PosZ), size);
    float sphereDist = sdfSphere(samplePoint, float3(0, 0, 0), size);
	
    return min(cube0, sphereDist);
}

float4x4 generateView(float3 eye, float3 at, float3 up)
{
    float3 f = normalize(at - eye);
    float3 s = normalize(cross(f, up));
    float3 u = cross(s, f);
	
    return float4x4(
		float4(s, 0),
		float4(u, 0),
		float4(-f, 0),
		float4(0, 0, 0, 1));
}

//float3 ApproximateNormal(float3 samplePoint)
//{
//    float x = sdfScene(float3(samplePoint.x + EPSILON, samplePoint.y, samplePoint.z))
//						 - sdfScene(float3(samplePoint.x - EPSILON, samplePoint.y, samplePoint.z));
//    float y = sdfScene(float3(samplePoint.x, samplePoint.y + EPSILON, samplePoint.z))
//						 - sdfScene(float3(samplePoint.x, samplePoint.y - EPSILON, samplePoint.z));
//    float z = sdfScene(float3(samplePoint.x, samplePoint.y, samplePoint.z + EPSILON))
//						 - sdfScene(float3(samplePoint.x, samplePoint.y, samplePoint.z - EPSILON));

//    return float3(x, y, z);
//}

float Cloud(float3 rayDir, float3 viewPos, float3 viewDir, float3 lightDir, float near, float far, float size)
{
    float depth = near;
    float stepUnit = 1.0f / MAX_STEPS;
    float absorption = 0.0f;
    float absorptionSigma = 0.0f;
    
    [unroll(MAX_STEPS)]
    for (int i = 0; i < MAX_STEPS; i++)
    {
        float3 samplePoint = viewPos + (depth * rayDir);
        float3 texSample = samplePoint * stepUnit;
        texSample.x += 0.5f;
        texSample.y += 0.5f;
        texSample.z += AppTime * AnimateSpeed;
        
        float cloudSample = mCloudTexture.Sample(DefaultSamplerState, texSample).x;
        float cloudDensity = pow(cloudSample, Density);
        float sceneDist = sdfScene(samplePoint, size);
        
        absorptionSigma += cloudDensity * Mask;
        depth += max(sceneDist, cloudDensity) ;

        absorption = exp(-(absorptionSigma * stepUnit * depth));
        
        if (depth > far)
        {
            return far;
        }
        
    }
    
    return absorption;
}

float4 frag(Pixel input) : SV_Target0
{
    float3 eye = float3(EyeX, EyeY, EyeZ);
    float4x4 worldMat = float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, PosX, PosY, PosZ, 1);
    float4x4 viewMat = generateView(eye, float3(0, 0, 0), float3(0, 1, 0));
    float2 fragCoord = input.Texcoord * float2(WIDTH, HEIGHT);
    float3 dir = GetRayDirection(eye, Fov, fragCoord);
    float3 worldDir = mul(dir, (float3x3) viewMat);
    float3 lightDir = normalize(gLightPosition - float3(PosX, PosY, PosZ));
    float3 cloudColor = float3(0.6, 0.5f, 0.2);
    float size = Size;

    float dist = Cloud(worldDir, eye, dir, lightDir, 0.01f, MAX_DISTANCE, size);
	
    float4 background = float4(0.4, 0.5, 0.8f, 1.0f);
	
    if (dist > MAX_DISTANCE - EPSILON)
    {
        return background;
    }
    float4 retColor = max(dist.xxxx, background);

    return retColor;
}