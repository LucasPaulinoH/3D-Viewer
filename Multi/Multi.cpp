#include "DXUT.h"
#include "ViewportsInitializer.h"

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
    ID3D12RootSignature* rootSignature = nullptr;
    ID3D12PipelineState* pipelineState = nullptr;
    vector<Object> scene;

    XMFLOAT4X4 Identity = {};
    XMFLOAT4X4 View = {};
    XMFLOAT4X4 PerspectiveProjection = {};

    vector<D3D12_VIEWPORT> viewports;

    float theta = 0;
    float phi = 0;
    float radius = 0;
    float lastMousePosX = 0;
    float lastMousePosY = 0;

    bool showPerspectiveOnly = true;

public:
    void Init();
    void Update();
    void Draw();
    void Finalize();

    Object generateObjectFromGeometry(Geometry geometry, XMMATRIX scaleMatrix, XMMATRIX translationMatrix);

    void BuildRootSignature();
    void BuildPipelineState();
};

void Multi::Init()
{
    graphics->ResetCommands();

    theta = XM_PIDIV4;
    phi = 1.3f;
    radius = 5.0f;

    lastMousePosX = (float) input->MouseX();
    lastMousePosY = (float) input->MouseY();

    Identity = View = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f };

    XMStoreFloat4x4(&PerspectiveProjection, XMMatrixPerspectiveFovLH(
        XMConvertToRadians(45.0f), 
        window->AspectRatio(),
        1.0f, 100.0f));

    Box box(2.0f, 2.0f, 2.0f);
    Cylinder cylinder(1.0f, 0.5f, 3.0f, 20, 20);
    Sphere sphere(1.0f, 20, 20);
    Grid grid(3.0f, 3.0f, 20, 20);

    for (auto& v : box.vertices) v.color = XMFLOAT4(DirectX::Colors::Orange);
    for (auto& v : cylinder.vertices) v.color = XMFLOAT4(DirectX::Colors::Orange);
    for (auto& v : sphere.vertices) v.color = XMFLOAT4(DirectX::Colors::Orange);

    for (auto& v : grid.vertices) v.color = XMFLOAT4(DirectX::Colors::DimGray);

    scene.push_back(generateObjectFromGeometry(box, XMMatrixScaling(0.4f, 0.4f, 0.4f), XMMatrixTranslation(-1.0f, 0.41f, 1.0f)));
    scene.push_back(generateObjectFromGeometry(cylinder, XMMatrixScaling(0.5f, 0.5f, 0.5f), XMMatrixTranslation(1.0f, 0.75f, -1.0f)));
    scene.push_back(generateObjectFromGeometry(sphere, XMMatrixScaling(0.5f, 0.5f, 0.5f), XMMatrixTranslation(0.0f, 0.5f, 0.0f)));

    Object gridObj = generateObjectFromGeometry(grid, XMMatrixScaling(1.0f, 1.0f, 1.0f), XMMatrixTranslation(1.0f, 1.0f, 1.0f));
    gridObj.world = Identity;
    scene.push_back(gridObj);
 
    viewports = initializeViewports(window);

    BuildRootSignature();
    BuildPipelineState();    

    graphics->SubmitCommands();
}

void Multi::Update()
{
    if (input->KeyPress(VK_ESCAPE))
        window->Close();

    if (input->KeyPress('V'))
        showPerspectiveOnly = !showPerspectiveOnly;

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

    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&View, view);

    XMMATRIX perspectiveProjectionMatrix = XMLoadFloat4x4(&PerspectiveProjection);

    for (uint i = 0; i < viewports.size(); i++) {
        if (i < 2) continue;

        for (auto& obj : scene)
        {
            XMMATRIX world = XMLoadFloat4x4(&obj.world);

            XMMATRIX WorldViewProjPerspective = world * view * perspectiveProjectionMatrix;

            ObjectConstants constants;
            XMStoreFloat4x4(&constants.WorldViewProj, XMMatrixTranspose(WorldViewProjPerspective));

            obj.mesh->CopyConstants(&constants);
        }
    }
}

void Multi::Draw()
{
    graphics->Clear(pipelineState);
    
    for (uint i = 0; i < viewports.size(); i++) {
        if (!showPerspectiveOnly && i == 0) continue;

        graphics->CommandList()->RSSetViewports(1, &viewports.at(i));

        for (auto& obj : scene)
        {
            ID3D12DescriptorHeap* descriptorHeap = obj.mesh->ConstantBufferHeap();
            graphics->CommandList()->SetDescriptorHeaps(1, &descriptorHeap);
            graphics->CommandList()->SetGraphicsRootSignature(rootSignature);
            graphics->CommandList()->IASetVertexBuffers(0, 1, obj.mesh->VertexBufferView());
            graphics->CommandList()->IASetIndexBuffer(obj.mesh->IndexBufferView());
            graphics->CommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            graphics->CommandList()->SetGraphicsRootDescriptorTable(0, obj.mesh->ConstantBufferHandle(0));
            graphics->CommandList()->DrawIndexedInstanced(
                obj.submesh.indexCount, 1,
                obj.submesh.startIndex,
                obj.submesh.baseVertex,
                0);
        }

        if (showPerspectiveOnly && i == 0) break;
    }

    graphics->Present();    
}

void Multi::Finalize()
{
    rootSignature->Release();
    pipelineState->Release();

    for (auto& obj : scene)
        delete obj.mesh;
}


Object Multi::generateObjectFromGeometry(Geometry geometry, XMMATRIX scaleMatrix, XMMATRIX translationMatrix) {
    Object object;
    XMStoreFloat4x4(&object.world, scaleMatrix * translationMatrix);

    object.mesh = new Mesh();
    object.mesh->VertexBuffer(geometry.VertexData(), geometry.VertexCount() * sizeof(Vertex), sizeof(Vertex));
    object.mesh->IndexBuffer(geometry.IndexData(), geometry.IndexCount() * sizeof(uint), DXGI_FORMAT_R32_UINT);
    object.mesh->ConstantBuffer(sizeof(ObjectConstants));
    object.submesh.indexCount = geometry.IndexCount();
    
    return object;
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
    catch (Error & e)
    {
        MessageBox(nullptr, e.ToString().data(), "Multi", MB_OK);
    }

    return 0;
}