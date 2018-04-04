#include <pch.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <3rdParty\stb_image.h>

#include <Foundation\Direct3DAppDelegate.hpp>
#include <Rendering\FrameGraph\GPUGraphicsGraphNode.hpp>
#include <Rendering\FrameGraph\GPUPresentGraphNode.hpp>
#include <Rendering\Data\GPURenderItem.hpp>
#include <Rendering\Data\Vertex.hpp>

struct VertHeader
{
    std::uint32_t vertexCount_ = 0;
    std::uint32_t vertexSize_ = 0;
    std::uint32_t indexCount_ = 0;
    std::uint32_t indexSize_ = 0;
};

Direct3DAppDelegate::Direct3DAppDelegate(Application& application)
    : Application::Delegate{ application }
    , mainWindow{ nullptr }
    , winProcDelegate_{}
    , gpuDelegate_{ nullptr }
    , gameTimer_{}
    , mainUpdateIterator_{ 0 }
    , windowText_{}
{ }

void Direct3DAppDelegate::start()
{
    winProcDelegate_ = DirectWinProcDelegate{ this };
    MainApplication().window().winProcHandler(&winProcDelegate_);
    
    auto constexpr framesCount = DXRL::GPUDelegate::SWAP_CHAIN_BUFFER_COUNT;
    /*

    gpuDelegate_ = std::make_unique<DXRL::GPUDelegate>(MainApplication());
    gameTimer_.Reset();

    auto& initializationEngine = gpuDelegate_->Engine<DXRL::GPU_ENGINE_TYPE_DIRECT>();


    ////////////////////////////////////////////////////////////////////////////
	float constexpr planeVertexData[] = {
		 1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f,  1.0f,   0.0f, 1.0f, 0.0f,
		 1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
		-1.0f, 0.0f, -1.0f,   0.0f, 1.0f, 0.0f,
	};

	std::int32_t constexpr planeIndexData[] = {
		2, 1, 0,
		3, 1, 2
	};

    std::ifstream ifstream("ball.model", std::ios_base::binary);
    if (!ifstream.is_open()) {
        assert(false);
    }
    ifstream.seekg(std::ios_base::beg);
    VertHeader header;
    ifstream.read(reinterpret_cast<char*>(&header), sizeof(VertHeader));

    std::size_t const vertexSize = header.vertexSize_;
    std::size_t const indexSize = header.indexSize_;

    std::size_t const vertexBytes = vertexSize * header.vertexCount_;
    std::size_t const indexBytes = indexSize * header.indexCount_;

    BYTE* vertexData = new BYTE[vertexBytes + sizeof(planeVertexData)];
    BYTE* indexData = new BYTE[indexBytes + sizeof(planeIndexData)];

    ifstream.read(reinterpret_cast<char*>(vertexData), vertexBytes);
    ifstream.read(reinterpret_cast<char*>(indexData), indexBytes);

    ifstream.close();

	std::memcpy(vertexData + vertexBytes, planeVertexData, sizeof(planeVertexData));
	std::memcpy(indexData + indexBytes, planeIndexData, sizeof(planeIndexData));


    auto uploadVertexBuffer = gpuDelegate_->AllocUploadResource(CD3DX12_RESOURCE_DESC::Buffer(vertexBytes + sizeof(planeVertexData)), D3D12_RESOURCE_STATE_GENERIC_READ);
    auto vertexBuffer = gpuDelegate_->AllocDefaultResource(CD3DX12_RESOURCE_DESC::Buffer(vertexBytes + sizeof(planeVertexData)), D3D12_RESOURCE_STATE_COPY_DEST);
    auto uploadIndexBuffer = gpuDelegate_->AllocUploadResource(CD3DX12_RESOURCE_DESC::Buffer(indexBytes + sizeof(planeIndexData)), D3D12_RESOURCE_STATE_GENERIC_READ);
    auto indexBuffer = gpuDelegate_->AllocDefaultResource(CD3DX12_RESOURCE_DESC::Buffer(indexBytes + sizeof(planeIndexData)), D3D12_RESOURCE_STATE_COPY_DEST);


    BYTE* mappedVertexData = nullptr;
    uploadVertexBuffer.Resource().Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData));
    std::memcpy(mappedVertexData, vertexData, vertexBytes);
	std::memcpy(mappedVertexData + vertexBytes, planeVertexData, sizeof(planeVertexData));
    D3D12_RANGE writtenVertexRange{ 0, vertexBytes + sizeof(planeVertexData) };
    uploadVertexBuffer.Resource().Get()->Unmap(0, &writtenVertexRange);
    mappedVertexData = nullptr;

    BYTE* mappedIndexData = nullptr;
    uploadIndexBuffer.Resource().Get()->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndexData));
    std::memcpy(mappedIndexData, indexData, indexBytes);
	std::memcpy(mappedIndexData + indexBytes, planeIndexData, sizeof(planeIndexData));
    D3D12_RANGE writtenIndexRange{ 0, indexBytes + sizeof(planeIndexData) };
    uploadIndexBuffer.Resource().Get()->Unmap(0, &writtenIndexRange);
    mappedIndexData = nullptr;

    initializationEngine.Commit().CopyBufferRegion(vertexBuffer.Resource().GetPtr(), 0, uploadVertexBuffer.Resource().GetPtr(), 0, vertexBytes + sizeof(planeVertexData));
    initializationEngine.Commit().CopyBufferRegion(indexBuffer.Resource().GetPtr(), 0, uploadIndexBuffer.Resource().GetPtr(), 0, indexBytes + sizeof(planeIndexData));


    vertexBuffer.Resource().Transition(initializationEngine, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
    indexBuffer.Resource().Transition(initializationEngine, D3D12_RESOURCE_STATE_INDEX_BUFFER);

    delete[] vertexData; vertexData = nullptr;
    delete[] indexData; indexData = nullptr;
    initializationEngine.FlushReset();

    D3D12_VERTEX_BUFFER_VIEW vbView{};
    vbView.BufferLocation = vertexBuffer.Resource().Get()->GetGPUVirtualAddress();
    vbView.SizeInBytes = static_cast<UINT>(vertexBytes);
    vbView.StrideInBytes = sizeof(Vertex);

    D3D12_INDEX_BUFFER_VIEW ibView{};
    ibView.BufferLocation = indexBuffer.Resource().Get()->GetGPUVirtualAddress();
    ibView.Format = DXGI_FORMAT_R32_UINT;
    ibView.SizeInBytes = static_cast<UINT>(indexBytes);

	//////////////////////

	int albedoHeight = 0;
	int albedoWidth = 0;
	int albedoComponents = 0;
	unsigned char* albedoData = stbi_load("rustediron-streaks_basecolor.png", &albedoHeight, &albedoWidth, &albedoComponents, STBI_rgb_alpha);
	//unsigned char* albedoData = stbi_load("slipperystonework-albedo.png", &albedoHeight, &albedoWidth, &albedoComponents, STBI_rgb_alpha);
	int albedoBytesCount = albedoWidth * albedoHeight * albedoComponents;

	D3D12_RESOURCE_DESC albedoMapDesc;
	albedoMapDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	albedoMapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	albedoMapDesc.Alignment = 0;
	albedoMapDesc.Width = albedoWidth;
	albedoMapDesc.Height = albedoHeight;
	albedoMapDesc.DepthOrArraySize = 1;
	albedoMapDesc.MipLevels = 1;
	albedoMapDesc.SampleDesc.Count = 1;
	albedoMapDesc.SampleDesc.Quality = 0;
	albedoMapDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	albedoMapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	UINT64 albedoCopyableSize = 0;
	gpuDelegate_->Device()->GetCopyableFootprints(&albedoMapDesc, 0, 1, 0, nullptr, nullptr, nullptr, &albedoCopyableSize);

	DXRL::GPUResourceHandle albedoMapUploadHeap = gpuDelegate_->AllocUploadResource(CD3DX12_RESOURCE_DESC::Buffer(albedoCopyableSize), D3D12_RESOURCE_STATE_GENERIC_READ);
	DXRL::GPUResourceHandle albedoMap = gpuDelegate_->AllocDefaultResource(albedoMapDesc, D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12_SUBRESOURCE_DATA albedoSubData;
	albedoSubData.pData = albedoData;
	albedoSubData.RowPitch = albedoWidth * 4;
	albedoSubData.SlicePitch = albedoSubData.RowPitch * albedoHeight;

	UINT64 result = UpdateSubresources(
		initializationEngine.CommandList(), 
		albedoMap.Resource().GetPtr(),
		albedoMapUploadHeap.Resource().GetPtr(),
		0,
		0,
		1,
		&albedoSubData);

	albedoMap.Resource().Transition(initializationEngine, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	initializationEngine.Commit().DiscardResource(albedoMapUploadHeap.Resource().GetPtr(), nullptr);
	initializationEngine.FlushReset();

	stbi_image_free(albedoData); albedoData = nullptr;

	D3D12_SHADER_RESOURCE_VIEW_DESC albedoViewDesc;
	albedoViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	albedoViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	albedoViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	albedoViewDesc.Texture2D.MipLevels = 1;
	albedoViewDesc.Texture2D.MostDetailedMip = 0;
	albedoViewDesc.Texture2D.PlaneSlice = 0;
	albedoViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	DXRL::GPUResourceViewHandle albedoView = gpuDelegate_->AllocSRV(1, &albedoMap, &albedoViewDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//////////////////////

	int normalHeight = 0;
	int normalWidth = 0;
	int normalComponents = 0;
	unsigned char* normalData = stbi_load("rustediron-streaks_normal.png", &normalHeight, &normalWidth, &normalComponents, STBI_rgb_alpha);
	//unsigned char* normalData = stbi_load("slipperystonework-normal.png", &normalHeight, &normalWidth, &normalComponents, STBI_rgb_alpha);
	int normalBytesCount = normalHeight * normalHeight * normalComponents;

	D3D12_RESOURCE_DESC normalMapDesc;
	normalMapDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	normalMapDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	normalMapDesc.Alignment = 0;
	normalMapDesc.Height = normalHeight;
	normalMapDesc.Width = normalWidth;
	normalMapDesc.DepthOrArraySize = 1;
	normalMapDesc.MipLevels = 1;
	normalMapDesc.SampleDesc.Count = 1;
	normalMapDesc.SampleDesc.Quality = 0;
	normalMapDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	normalMapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
	

	DXRL::GPUResourceHandle normalMap = gpuDelegate_->AllocDefaultResource(normalMapDesc, D3D12_RESOURCE_STATE_COPY_DEST);
	
	D3D12_SUBRESOURCE_DATA normalSubData;
	normalSubData.pData = normalData;
	normalSubData.RowPitch = normalWidth * 4;
	normalSubData.SlicePitch = normalSubData.RowPitch * normalHeight;
	
	UpdateSubresources(
		initializationEngine.CommandList(),
		normalMap.Resource().GetPtr(),
		albedoMapUploadHeap.Resource().GetPtr(),
		0,
		0,
		1,
		&normalSubData);

	stbi_image_free(normalData);
	
	normalMap.Resource().Transition(initializationEngine, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	initializationEngine.FlushReset();

	D3D12_SHADER_RESOURCE_VIEW_DESC normalViewDesc;
	normalViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	normalViewDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	normalViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	normalViewDesc.Texture2D.MipLevels = 1;
	normalViewDesc.Texture2D.MostDetailedMip = 0;
	normalViewDesc.Texture2D.PlaneSlice = 0;
	normalViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	DXRL::GPUResourceViewHandle normalView = gpuDelegate_->AllocSRV(1, &normalMap, &normalViewDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//////////////////////

	int metallnessHeight = 0;
	int metallnessWidth = 0;
	int metallnessComponents = 0;
	unsigned char* metallnessData = stbi_load("rustediron-streaks_metallic.png", &metallnessHeight, &metallnessWidth, &metallnessComponents, 1);
	//unsigned char* metallnessData = stbi_load("slipperystonework-metalness.png", &metallnessHeight, &metallnessWidth, &metallnessComponents, 1);
	int metallnessBytesCount = metallnessHeight * metallnessHeight * metallnessComponents;

	D3D12_RESOURCE_DESC metallnessMapDesc;
	metallnessMapDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	metallnessMapDesc.Format = DXGI_FORMAT_R8_UNORM;
	metallnessMapDesc.Alignment = 0;
	metallnessMapDesc.Height = metallnessHeight;
	metallnessMapDesc.Width = metallnessWidth;
	metallnessMapDesc.DepthOrArraySize = 1;
	metallnessMapDesc.MipLevels = 1;
	metallnessMapDesc.SampleDesc.Count = 1;
	metallnessMapDesc.SampleDesc.Quality = 0;
	metallnessMapDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	metallnessMapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	DXRL::GPUResourceHandle metallnessMap = gpuDelegate_->AllocDefaultResource(metallnessMapDesc, D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12_SUBRESOURCE_DATA metallnessSubData;
	metallnessSubData.pData = metallnessData;
	metallnessSubData.RowPitch = metallnessWidth;
	metallnessSubData.SlicePitch = metallnessSubData.RowPitch * metallnessHeight;

	UpdateSubresources(
		initializationEngine.CommandList(),
		metallnessMap.Resource().GetPtr(),
		albedoMapUploadHeap.Resource().GetPtr(),
		0,
		0,
		1,
		&metallnessSubData);

	stbi_image_free(metallnessData);

	metallnessMap.Resource().Transition(initializationEngine, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	initializationEngine.FlushReset();

	D3D12_SHADER_RESOURCE_VIEW_DESC metallnessViewDesc;
	metallnessViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	metallnessViewDesc.Format = DXGI_FORMAT_R8_UNORM;
	metallnessViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	metallnessViewDesc.Texture2D.MipLevels = 1;
	metallnessViewDesc.Texture2D.MostDetailedMip = 0;
	metallnessViewDesc.Texture2D.PlaneSlice = 0;
	metallnessViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	DXRL::GPUResourceViewHandle metallnessView = gpuDelegate_->AllocSRV(1, &metallnessMap, &metallnessViewDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

	//////////////////////

	int roughnessHeight = 0;
	int roughnessWidth = 0;
	int roughnessComponents = 0;
	unsigned char* roughnessData = stbi_load("rustediron-streaks_roughness.png", &roughnessHeight, &roughnessWidth, &roughnessComponents, 1);
	//unsigned char* roughnessData = stbi_load("slipperystonework-rough.png", &roughnessHeight, &roughnessWidth, &roughnessComponents, 1);
	int roughnessBytesCount = roughnessHeight * roughnessHeight * roughnessComponents;

	D3D12_RESOURCE_DESC roughnessMapDesc;
	roughnessMapDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	roughnessMapDesc.Format = DXGI_FORMAT_R8_UNORM;
	roughnessMapDesc.Alignment = 0;
	roughnessMapDesc.Height = roughnessHeight;
	roughnessMapDesc.Width = roughnessWidth;
	roughnessMapDesc.DepthOrArraySize = 1;
	roughnessMapDesc.MipLevels = 1;
	roughnessMapDesc.SampleDesc.Count = 1;
	roughnessMapDesc.SampleDesc.Quality = 0;
	roughnessMapDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	roughnessMapDesc.Flags = D3D12_RESOURCE_FLAG_NONE;


	DXRL::GPUResourceHandle roughnessMap = gpuDelegate_->AllocDefaultResource(roughnessMapDesc, D3D12_RESOURCE_STATE_COPY_DEST);

	D3D12_SUBRESOURCE_DATA roughnessSubData;
	roughnessSubData.pData = roughnessData;
	roughnessSubData.RowPitch = roughnessWidth;
	roughnessSubData.SlicePitch = roughnessSubData.RowPitch * roughnessHeight;

	UpdateSubresources(
		initializationEngine.CommandList(),
		roughnessMap.Resource().GetPtr(),
		albedoMapUploadHeap.Resource().GetPtr(),
		0,
		0,
		1,
		&roughnessSubData);

	stbi_image_free(roughnessData);

	roughnessMap.Resource().Transition(initializationEngine, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
	initializationEngine.FlushReset();

	D3D12_SHADER_RESOURCE_VIEW_DESC roughnessViewDesc;
	roughnessViewDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	roughnessViewDesc.Format = DXGI_FORMAT_R8_UNORM;
	roughnessViewDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	roughnessViewDesc.Texture2D.MipLevels = 1;
	roughnessViewDesc.Texture2D.MostDetailedMip = 0;
	roughnessViewDesc.Texture2D.PlaneSlice = 0;
	roughnessViewDesc.Texture2D.ResourceMinLODClamp = 0.0f;

	DXRL::GPUResourceViewHandle roughnessView = gpuDelegate_->AllocSRV(1, &roughnessMap, &roughnessViewDesc, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);


    /////////////////////////////////////////////////////////////////////////////


    auto rootSignature = CreateRootSignature();
    DXRL::GPURootSignature triangleRootSignature{ rootSignature };

    auto constexpr sceneConstBufferSize = (sizeof(sceneBufferData_) + 255) & ~255;
    DXRL::GPUResourceHandle constBufferHandles[framesCount];
    for (std::size_t i = 0; i < framesCount; i++) {
        constBufferHandles[i] = gpuDelegate_->AllocUploadResource(CD3DX12_RESOURCE_DESC::Buffer(sceneConstBufferSize), D3D12_RESOURCE_STATE_GENERIC_READ);
    }

    D3D12_CONSTANT_BUFFER_VIEW_DESC sceneCbvDesc[framesCount];
    for (size_t i = 0; i < framesCount; i++) {
        sceneCbvDesc[i].BufferLocation = constBufferHandles[i].Resource().Get()->GetGPUVirtualAddress();
        sceneCbvDesc[i].SizeInBytes = sceneConstBufferSize;
    }
    
    sceneBuffer_ = gpuDelegate_->AllocCBV(framesCount, constBufferHandles, sceneCbvDesc, D3D12_RESOURCE_STATE_GENERIC_READ);


	triangleRootSignature.PushRootArgument(0, DXRL::GPUResourceViewTable{ 1, &sceneBuffer_ });

	DXRL::GPUResourceViewHandle textures[4];
	textures[0] = albedoView;
	textures[1] = normalView;
	textures[2] = metallnessView;
	textures[3] = roughnessView;
	triangleRootSignature.PushRootArgument(1, DXRL::GPUResourceViewTable{ 4, textures });


    auto pipelineState = CreatePipelineState(rootSignature);
    DXRL::GPUPipelineState trianglePipelineState_{ pipelineState };
    auto& graphicsGraphNode = gpuDelegate_->FrameGraph().AddGraphicsNode(nullptr, gpuDelegate_->Engine<DXRL::GPU_ENGINE_TYPE_DIRECT>(), std::move(trianglePipelineState_), std::move(triangleRootSignature));

	std::size_t constexpr renderItemsCount = 3;
	DXRL::GPURenderItem renderItem[renderItemsCount];
	for (std::size_t i = 0; i < renderItemsCount; i++) {
		renderItem[i].transform_.Position(DirectX::XMFLOAT3A{ static_cast<float>(i * 1.7f) - 1.7f , 0.0f, 0.0f });
		renderItem[i].transform_.RotationRollPitchYaw(DirectX::XMFLOAT3A{ 90.0f, (i * -45.0f) + 45.0f0.0f, 0.0f, 0.0f });
		renderItem[i].transform_.Scale(0.8f);
		renderItem[i].vertexBuffer_ = vertexBuffer;
		renderItem[i].vertexBufferDescriptor_ = vbView;
		renderItem[i].vertexCount_ = header.vertexCount_;
		renderItem[i].primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
		renderItem[i].indexBuffer_ = indexBuffer;
		renderItem[i].indexBufferDescriptor_ = ibView;
		renderItem[i].indexCount_ = header.indexCount_;
		renderItem[i].CreateTransformBuffer(framesCount, 2, *gpuDelegate_);

		graphicsGraphNode.ImportRenderItem(std::move(renderItem[i]));
	}

	//D3D12_VERTEX_BUFFER_VIEW planeVbView{};
	//planeVbView.BufferLocation = vertexBuffer.Resource().Get()->GetGPUVirtualAddress() + vertexBytes;
	//planeVbView.SizeInBytes = static_cast<UINT>(sizeof(planeVertexData));
	//planeVbView.StrideInBytes = sizeof(Vertex);
	//
	//D3D12_INDEX_BUFFER_VIEW planeIbView{};
	//planeIbView.BufferLocation = indexBuffer.Resource().Get()->GetGPUVirtualAddress() + indexBytes;
	//planeIbView.Format = DXGI_FORMAT_R32_UINT;
	//planeIbView.SizeInBytes = static_cast<UINT>(sizeof(planeIndexData));
	//
	//DXRL::GPURenderItem planeRenderItem{};
	//planeRenderItem.transform_.Position(DirectX::XMFLOAT3A{ 0.0f, -2.0f, 0.0f });
	//planeRenderItem.transform_.RotationRollPitchYaw(DirectX::XMFLOAT3A{ 0.0f, 0.0f, 0.0f });
	//planeRenderItem.transform_.Scale(6.0f);
	//planeRenderItem.vertexBuffer_ = vertexBuffer;
	//planeRenderItem.vertexBufferDescriptor_ = planeVbView;
	//planeRenderItem.vertexCount_ = 4;
	//planeRenderItem.primitiveTopology_ = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//planeRenderItem.indexBuffer_ = indexBuffer;
	//planeRenderItem.indexBufferDescriptor_ = planeIbView;
	//planeRenderItem.indexCount_ = 6;
	//planeRenderItem.CreateTransformBuffer(framesCount, 1, *gpuDelegate_);

	//graphicsGraphNode.ImportRenderItem(std::move(planeRenderItem));

	/////////////////////////////////////////////////////////////

    auto swapChainRTV = gpuDelegate_->SwapChainRTV();
    graphicsGraphNode.ImportRenderTargets(1, &swapChainRTV);
    DXRL::Color clearColor{ 0.1f, 0.11f, 0.12f, 1.0f };
    graphicsGraphNode.ImportClearColors(&clearColor, 1);

    D3D12_RESOURCE_DESC depthStencilDesc;
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.Height = DXRL::GPUDelegate::HEIGHT;
    depthStencilDesc.Width = DXRL::GPUDelegate::WIDTH;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    DXRL::GPUResourceHandle depthStencilBuffers_[framesCount];
    for (size_t i = 0; i < framesCount; i++) {
        depthStencilBuffers_[i] = gpuDelegate_->AllocDefaultResource(depthStencilDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    }

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.Texture2D.MipSlice = 0;
    auto depthStencilView = gpuDelegate_->AllocDSV(framesCount, depthStencilBuffers_, dsvDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    DXRL::GPUDepthStencilSettings dsSettings{};
    dsSettings.ActivateDepth();
    dsSettings.depthStencilClearFlags_ = D3D12_CLEAR_FLAG_DEPTH;

    graphicsGraphNode.ImportDepthStencilTarget(depthStencilView);
    graphicsGraphNode.ImportDepthStencilSettings(dsSettings);
    

    D3D12_VIEWPORT viewport{};
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.Width = static_cast<float>(DXRL::GPUDelegate::WIDTH);
    viewport.Height = static_cast<float>(DXRL::GPUDelegate::HEIGHT);
    CD3DX12_RECT scissorRect{ 0, 0, static_cast<LONG>(DXRL::GPUDelegate::WIDTH), static_cast<LONG>(DXRL::GPUDelegate::HEIGHT) };
    graphicsGraphNode.ImportViewportScissor(viewport, scissorRect);


    auto& presentNode_ = gpuDelegate_->FrameGraph().AddPresentNode(graphicsGraphNode, gpuDelegate_->SwapChain(), gpuDelegate_->Engine<DXRL::GPU_ENGINE_TYPE_DIRECT>());
    presentNode_.ImportRenderTarget(swapChainRTV);
    

    gpuDelegate_->FrameGraph().ParseGraphToQueue();

    initializationEngine.FlushReset();

	camera_.Transform().Position(DirectX::XMFLOAT3A{ 0.0f, 0.0f, -4.0f });
    camera_.NearPlane(0.1f);
    camera_.FarPlane(1000.0f);
    camera_.Fow(60.0f);
    camera_.AspectRatio(static_cast<float>(DXRL::GPUDelegate::WIDTH) / static_cast<float>(DXRL::GPUDelegate::HEIGHT));

    */
}

