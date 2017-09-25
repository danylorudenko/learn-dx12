#pragma once

#include <pch.hpp>

class GPUResource
{
public:
    GPUResource(ID3D12Device* device, UINT64 size);
    GPUResource(const GPUResource&) = delete;
    GPUResource(GPUResource&& rhs);

    GPUResource& operator=(const GPUResource&) = delete;
    GPUResource& operator=(GPUResource&& rhs);
    
    ID3D12Resource* Get() const { return resource_.Get(); }
    UINT64 Size() const { return size_; };

    D3D12_GPU_VIRTUAL_ADDRESS GPUVirtualAddress() const { return gpuAddress_; }

    void Map(void** dest, const D3D12_RANGE& range) { ThrowIfFailed(resource_->Map(0, &range, dest)); }
    void Unmap(const D3D12_RANGE& range) { resource_->Unmap(0, &range); }

    D3D12_RESOURCE_STATES SetTargetState(D3D12_RESOURCE_STATES state) { auto previousState = targetState_; targetState_ = state; return previousState; }
    D3D12_RESOURCE_STATES TargetState() const { return targetState_; }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;
    UINT64 size_;
    D3D12_GPU_VIRTUAL_ADDRESS gpuAddress_ = 0;
    D3D12_RESOURCE_STATES targetState_ = D3D12_RESOURCE_STATE_COMMON;
};