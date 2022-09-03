#pragma once

#include <Windows.h>

#include <stdio.h>
#include <functional>

typedef std::function<LRESULT (HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)> wndFunc;

class Win32
{
public:
    Win32();
    ~Win32();

    bool init();
    bool kill();
    bool broadcast();

    bool isRunning();
    HWND getHWND();

    virtual void onCreate();
    virtual void onDestroy();
    // virtual void onResize();

private:
    HWND hwnd;
    bool running;
};