#include "Hitbox.h"
#include "Simulation.h"


void cuboid_hitbox(Simulation* simulation, Hitbox3d* hitbox, const double mass, const double size_x, const double size_y, const double size_z) {
    hitbox->body = dBodyCreate(simulation->world);
    dMassSetBoxTotal(&hitbox->mass, mass, size_x, size_y, size_z);
    dBodySetMass(hitbox->body, &hitbox->mass);
    hitbox->geometry = dCreateBox(simulation->space, size_x, size_y, size_z);
    dGeomSetBody(hitbox->geometry, hitbox->body);
}

void sphere_hitbox(Simulation* simulation, Hitbox3d* hitbox, const double mass, const double radius) {
    hitbox->body = dBodyCreate(simulation->world);
    dMassSetSphereTotal(&hitbox->mass, mass, radius);
    dBodySetMass(hitbox->body, &hitbox->mass);
    hitbox->geometry = dCreateSphere(simulation->space, radius);
    dGeomSetBody(hitbox->geometry, hitbox->body);
}

void cylinder_hitbox(Simulation *simulation, Hitbox3d *hitbox, const double mass, const double radius, const double length) {
    hitbox->body = dBodyCreate(simulation->world);
    dMassSetCylinderTotal(&hitbox->mass, mass, 1, radius, length);
    dBodySetMass(hitbox->body, &hitbox->mass);
    hitbox->geometry = dCreateCylinder(simulation->space, radius, length);
    dGeomSetBody(hitbox->geometry, hitbox->body);
}
