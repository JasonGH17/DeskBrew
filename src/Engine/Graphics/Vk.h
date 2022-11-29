#pragma once

#include "Window/Window.h"
#include <vulkan/vulkan.h>

#include <vector>
#include <optional>
#include <string.h>
#include <set>
#include <cstdint>
#include <limits>
#include <algorithm>
#include <chrono>

#include "Utils/Shader.h"
#include "Utils/Vertex.h"
#include "Utils/BufferMemory.h"

#include "api.h"

#define EXTCOUNT 2
#define VALCOUNT 1
#define GPUEXTCOUNT 1

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class DBAPI VkWindow : public Window {
public:
    VkWindow();
    ~VkWindow();

    bool checkInstanceExtSupport();
    bool checkValidationLayersSupport();
    bool checkGPUExtSupport(VkPhysicalDevice device);
    bool isDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    SwapChainSupportDetails getSwapChainSupport(VkPhysicalDevice device);

    VkSurfaceFormatKHR pickSurfaceFormat(std::vector<VkSurfaceFormatKHR> &formats);
    VkPresentModeKHR pickPresentMode(std::vector<VkPresentModeKHR> &modes);
    VkExtent2D pickSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities);

    void createInstance();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createWindowSurface();
    void createSwapChain();
    void createImageViews();
    void createGraphicsPipelineCache();
    void createRenderPass();
    void createGraphicsPipeline();
    void createFramebuffers();
    void createCommandPool();
    void createSyncObjects();
    void cleanup();
    void cleanupSwap();

    void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    void recreateSwapChain();
    
    void createBuffer(VkBufferUsageFlags usage, uint64_t size, VkMemoryPropertyFlags props, VkBuffer &buff, VkDeviceMemory &buffMem);
    void createVertexBuffer();
    void createIndexBuffer();
    void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

    virtual void paint() override;
    virtual void onClose() override;
    virtual void onResize() override;
    virtual void onMinimize() override;
    virtual void mainLoop() override;

    float getDT();

private:
    VkInstance instance;
    const char *extensionNames[EXTCOUNT] = {"VK_KHR_surface", "VK_KHR_win32_surface"};
    const char *validationLayers[VALCOUNT] = {"VK_LAYER_KHRONOS_validation"};
    const char *deviceExts[GPUEXTCOUNT] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

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
    VkSwapchainKHR swap;
    std::vector<VkImage> swapImages;
    VkFormat swapImageFormat;
    VkExtent2D swapExtent;
    std::vector<VkImageView> swapImageViews;

    // GRAPHICS PIPELINE
    VkPipelineLayout pipelineLayout;
    VkPipelineCache pipelineCache;
    VkRenderPass renderPass;
    VkPipeline graphicsPipeline;

    std::vector<VkFramebuffer> framebuffers;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;

    VkSemaphore imageAvailableSemaphore;
    VkSemaphore renderFinishedSemaphore;
    VkFence inFlightFence;

    VkBuffer vertexBuffer;
    VkDeviceMemory vertexBufferMem;
    VkBuffer indexBuffer;
    VkDeviceMemory indexBufferMem;

    bool resized;
    bool minimized;

    std::vector<vert> vertices = {
        {{-.5f, -.5f}, {1.0f, 0.0f, 0.0f}},
        {{.5f, -.5f}, {0.0f, 1.0f, 0.0f}},
        {{.5f, .5f}, {0.0f, 0.0f, 1.0f}},
        {{-.5f, .5f}, {1.0f, 1.0f, 1.0f}}
    };
    uint16_t vertIndices[6] = {0, 1, 2, 2, 3, 0};

    std::chrono::milliseconds dt;
};
