#include <iostream>
#include <Engine/Graphics/Vk.h>
#include <Engine/Window/Window.h>

class X11Window : public PlatformWindow
{
public:
    X11Window(){};
    ~X11Window(){};

    virtual void mainLoop() {}
    virtual void paint() {}
    virtual void onClose() {}
    virtual void onResize() {}
    virtual void onMinimize() {}
};

int main()
{
#ifdef DB_PLAT_WIN64
    VkWindow window = VkWindow();
#else
    X11Window window = X11Window();
    window.init();
    window.start();
#endif

    return 0;
}