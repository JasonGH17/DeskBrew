#pragma once

#include "Window.h"
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <string.h>

#define VK_DEBUG true

#define EXTCOUNT 2

class VkWindow : public Window {
public:
    VkWindow();
    ~VkWindow();

    bool checkInstanceExtSupport();
    void createInstance();
    void cleanup();

private:
    VkInstance instance;
    const char *extensionNames[EXTCOUNT] = {"VK_KHR_surface", "VK_KHR_win32_surface"};
};
