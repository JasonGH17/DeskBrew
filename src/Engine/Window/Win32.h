#pragma once

#ifndef UNICODE
#define UNICODE
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <windows.h>
#include <stdio.h>

#include "api.h"

class Win32
{
public:
    Win32();
    ~Win32();

    static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    bool init();
    bool kill();
    bool broadcast();

    bool isRunning();
    HWND getHWND();

protected:
    virtual LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) = 0;
    HWND hwnd;
    bool running;
};