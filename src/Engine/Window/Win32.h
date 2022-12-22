#pragma once

#include "api.h"
#ifdef DB_PLAT_WIN64

#ifndef UNICODE
#define UNICODE
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <stdio.h>

#include "Math/Math.h"
#include "Core/Event/EventUser.h"
#include "Core/Logger/Logger.h"

class Win32 : public EventUser
{
public:
    Win32(EventController *e);
    ~Win32();

    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    bool init();
    bool kill();
    bool broadcast();

    bool isRunning();
    HWND getHWND();

    virtual void mainLoop() = 0;
    virtual void paint() = 0;

protected:
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    HWND hwnd;
    bool running;

    Vec2f dimensions;
};

#endif