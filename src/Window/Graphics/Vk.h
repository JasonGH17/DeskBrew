#pragma once

#include "Window.h"
#include <vulkan/vulkan.h>
#include <stdexcept>

class VkWindow : public Window {
public:
    VkWindow();
    ~VkWindow();

    void createInstance();
    void cleanup();

private:
    VkInstance instance;
};
