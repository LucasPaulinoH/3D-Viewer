#ifndef GO_SPAWNER_H
#define GO_SPAWNER_H

#include "DXUT.h"
#include "GeometricObject.h"

class GOSpawner {
private:
	XMFLOAT4 DEFAULT_COLOR = XMFLOAT4(DirectX::Colors::Orange);
	uint viewportsQuantity, sizeOfConstants;

	GeometricObject box();
	GeometricObject cylinder();
	GeometricObject geosphere();
	GeometricObject plane();
	GeometricObject quad();
	GeometricObject sphere();
public:
	GOSpawner() {};
	GOSpawner(uint viewportsQuantity, uint sizeOfConstants) :
		viewportsQuantity(viewportsQuantity), sizeOfConstants(sizeOfConstants) {};

	GeometricObject generateGO(Input * input);
};

#endif