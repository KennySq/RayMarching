struct Vertex
{
    float4 Position : POSITION;
    float2 Texcoord : TEXCOORD0;
};

struct Pixel
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD0;
};

Pixel vert(Vertex input)
{
    Pixel output = (Pixel) 0;

    output.Position = input.Position;
    output.Texcoord = input.Texcoord;

	return output;
}

float4 frag(Pixel input) : SV_Target0
{
    return float4(1, 1, 1, 1);
}