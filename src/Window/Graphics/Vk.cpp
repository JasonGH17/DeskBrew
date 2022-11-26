#include "Vk.h"

VkWindow::VkWindow() {
    createInstance();
    if(!init()) {
        fprintf(stderr, "[Window] Couldn't initialize WIN32 window instance\n");
    };
    createWindowSurface();
    pickPhysicalDevice();
    createLogicalDevice();
    createSwapChain();
    createImageViews();
    createGraphicsPipeline();
    createFramebuffers();
    createCommandPool();
    createSyncObjects();
    createVertexBuffer();
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
        fprintf(stdout, "[VK_DEBUG] (%d) Available vulkan instance extensions:\n", extCount);
        for (VkExtensionProperties &ext : extProps)
            fprintf(stdout, "\t- %s\n", ext.extensionName);
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
        fprintf(stdout, "[VK_DEBUG] (%d) Available vulkan validation layers:\n", valCount);
        for (VkLayerProperties &valLayer : valProps)
            fprintf(stdout, "\t- %s\n", valLayer.layerName);
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
        fprintf(stderr, "[VK_DEBUG] Some validation layers aren't supported on this system...\n");
        exit(1);
    }
    if(!checkInstanceExtSupport()) {
        fprintf(stderr, "[VK] Some needed Vulkan instance extensions aren't supported on this system...\n");
        exit(1);
    }

    fprintf(stdout, "[VK] Creating instance...\n");

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
    
    fprintf(stdout, "[VK] Created Vulkan instance\n");
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
        fprintf(stderr, "[VK] No devices support Vulkan on this machine...\n");
        exit(1);
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    if(VK_DEBUG) { 
        fprintf(stdout, "[VK_DEBUG] (%d) Available physical devices found:\n", deviceCount);
        for(const VkPhysicalDevice& device : devices) {
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            fprintf(stdout, "\t- %s\n", deviceProperties.deviceName);
        }

        for(const VkPhysicalDevice& device : devices) {
            if(isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }
    }

    if(physicalDevice == VK_NULL_HANDLE) {
        fprintf(stderr, "[VK] No suitable Vulkan compliant devices were found on this machine...\n");
        exit(1);
    }

    fprintf(stdout, "[VK] Picked graphics device\n");
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
        fprintf(stderr, "[VK] Error when creating Vulkan logical device...\n");
        exit(1);
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    vkGetDeviceQueue(logicalDevice, indices.presentFamily.value(), 0, &presentQueue);
    
    fprintf(stdout, "[VK] Created Vulkan logical device\n");
}

void VkWindow::createWindowSurface() {
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = getHWND();
    createInfo.hinstance = GetModuleHandle(nullptr);

    if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Error when trying to create Vulkan window surface");
        exit(1);
    }

    fprintf(stdout, "[VK] Created window surface\n");
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
        fprintf(stderr, "[VK] Error when creating Vulkan swapchain...\n");
        exit(1);
    }

    uint32_t imgCount;
    vkGetSwapchainImagesKHR(logicalDevice, swap, &imgCount, nullptr);
    swapImages.resize(imgCount);
    vkGetSwapchainImagesKHR(logicalDevice, swap, &imgCount, swapImages.data());

    swapImageFormat = format.format;
    swapExtent = extent;

    fprintf(stdout, "[VK] Created Vulkan swapchain\n");
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
            fprintf(stderr, "[VK] Error when creating Vulkan image view...\n");
            exit(1);
        }
    }

    fprintf(stdout, "[VK] Created Vulkan image views\n");
}

void VkWindow::createGraphicsPipelineCache() {
    VkPipelineCacheCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    createInfo.pInitialData = nullptr;
    createInfo.initialDataSize = (size_t)0;

    if(vkCreatePipelineCache(logicalDevice, &createInfo, nullptr, &pipelineCache)!=VK_SUCCESS){
        fprintf(stderr, "[VK] Failed to create pipeline cache...\n");
        exit(1);
    }
    fprintf(stdout, "[VK] Created graphics pipeline cache successfully\n");
}

