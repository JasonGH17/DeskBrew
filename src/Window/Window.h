#pragma once

#include "Win32.h"

#include "stdio.h"

class Window : public Win32
{
public:
    Window();
    ~Window();

    void start();

    virtual void paint() = 0;

    virtual LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) override;
};