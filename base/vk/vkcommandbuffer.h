#pragma once

#include "base/utils.h"
#include "vkobject.h"

#include <cassert>
#include <vector>
#include <string>


class VKCommandBuffer: public VKObject<VkCommandBuffer>{
public:
    VKCommandBuffer(const VkCommandBuffer& buffer){
        this->handle = buffer;
    }

};