void Direct3DAppDelegate::update()
{
    /*std::size_t const normalizedFrameIndex = mainUpdateIterator_ % DXRL::GPUDelegate::SWAP_CHAIN_BUFFER_COUNT;
    CustomAction(normalizedFrameIndex);
    MainUpdate(normalizedFrameIndex);

    gameTimer_.Tick();
    DisplayFrameTime(application, Timer().DeltaTime());

    ++mainUpdateIterator_;
    */
}

void Direct3DAppDelegate::shutdown()
{

}

PerformanceTimer& Direct3DAppDelegate::Timer()
{
    return gameTimer_;
}

void Direct3DAppDelegate::DisplayFrameTime(Application& application, float drawTime)
{
    windowText_.resize(0);
    windowText_ = L"SPF: ";
    windowText_ += std::to_wstring(drawTime);

    windowText_ += L";    FPS: ";
    windowText_ += std::to_wstring(1 / drawTime);

    HWND windowHandle = application.window().nativeHandle();
    SetWindowText(windowHandle, windowText_.c_str());
}

//Microsoft::WRL::ComPtr<ID3D12RootSignature> Direct3DAppDelegate::CreateRootSignature()
//{
//    CD3DX12_ROOT_PARAMETER rootParameters[3];
//    CD3DX12_DESCRIPTOR_RANGE ranges[3];
//
//    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
//    rootParameters[0].InitAsDescriptorTable(1, ranges);
//
//	ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 0);
//	rootParameters[1].InitAsDescriptorTable(1, ranges + 1);
//
//    ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
//    rootParameters[2].InitAsDescriptorTable(1, ranges + 2);
//
//    D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
//
//	CD3DX12_STATIC_SAMPLER_DESC samplerDesc;
//	samplerDesc.Init(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);
//
//
//    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
//    rootSignatureDesc.Init(3, rootParameters, 1, &samplerDesc, rootSignatureFlags);
//
//    Microsoft::WRL::ComPtr<ID3DBlob> signature;
//    Microsoft::WRL::ComPtr<ID3DBlob> errors;
//
//    {
//        auto const result = D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &signature, &errors);
//
//		//if (errors->GetBufferSize() > 0) {
//		//	char* error = new char[errors->GetBufferSize()];
//		//	std::memcpy(error, errors->GetBufferPointer(), errors->GetBufferSize());
//		//
//		//}
//		
//        DXRL_THROW_IF_FAILED(result);
//    }
//    
//    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
//    gpuDelegate_->CreateRootSignature(signature, rootSignature);
//    return rootSignature;
//}

