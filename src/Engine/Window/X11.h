#pragma once

#include "api.h"
#ifdef DB_PLAT_LINUX

#include <xcb/xcb.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xlib.h>
#include <X11/Xlib-xcb.h>

#include <stdio.h>
#include <stdlib.h>

#include "Math/Math.h"
#include "Core/Event/EventUser.h"
#include "Core/Logger/Logger.h"

class X11 : public EventUser
{
public:
    X11(EventController *e);
    ~X11();

    bool init();
    bool kill();
    bool broadcast();

    bool isRunning();
    xcb_window_t getWindow();
    xcb_connection_t* getConnection();

    virtual void mainLoop() = 0;
    virtual void paint() = 0;

private:
    Display *display;
    xcb_connection_t *xConn;
    xcb_window_t window;
    xcb_screen_t *screen;
    xcb_atom_t wmProto;
    xcb_atom_t wmDeleteWin;
    bool running;

protected:
    Vec2f dimensions;
};

#endif