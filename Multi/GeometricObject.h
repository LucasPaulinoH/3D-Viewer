#ifndef GEOMETRIC_OBJECT_H
#define GEOMETRIC_OBJECT_H

#include "DXUT.h"

class GeometricObject
{
private:
	uint viewportsCount;
	uint constantBufferSize;
public:
	vector<Vertex> vertexes;
	vector<uint> indexes;

	Object object;

	vector<SubMesh> subMeshes;

	uint vertexCount;
	uint indexCount;

	uint vertexBufferSize;
	uint indexBufferSize;

	GeometricObject(Geometry geometry, XMFLOAT4 color, uint viewportsQuantity, uint constantBufferSize)
		: viewportsCount(viewportsQuantity), constantBufferSize(constantBufferSize)
	{
		for (auto& iterableVertex : geometry.vertices)
			iterableVertex.color = color;

		vertexCount = geometry.VertexCount();
		indexCount = geometry.IndexCount();

		vertexBufferSize = vertexCount * sizeof(Vertex);
		indexBufferSize = indexCount * sizeof(uint);

		for (const auto& iterableVertex : geometry.vertices)
			vertexes.push_back(iterableVertex);

		for (const auto& index : geometry.indices)
			indexes.push_back(index);

		XMStoreFloat4x4(&object.world,
			XMMatrixScaling(0.5f, 0.5f, 0.5f) *
			XMMatrixTranslation(0.0f, 0.0f, 0.0f));

		object.mesh = new Mesh();
		object.mesh->VertexBuffer(vertexes.data(), vertexBufferSize, sizeof(Vertex));
		object.mesh->IndexBuffer(indexes.data(), indexBufferSize, DXGI_FORMAT_R32_UINT);
		object.mesh->ConstantBuffer(constantBufferSize, viewportsCount);
		object.cbIndex = viewportsCount;

		SubMesh subMesh;
		subMesh.indexCount = uint(indexCount);
		subMesh.startIndex = 0;
		subMesh.baseVertex = 0;

		for (uint i = 0; i < viewportsCount; i++) {
			subMeshes.push_back(subMesh);
		}
	}

	~GeometricObject() {
	}

	void changeGOColor(XMFLOAT4 color) {
		for (auto& iterableVertex : vertexes)
			iterableVertex.color = color;

		object.mesh->VertexBuffer(vertexes.data(), vertexBufferSize, sizeof(Vertex));
		object.mesh->IndexBuffer(indexes.data(), indexBufferSize, DXGI_FORMAT_R32_UINT);
	}
};

#endif