//Microsoft::WRL::ComPtr<ID3D12PipelineState> Direct3DAppDelegate::CreatePipelineState(Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature)
//{
//    Microsoft::WRL::ComPtr<ID3DBlob> vertexShader;
//    Microsoft::WRL::ComPtr<ID3DBlob> pixelShader;
//
//    gpuDelegate_->CompileShader(L"Shaders\\CookTorrance.hlsl", vertexShader, "VS", "vs_5_0");
//    gpuDelegate_->CompileShader(L"Shaders\\CookTorrance.hlsl", pixelShader, "PS", "ps_5_0");
//
//    // Define the vertex input layout.
//    D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
//    {
//        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "BITANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
//		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 48, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
//    };
//
//    // Setup pipeline state, which inludes setting shaders.
//    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
//    psoDesc.InputLayout = { inputElementDescs, 5 };
//    psoDesc.pRootSignature = rootSignature.Get();
//    psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
//    psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
//    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
//    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
//    psoDesc.DepthStencilState.DepthEnable = TRUE;
//    psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
//    psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
//    psoDesc.DepthStencilState.StencilEnable = FALSE;
//    psoDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
//    psoDesc.SampleMask = UINT_MAX;
//    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
//    psoDesc.NumRenderTargets = 1;
//    psoDesc.RTVFormats[0] = DXRL::GPUDelegate::backBufferFormat_;
//    psoDesc.SampleDesc.Count = 1;
//    psoDesc.SampleDesc.Quality = 0;
//
//    Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState = nullptr;
//    gpuDelegate_->CreatePSO(pipelineState, &psoDesc);
//    return pipelineState;
//}