void VkWindow::createRenderPass() {
    VkAttachmentDescription colorAttachment{};
    colorAttachment.format = swapImageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef{};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass{};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkSubpassDependency dependency{};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    createInfo.subpassCount = 1;
    createInfo.pSubpasses = &subpass;
    createInfo.attachmentCount = 1;
    createInfo.pAttachments = &colorAttachment;
    createInfo.dependencyCount = 1;
    createInfo.pDependencies = &dependency;

    if(vkCreateRenderPass(logicalDevice, &createInfo, nullptr, &renderPass) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to create graphics pipeline render pass...\n");
        exit(1);
    }
    fprintf(stdout, "[VK] Created graphics pipeline render pass\n");
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

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = 2;
    VkVertexInputBindingDescription desc = vert::getBindingDesc();
    vertexInputInfo.pVertexBindingDescriptions = &desc;
    vertexInputInfo.pVertexAttributeDescriptions = vert::getAttrDesc();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) swapExtent.width;
    viewport.height = (float) swapExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = swapExtent;
    
    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;
    rasterizer.depthBiasConstantFactor = 0.0f;
    rasterizer.depthBiasClamp = 0.0f;
    rasterizer.depthBiasSlopeFactor = 0.0f;
    
    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading = 1.0f;
    multisampling.pSampleMask = nullptr;
    multisampling.alphaToCoverageEnable = VK_FALSE;
    multisampling.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;    colorBlendAttachment.blendEnable = VK_TRUE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    /* std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_LINE_WIDTH
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data(); */

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;
    if (vkCreatePipelineLayout(logicalDevice, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to create pipeline layout...\n");
        exit(1);
    }

    VkGraphicsPipelineCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    createInfo.pInputAssemblyState = &inputAssembly;
    createInfo.pVertexInputState = &vertexInputInfo;
    createInfo.pInputAssemblyState = &inputAssembly;
    createInfo.pViewportState = &viewportState;
    createInfo.pRasterizationState = &rasterizer;
    createInfo.pMultisampleState = &multisampling;
    createInfo.pColorBlendState = &colorBlending;
    createInfo.pDynamicState = nullptr;
    createInfo.layout = pipelineLayout;
    createInfo.pStages = shaderStages;
    createInfo.stageCount = (sizeof(shaderStages)) / (sizeof(VkPipelineShaderStageCreateInfo));

    createRenderPass();
    createInfo.renderPass = renderPass;
    createInfo.subpass = 0;

    createGraphicsPipelineCache();
    if(vkCreateGraphicsPipelines(logicalDevice, pipelineCache, 1, &createInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to create graphics pipeline...\n");
        exit(1);
    }
    fprintf(stdout, "[VK] Created graphics pipeline successfully\n");

    vkDestroyShaderModule(logicalDevice, vertexShader, nullptr);
    vkDestroyShaderModule(logicalDevice, fragmentShader, nullptr);
}

void VkWindow::createFramebuffers() {
    framebuffers.resize(swapImageViews.size());

    for (size_t i = 0; i < swapImageViews.size(); i++) {
        VkImageView attachments[] = {
            swapImageViews[i]
        };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 1;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = swapExtent.width;
        framebufferInfo.height = swapExtent.height;
        framebufferInfo.layers = 1;

        if (vkCreateFramebuffer(logicalDevice, &framebufferInfo, nullptr, &framebuffers[i]) != VK_SUCCESS) {
            fprintf(stderr, "[VK] Failed to create framebuffers...\n");
            exit(1);
        }

        fprintf(stdout, "[VK] Created framebuffer: %d\n", (uint32_t)i);
    }
}

void VkWindow::createCommandPool() {
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.queueFamilyIndex = findQueueFamilies(physicalDevice).graphicsFamily.value();
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    if(vkCreateCommandPool(logicalDevice, &createInfo, nullptr, &commandPool) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to create command pool...\n");
        exit(1);
    }
    fprintf(stdout, "[VK] Created command pool\n");

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    if (vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to create command buffer...\n");
        exit(1);
    }
    fprintf(stdout, "[VK] Created command buffer\n");
}

void VkWindow::createBuffer(VkBufferUsageFlags usage, uint64_t size, VkMemoryPropertyFlags props, VkBuffer &buff, VkDeviceMemory &buffMem) {
    VkBufferCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usage;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateBuffer(logicalDevice, &createInfo, nullptr, &buff) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to create Vulkan vertex buffer...\n");
        exit(1);
    }

    VkMemoryRequirements memReq;
    vkGetBufferMemoryRequirements(logicalDevice, buff, &memReq);
    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memReq.size;
    allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memReq.memoryTypeBits, props);

    if(vkAllocateMemory(logicalDevice, &allocInfo, nullptr, &buffMem) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to allocate memory for the Vulkan vertex buffer...\n");
        exit(1);
    }

    vkBindBufferMemory(logicalDevice, buff, buffMem, 0);
}

void VkWindow::createVertexBuffer() {
    uint64_t buffSize = sizeof(vertices[0]) * vertices.size();
    
    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMem;
    createBuffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, buffSize, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMem);

    fprintf(stdout, "[VK] Created the vertex buffer\n");

    void *data;
    vkMapMemory(logicalDevice, stagingBufferMem, 0, buffSize, 0, &data);
    memcpy(data, vertices.data(), buffSize);
    vkUnmapMemory(logicalDevice, stagingBufferMem);
    
    createBuffer(VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, buffSize, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMem);
    copyBuffer(stagingBuffer, vertexBuffer, buffSize);
    vkDestroyBuffer(logicalDevice, stagingBuffer, nullptr);
    vkFreeMemory(logicalDevice, stagingBufferMem, nullptr);
}

