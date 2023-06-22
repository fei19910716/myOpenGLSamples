#pragma once

#include "vulkan/vulkan.h"

template<typename T>
class VKObject{
    public:

    virtual ~VKObject() = default;
    VKObject() = default;

    virtual const T& Handle() const{
        return handle;
    }

    virtual const T* pHandle() const{
        return &handle;
    }

protected:
    T handle;
};