#include <Rendering\Data\GPUFrameRootTablesMap.hpp>

GPUFrameRootTablesMap::GPUFrameRootTablesMap() = default;

GPUFrameRootTablesMap::GPUFrameRootTablesMap(int frameCount, std::vector<GPUFrameResourceDescriptor> const& map, std::vector<StateAndResource> const& describedResources) :
    frameCount_(frameCount), descriptorTable_(map), describedResources_(describedResources)
{ }

GPUFrameRootTablesMap::GPUFrameRootTablesMap(int frameCount, std::vector<GPUFrameResourceDescriptor>&& map, std::vector<StateAndResource>&& describedResources) :
    frameCount_(std::move(frameCount)), descriptorTable_(std::move(map)), describedResources_(describedResources)
{ }

GPUFrameRootTablesMap::GPUFrameRootTablesMap(GPUFrameRootTablesMap const& rhs) = default;

GPUFrameRootTablesMap::GPUFrameRootTablesMap(GPUFrameRootTablesMap&& rhs) = default;

GPUFrameRootTablesMap& GPUFrameRootTablesMap::operator=(GPUFrameRootTablesMap const& rhs) = default;

GPUFrameRootTablesMap& GPUFrameRootTablesMap::operator=(GPUFrameRootTablesMap&& rhs) = default;

D3D12_CPU_DESCRIPTOR_HANDLE GPUFrameRootTablesMap::DescriptorTableCPUHandle(int frameIndex, int bindingPoint) const
{
    return descriptorTable_[bindingPoint].CPUViewHandle(frameIndex);
}

D3D12_GPU_DESCRIPTOR_HANDLE GPUFrameRootTablesMap::DescirptorTableGPUHandle(int frameIndex, int bindingPoint) const
{
    return descriptorTable_[bindingPoint].GPUViewHandle(frameIndex);
}

int GPUFrameRootTablesMap::TableSize() const
{
    return static_cast<int>(descriptorTable_.size());
}

int GPUFrameRootTablesMap::DescribedResourceCount(int frameIndex) const
{
    return static_cast<int>(describedResources_.size());
}

GPUFrameResource* GPUFrameRootTablesMap::DescribedResource(int resourceIndex)
{
    return describedResources_[resourceIndex].second;
}

D3D12_RESOURCE_STATES GPUFrameRootTablesMap::DescribedResourceTargetState(int resourceIndex) const
{
    return describedResources_[resourceIndex].first;
}

D3D12_RESOURCE_STATES GPUFrameRootTablesMap::DescribedResourceCurrentState(int frameIndex, int resourceIndex) const
{
    return describedResources_[resourceIndex].second->State(frameIndex);
}
