#ifndef BADGAME_SPRITE_H
#define BADGAME_SPRITE_H


#include "Constants.h"
#include "Hitbox.h"
#include "Simulation.h"


#include <raylib.h>
#include <raymath.h>


typedef struct {
    Hitbox3d hitbox;
    Model model;
    Matrix base_transform;
    unsigned char do_render;
} Sprite3d;


void update_model_rotation(Sprite3d* sprite);

void draw(Sprite3d* sprite);

void create_cuboid(Simulation* simulation, Sprite3d* sprite, const float mass, const float size_x, const float size_y, const float size_z);

void create_sphere(Simulation* simulation, Sprite3d* sprite, const float mass, const float radius);

#endif //BADGAME_SPRITE_H
