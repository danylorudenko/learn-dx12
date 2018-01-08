#include <Rendering\Data\Resource\GPUStaticResourceAllocator.hpp>
#include <Rendering\Data\Resource\GPUResourceFrameTable.hpp>

GPUStaticResourceAllocator::GPUStaticResourceAllocator() = default;

GPUStaticResourceAllocator::GPUStaticResourceAllocator(GPUFoundation const& foundation) :
    foundation_{ &foundation }
{ }

GPUStaticResourceAllocator::GPUStaticResourceAllocator(GPUStaticResourceAllocator&&) = default;

GPUStaticResourceAllocator& GPUStaticResourceAllocator::operator=(GPUStaticResourceAllocator&&) = default;

GPUResourceDirectID GPUStaticResourceAllocator::Alloc(D3D12_RESOURCE_DESC const& resourceDesc, D3D12_RESOURCE_STATES initialState)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> tempResourcePtr{ nullptr };
    auto& device = foundation_->Device();

    {
        auto const result = device->CreateCommittedResource(
            &CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            initialState,
            nullptr,
            IID_PPV_ARGS(tempResourcePtr.GetAddressOf()));
        
        ThrowIfFailed(result);
    }

    committedResources_.push_back(std::make_unique<GPUResource>(std::move(tempResourcePtr), D3D12_RESOURCE_STATE_COMMON, L""));
    return GPUResourceDirectID{ committedResources_.size() - 1 };
}
