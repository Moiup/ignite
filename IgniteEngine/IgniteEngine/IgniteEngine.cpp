#include "EngineApp.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    EngineApp app{};

    app.init();
    app.start();
    app.update();
    app.close();

    return 0;
}