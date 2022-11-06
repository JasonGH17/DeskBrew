#pragma once

#include "Window.h"
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <string.h>

#define VK_DEBUG true

#define EXTCOUNT 2
#define VALCOUNT 1

class VkWindow : public Window {
public:
    VkWindow();
    ~VkWindow();

    bool checkInstanceExtSupport();
    bool checkValidationLayersSupport();

    void createInstance();
    void pickPhysicalDevice();
    void cleanup();

private:
    VkInstance instance;
    const char *extensionNames[EXTCOUNT] = {"VK_KHR_surface", "VK_KHR_win32_surface"};
    const char *validationLayers[VALCOUNT] = {"VK_LAYER_KHRONOS_validation"};

    #ifdef NDEBUG
    const bool enableValidationLayers = false;
    #else
    const bool enableValidationLayers = true;
    #endif
};