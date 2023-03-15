#include "EngineApp.h"
#include <SDL.h>

int main(int argc, char* argv[]) {
    EngineApp app{};

    app.init();
    app.start();
    app.update();
    app.close();

    //SDL_Event event;

    //SDL_Window* window{ nullptr };

    //SDL_Surface* surface{ nullptr };
    //if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    //    std::cout << "SDL could not be initialized!" << std::endl;
    //    return -1;
    //}

    //window = SDL_CreateWindow("TEST", 100, 100, 200, 200, SDL_WINDOW_SHOWN);
    //if (window == NULL) {
    //    std::cout << "Failed." << std::endl;
    //}

    //while (true) {
    //    if (SDL_PollEvent(&event)) {
    //        if (event.type == SDL_QUIT) {
    //            break;
    //        }
    //    }
    //}


    //SDL_DestroyWindow(window);
    //SDL_Quit();

    return 0;
}