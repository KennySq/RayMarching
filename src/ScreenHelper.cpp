#include"pch.h"
#include"ScreenHelper.h"

void ScreenHelper::MakeQuad(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
	vertices.emplace_back(Vertex(-1.0f, -1.0f, 0.0f, 0.0f, 1.0f));
	vertices.emplace_back(Vertex(1.0f, -1.0f, 0.0f, 1.0f, 1.0f));
	vertices.emplace_back(Vertex(1.0f, 1.0f, 0.0f, 1.0f, 0.0f));
	vertices.emplace_back(Vertex(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f));

	indices.emplace_back(3);
	indices.emplace_back(2);
	indices.emplace_back(1);

	indices.emplace_back(3);
	indices.emplace_back(1);
	indices.emplace_back(0);
}