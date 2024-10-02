#ifndef GEOMETRIC_OPERATION_HANDLER_H
#define GEOMETRIC_OPERATION_HANDLER_H

#include "DXUT.h"

class OperationHandler {
private:
	const float SCALE_FACTOR = 1.01f;
	const float ROTATION_FACTOR = 0.03f;
	const float TRANSLATION_FACTOR = 0.03f;

	XMMATRIX getScaleMatrix(Input* input);
	XMMATRIX getRotationMatrix(Input* input);
	XMMATRIX getTranslationMatrix(Input* input);

public:
	XMMATRIX executeGeometricOperation(Input* input, XMMATRIX currentWorldMatrix);
};

#endif