void VkWindow::copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size) {
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;
    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(logicalDevice, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VkBufferCopy copyReg{};
    copyReg.size = size;
    vkCmdCopyBuffer(commandBuffer, src, dst, 1, &copyReg);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);
    vkFreeCommandBuffers(logicalDevice, commandPool, 1, &commandBuffer);
}

void VkWindow::createSyncObjects() {
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (
        vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &imageAvailableSemaphore) != VK_SUCCESS || 
        vkCreateSemaphore(logicalDevice, &semaphoreInfo, nullptr, &renderFinishedSemaphore) != VK_SUCCESS ||
        vkCreateFence(logicalDevice, &fenceInfo, nullptr, &inFlightFence) != VK_SUCCESS
    ) {
        fprintf(stderr, "[VK] Failed to create semaphores...\n");
        exit(1);
    }
}

void VkWindow::cleanup() {
    vkDestroySemaphore(logicalDevice, imageAvailableSemaphore, nullptr);
    vkDestroySemaphore(logicalDevice, renderFinishedSemaphore, nullptr);
    vkDestroyFence(logicalDevice, inFlightFence, nullptr);
    vkDestroyCommandPool(logicalDevice, commandPool, nullptr);
    cleanupSwap();
    vkDestroyBuffer(logicalDevice, vertexBuffer, nullptr);
    vkFreeMemory(logicalDevice, vertexBufferMem, nullptr);
    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void VkWindow::cleanupSwap() {
    for (auto framebuffer : framebuffers) {
        vkDestroyFramebuffer(logicalDevice, framebuffer, nullptr);
    }
    vkDestroyPipeline(logicalDevice, graphicsPipeline, nullptr);
    vkDestroyPipelineCache(logicalDevice, pipelineCache, nullptr);
    vkDestroyPipelineLayout(logicalDevice, pipelineLayout, nullptr);
    vkDestroyRenderPass(logicalDevice, renderPass, nullptr);
    for (auto imageView : swapImageViews) {
        vkDestroyImageView(logicalDevice, imageView, nullptr);
    }
    vkDestroySwapchainKHR(logicalDevice, swap, nullptr);
}

void VkWindow::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to start up the command buffer...\n");
        exit(1);
    }

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = framebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapExtent;
    VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
    VkBuffer vertexBuffs[] = {vertexBuffer};
    uint64_t offsets[] = {0};
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffs, offsets);
    vkCmdDraw(commandBuffer, 3, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);
    if(vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to end command buffer successfully");
        exit(1);
    }
}

void VkWindow::recreateSwapChain() {
    vkDeviceWaitIdle(logicalDevice);
    cleanupSwap();

    createSwapChain();
    createImageViews();
    createRenderPass();
    createGraphicsPipeline();
    createFramebuffers();
}

void VkWindow::paint() {
    std::chrono::milliseconds startTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch());

    if (minimized)
        return;

    vkWaitForFences(logicalDevice, 1, &inFlightFence, VK_TRUE, UINT64_MAX);

    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(logicalDevice, swap, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || resized) {
        resized = false;
        recreateSwapChain();
        return;
    } else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        fprintf(stderr, "[VK] Something went wrong acquiring a new image to render...\nVkResult Code: %d\n", result);
        exit(1);
    }

    vkResetFences(logicalDevice, 1, &inFlightFence);
    vkResetCommandBuffer(commandBuffer, 0);
    recordCommandBuffer(commandBuffer, imageIndex);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFence) != VK_SUCCESS) {
        fprintf(stderr, "[VK] Failed to submit draw command buffer...\n");
        exit(1);
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    VkSwapchainKHR swapChains[] = {swap};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentQueue, &presentInfo);

    dt = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()) - startTime;
}

void VkWindow::onClose() {
    vkDeviceWaitIdle(logicalDevice);
}

void VkWindow::onResize() {
    resized = true;
    minimized = false;
}

void VkWindow::onMinimize() {
    minimized = true;
}

float VkWindow::getDT() {
    return dt.count();
}

void VkWindow::mainLoop() {
    float dt = getDT();

    fprintf(stdout, "DT: %f\tFPS: %d\n", dt,  (int) (1000 / dt));

    vertices = {
        {{.0f, -.5f}, {1.0f - dt, 0.0f, 0.0f}},
        {{.5f, .5f}, {0.0f, 1.0f - dt, 0.0f}},
        {{-.5f, .5f}, {0.0f, 0.0f, 1.0f - dt}}
    };
}