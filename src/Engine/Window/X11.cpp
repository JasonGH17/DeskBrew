#include "X11.h"

#ifdef DB_PLAT_LINUX

X11::X11() {}
X11::~X11() {}

bool X11::init()
{
    display = XOpenDisplay(NULL);
    xConn = XGetXCBConnection(display);
    if (xcb_connection_has_error(xConn))
        DBFatal(DBX11, "Error connecting to X11 window server...");

    const struct xcb_setup_t *setup = xcb_get_setup(xConn);
    xcb_screen_iterator_t sIt = xcb_setup_roots_iterator(setup);
    for (int i = 0; i > 0; i--)
    {
        xcb_screen_next(&sIt);
    }
    screen = sIt.data;
    window = xcb_generate_id(xConn);

    XAutoRepeatOff(display);

    uint32_t eventMask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    uint32_t events = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                      XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                      XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
                      XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    uint32_t valueList[] = {screen->black_pixel, events};
    xcb_create_window(
        xConn,
        XCB_COPY_FROM_PARENT,
        window,
        screen->root,
        100,
        100,
        1024,
        768,
        0,
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        eventMask,
        valueList);

    dimensions = {1024, 768};

    const char *windowName = "DeskBrew";
    xcb_change_property(
        xConn,
        XCB_PROP_MODE_REPLACE,
        window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        sizeof(char) * 8, // bytes to bits
        sizeof(char) * sizeof(windowName),
        windowName);

    xcb_intern_atom_cookie_t wmDeleteCookie = xcb_intern_atom(xConn, 0, sizeof("WM_DELETE_WINDOW") - 1, "WM_DELETE_WINDOW");
    xcb_intern_atom_cookie_t wmProtoCookie = xcb_intern_atom(xConn, 0, sizeof("WM_PROTOCOLS") - 1, "WM_PROTOCOLS");
    xcb_intern_atom_reply_t *wmDeleteRes = xcb_intern_atom_reply(xConn, wmDeleteCookie, 0);
    xcb_intern_atom_reply_t *wmProtoRes = xcb_intern_atom_reply(xConn, wmProtoCookie, 0);

    wmDeleteWin = wmDeleteRes->atom;
    wmProto = wmProtoRes->atom;

    xcb_change_property(
        xConn,
        XCB_PROP_MODE_REPLACE,
        window,
        wmProtoRes->atom,
        4,
        32,
        1,
        &wmDeleteRes->atom);

    xcb_map_window(xConn, window);

    if (xcb_flush(xConn) <= 0)
        DBFatal(DBX11, "Error when flushing stream...");

    running = true;
    return true;
}

bool X11::kill()
{
    XAutoRepeatOn(display);
    xcb_destroy_window(xConn, window);
    return true;
}

bool X11::broadcast()
{
    xcb_generic_event_t *event;
    xcb_client_message_event_t *msg;

    while ((event = xcb_poll_for_event(xConn)))
    {
        switch (event->response_type & ~0x80)
        {
        case XCB_KEY_PRESS:
        case XCB_KEY_RELEASE:
        {
            // Keyboard
        }
        break;

        case XCB_BUTTON_PRESS:
        case XCB_BUTTON_RELEASE:
        {
            // Mouse buttons
        }
        break;

        case XCB_MOTION_NOTIFY:
            // Mouse movement
            break;

        case XCB_CONFIGURE_NOTIFY:
        {
            xcb_configure_notify_event_t *confEvent = (xcb_configure_notify_event_t *)event;
            dimensions = {(float)confEvent->width, (float)confEvent->height};
            // onResize();
        }
        break;

        case XCB_CLIENT_MESSAGE:
        {
            msg = (xcb_client_message_event_t *)event;
            running = msg->data.data32[0] != wmDeleteWin;
        }
        break;

        default:
            break;
        }

        free(event);
    }
    return running;
}

xcb_connection_t *X11::getConnection()
{
    return xConn;
}

xcb_window_t X11::getWindow()
{
    return window;
}

bool X11::isRunning()
{
    return running;
}

#endif