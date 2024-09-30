#include "DXUT.h"
#include "ViewportsInitializer.h"
#include "GeometricObject.h"

struct ObjectConstants
{
	XMFLOAT4X4 WorldViewProj =
	{ 1.0f, 0.0f, 0.0f, 0.0f,
	  0.0f, 1.0f, 0.0f, 0.0f,
	  0.0f, 0.0f, 1.0f, 0.0f,
	  0.0f, 0.0f, 0.0f, 1.0f };
};

class Multi : public App
{
private:
	vector<D3D12_VIEWPORT> viewports;
	ID3D12RootSignature* rootSignature = nullptr;
	ID3D12PipelineState* pipelineState = nullptr;
	vector<GeometricObject> scene;

	XMFLOAT4X4 Identity = {};

	XMFLOAT4 FIXED_OBJ_COLOR = XMFLOAT4(DirectX::Colors::DimGray);
	XMFLOAT4 UNSELECTED_OBJ_COLOR = XMFLOAT4(DirectX::Colors::Orange);
	XMFLOAT4 SELECTED_OBJ_COLOR = XMFLOAT4(DirectX::Colors::Red);

	XMFLOAT4X4 TLView = {};
	XMFLOAT4X4 TRView = {};
	XMFLOAT4X4 BLView = {};
	XMFLOAT4X4 BRView = {};

	XMFLOAT4X4 PerspectiveProj = {};
	XMFLOAT4X4 OrthographicProj = {};

	bool showPerspectiveOnly = true;

	uint selectedGOIndex = 1;

	float theta = 0;
	float phi = 0;
	float radius = 0;
	float lastMousePosX = 0;
	float lastMousePosY = 0;

public:
	void Init();
	void Update();
	void Draw();
	void Finalize();

	XMMATRIX generateObjectViewMatrix(float x, float y, float z);

	void BuildRootSignature();
	void BuildPipelineState();
};

void Multi::Init()
{
	graphics->ResetCommands();

	viewports = initViewports(window);

	theta = XM_PIDIV4;
	phi = 1.3f;
	radius = 5.0f;

	lastMousePosX = (float)input->MouseX();
	lastMousePosY = (float)input->MouseY();

	Identity = TLView = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f };

	XMStoreFloat4x4(&PerspectiveProj, XMMatrixPerspectiveFovLH(
		XMConvertToRadians(45.0f),
		window->AspectRatio(),
		1.0f, 100.0f));
	XMStoreFloat4x4(&OrthographicProj, XMMatrixOrthographicLH(
		10, 7, 1.0f, 100.0f
	));

	// ==========================================================

	Grid grid(6.0f, 6.0f, 20, 20);
	GeometricObject gridGO(grid, FIXED_OBJ_COLOR, viewports.size(), sizeof(ObjectConstants));

	scene.push_back(gridGO);
	
	Box box(2.0f, 2.0f, 2.0f);
	GeometricObject boxGO(box, UNSELECTED_OBJ_COLOR, viewports.size(), sizeof(ObjectConstants));

	scene.push_back(boxGO);

	Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20);
	GeometricObject cylinderGO(cylinder, UNSELECTED_OBJ_COLOR, viewports.size(), sizeof(ObjectConstants));

	scene.push_back(cylinderGO);

	Sphere sphere(1.0f, 20, 20);


	// ==========================================================

	BuildRootSignature();
	BuildPipelineState();

	graphics->SubmitCommands();
}

