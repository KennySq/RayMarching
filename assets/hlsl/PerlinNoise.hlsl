struct Vertex
{
    float4 Position : POSITION0;
    float2 Texcoord : TEXCOORD0;
    uint InstanceID : SV_InstanceID;
};

struct Pixel
{
    float4 Position : SV_Position;
    float2 Texcoord : TEXCOORD;
    uint TargetIndex : SV_RenderTargetArrayIndex;
};

Pixel vert(Vertex input)
{
    Pixel output = (Pixel) 0;
    output.Position = input.Position;
    output.Texcoord = input.Texcoord;
    output.TargetIndex = input.InstanceID;
    
    return output;
}

float4 frag(Pixel input) : SV_Target0
{
    return float4(1, 0, 0, 0);
}