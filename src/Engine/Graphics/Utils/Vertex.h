#pragma once

#include <vulkan/vulkan.h>

#include <Math/Math.h>

struct vert {
    Vec2f pos;
    Vec3f color;

    static const VkVertexInputBindingDescription getBindingDesc() {
        VkVertexInputBindingDescription desc{};
        desc.binding = 0;
        desc.stride = sizeof(vert);
        desc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return desc;
    }

    static VkVertexInputAttributeDescription* getAttrDesc() {
        static VkVertexInputAttributeDescription attrDescs[2];

        // Pos
        attrDescs[0].binding = 0;
        attrDescs[0].location = 0;
        attrDescs[0].format = VK_FORMAT_R32G32_SFLOAT;
        attrDescs[0].offset = offsetof(vert, pos);

        // Color
        attrDescs[1].binding = 0;
        attrDescs[1].location = 1;
        attrDescs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attrDescs[1].offset = offsetof(vert, color);

        return attrDescs;
    }
};