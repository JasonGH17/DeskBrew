#include "Vk.h"

VkWindow::VkWindow() {
    createInstance();
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

void VkWindow::createInstance() {
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
    createInfo.enabledLayerCount = 0;

    if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
    {
        throw std::runtime_error("[VK] Error when creating Vulkan instance\n");
    };
    
    printf("[VK] Created Vulkan instance\n");
}

void VkWindow::cleanup() {
    vkDestroyInstance(instance, nullptr);
}
