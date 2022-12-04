#include "Window.h"

PlatformWindow::PlatformWindow(){};

PlatformWindow::~PlatformWindow(){};

void PlatformWindow::start()
{
    while (isRunning())
    {
        mainLoop();
        if (!broadcast())
            break;
        paint();
    }
    kill();
}

const char **PlatformWindow::getVkInstanceExtensions(uint32_t *count)
{
    (*count) = EXTCOUNT;
    return vkInstanceExts;
}

Vec2f PlatformWindow::getInnerDimensions()
{
    return dimensions;
};
