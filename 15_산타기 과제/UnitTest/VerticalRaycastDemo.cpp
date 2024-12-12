#include "stdafx.h"
#include "VerticalRaycastDemo.h"

void VerticalRaycastDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(13, 0, 0);
	Context::Get()->GetCamera()->Position(54, 10, -17);
	((Freedom *)Context::Get()->GetCamera())->Speed(50, 2);



	shader = new Shader(L"12_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	terrain->BaseMap(L"Terrain/Dirt3.png");


	cubeShader = new Shader(L"09_Cube.fx");


	vertexCount = 24;
	vertices = new Vertex[vertexCount];


	float w = 0.5f, h = 0.5f, d = 0.5f;

	//Front
	vertices[0].Position = Vector3(-w, -h, -d);
	vertices[1].Position = Vector3(-w, +h, -d);
	vertices[2].Position = Vector3(+w, -h, -d);
	vertices[3].Position = Vector3(+w, +h, -d);

	//Back
	vertices[4].Position = Vector3(-w, -h, +d);
	vertices[5].Position = Vector3(+w, -h, +d);
	vertices[6].Position = Vector3(-w, +h, +d);
	vertices[7].Position = Vector3(+w, +h, +d);

	//Top
	vertices[8].Position = Vector3(-w, +h, -d);
	vertices[9].Position = Vector3(-w, +h, +d);
	vertices[10].Position = Vector3(+w, +h, -d);
	vertices[11].Position = Vector3(+w, +h, +d);

	//Bottom
	vertices[12].Position = Vector3(-w, -h, -d);
	vertices[13].Position = Vector3(+w, -h, -d);
	vertices[14].Position = Vector3(-w, -h, +d);
	vertices[15].Position = Vector3(+w, -h, +d);

	//Left
	vertices[16].Position = Vector3(-w, -h, +d);
	vertices[17].Position = Vector3(-w, +h, +d);
	vertices[18].Position = Vector3(-w, -h, -d);
	vertices[19].Position = Vector3(-w, +h, -d);

	//Right
	vertices[20].Position = Vector3(+w, -h, -d);
	vertices[21].Position = Vector3(+w, +h, -d);
	vertices[22].Position = Vector3(+w, -h, +d);
	vertices[23].Position = Vector3(+w, +h, +d);



	//Create VertexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}


	indexCount = 36;
	indices = new UINT[indexCount]
	{
		0, 1, 2, 2, 1, 3,
		4, 5, 6, 6, 5, 7,
		8, 9, 10, 10, 9, 11,
		12, 13, 14, 14, 13, 15,
		16, 17, 18, 18, 17, 19,
		20, 21, 22, 22, 21, 23,
	};

	//Create IndexBuffer
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer));
	}

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	D3DXMatrixIdentity(&world);

}

void VerticalRaycastDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(terrain);

	SafeDelete(cubeShader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void VerticalRaycastDemo::Update()
{
	

	terrain->Update();

	if (Keyboard::Get()->Press(VK_SHIFT))
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			rotation.y += speed * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			rotation.y -= speed * Time::Delta();
	}
	else
	{
		if (Keyboard::Get()->Press(VK_RIGHT))
			position += right * speed * Time::Delta();
		else if (Keyboard::Get()->Press(VK_LEFT))
			position -= right * speed * Time::Delta();

		if (Keyboard::Get()->Press(VK_UP))
			position += forward * speed * Time::Delta();
		else if (Keyboard::Get()->Press(VK_DOWN))
			position -= forward * speed * Time::Delta();
	}
	
	position.y = terrain->GetVerticalRaycast(position) + 1.0f; // 큐브의 y 크기 보정 + 1.0f 

	//-----------외적은 두 벡터와 수직을 이룬다 
	//-----------내적을 한 뒤 -90도를 하면 회전해야하는 각도를 얻을 수 있다.
	//-----------Z축회전 
	{
		Vector3 normal = terrain->GetNormal(position);
		Vector3 test;
		D3DXVec3Normalize(&test, &(position + right));
		PF = test;
		float cos = D3DXVec3Dot(&normal, &right);
		string str1 = "nomarl test : " + to_string(test.x) + ", " + to_string(test.y) + ", " + to_string(test.z);
		Gui::Get()->RenderText(5, 155, str1);

		float degree = acosf(cos) - Math::PI / 2.f;  // 각 구하기
		rotation.z += degree;


	}

	//-------------------------------------------------------------------------------


	//-----------x축회전-> forward는 회전에 대한 전방을 나타내기에 forward를 사용  
	{
		Vector3 normal = terrain->GetNormal(position);
		Vector3 test; 
		D3DXVec3Normalize(&test, &(position + forward));
		PF = test;
		float cos = D3DXVec3Dot(&normal,&forward);
		string str1 = "nomarl test : " + to_string(test.x) + ", " + to_string(test.y) + ", " + to_string(test.z);
		Gui::Get()->RenderText(5, 155, str1);
		
			float degree = acosf(cos) - Math::PI / 2.f;  // 각 구하기
			rotation.x += -degree;

			string str = "r: "+to_string(degree *180.f/ Math::PI); 
			Gui::Get()->RenderText(5, 125, str);


	}
	
	//-------------------------------------------------------------------------------
	


	Matrix S, R, T;
	D3DXMatrixScaling(&S, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&R, rotation.y, rotation.x, rotation.z);
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);

	world = S * R * T;

	D3DXVec3TransformNormal(&forward, &Vector3(0, 0, 1), &R);
	D3DXVec3TransformNormal(&up, &Vector3(0, 1, 0), &R);
	D3DXVec3TransformNormal(&right, &Vector3(1, 0, 0), &R);


	cubeShader->AsVector("Color")->SetFloatVector(color); // 쉐이더 컬러 변수에 컬러값 넘겨줌 
	cubeShader->AsMatrix("World")->SetMatrix(world);
	cubeShader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	cubeShader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	
}

