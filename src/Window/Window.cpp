#include "Window.h"

Window::Window()
{
    init();
};

Window::~Window() {};

void Window::start() {
    while(isRunning()) {
        broadcast();
    }
}

LRESULT Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam){
    HWND hwnd = getHWND();

    switch (msg)
    {
    case WM_DESTROY:
        printf("Destroyed window (WIN32)\n");
        PostQuitMessage(0);
        return 0;

    case WM_CREATE:
        printf("Created new window (WIN32)\n");
        return 0;

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));
            EndPaint(hwnd, &ps);

            printf("Updated window (WIN32)\n");
        }
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return TRUE;
};
