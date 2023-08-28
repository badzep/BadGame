#ifndef BADGAME_SCENE_H
#define BADGAME_SCENE_H

#include "Constants.h"
#include "Sprite.h"


#include <iostream>
#include <raylib.h>
#include <vector>


typedef struct {
    Camera3D* camera;
    std::vector<Sprite3d*> sprites;
} Scene3d;


void render(Scene3d* scene);

#endif //BADGAME_SCENE_H