void VerticalRaycastDemo::Render()
{
	terrain->Render();


	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	static bool bWireframe = false;
	ImGui::Checkbox("CubeWireframe", &bWireframe);

	cubeShader->DrawIndexed(0, bWireframe ? 1 : 0, indexCount);

	// Light
	static bool Lighting = true;
	ImGui::Checkbox("Lighting", &Lighting);
	shader->AsScalar("Lighting")->SetInt(Lighting ? 1 : 0);

	static Vector3 Direction = Vector3(-1, -1, +1);
	ImGui::SliderFloat3("Direction", Direction, -1, +1);
	shader->AsVector("LightDirection")->SetFloatVector(Direction);
	//












	// Cube Right , Up , Forward  렌더링   
	{
		DebugLine::Get()->RenderLine(position,position + forward * 3 ,Color(0,0,1,1));
		DebugLine::Get()->RenderLine(position, position + up * 3, Color(0, 1, 1, 1));
		DebugLine::Get()->RenderLine(position, position + right * 3, Color(1, 0, 0, 1));

		static float cosz = 0.0f;
		ImGui::SliderFloat("   Theta    ", &cosz, 0, 720);
		/*DebugLine::Get()->RenderLine( Vector3(0, 0, 0), Vector3(0, 0, sin(cosz * Math::PI / 180)) , Color(1, 0, 0, 1));
		DebugLine::Get()->RenderLine(Vector3(1, 0, 0), Vector3(1, 0, cos(cosz * Math::PI / 180)), Color(1, 0, 0, 1));
		DebugLine::Get()->RenderLine(Vector3(-1, 0, 0), Vector3(-1, 0, tan(cosz * Math::PI / 180)), Color(1, 0, 0, 1));*/
		Vector3 test ,test1, test2;
		test1 = Vector3(1, 0, 0);
		test2 = Vector3(0, 0, 1);
		D3DXVec3Cross(&test,&test1 ,&test2 );
		//DebugLine::Get()->RenderLine(Vector3(0,0,0) , Vector3(1,0,0), Color(1, 0, 0, 1));
		/*Vector3 test;
		test = Vector3(0, 0, 1) - Vector3(1, 0, 1);
		test = Vector3(1, 0, 1)+test ;
		DebugLine::Get()->RenderLine(test , test*20, Color(1, 0, 0, 1));*/
	}

	string str = "position : "+to_string(position.x) + ", " + to_string(position.y) + ", " + to_string(position.z);
	Gui::Get()->RenderText(5, 60, str);


	string str1 = "forward : "+ to_string(forward.x) + ", " + to_string(forward.y) + ", " + to_string(forward.z);
	Gui::Get()->RenderText(5, 75, str1);

	string str2 = "Up : " + to_string(up.x) + ", " + to_string(up.y) + ", " + to_string(up.z);
	Gui::Get()->RenderText(5, 90, str2);

	string str3 = "Right : " + to_string(right.x) + ", " + to_string(right.y) + ", " + to_string(right.z);
	Gui::Get()->RenderText(5, 105, str3);
}