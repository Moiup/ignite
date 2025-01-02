#include "EngineApp.h"
#include <SDL.h>

struct LaunchedModules {
    MenuModule menu;
    Bistro bistro;
    DefaultModule default_module;
};

int main(int argc, char* argv[]) {
    EngineApp app{};
    LaunchedModules modules{};
 
    app.launch();

    return 0;
}