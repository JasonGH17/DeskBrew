#pragma once

#include "Win32.h"

#include "stdio.h"

class Window : public Win32
{
public:
    Window();
    ~Window();

    void start();

    virtual void mainLoop() = 0;
    virtual void paint() = 0;
    virtual void onClose() = 0;
    virtual void onResize() = 0;
    virtual void onMinimize() = 0;

    virtual LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
};