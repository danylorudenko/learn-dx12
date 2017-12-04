#pragma once

#include <pch.hpp>

#include <Rendering\Data\GPUResource.hpp>

class IResourceSizeGetter
{
public:
    virtual std::size_t GetResourceSize(GPUResource const& resource) = 0;
};