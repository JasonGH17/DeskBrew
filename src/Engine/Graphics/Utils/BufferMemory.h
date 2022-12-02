#pragma once

#include <vulkan/vulkan.h>

#include "Core/Logger/Logger.h"

#include <stdio.h>
#include <stdlib.h>

inline uint32_t findMemoryType(VkPhysicalDevice &device, uint32_t typeBits, VkMemoryPropertyFlags props)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if ((typeBits & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & props) == props)
        {
            return i;
        }
    }

    DBFatal(DBUtility, "Couldn't find suitable memory for your buffer's needs...");
    return 0;
}