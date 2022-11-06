#include "Win32.h"

#include "Window.h"

Win32::Win32() {
    running = true;
}
Win32::~Win32() {}

LRESULT CALLBACK Win32::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    Window *pthis = NULL;

    // Sets up class instance pointer
    if (msg == WM_NCCREATE) 
    {
        pthis = (Window *)((CREATESTRUCT *)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pthis);

        pthis->hwnd = hwnd;
    } 
    else 
    {
        pthis = (Window *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    }

    if (pthis) 
    {
        return pthis->handleMessage(msg, wParam, lParam);
    }
    else
    {
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}

bool Win32::init()
{
    WNDCLASSEX wc;

    wc.cbClsExtra = 0;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.cbWndExtra = 0;
    wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
    wc.hInstance = NULL;
    wc.lpszClassName = L"DeskBrew";
    wc.lpszMenuName = L"";
    wc.style = 0;
    wc.lpfnWndProc = Window::wndProc;

    if (!::RegisterClassEx(&wc))
    {
        printf("[WIN32] Error registering window class:\n ERROR CODE:%ld\n", GetLastError());
        return false;
    }

    hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, L"DeskBrew", L"DeskBrew", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, this);

    if (!hwnd)
        return false;

    ::ShowWindow(hwnd, SW_SHOW);
    ::UpdateWindow(hwnd);

    running = true;
    return true;
}

bool Win32::kill()
{
    if (!::DestroyWindow(hwnd))
        return false;

    running = false;
    return true;
}

bool Win32::broadcast()
{
    MSG msg;

    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    Sleep(0);

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