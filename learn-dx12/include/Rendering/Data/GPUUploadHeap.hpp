#pragma once

#include <pch.hpp>

#include <Rendering\Data\GPUFrameResource.hpp>

class GPUUploadHeap : public GPUFrameResource
{
public:
    GPUUploadHeap();
    GPUUploadHeap(int framesCount, ID3D12Device* device, void const* data, std::size_t size, bool isConstBuffer = false);
    GPUUploadHeap(GPUUploadHeap const&) = delete;
    GPUUploadHeap(GPUUploadHeap&& rhs);

    GPUUploadHeap& operator=(GPUUploadHeap const&) = delete;
    GPUUploadHeap& operator=(GPUUploadHeap&& rhs);

    void Map(int frameIndex, void** dest, D3D12_RANGE* range);
    void Unmap(int frameIndex, D3D12_RANGE* range);
};