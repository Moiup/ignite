#include "EngineApp.h"
#include <SDL.h>

struct LaunchedModules {
    MenuModule menu;
    Bistro bistro;
    DefaultInit init_module;
    DefaultRenderingModule default_module;
};

int main(int argc, char* argv[]) {
    EngineApp app{};
    LaunchedModules modules{};
 
    app.launch();

    return 0;
}