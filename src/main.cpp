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

    RenderTexture2D target = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    Debug0 debug;
    debug.run(&target, &config);

    UnloadRenderTexture(target);

    CloseWindow();
    return 0;
}
