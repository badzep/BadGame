#pragma once


#include <ode/ode.h>


typedef struct {
    dWorldID world;
    dSpaceID space;
    dJointGroupID contacts;
} Simulation;


void initialize_simulation(Simulation* simulation);

void nearCallback (void *data, dGeomID geometry1, dGeomID geometry2);
