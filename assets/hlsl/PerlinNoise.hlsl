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

float Random(float2 uv)
{
    return frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
}

float3 RandomFloat3(float3 uvw)
{
    float x = frac(sin(dot(uvw, float3(12.9898, 78.233, 43.1554))) * 43758.5453);
    float y = frac(sin(dot(uvw, float3(12.9898, 78.233, 43.1554) * 2.0f)) * 43758.5453);
    float z = frac(sin(dot(uvw, float3(12.9898, 78.233, 43.1554) / 2.0f)) * 43758.5453);
    
    return float3(x, y, z);
}

uint Hash(uint s)
{
    s ^= 2747636419u;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    return s;
}

float fade(float t)
{
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Gradient(int hash, float x, float y, float z)
{
    int h = hash & 15;
    
    float u = (h < 8) ? x : y;
    float v;
    
    if (h < 4)
    {
        v = y;
    }
    else if (h == 12 || h == 14)
    {
        v = x;
    }
    else
    {
        v = z;
    }
    
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

static const int permutation[] =
{
    151, 160, 137, 91, 90, 15, // Hash lookup table as defined by Ken Perlin.  This is a randomly
	131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, // arranged array of all numbers from 0-255 inclusive.
	190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
	88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
	77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
	102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
	135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
	5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
	223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
	129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
	251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
	49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
	138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
};

float PerlinNoise(float x, float y, float z)
{
    
    float3 p0 = float3(x, y, z);
    float3 p1 = float3(-x, y, z);
    float3 p2 = float3(x, -y, z);
    float3 p3 = float3(x, y, -z);
    float3 p4 = float3(-x, -y, z);
    float3 p5 = float3(x, -y, -z);
    float3 p6 = float3(-x, y, -z);
    float3 p7 = float3(-x, -y, -z);
    
    float2 uv = float2(x, y);
    
    //float3 rv0 = float3(Random(uv * x), Random(uv * y), Random(uv * z));
    //float3 rv1 = float3(Random(uv * -x), Random(uv * y), Random(uv * z));
    //float3 rv2 = float3(Random(uv * x), Random(uv * -y), Random(uv * z));
    //float3 rv3 = float3(Random(uv * x), Random(uv * y), Random(uv * -z));
    //float3 rv4 = float3(Random(uv * -x), Random(uv * -y), Random(uv * z));
    //float3 rv5 = float3(Random(uv * x), Random(uv * -y), Random(uv * -z));
    //float3 rv6 = float3(Random(uv * -x), Random(uv * y), Random(uv * -z));
    //float3 rv7 = float3(Random(uv * -x), Random(uv * -y), Random(uv * -z));
    float3 rv0 = float3(RandomFloat3(p0));
    float3 rv1 = float3(RandomFloat3(p1));
    float3 rv2 = float3(RandomFloat3(p2));
    float3 rv3 = float3(RandomFloat3(p3));
    float3 rv4 = float3(RandomFloat3(p4));
    float3 rv5 = float3(RandomFloat3(p5));
    float3 rv6 = float3(RandomFloat3(p6));
    float3 rv7 = float3(RandomFloat3(p7));
    
    float3 p = float3(x, y, z);
    
    float result = 0.0f;
   
    int xi = (int) x & 255;
    int yi = (int) y & 255;
    int zi = (int) z & 255;
    
    float xf = x - (int) x;
    float yf = y - (int) y;
    float zf = z - (int) z;
    
    float3 uvw = float3(fade(xf), fade(yf), fade(zf));
    
    int aaa, aba, aab, abb, baa, bba, bab, bbb;
    aaa = permutation[permutation[permutation[xi] + yi] + zi];
    aba = permutation[permutation[permutation[xi] + (yi + 1)] + zi];
    aab = permutation[permutation[permutation[xi] + yi] + (zi + 1)];
    abb = permutation[permutation[permutation[xi] + (yi + 1)] + (zi + 1)];
    baa = permutation[permutation[permutation[xi + 1] + yi] + zi];
    bba = permutation[permutation[permutation[xi + 1] + (yi + 1)] + zi];
    bab = permutation[permutation[permutation[xi + 1] + yi] + (zi + 1)];
    bbb = permutation[permutation[permutation[xi + 1] + (yi + 1)] + (zi + 1)];
    
    float interp0 = lerp(Gradient(aaa, xf, yf, zf), Gradient(baa, (xf - 1), yf, zf), uvw.x);
    float interp1 = lerp(Gradient(aba, xf, (yf - 1), zf), Gradient(bba, (xf - 1), (yf - 1), zf), uvw.x);
    float interp2 = lerp(Gradient(aab, xf, yf, (zf - 1)), Gradient(bab, (xf - 1), yf, (zf - 1)), uvw.x);
    float interp3 = lerp(Gradient(abb, xf, (yf - 1), (zf - 1)), Gradient(bbb, (xf - 1), (yf - 1), (zf - 1)), uvw.x);
    
    float avg0 = lerp(interp0, interp1, uvw.y);
    float avg1 = lerp(interp2, interp3, uvw.y);
    
    result = (lerp(avg0, avg1, uvw.z) + 1) / 2;
    
    return result;
}

float OctavePerlin(float x, float y, float z, int octaves, float persistence)
{
    float total = 0;
    float maxValue = 0;
    
    float frequency = 1;
    float amplitude = 1;
    
    for (int i = 0; i < octaves; i++)
    {
        total += PerlinNoise(x * frequency, y * frequency, z * frequency) * amplitude;
        
        maxValue += amplitude;
        
        amplitude *= persistence;
        frequency *= 2.0f;
    }

    return total / maxValue;
}

float4 frag(Pixel input) : SV_Target0
{
    const int octv = 4;
    const int persis = 10;
    
    float f1 = OctavePerlin(input.Texcoord.x, input.Texcoord.y,
     input.TargetIndex / 512.0f, octv, 128);
    
    float f2 = OctavePerlin(input.Texcoord.x, input.Texcoord.y,
     input.TargetIndex / 512.0f, octv, 64);
    
    float f3 = OctavePerlin(input.Texcoord.x, input.Texcoord.y,
     input.TargetIndex / 512.0f, octv, 32);
    
    float f4 = OctavePerlin(input.Texcoord.x, input.Texcoord.y,
     input.TargetIndex / 512.0f, octv, 16);
    
    float f5 = OctavePerlin(input.Texcoord.x, input.Texcoord.y,
     input.TargetIndex / 512.0f, octv, 8);
    
    float f6 = OctavePerlin(input.Texcoord.x, input.Texcoord.y,
     input.TargetIndex / 512.0f, octv, 4);

    return (f1 + f2 + f3 + f4 + f5 + f6) / 6.0f;
}