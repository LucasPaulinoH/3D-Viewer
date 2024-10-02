#ifndef GEOMETRIC_OBJECT_H
#define GEOMETRIC_OBJECT_H

#include "DXUT.h"

class GeometricObject {
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

	GeometricObject(Geometry geometry, XMFLOAT4 color, uint viewportsQuantity, uint constantBufferSize);
	~GeometricObject();

	void changeGOColor(XMFLOAT4 color);
};

#endif