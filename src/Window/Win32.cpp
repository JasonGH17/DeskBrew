#include "Win32.h"

Win32::Win32()
{
    running = true;
    hwnd = NULL;
}
Win32::~Win32() {}

LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    Win32 *window = (Win32 *)GetWindowLong(hwnd, GWLP_USERDATA);

    switch (msg)
    {
    case WM_CREATE:
    {
        window->onCreate();
        break;
    }

    case WM_DESTROY:
    {
        window->onDestroy();
        ::PostQuitMessage(0);
        break;
    }

    case WM_SIZE:
        // window->onResize();
        break;

    default:
        return ::DefWindowProc(hwnd, msg, wparam, lparam);
    }
};

bool Win32::init()
{
    WNDCLASSEX wc;

    wc.cbClsExtra = NULL;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = NULL;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = NULL;
    wc.lpszClassName = L"DeskBrew";
    wc.lpszMenuName = L"";
    wc.style = NULL;
    wc.lpfnWndProc = &wndProc;

    if (!::RegisterClassEx(&wc))
    {
        printf("ERROR\n");
        return false;
    }

    hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"DeskBrew", L"DeskBrew", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, this);

    SetWindowLong(hwnd, GWLP_USERDATA, (long)this);

    if (!hwnd)
        return false;

    ::ShowWindow(hwnd, SW_SHOW);
    ::UpdateWindow(hwnd);

    running = true;
    return true;
}

bool Win32::isRunning()
{
    return running;
}

HWND Win32::getHWND()
{
    return hwnd;
}