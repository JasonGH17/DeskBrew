#pragma once

// VERY IMPORTANT DEFINE. DO NOT DELETE.
#define VK_USE_PLATFORM_WIN32_KHR

#include "Window.h"
#include <vulkan/vulkan.h>

#include <stdexcept>
#include <vector>
#include <optional>
#include <string.h>
#include <set>

#define VK_DEBUG true

#define EXTCOUNT 2
#define VALCOUNT 1

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;


    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

class VkWindow : public Window {
public:
    VkWindow();
    ~VkWindow();

    bool checkInstanceExtSupport();
    bool checkValidationLayersSupport();

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createWindowSurface();
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

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice logicalDevice = VK_NULL_HANDLE;
    
    // QUEUES
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    VkSurfaceKHR surface;
};
