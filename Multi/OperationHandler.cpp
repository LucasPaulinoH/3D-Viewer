#include "OperationHandler.h"

XMMATRIX OperationHandler::getScaleMatrix(Input* input) {
	XMMATRIX scaleMatrix = XMMatrixIdentity();

	if (input->KeyDown('I'))
		scaleMatrix = XMMatrixScaling(SCALE_FACTOR, SCALE_FACTOR, SCALE_FACTOR);

	if (input->KeyDown('O'))
		scaleMatrix = XMMatrixScaling(SCALE_FACTOR - 0.02f, SCALE_FACTOR - 0.02f, SCALE_FACTOR - 0.02f);

	return scaleMatrix;
}

XMMATRIX  OperationHandler::getRotationMatrix(Input* input) {
	XMMATRIX rotationMatrix = XMMatrixIdentity();

	if (input->KeyDown(VK_NUMPAD5))
		rotationMatrix = XMMatrixRotationX(-ROTATION_FACTOR);

	if (input->KeyDown(VK_NUMPAD2))
		rotationMatrix = XMMatrixRotationX(ROTATION_FACTOR);

	if (input->KeyDown(VK_NUMPAD1))
		rotationMatrix = XMMatrixRotationZ(-ROTATION_FACTOR);

	if (input->KeyDown(VK_NUMPAD3))
		rotationMatrix = XMMatrixRotationZ(ROTATION_FACTOR);

	if (input->KeyDown(VK_NUMPAD7))
		rotationMatrix = XMMatrixRotationY(-ROTATION_FACTOR);

	if (input->KeyDown(VK_NUMPAD9))
		rotationMatrix = XMMatrixRotationY(ROTATION_FACTOR);

	return rotationMatrix;
}

XMMATRIX OperationHandler::getTranslationMatrix(Input* input) {
	XMMATRIX translationMatrix = XMMatrixIdentity();

	if (input->KeyDown(VK_UP) && input->KeyDown(VK_SHIFT))
		translationMatrix = XMMatrixTranslation(0.0f, TRANSLATION_FACTOR, 0.0f);

	else if (input->KeyDown(VK_DOWN) && input->KeyDown(VK_SHIFT))
		translationMatrix = XMMatrixTranslation(0.0f, -TRANSLATION_FACTOR, 0.0f);

	else if (input->KeyDown(VK_UP))
		translationMatrix = XMMatrixTranslation(0.0f, 0.0f, -TRANSLATION_FACTOR);

	else if (input->KeyDown(VK_DOWN))
		translationMatrix = XMMatrixTranslation(0.0f, 0.0f, TRANSLATION_FACTOR);

	if (input->KeyDown(VK_RIGHT))
		translationMatrix = XMMatrixTranslation(-TRANSLATION_FACTOR, 0.0f, 0.0f);

	if (input->KeyDown(VK_LEFT))
		translationMatrix = XMMatrixTranslation(TRANSLATION_FACTOR, 0.0f, 0.0f);

	return translationMatrix;
}

XMMATRIX OperationHandler::executeGeometricOperation(Input* input, XMMATRIX currentWorldMatrix) {
	return getScaleMatrix(input) * getRotationMatrix(input) * getTranslationMatrix(input) * currentWorldMatrix;
}