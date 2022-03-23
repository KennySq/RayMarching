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