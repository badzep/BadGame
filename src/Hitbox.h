#ifndef BADGAME_HITBOX_H
#define BADGAME_HITBOX_H


#include "Simulation.h"


#include <ode/ode.h>


typedef struct {
    dBodyID body;
    dGeomID geometry;
    dMass mass;
} Hitbox3d;


void set_position(Hitbox3d* hitbox, double x, double y, double z);

void cuboid_hitbox(Simulation* simulation, Hitbox3d* hitbox, const double mass, const double size_x, const double size_y, const double size_z);

void sphere_hitbox(Simulation* simulation, Hitbox3d* hitbox, const double mass, const double radius);

#endif //BADGAME_HITBOX_H
