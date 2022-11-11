#include "Vk.h"

#include "Utils/Shader.h"

VkWindow::VkWindow() {
    createInstance();
    if(init()) {
        ::SetWindowLong(getHWND(), GWL_STYLE, GetWindowLong(getHWND(), GWL_STYLE)&~WS_SIZEBOX);
    } else {
        printf("[Window] Couldn't initialize WIN32 window instance\n");
    };
    createWindowSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createGraphicsPipeline();
    start();
    cleanup();
}

VkWindow::~VkWindow() {}

bool VkWindow::checkInstanceExtSupport() {
    uint32_t extCount = 0;
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, nullptr);
    std::vector<VkExtensionProperties> extProps(extCount);
    vkEnumerateInstanceExtensionProperties(nullptr, &extCount, extProps.data());

    if(VK_DEBUG) {
        printf("[VK_DEBUG] (%d) Available vulkan instance extensions:\n", extCount);
        for (VkExtensionProperties &ext : extProps)
            printf("\t- %s\n", ext.extensionName);
    }

    if(EXTCOUNT > extCount) {
        return false;
    }

    for(const char* extName : extensionNames) {
        bool found = false;
        for(VkExtensionProperties& ext : extProps) {
            if(strcmp(ext.extensionName, extName) == 0){
                found = true;
                break;
            }
        }
        if(!found)
            return false;
    }

    return true;
}

bool VkWindow::checkValidationLayersSupport() {
    uint32_t valCount = 0;
    vkEnumerateInstanceLayerProperties(&valCount, nullptr);
    std::vector<VkLayerProperties> valProps(valCount);
    vkEnumerateInstanceLayerProperties(&valCount, valProps.data());

    if(VK_DEBUG) {
        printf("[VK_DEBUG] (%d) Available vulkan validation layers:\n", valCount);
        for (VkLayerProperties &valLayer : valProps)
            printf("\t- %s\n", valLayer.layerName);
    }

    if(VALCOUNT > valCount) {
        return false;
    }

    for(const char* valLayerName : validationLayers) {
        bool found = false;
        for(VkLayerProperties& valLayer : valProps) {
            if(strcmp(valLayer.layerName, valLayerName) == 0){
                found = true;
                break;
            }
        }
        if(!found)
            return false;
    }

    return true;
};

void VkWindow::createInstance() {
    if(enableValidationLayers && !checkValidationLayersSupport()) {
        printf("[VK_DEBUG] Some validation layers aren't supported on this system...\n");
        exit(1);
    }
    if(!checkInstanceExtSupport()) {
        printf("[VK] Some needed Vulkan instance extensions aren't supported on this system...\n");
        exit(1);
    }

    printf("[VK] Creating instance...\n");

    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "DeskBrew";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = EXTCOUNT;
    createInfo.ppEnabledExtensionNames = extensionNames;

    if(enableValidationLayers) {
        createInfo.enabledLayerCount = VALCOUNT;
        createInfo.ppEnabledLayerNames = validationLayers;
    } else
        createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("[VK] Error when creating Vulkan instance\n");
    };
    
    printf("[VK] Created Vulkan instance\n");
}

QueueFamilyIndices VkWindow::findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueFamilies.data());

    int i = 0;
    for (VkQueueFamilyProperties queueFamily : queueFamilies) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if(presentSupport)
            indices.presentFamily = i;
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) 
            indices.graphicsFamily = i;
        
        if(indices.isComplete())
            break;
        i++;
    }

    return indices;
}

bool VkWindow::checkGPUExtSupport(VkPhysicalDevice device) {
    uint32_t extCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, nullptr);
    if(GPUEXTCOUNT > extCount)
        return false;
    std::vector<VkExtensionProperties> extProps(extCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extCount, extProps.data());

    for(const char* extName : deviceExts) {
        bool found = false;
        for(VkExtensionProperties& ext : extProps) {
            if(strcmp(ext.extensionName, extName) == 0){
                found = true;
                break;
            }
        }
        if(!found)
            return false;
    }

    return true;
}

SwapChainSupportDetails VkWindow::getSwapChainSupport(VkPhysicalDevice device) {
    SwapChainSupportDetails details{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
    
    uint32_t sFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &sFormatCount, nullptr);
    if(sFormatCount > 0) {
        details.formats.resize(sFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &sFormatCount, details.formats.data());
    }

    uint32_t sModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &sModesCount, nullptr);
    if(sModesCount > 0) {
        details.presentModes.resize(sModesCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &sModesCount, details.presentModes.data());
    }

    return details;
}

// Implement better system later on (Or make it changeable by the user via UI)
bool VkWindow::isDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader) {
        QueueFamilyIndices indices = findQueueFamilies(device);
        SwapChainSupportDetails swapDetails = getSwapChainSupport(device);
        return 
            indices.isComplete() && 
            checkGPUExtSupport(device) && 
            !swapDetails.formats.empty() && 
            !swapDetails.presentModes.empty();
    };

    return false;
}

