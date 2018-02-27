#pragma once

#include <pch.hpp>

#include <Math\Transform.hpp>
#include <Rendering\Data\Resource\GPUResourceHandle.hpp>
#include <Rendering\Data\Resource\ResourceView\GPUResourceViewTable.hpp>
#include <Rendering\GPUFoundation.hpp>

namespace DXRL
{

class GPURenderItem
{
public:
	GPURenderItem();
	GPURenderItem(GPURenderItem const&) = delete;
	GPURenderItem(GPURenderItem&& rhs);

	GPURenderItem& operator=(GPURenderItem const&) = delete;
	GPURenderItem& operator=(GPURenderItem&& rhs);

	void CreateTransformBuffer(std::size_t frames, std::size_t bindSlot, GPUFoundation& foundation);


public:
	Math::Transform transform_;

    D3D_PRIMITIVE_TOPOLOGY primitiveTopology_;

    GPUResourceHandle vertexBuffer_;
    GPUResourceHandle indexBuffer_;

    D3D12_VERTEX_BUFFER_VIEW vertexBufferDescriptor_;
    D3D12_INDEX_BUFFER_VIEW indexBufferDescriptor_;

    UINT vertexCount_ = 0;
    UINT indexCount_ = 0;

    struct GPUDynamicRootArgument
    {
        std::size_t bindSlot_;
        GPUResourceViewTable itemTable_;
    };

    GPUDynamicRootArgument dynamicArg_;
};

}