void Multi::Update()
{
	/* 
		========== COMANDOS ==========

		B -> Box 
		Q -> Quad
		C -> Cylinder 
		S -> Sphere 
		G -> GeoSphere 
		P -> Plane (Grid) 
		1 a 5 -> Arquivos
		TAB -> Seleciona
		DEL -> Remove
		V -> Modo de Visualização

		==============================
	
	*/

	if (input->KeyPress(VK_ESCAPE))
		window->Close();

	if (input->KeyPress('V'))
		showPerspectiveOnly = !showPerspectiveOnly;

	if (input->KeyPress(VK_TAB)) {
		selectedGOIndex = (selectedGOIndex + 1) % scene.size();
		if (selectedGOIndex == 0) selectedGOIndex++;
	}

	
	//	========== ESCALA ==========

	if (input->KeyDown('I')) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX scalingMatrix = XMMatrixScaling(1.01f, 1.01f, 1.01f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * scalingMatrix);
	}

	if (input->KeyDown('O')) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX scalingMatrix = XMMatrixScaling(0.99f, 0.99f, 0.99f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * scalingMatrix);
	}

	//	============================


	//	========== TRANSLAÇÃO ==========

	if (input->KeyDown(VK_UP) && input->KeyDown(VK_SHIFT)) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX xTranslationMatrix = XMMatrixTranslation(0.0f, 0.01f, 0.0f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * xTranslationMatrix);
	} else if (input->KeyDown(VK_DOWN) && input->KeyDown(VK_SHIFT)) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX xTranslationMatrix = XMMatrixTranslation(0.0f, -0.01f, 0.0f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * xTranslationMatrix);
	} else if (input->KeyDown(VK_UP)) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX xTranslationMatrix =  XMMatrixTranslation(0.0f, 0.0f, -0.01f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * xTranslationMatrix);
	} else if (input->KeyDown(VK_DOWN)) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX xTranslationMatrix = XMMatrixTranslation(0.0f, 0.0f, 0.01f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * xTranslationMatrix);
	}

	if (input->KeyDown(VK_RIGHT)) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX xTranslationMatrix = XMMatrixTranslation(-0.01f, 0.0f, 0.0f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * xTranslationMatrix);
	}

	if (input->KeyDown(VK_LEFT)) {
		XMMATRIX selectedGOWorldMatrix = XMLoadFloat4x4(&scene[selectedGOIndex].object.world);
		XMMATRIX xTranslationMatrix = XMMatrixTranslation(0.01f, 0.0f, 0.0f);

		XMStoreFloat4x4(&scene[selectedGOIndex].object.world, selectedGOWorldMatrix * xTranslationMatrix);
	}

	//	================================


	float mousePosX = (float)input->MouseX();
	float mousePosY = (float)input->MouseY();

	if (input->KeyDown(VK_LBUTTON))
	{
		float dx = XMConvertToRadians(-(0.25f * (mousePosX - lastMousePosX)));
		float dy = XMConvertToRadians(-(0.25f * (mousePosY - lastMousePosY)));

		theta += dx;
		phi += dy;

		phi = phi < 0.1f ? 0.1f : (phi > (XM_PI - 0.1f) ? XM_PI - 0.1f : phi);
	}
	else if (input->KeyDown(VK_RBUTTON))
	{
		float dx = 0.05f * (mousePosX - lastMousePosX);
		float dy = 0.05f * (mousePosY - lastMousePosY);

		radius += dx - dy;

		radius = radius < 3.0f ? 3.0f : (radius > 15.0f ? 15.0f : radius);
	}

	lastMousePosX = mousePosX;
	lastMousePosY = mousePosY;

	float x = radius * sinf(phi) * cosf(theta);
	float z = radius * sinf(phi) * sinf(theta);
	float y = radius * cosf(phi);

	XMMATRIX tlView = generateObjectViewMatrix(x, y, z);
	XMStoreFloat4x4(&TLView, tlView);

	XMMATRIX trView = generateObjectViewMatrix(0.0f, 10.0f, 0.01f);
	XMStoreFloat4x4(&TRView, trView);

	XMMATRIX blView = generateObjectViewMatrix(10.0f, 0.01f, 0.0f);
	XMStoreFloat4x4(&BLView, blView);

	XMMATRIX brView = generateObjectViewMatrix(0.0f, 0.01f, 10.0f);
	XMStoreFloat4x4(&BRView, brView);

	XMMATRIX perspectiveProj = XMLoadFloat4x4(&PerspectiveProj);
	XMMATRIX orthographicProj = XMLoadFloat4x4(&OrthographicProj);

	for (auto& iterableObject : scene)
	{
		XMMATRIX objectWorld = XMLoadFloat4x4(&iterableObject.object.world);

		XMMATRIX WorldViewProj;
		ObjectConstants constants;

		for (uint i = 0; i < viewports.size(); i++) {
			switch (i)
			{
			case 0:
			case 1:
				WorldViewProj = objectWorld * tlView * perspectiveProj;
				break;
			case 2:
				WorldViewProj = objectWorld * trView * orthographicProj;
				break;
			case 3:
				WorldViewProj = objectWorld * blView * orthographicProj;
				break;
			case 4:
				WorldViewProj = objectWorld * brView * orthographicProj;
				break;
			}

			XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProj));

			iterableObject.object.mesh->CopyConstants(&constants, i);
		}
	}

}

void Multi::Draw()
{
	graphics->Clear(pipelineState);

	for (auto& iterableGO : scene)
	{
		ID3D12DescriptorHeap* descriptorHeap = iterableGO.object.mesh->ConstantBufferHeap();
		graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
		graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
		graphics->CommandList()->IASetVertexBuffers(0, 1, iterableGO.object.mesh->VertexBufferView());
		graphics->CommandList()->IASetIndexBuffer(iterableGO.object.mesh->IndexBufferView());
		graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (uint i = 0; i < viewports.size(); i++) {
			if (i == 0 && !showPerspectiveOnly) continue;

			graphics->CommandList()->RSSetViewports(1, &viewports[i]);

			graphics->CommandList()->SetGraphicsRootDescriptorTable(0, iterableGO.object.mesh->ConstantBufferHandle(i));

			graphics->CommandList()->DrawIndexedInstanced(
				iterableGO.subMeshes[i].indexCount, 1,
				iterableGO.subMeshes[i].startIndex,
				iterableGO.subMeshes[i].baseVertex,
				0);

			if (i == 0 && showPerspectiveOnly) break;
		}
	}

	graphics->Present();
}

void Multi::Finalize()
{
	rootSignature->Release();
	pipelineState->Release();

	for (auto& iterableGO : scene)
		delete iterableGO.object.mesh;
}

