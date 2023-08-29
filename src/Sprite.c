#include "Sprite.h"
#include "Simulation.h"
#include <raylib.h>
#include <raymath.h>
#include <ode/ode.h>


void update_model_rotation(Sprite3d* sprite)  {
    const double* rotation = dBodyGetRotation(sprite->hitbox.body);

    Vector3 euler = QuaternionToEuler(QuaternionFromMatrix((Matrix) {(float) rotation[0], (float) rotation[1], (float) rotation[2], (float) rotation[3],
                                                                     (float) rotation[4], (float) rotation[5], (float) rotation[6], (float) rotation[7],
                                                                     (float) rotation[8], (float) rotation[9], (float) rotation[10], (float) rotation[11],
                                                                     (float) rotation[12], (float) rotation[13], (float) rotation[14], (float) rotation[15]}));
    sprite->model.transform = MatrixMultiply(MatrixRotateXYZ(euler), sprite->base_transform);
}

void draw(Sprite3d* sprite) {
    if (!sprite->do_render) {
        return;
    }
    update_model_rotation(sprite);
    const double* const position = dBodyGetPosition(sprite->hitbox.body);
    DrawModel(sprite->model, (Vector3) {(float) position[0], (float) position[1], (float) position[2]}, 1, WHITE);
}

void create_cuboid(Simulation* simulation, Sprite3d* sprite, const float mass, const float size_x, const float size_y, const float size_z) {
    cuboid_hitbox(simulation, &sprite->hitbox, mass, size_x, size_y, size_z);
    sprite->model = LoadModelFromMesh(GenMeshCube(size_x, size_y, size_z));
    sprite->base_transform = MatrixMultiply(MatrixRotateXYZ((Vector3) {0,0,0}), sprite->model.transform);
    sprite->do_render = 1;
}

void create_sphere(Simulation* simulation, Sprite3d* sprite, const float mass, const float radius) {
    sphere_hitbox(simulation, &sprite->hitbox, mass, radius);
    sprite->model = LoadModelFromMesh(GenMeshSphere(radius, 10, 50));
    sprite->base_transform = MatrixMultiply(MatrixRotateXYZ((Vector3) {0,0,0}), sprite->model.transform);
    sprite->do_render = 1;
}