void Direct3DAppDelegate::MainUpdate(std::size_t frameIndex)
{
    auto& graph = gpuDelegate_->FrameGraph();
    auto& graphIterator = graph.GraphQueueStart();
    auto& graphEnd = graph.GraphQueueEnd();
    
    auto& directEngine = gpuDelegate_->Engine<DXRL::GPU_ENGINE_TYPE_DIRECT>();

    gpuDelegate_->SetDefaultDescriptorHeaps();

    while (graphIterator != graphEnd) {
        (*graphIterator)->Process(frameIndex);
        ++graphIterator;
    }
}

//void Direct3DAppDelegate::CustomAction(std::size_t frameIndex)
//{
//	float constexpr PIXEL_TO_ANGLE = 0.3f;
//
//	cameraTargetPitch_ += winProcDelegate_.mouseYDelta_ * PIXEL_TO_ANGLE * DEGREE_TO_RAD;
//	cameraTargetYaw_ += winProcDelegate_.mouseXDelta_ * PIXEL_TO_ANGLE * DEGREE_TO_RAD;
//
//	camera_.Transform().Rotation(DirectX::XMQuaternionRotationRollPitchYaw(cameraTargetPitch_, cameraTargetYaw_, 0.0f));
//	
//	/////////////////////////////////
//
//	float constexpr SPEED_PER_FRAME = 0.02f;
//
//	auto pos = camera_.Transform().PositionSIMD();
//	auto forward = camera_.Transform().ForwardSIMD();
//	auto right = camera_.Transform().RightSIMD();
//
//	if (winProcDelegate_.WPressed()) {
//		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorMultiply(forward, DirectX::XMVectorReplicate(SPEED_PER_FRAME)));
//	}
//	if (winProcDelegate_.SPressed()) {
//		pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorMultiply(forward, DirectX::XMVectorReplicate(SPEED_PER_FRAME)));
//	}
//	if (winProcDelegate_.APressed()) {
//		pos = DirectX::XMVectorAdd(pos, DirectX::XMVectorMultiply(right, DirectX::XMVectorReplicate(SPEED_PER_FRAME)));
//	}
//	if (winProcDelegate_.DPressed()) {
//		pos = DirectX::XMVectorSubtract(pos, DirectX::XMVectorMultiply(right, DirectX::XMVectorReplicate(SPEED_PER_FRAME)));
//	}
//	camera_.Transform().Position(pos);
//
//
//	//auto static clip = [](float val, float l, float h) -> float { return (std::max)(l, (std::min)(val, h)); };
//	//float constexpr STEP = 0.001f;
//	//if (winProcDelegate_.UPressed()) {
//	//	sceneBufferData_.cameraPosition_roghness_.w = clip(sceneBufferData_.cameraPosition_roghness_.w + STEP, 0.0f, 1.0f);
//	//}
//	//if (winProcDelegate_.JPressed()) {
//	//	sceneBufferData_.cameraPosition_roghness_.w = clip(sceneBufferData_.cameraPosition_roghness_.w - STEP, 0.0f, 1.0f);
//	//}
//	//if (winProcDelegate_.IPressed()) {
//	//	sceneBufferData_.metalness = clip(sceneBufferData_.metalness + STEP, 0.0f, 1.0f);
//	//}
//	//if (winProcDelegate_.KPressed()) {
//	//	sceneBufferData_.metalness = clip(sceneBufferData_.metalness - STEP, 0.0f, 1.0f);
//	//}
//
//
//	/////////////////////////////////
//
//	const auto& cameraPos = camera_.Transform().Position();
//	sceneBufferData_.projectionMatrix_ = camera_.PerspectiveMatrix();
//    sceneBufferData_.viewMatrix_ = camera_.ViewMatrix();
//	sceneBufferData_.cameraPosition_ = DirectX::XMFLOAT3A{ cameraPos.x, cameraPos.y, cameraPos.z };
//
//	const auto t = Timer().TotalTime();
//	const auto lx = DirectX::XMScalarCos(t);
//	const auto lz = DirectX::XMScalarSin(t);
//	sceneBufferData_.lightDirection_ = DirectX::XMFLOAT3A{ lx, 0.0f, lz };
//	
//    char* mappedCameraData = nullptr;
//    auto& cameraBuffer = sceneBuffer_.View(frameIndex).Resource();
//
//	int kek = sizeof(sceneBufferData_);
//
//    cameraBuffer.Get()->Map(0, nullptr, (void**)&mappedCameraData);
//    std::memcpy(mappedCameraData, &sceneBufferData_, sizeof(sceneBufferData_));
//
//    D3D12_RANGE writtenRange{ 0, sizeof(sceneBufferData_) };
//    cameraBuffer.Get()->Unmap(0, &writtenRange);
//}

