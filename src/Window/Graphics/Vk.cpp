#include "Vk.h"

VkWindow::VkWindow() {
    createInstance();
    pickPhysicalDevice();
    createLogicalDevice();
    if(init()) {
        start();
    } else {
        printf("[Window] Couldn't initialize WIN32 window instance\n");
    };
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

QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
    QueueFamilyIndices indices;

    uint32_t queueCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilies(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queueFamilies.data());

    int i = 0;
    for (VkQueueFamilyProperties queueFamily : queueFamilies) {
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            indices.graphicsFamily = i;
        }
        if(indices.isComplete())
            break;
        i++;
    }

    return indices;
}

// Implement better system later on (Or make it changeable by the user via UI)
bool isDeviceSuitable(VkPhysicalDevice device) {
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(device, &deviceProperties);
    vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
    if(deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU && deviceFeatures.geometryShader) {
        QueueFamilyIndices indices = findQueueFamilies(device);
        return indices.isComplete();
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

    for(const VkPhysicalDevice& device : devices) {
        if(isDeviceSuitable(device)) {
            physicalDevice = device;
            break;
        }
    }

    if(physicalDevice == VK_NULL_HANDLE) {
        printf("[VK] No suitable Vulkan compliant devices were found on this machine...\n");
        exit(1);
    }
}

void VkWindow::createLogicalDevice() {
    QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
    queueCreateInfo.queueCount = 1;
    const float queuePrio = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePrio;

    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.queueCreateInfoCount = 1;
    createInfo.pEnabledFeatures = &deviceFeatures;
    createInfo.enabledExtensionCount = 0;
    if (enableValidationLayers) {
        createInfo.enabledLayerCount =
        static_cast<uint32_t>(VALCOUNT);
        createInfo.ppEnabledLayerNames = validationLayers;
    } else {
        createInfo.enabledLayerCount = 0;
    }

    if(vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicalDevice) != VK_SUCCESS) {
        printf("[VK] Error when creating Vulkan logical device...\n");
        exit(1);
    }

    vkGetDeviceQueue(logicalDevice, indices.graphicsFamily.value(), 0, &graphicsQueue);
    
    printf("[VK] Created Vulkan logical device\n");
}

void VkWindow::cleanup() {
    vkDestroyDevice(logicalDevice, nullptr);
    vkDestroyInstance(instance, nullptr);
}
