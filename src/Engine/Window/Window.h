#pragma once

#include "Win32.h"
#include "X11.h"

#include <stdio.h>
#include "api.h"

#ifdef DB_PLAT_WIN64
class DBAPI PlatformWindow : public Win32
#else
class DBAPI PlatformWindow : public X11
#endif
{
public:
    PlatformWindow();
    ~PlatformWindow();

    void start();

    virtual void mainLoop() = 0;
    virtual void paint() = 0;
    virtual void onClose() = 0;
    virtual void onResize() = 0;
    virtual void onMinimize() = 0;
};