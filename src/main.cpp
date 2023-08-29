#include "Constants.h"
#include "Scene.h"
#include "Simulation.h"
#include "Config.h"


#include <raylib.h>


int main() {
    Config config;
    default_config(&config);

    Simulation simulation;
    initialize_simulation(&simulation);

    if (config.target_fps > 0) {
        SetTargetFPS(config.target_fps);
    }

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Bad Game");
    SetWindowMinSize((int) GAME_WIDTH, (int) GAME_HEIGHT);
    SetExitKey(0);

    MainScreen main_screen;
    main_screen.run(&config);

    Debug0 debug;
    debug.run(&config);

    CloseWindow();
    return 0;
}
