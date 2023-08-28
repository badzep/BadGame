#define RLIGHTS_IMPLEMENTATION


#include "rlights.h"
#include "Constants.h"
#include "Sprite.h"
#include "Scene.h"
#include "Simulation.h"
#include "Config.h"


#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <ode/ode.h>
#include <chrono>


int main() {
    Config config;
    default_config(&config);

    dInitODE2(0);

    Simulation simulation;
    initialize_simulation(&simulation);

    if (config.target_fps > 0) {
        SetTargetFPS(config.target_fps);
    }

    static double fps = 0;
    static double frame_time_seconds = 1.0 / 60;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, "Bad Game");
    SetWindowMinSize((int) GAME_WIDTH, (int) GAME_HEIGHT);
    SetExitKey(0);

    RenderTexture2D target = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    Rectangle target_rectangle = (Rectangle) {0.0f, 0.0f, (float) target.texture.width, (float) -target.texture.height};

    Sprite3d floor;
    create_cuboid(&simulation, &floor, 1, 50, 2, 50);
    set_position(&floor.hitbox, 0, -1, 0);
    dBodySetKinematic(floor.hitbox.body);

    Sprite3d player;
    create_cuboid(&simulation, &player, 87.72f, .5, 2, .5);
    player.do_render = false;
    set_position(&player.hitbox, 0, 1, 0);
    dBodySetMaxAngularSpeed(player.hitbox.body, 0);

    Sprite3d ball;
    create_sphere(&simulation, &ball, 100, .5);
    set_position(&ball.hitbox, 1, 1, 1);

    dMatrix3 wall_rotation;
    dRFromEulerAngles(wall_rotation, PI, 0, 0);

    Sprite3d wall1;
    create_cuboid(&simulation, &wall1, 1, 1, 50, 50);
    set_position(&wall1.hitbox, 24, 4, 0);
    dBodySetRotation(wall1.hitbox.body, wall_rotation);
    dBodySetKinematic(wall1.hitbox.body);

    Sprite3d wall2;
    create_cuboid(&simulation, &wall2, 1, 1, 50, 50);
    set_position(&wall2.hitbox, -24, 4, 0);
    dBodySetRotation(wall2.hitbox.body, wall_rotation);
    dBodySetKinematic(wall2.hitbox.body);

    Sprite3d wall3;
    create_cuboid(&simulation, &wall3, 1, 50, 50, 1);
    set_position(&wall3.hitbox, 0, 4, 24);
    dBodySetRotation(wall3.hitbox.body, wall_rotation);
    dBodySetKinematic(wall3.hitbox.body);

    Sprite3d wall4;
    create_cuboid(&simulation, &wall4, 1, 50, 50, 1);
    set_position(&wall4.hitbox, 0, 4, -24);
    dBodySetRotation(wall4.hitbox.body, wall_rotation);
    dBodySetKinematic(wall4.hitbox.body);

    Camera3D camera{Vector3{ 0, 1, 0 },
                    Vector3{ 9999, 1, 0},
                    Vector3{ 0, 1, 0 },
                    config.fov,
                    CAMERA_PERSPECTIVE};

    Scene3d scene = (Scene3d){&camera, (std::vector<Sprite3d*>) {&floor, &player, &ball, &wall1, &wall2, &wall3, &wall4}};

    Texture plasma = LoadTexture("resources/plasma.png");
    Texture brick = LoadTexture("resources/brick.png");

    /* To be unloaded on shutdown */
    std::vector<Texture*> loaded_textures = (std::vector<Texture*>) {&plasma, &brick};

    ball.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    floor.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall1.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall2.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall3.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall4.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;

    HideCursor();
    DisableCursor();

    Shader color_limit = LoadShader(0, "resources/color_limit.fs");
    Shader lighting = LoadShader("resources/lighting.vs",
                                 "resources/fog.fs");
    lighting.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(lighting, "viewPos");

    int fogDensityLoc = GetShaderLocation(lighting, "fog_density");
    SetShaderValue(lighting, fogDensityLoc, &config.fog_density, SHADER_UNIFORM_FLOAT);

    Light light = CreateLight(LIGHT_POINT, (Vector3) {0, 5, 0}, ZERO_ZERO_ZERO, (Color){10, 5, 0}, lighting);

    for (Sprite3d* sprite: scene.sprites) {
        sprite->model.materials[0].shader = lighting;
    }

    while (!WindowShouldClose()) {
        std::chrono::system_clock::time_point frame_start = std::chrono::high_resolution_clock::now();

        const float scale = MIN((float) GetScreenWidth() / GAME_WIDTH, (float) GetScreenHeight() / GAME_HEIGHT);

        Vector2 force = (Vector2) {0, 0};
        if (IsKeyDown(KEY_W)) {
            force.x += 1;
        }
        if (IsKeyDown(KEY_A)) {
            force.y -= 1;
        }
        if (IsKeyDown(KEY_S)) {
            force.x -= 1;
        }
        if (IsKeyDown(KEY_D)) {
            force.y += 1;
        }
        float movement_speed_multiplier = 1;
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            movement_speed_multiplier = 2;
        }

        const float angle = Vector2Angle((Vector2) {0, 1},{camera.target.x, camera.target.z});
        force = Vector2Rotate(force, angle);
        force = Vector2Normalize(force);

        dBodyAddForce(player.hitbox.body, force.x * PLAYER_MOVEMENT_SPEED * movement_speed_multiplier, 0, force.y * PLAYER_MOVEMENT_SPEED * movement_speed_multiplier);
        dSpaceCollide(simulation.space, &simulation, &nearCallback);
        dWorldStep(simulation.world, frame_time_seconds);
        dJointGroupEmpty(simulation.contacts);

        Vector2 mouse_delta = GetMouseDelta();
        UpdateCameraPro(&camera, ZERO_ZERO_ZERO, (Vector3) {mouse_delta.x * config.sensitivity, mouse_delta.y * config.sensitivity, 0}, 0);

        const double *player_hitbox_position = dBodyGetPosition(player.hitbox.body);

        camera.position.x = (float) player_hitbox_position[0];
        camera.position.y = (float) player_hitbox_position[1] + 0.5f;
        camera.position.z = (float) player_hitbox_position[2];

        light.position = camera.position;
        UpdateLightValues(lighting, light);
        float camera_position[3] =  {camera.position.x, camera.position.y, camera.position.z};
        SetShaderValue(lighting, lighting.locs[SHADER_LOC_VECTOR_VIEW], camera_position, SHADER_UNIFORM_VEC3);

        BeginTextureMode(target);
            render(&scene);
        EndTextureMode();

        BeginDrawing();
            BeginShaderMode(color_limit);
                DrawTexturePro(target.texture,
                               target_rectangle,
                               (Rectangle) {((float) GetScreenWidth() - ((float) GAME_WIDTH * scale)) * 0.5f,
                                         ((float) GetScreenHeight() - ((float) GAME_HEIGHT * scale)) * 0.5f,
                                         (float) GAME_WIDTH * scale, (float) GAME_HEIGHT * scale},
                               ZERO_ZERO,
                               0.0f,
                               WHITE);
            EndShaderMode();
            DrawFPS((int) (.9 * GetScreenWidth()), (int) (.02 * GetScreenHeight()));
        EndDrawing();

        std::chrono::system_clock::time_point frame_end = std::chrono::high_resolution_clock::now();
        frame_time_seconds = MAX(((double)(std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_start).count())) / 1e9, 1e-9);
        fps = 1 / frame_time_seconds;
//        printf("FPS: %f, Frame Time: %f sec\n", fps, frame_time_seconds);
    }

    for (Texture* loaded_texture : loaded_textures) {
        UnloadTexture(*loaded_texture);
    }

    UnloadRenderTexture(target);

    CloseWindow();

    return 0;
}
