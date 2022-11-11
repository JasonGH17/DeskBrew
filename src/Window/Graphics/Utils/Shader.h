#pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <fstream>

std::vector<char> readFile(const char* filepath) {
    std::ifstream file(filepath, std::ios::ate | std::ios::binary);

    if(!file.is_open()) {
        printf("[UTIL] Unable to open file: %s\n", filepath);
        exit(1);
    }

    size_t fSize = (size_t) file.tellg();
    std::vector<char> buff(fSize);
    file.seekg(0);
    file.read(buff.data(), fSize);

    file.close();

    return buff;
}

VkShaderModule loadShader(VkDevice& device, const char* filepath) {
    std::vector<char> file = readFile(filepath);

    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = file.size();
    createInfo.pCode = reinterpret_cast<const uint32_t *>(file.data());

    VkShaderModule shader;
    if(vkCreateShaderModule(device, &createInfo, nullptr, &shader) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to load shader file: %s\n", filepath);
        exit(1);
    }

    return shader;
}