XMMATRIX Multi::generateObjectViewMatrix(float x, float y, float z) {
	XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
	XMVECTOR target = XMVectorZero();
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return XMMatrixLookAtLH(pos, target, up);
}

void Multi::BuildRootSignature()
{
	D3D12_DESCRIPTOR_RANGE cbvTable = {};
	cbvTable.RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_CBV;
	cbvTable.NumDescriptors = 1;
	cbvTable.BaseShaderRegister = 0;
	cbvTable.RegisterSpace = 0;
	cbvTable.OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	D3D12_ROOT_PARAMETER rootParameters[1];
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
	rootParameters[0].DescriptorTable.NumDescriptorRanges = 1;
	rootParameters[0].DescriptorTable.pDescriptorRanges = &cbvTable;

	D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
	rootSigDesc.NumParameters = 1;
	rootSigDesc.pParameters = rootParameters;
	rootSigDesc.NumStaticSamplers = 0;
	rootSigDesc.pStaticSamplers = nullptr;
	rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob* serializedRootSig = nullptr;
	ID3DBlob* error = nullptr;

	ThrowIfFailed(D3D12SerializeRootSignature(
		&rootSigDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&serializedRootSig,
		&error));

	if (error != nullptr)
	{
		OutputDebugString((char*)error->GetBufferPointer());
	}

	ThrowIfFailed(graphics->Device()->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature)));
}

void Multi::BuildPipelineState()
{
	D3D12_INPUT_ELEMENT_DESC inputLayout[2] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	D3DReadFileToBlob(L"Shaders/Vertex.cso", &vertexShader);
	D3DReadFileToBlob(L"Shaders/Pixel.cso", &pixelShader);

	D3D12_RASTERIZER_DESC rasterizer = {};
	//rasterizer.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizer.FillMode = D3D12_FILL_MODE_WIREFRAME;
	rasterizer.CullMode = D3D12_CULL_MODE_BACK;
	rasterizer.FrontCounterClockwise = FALSE;
	rasterizer.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
	rasterizer.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
	rasterizer.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
	rasterizer.DepthClipEnable = TRUE;
	rasterizer.MultisampleEnable = FALSE;
	rasterizer.AntialiasedLineEnable = FALSE;
	rasterizer.ForcedSampleCount = 0;
	rasterizer.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	D3D12_BLEND_DESC blender = {};
	blender.AlphaToCoverageEnable = FALSE;
	blender.IndependentBlendEnable = FALSE;
	const D3D12_RENDER_TARGET_BLEND_DESC defaultRenderTargetBlendDesc =
	{
		FALSE,FALSE,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_BLEND_ONE, D3D12_BLEND_ZERO, D3D12_BLEND_OP_ADD,
		D3D12_LOGIC_OP_NOOP,
		D3D12_COLOR_WRITE_ENABLE_ALL,
	};

	for (UINT i = 0; i < D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT; ++i)
		blender.RenderTarget[i] = defaultRenderTargetBlendDesc;

	D3D12_DEPTH_STENCIL_DESC depthStencil = {};
	depthStencil.DepthEnable = TRUE;
	depthStencil.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencil.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencil.StencilEnable = FALSE;
	depthStencil.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
	depthStencil.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
	const D3D12_DEPTH_STENCILOP_DESC defaultStencilOp =
	{ D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_STENCIL_OP_KEEP, D3D12_COMPARISON_FUNC_ALWAYS };
	depthStencil.FrontFace = defaultStencilOp;
	depthStencil.BackFace = defaultStencilOp;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso = {};
	pso.pRootSignature = rootSignature;
	pso.VS = { reinterpret_cast<BYTE*>(vertexShader->GetBufferPointer()), vertexShader->GetBufferSize() };
	pso.PS = { reinterpret_cast<BYTE*>(pixelShader->GetBufferPointer()), pixelShader->GetBufferSize() };
	pso.BlendState = blender;
	pso.SampleMask = UINT_MAX;
	pso.RasterizerState = rasterizer;
	pso.DepthStencilState = depthStencil;
	pso.InputLayout = { inputLayout, 2 };
	pso.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pso.NumRenderTargets = 1;
	pso.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pso.SampleDesc.Count = graphics->Antialiasing();
	pso.SampleDesc.Quality = graphics->Quality();
	graphics->Device()->CreateGraphicsPipelineState(&pso, IID_PPV_ARGS(&pipelineState));

	vertexShader->Release();
	pixelShader->Release();
}

int APIENTRY WinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine,
	_In_ int nCmdShow)
{
	try
	{
		Engine* engine = new Engine();
		engine->window->Mode(WINDOWED);
		engine->window->Size(1024, 720);
		engine->window->Color(25, 25, 25);
		engine->window->Title("Multi");
		engine->window->Icon(IDI_ICON);
		engine->window->Cursor(IDC_CURSOR);
		engine->window->LostFocus(Engine::Pause);
		engine->window->InFocus(Engine::Resume);

		engine->Start(new Multi());
		delete engine;
	}
	catch (Error& e)
	{
		MessageBox(nullptr, e.ToString().data(), "Multi", MB_OK);
	}

	return 0;
}