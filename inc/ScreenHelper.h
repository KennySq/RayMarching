#pragma once

struct Vertex
{
	Vertex(float px, float py, float pz, float u, float v)
		: Position(px, py, pz), Texcoord(u, v)
	{

	}

	~Vertex()
	{

	}

	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 Texcoord;
};


class ScreenHelper
{
public:
	static void MakeQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
};
