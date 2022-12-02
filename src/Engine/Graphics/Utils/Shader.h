#pragma once

#include <vulkan/vulkan.h>

#include "Core/Logger/Logger.h"

#include <vector>
#include <fstream>

inline std::vector<char> readFile(const char *filepath)
{
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if (!file.is_open())
        DBFatal(DBUtility, "Unable to open file: %s", filepath);

    size_t fSize = (size_t)file.tellg();
    std::vector<char> buff(fSize);
    file.seekg(0);
    file.read(buff.data(), fSize);

    file.close();

    return buff;
}

inline VkShaderModule loadShader(VkDevice &device, const char *filepath)
{
    std::vector<char> file = readFile(filepath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = file.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(file.data());

    VkShaderModule shader;
    if (vkCreateShaderModule(device, &createInfo, nullptr, &shader) != VK_SUCCESS)
        DBFatal(DBVulkan, "Failed to load shader file: %s", filepath);

    return shader;
}