void VkWindow::pickPhysicalDevice() {
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    
    if(deviceCount == 0) {
        printf("[VK] No devices support Vulkan on this machine...\n");
        exit(1);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    if(VK_DEBUG) { 
        printf("[VK_DEBUG] (%d) Available physical devices found:\n", deviceCount);
        for(const VkPhysicalDevice& device : devices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            printf("\t- %s\n", deviceProperties.deviceName);
        }

        for(const VkPhysicalDevice& device : devices) {
            if(isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }
    }

    if(physicalDevice == VK_NULL_HANDLE) {
        printf("[VK] No suitable Vulkan compliant devices were found on this machine...\n");
        exit(1);
    }

    printf("[VK] Picked graphics device\n");
}

void VkWindow::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfoArr;
    std::set<uint32_t> uniqueQueues = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    const float queuePrio = 1.0f;
    for(uint32_t queue : uniqueQueues) {
        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queue;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePrio;

        queueCreateInfoArr.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures deviceFeatures{};
    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = queueCreateInfoArr.data();
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoArr.size());
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = GPUEXTCOUNT;
    createInfo.ppEnabledExtensionNames = deviceExts;

    if (enableValidationLayers){
        createInfo.enabledLayerCount = static_cast<uint32_t>(VALCOUNT);
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        printf("[VK] Error when creating Vulkan logical device...\n");
        exit(1);
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
    
    printf("[VK] Created Vulkan logical device\n");
}

void VkWindow::createWindowSurface() {
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = getHWND();
    createInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        printf("[VK] Error when trying to create Vulkan window surface");
        exit(1);
    }

    printf("[VK] Created window surface\n");
}

VkSurfaceFormatKHR VkWindow::pickSurfaceFormat(std::vector<VkSurfaceFormatKHR>& formats) {
    for(VkSurfaceFormatKHR format : formats) {
        if(format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            return format;
    }

    return formats[0];
}

VkPresentModeKHR VkWindow::pickPresentMode(std::vector<VkPresentModeKHR>& modes) {
    #ifdef PERFORMANCE
    for(VkPresentModeKHR mode : modes) {
        if(mode == VK_PRESENT_MODE_MAILBOX_KHR)
            return mode;
    }
    #endif

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VkWindow::pickSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities) {
    if(capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) 
        return capabilities.currentExtent;

    RECT rc;
    GetClientRect(getHWND(), &rc);
    int width, height;
    width = rc.bottom - rc.top;
    height = rc.right - rc.left;

    VkExtent2D extent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

    return extent;
}

void VkWindow::createSwapChain() {
    SwapChainSupportDetails scDetails = getSwapChainSupport(physicalDevice);

    VkSurfaceFormatKHR format = pickSurfaceFormat(scDetails.formats);
    VkPresentModeKHR pMode = pickPresentMode(scDetails.presentModes);
    VkExtent2D extent = pickSwapExtent(scDetails.capabilities);

    uint32_t imageCount = scDetails.capabilities.minImageCount + 1;
    if (scDetails.capabilities.maxImageCount > 0 && imageCount > scDetails.capabilities.maxImageCount) {
        imageCount = scDetails.capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.imageExtent = extent;
    createInfo.presentMode = pMode;
    createInfo.imageColorSpace = format.colorSpace;
    createInfo.imageFormat = format.format;
    createInfo.minImageCount = imageCount;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    createInfo.imageArrayLayers = 1; // Stays 1 unless making stereoscopic 3D apps

    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);
    uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
    if (indices.graphicsFamily != indices.presentFamily) {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    } else createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    
    createInfo.preTransform = scDetails.capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE; // WILL ADD A HANDLER LATER 

    if(vkCreateSwapchainKHR(logicalDevice, &createInfo, nullptr, &swap) != VK_SUCCESS) {
        printf("[VK] Error when creating Vulkan swapchain...\n");
        exit(1);
    }

    uint32_t imgCount;
    vkGetSwapchainImagesKHR(logicalDevice, swap, &imgCount, nullptr);
    swapImages.resize(imgCount);
    vkGetSwapchainImagesKHR(logicalDevice, swap, &imgCount, swapImages.data());

    swapImageFormat = format.format;
    swapExtent = extent;

    printf("[VK] Created Vulkan swapchain\n");
}

void VkWindow::createImageViews() {
    swapImageViews.resize(swapImages.size());

    for (size_t i = 0; i < swapImages.size(); i++) {
        VkImageViewCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        createInfo.image = swapImages[i];
        createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        createInfo.format = swapImageFormat;

        createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
        createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

        createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;

        if (vkCreateImageView(logicalDevice, &createInfo, nullptr, &swapImageViews[i]) != VK_SUCCESS) {
            printf("[VK] Error when creating Vulkan image view...\n");
            exit(1);
        }
    }

    printf("[VK] Created Vulkan image views\n");
}

void VkWindow::createGraphicsPipeline() {
    VkShaderModule vertexShader = loadShader(logicalDevice, "./vertex.spv");
    fprintf(stdout, "[VK] Successfully loaded in the vertex shader\n");
    VkShaderModule fragmentShader = loadShader(logicalDevice, "./fragment.spv");
    fprintf(stdout, "[VK] Successfully loaded in the fragment shader\n");

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertexShader;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragmentShader;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};
    (void)shaderStages;

    vkDestroyShaderModule(logicalDevice, vertexShader, nullptr);
    vkDestroyShaderModule(logicalDevice, fragmentShader, nullptr);
}

void VkWindow::cleanup() {
    for (auto imageView : swapImageViews) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }

    vkDestroySwapchainKHR(logicalDevice, swap, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}
