#pragma once

#include "api.h"

#ifdef DB_PLAT_WIN64

#include <Window/Window.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <gl/glaux.h>

#ifdef DB_PLAT_WIN64
#include <windows.h>
#endif

class OGL : public PlatformWindow
{
public:
    OGL(EventController *e);
    ~OGL();

    bool setupPF();
    void createCtx();

    virtual void mainLoop() override;
    virtual void paint() override;

private:
    HGLRC ctx;
    HDC hdc;
};

#endif
