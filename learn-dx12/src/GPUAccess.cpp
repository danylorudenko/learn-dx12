#include <Rendering\GPUAccess.hpp>

GPUAccess::GPUAccess() = default;

GPUAccess::GPUAccess(Application& application)
{
    // Create device and dxgiFactory.
    InitializeD3D12();

    // Create main accessors to GPU.
    CreateGPUEngines();

    // Create swap chain and supporting resources.
    CreateSwapChain(application, dxgiFactory_.Get());

    CreateDefaultDescriptorHeaps();
    CreateFrameResources();

    // Set default viewport and scissor values.
    SetViewportScissor();
}

GPUAccess::GPUAccess(GPUAccess&& rhs) = default;

GPUAccess& GPUAccess::operator=(GPUAccess&& rhs) = default;

void GPUAccess::GetHardwareAdapter(IDXGIAdapter1** dest, IDXGIFactory1* factory)
{
    Microsoft::WRL::ComPtr<IDXGIAdapter1> adapter;
    *dest = nullptr;

    for (UINT i = 0; factory->EnumAdapters1(i, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND; i++) {
        DXGI_ADAPTER_DESC1 desc;
        adapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }

        if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, __uuidof(ID3D12Device), nullptr))) {
            OutputDebugStringA("Selected device\n");
            break;
        }
    }

    *dest = adapter.Detach();
}

void GPUAccess::InitializeD3D12()
{
#if defined(DEBUG) || defined(_DEBUG)
    {
        Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
        D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
        debugController->EnableDebugLayer();
    }
#endif

    HRESULT res = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory_));
    assert(SUCCEEDED(res) && "Failed to create dxgiFactory\n");

    Microsoft::WRL::ComPtr<IDXGIAdapter1> hardwareAdapter;
    GetHardwareAdapter(hardwareAdapter.GetAddressOf(), dxgiFactory_.Get());
    
    D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device_.ReleaseAndGetAddressOf()));
    if (!device_) {
        OutputDebugStringA("Failed to create device\n");
    }
}

void GPUAccess::CreateFrameResources()
{
    assert(SWAP_CHAIN_BUFFER_COUNT <= 3 && "More than 3 framebuffers in not currently supported.");
    
    // Render targets fetch and view creation.
    Microsoft::WRL::ComPtr<ID3D12Resource> renderBuffers[3];
    for (int i = 0; i < SWAP_CHAIN_BUFFER_COUNT; i++) {
        swapChain_->GetBuffer(i, IID_PPV_ARGS(renderBuffers[i].GetAddressOf()));
    }

    int const framesCount = static_cast<int>(SWAP_CHAIN_BUFFER_COUNT);

    rendetTargetBuffers_ = new GPUFrameResource{ framesCount, HEIGHT * WIDTH, renderBuffers, D3D12_RESOURCE_STATE_RENDER_TARGET };
    auto rtv = descriptorHeap_->AllocRtvLinear(rendetTargetBuffers_, nullptr, D3D12_RESOURCE_STATE_RENDER_TARGET, "renderBuffer", framesCount);
    finalRenderTargetViews_ = new GPUFrameResourceDescriptor{ rtv };

    // Creation of depth-stencil buffers, creation of views.
    D3D12_RESOURCE_DESC depthStencilDesc{};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Format = depthStencilBufferFormat;
    depthStencilDesc.Width = WIDTH;
    depthStencilDesc.Height = HEIGHT;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    depthStencilDesc.MipLevels = 1;

    depthStencilBuffers_ = GPUFrameResource{ framesCount, device_.Get(), WIDTH * HEIGHT, &depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE };

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = depthStencilBufferFormat;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    finalDepthStencilViews_ = descriptorHeap_.AllocDsvLinear(&depthStencilBuffers_, &dsvDesc, D3D12_RESOURCE_STATE_DEPTH_READ, "depthStencil", framesCount);
}

void GPUAccess::CreateDefaultDescriptorHeaps()
{
    descriptorHeap_ = GPUDescriptorHeap{ device_, RTV_HEAP_CAPACITY, DSV_HEAP_CAPACITY, CBV_SRV_UAV_CAPACITY };
}

void GPUAccess::SetViewportScissor()
{
    viewportRect_.TopLeftX = 0.0f;
    viewportRect_.TopLeftY = 0.0f;
    viewportRect_.MinDepth = 0.0f;
    viewportRect_.MaxDepth = 1.0f;
    viewportRect_.Width = static_cast<float>(WIDTH);
    viewportRect_.Height = static_cast<float>(HEIGHT);

    scissorRect_ = CD3DX12_RECT{ 0, 0, static_cast<LONG>(WIDTH), static_cast<LONG>(HEIGHT) };
}

void GPUAccess::CreateSwapChain(Application& application, IDXGIFactory* factory)
{
    swapChain_.Reset();

    DXGI_SWAP_CHAIN_DESC sd;
    sd.BufferDesc.Format = backBufferFormat_;
    sd.BufferDesc.Width = WIDTH;
    sd.BufferDesc.Height = HEIGHT;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;

    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SWAP_CHAIN_BUFFER_COUNT;
    sd.OutputWindow = application.window().nativeHandle();
    sd.Windowed = true;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    {
        auto const result = factory->CreateSwapChain(Engine<GPU_ENGINE_TYPE_DIRECT>().CommandQueue(), &sd, swapChain_.GetAddressOf());
        ThrowIfFailed(result);
    }
}

void GPUAccess::CreateGPUEngines()
{
    engines_[0] = GPUEngine{ device_.Get(), GPU_ENGINE_TYPE_DIRECT, SWAP_CHAIN_BUFFER_COUNT };
    engines_[1] = GPUEngine{ device_.Get(), GPU_ENGINE_TYPE_COPY, SWAP_CHAIN_BUFFER_COUNT };
    engines_[2] = GPUEngine{ device_.Get(), GPU_ENGINE_TYPE_COMPUTE, SWAP_CHAIN_BUFFER_COUNT };
}

void GPUAccess::ResetAll()
{
    Engine<GPU_ENGINE_TYPE_DIRECT>().Reset();
    Engine<GPU_ENGINE_TYPE_COPY>().Reset();
    Engine<GPU_ENGINE_TYPE_COMPUTE>().Reset();
}

void GPUAccess::CommitDefaultViewportScissorRects()
{
    Engine<GPU_ENGINE_TYPE_DIRECT>().Commit().RSSetViewports(1, &viewportRect_);
    Engine<GPU_ENGINE_TYPE_DIRECT>().Commit().RSSetScissorRects(1, &scissorRect_);
}

void GPUAccess::CreateRootSignature(Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSignature, Microsoft::WRL::ComPtr<ID3D12RootSignature>& dest)
{
    auto const result = device_->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&dest));
    ThrowIfFailed(result);
}

void GPUAccess::CompileShader(LPCWSTR fileName, Microsoft::WRL::ComPtr<ID3DBlob>& dest, LPCSTR entryPoint, LPCSTR type)
{
#if defined (_DEBUG) | (DEBUG)
    UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT compileFlags = 0;
#endif
    {
        auto const result = D3DCompileFromFile(fileName, nullptr, nullptr, entryPoint, type, compileFlags, 0, &dest, nullptr);
        ThrowIfFailed(result);
    }
}

void GPUAccess::CreatePSO(Microsoft::WRL::ComPtr<ID3D12PipelineState>& dest, D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc)
{
    auto const result = device_->CreateGraphicsPipelineState(desc, IID_PPV_ARGS(&dest));
    ThrowIfFailed(result);
}
