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

void VkWindow::createInstance() {
    printf("[VK] Creating instance...\n");

    VkApplicationInfo appInfo{};

    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "DeskBrew";
    appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_API_VERSION(0, 0, 0, 1);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    // WINDOWS EXTENSIONS
    const char *extensionNames[2] = {"VK_KHR_surface", "VK_KHR_win32_surface"};

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = 2;
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
