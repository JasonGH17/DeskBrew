#pragma once

#include <vulkan/vulkan.h>

#include <stdio.h>
#include <stdlib.h>

inline uint32_t findMemoryType(VkPhysicalDevice& device, uint32_t typeBits, VkMemoryPropertyFlags props) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props) {
            return i;
        }
    }

    fprintf(stderr, "[UTIL] Couldn't find suitable memory for your buffer's needs...\n");
    exit(1);
}