#include "stdafx.h"
#include "GetHeightDemo.h"

void GetHeightDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(13, 0, 0);
	Context::Get()->GetCamera()->Position(54, 10, -17);
	((Freedom *)Context::Get()->GetCamera())->Speed(50, 2);


	shader = new Shader(L"12_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");


	triShader = new Shader(L"05_World.fx");

	Vertex vertices[3];
	vertices[0].Position = Vector3(+0, +1, 0);
	vertices[1].Position = Vector3(-1, +0, 0);
	vertices[2].Position = Vector3(+1, +0, 0);

	//Create Vertex Buffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}
}

void GetHeightDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);

	SafeDelete(triShader);
	SafeRelease(vertexBuffer);
}

void GetHeightDemo::Update()
{
	shader->AsScalar("Lighting")->SetInt(1);
	terrain->Update();

	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 20.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		position.z += 20.0f * Time::Delta();
	else if (Keyboard::Get()->Press(VK_DOWN))
		position.z -= 20.0f * Time::Delta();

	position.y = terrain->GetHeight(position) + 0.5f;


	Matrix R, T;
	D3DXMatrixRotationX(&R, Math::ToRadian(180));
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	Matrix world = R * T;


	triShader->AsMatrix("World")->SetMatrix(world);
	triShader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	triShader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void GetHeightDemo::Render()
{
	terrain->Render();


	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);

	triShader->Draw(0, 0, 3);


	string str = to_string(position.x) + ", " + to_string(position.y) + ", " + to_string(position.z);
	Gui::Get()->RenderText(5, 60, str);
}