LRESULT DirectWinProcDelegate::operator()(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Direct3DAppDelegate* directAppDelegate = reinterpret_cast<Direct3DAppDelegate*>(appDelegate_);

	mouseXDelta_ = mouseYDelta_ = 0.0f;

    switch (msg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
	case WM_LBUTTONDOWN:
		rotationOn = !rotationOn;
		return 0;
    case WM_MOUSEMOVE: {
		int xMouse = static_cast<int>(LOWORD(lParam));
		int yMouse = static_cast<int>(HIWORD(lParam));

        if (rotationOn) {
			mouseXDelta_ = static_cast<float>(xMouse - prevMouseX_);
			mouseYDelta_ = static_cast<float>(yMouse - prevMouseY_);
        }

		prevMouseX_ = xMouse;
		prevMouseY_ = yMouse;
        return 0;
    }
	case WM_KEYDOWN:
		if (wParam == 0x57) { // W
			keyMap_ |= (1 << 0);
		}
		if (wParam == 0x41) { // A
			keyMap_ |= (1 << 1);
		}
		if (wParam == 0x53) { // S
			keyMap_ |= (1 << 2);
		}
		if (wParam == 0x44) { // D
			keyMap_ |= (1 << 3);
		}
		if (wParam == 0x55) { // U
			keyMap_ |= (1 << 4);
		}
		if (wParam == 0x4A) { // J
			keyMap_ |= (1 << 5);
		}
		if (wParam == 0x49) { // I
			keyMap_ |= (1 << 6);
		}
		if (wParam == 0x4B) { // K
			keyMap_ |= (1 << 7);
		}
		return 0;
	case WM_KEYUP:
		if (wParam == 0x57) { // W
			keyMap_ &= ~(1 << 0);
		}
		if (wParam == 0x41) { // A
			keyMap_ &= ~(1 << 1);
		}
		if (wParam == 0x53) { // S
			keyMap_ &= ~(1 << 2);
		}
		if (wParam == 0x44) { // D
			keyMap_ &= ~(1 << 3);
		}
		if (wParam == 0x55) { // U
			keyMap_ &= ~(1 << 4);
		}
		if (wParam == 0x4A) { // J
			keyMap_ &= ~(1 << 5);
		}
		if (wParam == 0x49) { // I
			keyMap_ &= ~(1 << 6);
		}
		if (wParam == 0x4B) { // K
			keyMap_ &= ~(1 << 7);
		}
		return 0;
    default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

bool DirectWinProcDelegate::WPressed() const
{
	return keyMap_ & (1 << 0);
}

bool DirectWinProcDelegate::APressed() const
{
	return keyMap_ & (1 << 1);
}

bool DirectWinProcDelegate::SPressed() const
{
	return keyMap_ & (1 << 2);
}

bool DirectWinProcDelegate::DPressed() const
{
	return keyMap_ & (1 << 3);
}

bool DirectWinProcDelegate::UPressed() const
{
	return keyMap_ & (1 << 4);
}

bool DirectWinProcDelegate::JPressed() const
{
	return keyMap_ & (1 << 5);
}

bool DirectWinProcDelegate::IPressed() const
{
	return keyMap_ & (1 << 6);
}

bool DirectWinProcDelegate::KPressed() const
{
	return keyMap_ & (1 << 7);
}