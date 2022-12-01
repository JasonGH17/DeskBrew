#include "Window.h"

PlatformWindow::PlatformWindow() {};

PlatformWindow::~PlatformWindow() {};

void PlatformWindow::start() {
    while(isRunning()) {
        printf("Running: %s\n", isRunning()?"true":"false");
        mainLoop();
        if(!broadcast()) break;
        paint();
    }
    kill();
}
