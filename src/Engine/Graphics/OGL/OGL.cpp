#include "OGL.h"

#include <Core/Logger/Logger.h>
#include <Core/Event/WindowEvent.h>

#include <wingdi.h>

OGL::OGL(EventController *e) : PlatformWindow(e)
{
    if (!init())
    {
        DBError(DBWindow, "Couldn't initialize window instance");
    };
    hdc = GetDC(getHWND());

    setupPF();
    createCtx();
    start();
}
OGL::~OGL(){};

bool OGL::setupPF()
{
    PIXELFORMATDESCRIPTOR pfd;
    int pixelformat;

    pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL |
                  PFD_DOUBLEBUFFER;
    pfd.dwLayerMask = PFD_MAIN_PLANE;
    pfd.iPixelType = PFD_TYPE_COLORINDEX;
    pfd.cColorBits = 8;
    pfd.cDepthBits = 16;
    pfd.cAccumBits = 0;
    pfd.cStencilBits = 0;

    pixelformat = ChoosePixelFormat(hdc, &pfd);

    if ((pixelformat = ChoosePixelFormat(hdc, &pfd)) == 0)
    {
        MessageBox(NULL, L"ChoosePixelFormat failed", L"Error", MB_OK);
        return false;
    }

    if (SetPixelFormat(hdc, pixelformat, &pfd) == FALSE)
    {
        MessageBox(NULL, L"SetPixelFormat failed", L"Error", MB_OK);
        return false;
    }

    return true;
}

void OGL::createCtx()
{
#ifdef DB_PLAT_WIN64
    ctx = wglCreateContext(hdc);
    wglMakeCurrent(hdc, ctx);

    GLfloat aspect;

    glClearIndex((GLfloat)0);
    glClearDepth(1.0);

    glEnable(GL_DEPTH_TEST);

    Vec2f dim = getInnerDimensions();

    glMatrixMode(GL_PROJECTION);
    aspect = (GLfloat)dim.x / dim.y;
    gluPerspective(45.0, aspect, 3.0, 7.0);
    glMatrixMode(GL_MODELVIEW);

    events()->addEventListener<WCloseEvent>([this](WCloseEvent *e)
                                            {(void) e; if(ctx) wglDeleteContext(ctx); if(hdc) ReleaseDC(getHWND(), hdc); });
#else
    DBFatal(DBOpenGL, "OpenGL only implemented on Windows...");
#endif
}

void OGL::mainLoop() {}

void OGL::paint()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    SwapBuffers(hdc);
}