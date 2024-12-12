#pragma once
#include "Systems/IExecute.h"

class VerticalRaycastDemo : public IExecute
{
	virtual void Initialize() override;
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	Shader* shader;
	Terrain* terrain;

private: //  Cube  // 
	Shader* cubeShader;

	UINT vertexCount;
	Vertex* vertices;
	ID3D11Buffer* vertexBuffer;

	UINT indexCount;
	UINT* indices;
	ID3D11Buffer* indexBuffer;

	float speed = 10.0f;
	Vector3 scale = Vector3(1, 1, 3);
	Vector3 position = Vector3(64, 0, 64);
	Vector3 rotation = Vector3(0, 0, 0);

	Vector3 right = Vector3(1, 0, 0);
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = Vector3(0, 0, 1);

	Matrix world;
	Color color = Color(0, 0, 0, 1);

	//test
	Vector3 PF = Vector3(0, 0, 0);
};