#include "Window.h"

Window::Window() {};

Window::~Window() {};

void Window::start() {
    while(isRunning()) {
        mainLoop();
        broadcast();
        paint();
    }

    onClose();
}

LRESULT Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam){
    HWND hwnd = getHWND();

    switch (msg)
    {
    case WM_DESTROY:
        fprintf(stdout, "[WIN32] Destroyed window\n");
        running = false;
        PostQuitMessage(0);
        return 0;

    case WM_CREATE:
        fprintf(stdout, "[WIN32] Created new window\n");
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hwnd, &ps);

            fprintf(stdout, "[WIN32] Updated window\n");
        }
        return 0;

    case WM_SIZE:
        {
            if(wParam == SIZE_MINIMIZED) {
                onMinimize();
            } else {
                onResize();
            }
        }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return TRUE;
};