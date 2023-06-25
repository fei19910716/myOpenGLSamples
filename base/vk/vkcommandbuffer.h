#pragma once

#include "base/utils.h"
#include "vkobject.h"

#include <cassert>
#include <vector>
#include <string>


class VKCommandBuffer: public VKObject<VkCommandBuffer>{
public:
    // VKCommandBuffer的生命周期只能通过pool来管理
    VKCommandBuffer() = delete;

    VKCommandBuffer(const VkCommandBuffer& buffer){
        this->handle = buffer;
    }

};