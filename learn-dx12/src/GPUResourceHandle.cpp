#include <Rendering\Data\Resource\GPUResourceHandle.hpp>
#include <Rendering\Data\Resource\GPUStaticResourceAllocator.hpp>

GPUResourceHandle::GPUResourceHandle() = default;

GPUResourceHandle::GPUResourceHandle(std::size_t ID, GPUStaticResourceAllocator& allocator) :
    ID_{ ID }, allocator_{ &allocator }
{ }

GPUResourceHandle::GPUResourceHandle(GPUResourceHandle const&) = default;

GPUResourceHandle::GPUResourceHandle(GPUResourceHandle&&) = default;

GPUResourceHandle& GPUResourceHandle::operator=(GPUResourceHandle const&) = default;

GPUResourceHandle& GPUResourceHandle::operator=(GPUResourceHandle&&) = default;

std::size_t GPUResourceHandle::ID() const { return ID_; }

GPUResource& GPUResourceHandle::Resource() const
{
    return allocator_->AccessResource(*this);
}

/////////////////////////////////////////////////////////////////////////////////////////

GPUResourceViewHandle::GPUResourceViewHandle() = default;

GPUResourceViewHandle::GPUResourceViewHandle(std::size_t ID) :
    ID_{ ID }
{ }

GPUResourceViewHandle::GPUResourceViewHandle(GPUResourceViewHandle const& rhs) = default;

GPUResourceViewHandle::GPUResourceViewHandle(GPUResourceViewHandle&& rhs) = default;

GPUResourceViewHandle& GPUResourceViewHandle::operator=(GPUResourceViewHandle const& rhs) = default;

GPUResourceViewHandle& GPUResourceViewHandle::operator=(GPUResourceViewHandle&& rhs) = default;

std::size_t GPUResourceViewHandle::ID() const { return ID_; }

///////////////////////////////////////////////////////////////////////////////////////////

GPUResourceViewDirectHandle::GPUResourceViewDirectHandle() = default;

GPUResourceViewDirectHandle::GPUResourceViewDirectHandle(std::size_t ID) :
    ID_{ ID }
{ }

GPUResourceViewDirectHandle::GPUResourceViewDirectHandle(GPUResourceViewDirectHandle const& rhs) = default;

GPUResourceViewDirectHandle::GPUResourceViewDirectHandle(GPUResourceViewDirectHandle&& rhs) = default;

GPUResourceViewDirectHandle& GPUResourceViewDirectHandle::operator=(GPUResourceViewDirectHandle const& rhs) = default;

GPUResourceViewDirectHandle& GPUResourceViewDirectHandle::operator=(GPUResourceViewDirectHandle&& rhs) = default;

std::size_t GPUResourceViewDirectHandle::ID() const { return ID_; }