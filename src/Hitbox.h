#pragma once


#include "Simulation.h"


#include <ode/ode.h>


typedef struct {
    dBodyID body;
    dGeomID geometry;
    dMass mass;
} Hitbox3d;


void cuboid_hitbox(Simulation* simulation, Hitbox3d* hitbox, const double mass, const double size_x, const double size_y, const double size_z);

void sphere_hitbox(Simulation* simulation, Hitbox3d* hitbox, const double mass, const double radius);

void cylinder_hitbox(Simulation* simulation, Hitbox3d* hitbox, const double mass, const double radius, const double length);
