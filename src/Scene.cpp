#include "Scene.h"
#include "Sprite.h"


#include <raylib.h>


void render(Scene3d* scene) {
    ClearBackground(BLACK);
    BeginMode3D(*scene->camera);
    for (Sprite3d* sprite: scene->sprites) {
        draw(sprite);
    }
    EndMode3D();
}
