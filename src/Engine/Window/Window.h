#pragma once

#include "Win32.h"
#include "X11.h"

#include "Math/Math.h"

#include <stdio.h>
#include "api.h"

#define EXTCOUNT 2

#ifdef DB_PLAT_WIN64
class DBAPI PlatformWindow : public Win32
#else
class DBAPI PlatformWindow : public X11
#endif
{
public:
    #ifdef DB_PLAT_WIN64
    PlatformWindow(EventController *e) : Win32(e) {};
    #else
    PlatformWindow(EventController *e) : X11(e) {};
    #endif
    ~PlatformWindow();

    void start();

    virtual void mainLoop() = 0;
    virtual void paint() = 0;

    Vec2f getInnerDimensions();

protected:
    const char *vkInstanceExts[EXTCOUNT] = {
        "VK_KHR_surface",
        #ifdef DB_PLAT_WIN64
        "VK_KHR_win32_surface"
        #else
        "VK_KHR_xcb_surface"
        #endif
    };
    const char **getVkInstanceExtensions(uint32_t *count);
};