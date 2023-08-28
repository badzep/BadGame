#include "Scene.h"
#include "Sprite.h"


#include <raylib.h>
#include <chrono>


void render(Scene3d* scene) {
    ClearBackground(BLACK);
    BeginMode3D(*scene->camera);
    for (Sprite3d* sprite: scene->sprites) {
        draw(sprite);
    }
    EndMode3D();
}


void Chunk::render() {
    ClearBackground(BLACK);
    BeginMode3D(this->camera);
        for (Sprite3d* sprite: this->sprites) {
            if (!sprite->do_render) {
                continue;
            }
            update_model_rotation(sprite);
            const double* const position = dBodyGetPosition(sprite->hitbox.body);
            DrawModel(sprite->model, (Vector3) {(float) position[0], (float) position[1], (float) position[2]}, 1, WHITE);
        }
    EndMode3D();
}

void Chunk::run(RenderTexture2D* target) {
    this->load();
    double fps = 0;
    double frame_time = 1.0 / 60;
    while (true) {
        std::chrono::system_clock::time_point frame_start = std::chrono::high_resolution_clock::now();

        if (WindowShouldClose()) {
            break;
        }

        this->tick(frame_time);

        BeginTextureMode(*target);
            this->render();
        EndTextureMode();

        std::chrono::system_clock::time_point frame_end = std::chrono::high_resolution_clock::now();
        frame_time = MAX(((double)(std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_start).count())) / 1e9, 1e-9);
        fps = 1 / frame_time;
    }
    this->unload();
}

void MainScreen::load() {

}

void MainScreen::unload() {

}

char* MainScreen::id() {
    return (char*) "main_screen";
}

void MainScreen::tick(double frame_time) {

}

