#pragma once

template<typename T>
class VKObject{
    public:

    virtual ~VKObject() = default;
    VKObject() = default;

    virtual T& Handle(){
        return handle;
    }

    virtual const T* pHandle(){
        return &handle;
    }

protected:
    T handle;
};