#include "EngineApp.h"
#include <SDL.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

int main(int argc, char* argv[]) {
    EngineApp app{};

    app.init();
    app.start();
    app.update();
    app.close();

    return 0;
}