#include "Win32.h"

#ifdef DB_PLAT_WIN64

Win32::Win32()
{
    running = true;
}
Win32::~Win32() {}

LRESULT CALLBACK Win32::wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    Win32 *pthis = NULL;

    // Sets up class instance pointer
    if (msg == WM_NCCREATE)
    {
        pthis = (Win32 *)((CREATESTRUCT *)lParam)->lpCreateParams;
        SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pthis);

        pthis->hwnd = hwnd;
    }
    else
    {
        pthis = (Win32 *)GetWindowLongPtr(hwnd, GWLP_USERDATA);
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
    wc.lpfnWndProc = &wndProc;

    if (!::RegisterClassEx(&wc))
    {
        DBFatal(DBWin32, "Error registering window class:\n ERROR CODE:%ld", GetLastError());
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

LRESULT Win32::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
    HWND hwnd = getHWND();

    switch (msg)
    {
    case WM_DESTROY:
        DBInfo(DBWindow, "Destroyed window");
        running = false;
        PostQuitMessage(0);
        return 0;

    case WM_CREATE:
        DBInfo(DBWindow, "Created new window");
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(hwnd, &ps);

        DBInfo(DBWindow, "Updated window");
    }
        return 0;

    case WM_SIZE:
    {
        if (wParam == SIZE_MINIMIZED)
        {
            onMinimize();
        }
        else
        {
            onResize();
        }
    }
        return 0;

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return TRUE;
};

#endif