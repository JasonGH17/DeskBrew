#include "Window.h"

PlatformWindow::PlatformWindow() {};

PlatformWindow::~PlatformWindow() {};

void PlatformWindow::start() {
    while(isRunning()) {
        mainLoop();
        if(!broadcast()) break;
        paint();
    }
    kill();
}
