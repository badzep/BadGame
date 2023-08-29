#include "Sprite.h"
#include "Scene.h"

#include <raylib.h>
#include <chrono>


static int lightsCount = 0;

Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader) {
    Light light = { 0 };
    light.enabled = true;
    light.type = type;
    light.position = position;
    light.target = target;
    light.color = color;
    light.enabledLoc = GetShaderLocation(shader, TextFormat("lights[%i].enabled", lightsCount));
    light.typeLoc = GetShaderLocation(shader, TextFormat("lights[%i].type", lightsCount));
    light.positionLoc = GetShaderLocation(shader, TextFormat("lights[%i].position", lightsCount));
    light.targetLoc = GetShaderLocation(shader, TextFormat("lights[%i].target", lightsCount));
    light.colorLoc = GetShaderLocation(shader, TextFormat("lights[%i].color", lightsCount));
    UpdateLightValues(shader, light);
    lightsCount++;
    return light;
}


void UpdateLightValues(Shader shader, Light light){
    SetShaderValue(shader, light.enabledLoc, &light.enabled, SHADER_UNIFORM_INT);
    SetShaderValue(shader, light.typeLoc, &light.type, SHADER_UNIFORM_INT);
    float position[3] = { light.position.x, light.position.y, light.position.z };
    SetShaderValue(shader, light.positionLoc, position, SHADER_UNIFORM_VEC3);
    float target[3] = { light.target.x, light.target.y, light.target.z };
    SetShaderValue(shader, light.targetLoc, target, SHADER_UNIFORM_VEC3);
    float color[4] = {(float)light.color.r/(float)255, (float)light.color.g/(float)255,
                       (float)light.color.b/(float)255, (float)light.color.a/(float)255};
    SetShaderValue(shader, light.colorLoc, color, SHADER_UNIFORM_VEC4);
}


void Chunk::render(RenderTexture2D* target) {
    BeginTextureMode(*target);
        ClearBackground(BLACK);
        BeginMode3D(this->camera);
            for (Sprite3d* sprite: this->sprites) {
                draw(sprite);
            }
        EndMode3D();
    EndTextureMode();

    const float scale = MIN((float) GetScreenWidth() / GAME_WIDTH, (float) GetScreenHeight() / GAME_HEIGHT);
    BeginDrawing();
        BeginShaderMode(this->shader);
            DrawTexturePro(target->texture,
                           (Rectangle) {0.0f, 0.0f, (float) target->texture.width, (float) -target->texture.height},
                           (Rectangle) {((float) GetScreenWidth() - ((float) GAME_WIDTH * scale)) * 0.5f,
                                        ((float) GetScreenHeight() - ((float) GAME_HEIGHT * scale)) * 0.5f,
                                        (float) GAME_WIDTH * scale, (float) GAME_HEIGHT * scale},
                           ZERO_ZERO,
                           0.0f,
                           WHITE);
        EndShaderMode();
    DrawFPS((int) (.9 * GetScreenWidth()), (int) (.02 * GetScreenHeight()));
    EndDrawing();
}

void Chunk::run(RenderTexture2D* target, Config* _config) {
    this->config = _config;
    this->load();
    double fps = 0;
    double frame_time = 1.0 / 60;
    while (true) {
        std::chrono::system_clock::time_point frame_start = std::chrono::high_resolution_clock::now();

        if (WindowShouldClose()) {
            break;
        }

        this->tick(frame_time);

        this->render(target);

        std::chrono::system_clock::time_point frame_end = std::chrono::high_resolution_clock::now();
        frame_time = MAX(((double)(std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_start).count())) / 1e9, 1e-9);
        fps = 1 / frame_time;
    }
    this->unload();
}


void Debug0::load() {
    HideCursor();
    DisableCursor();

    dInitODE2(0);

    initialize_simulation(&this->simulation);

    this->camera = (Camera3D) {(Vector3) { 0, 1, 0 },
                                   Vector3{ 9999, 1, 0},
                                   Vector3{ 0, 1, 0 },
                                   config->fov,
                                   CAMERA_PERSPECTIVE};
    auto* floor = new Sprite3d();
    create_cuboid(&this->simulation, floor, 1, 50, 2, 50);
    set_position(&floor->hitbox, 0, -1, 0);
    dBodySetKinematic(floor->hitbox.body);

    auto* player = new Sprite3d();
    create_cuboid(&this->simulation, player, 87.72f, .5, 2, .5);
    player->do_render = false;
    set_position(&player->hitbox, 0, 1, 0);
    dBodySetMaxAngularSpeed(player->hitbox.body, 0);

    auto* ball = new Sprite3d();
    create_sphere(&this->simulation, ball, 100, .5);
    set_position(&ball->hitbox, 1, 1, 1);

    dMatrix3 wall_rotation;
    dRFromEulerAngles(wall_rotation, PI, 0, 0);

    auto* wall1 = new Sprite3d();
    create_cuboid(&this->simulation, wall1, 1, 1, 50, 50);
    set_position(&wall1->hitbox, 24, 4, 0);
    dBodySetRotation(wall1->hitbox.body, wall_rotation);
    dBodySetKinematic(wall1->hitbox.body);

    auto* wall2 = new Sprite3d();
    create_cuboid(&this->simulation, wall2, 1, 1, 50, 50);
    set_position(&wall2->hitbox, -24, 4, 0);
    dBodySetRotation(wall2->hitbox.body, wall_rotation);
    dBodySetKinematic(wall2->hitbox.body);

    auto* wall3 = new Sprite3d();
    create_cuboid(&this->simulation, wall3, 1, 50, 50, 1);
    set_position(&wall3->hitbox, 0, 4, 24);
    dBodySetRotation(wall3->hitbox.body, wall_rotation);
    dBodySetKinematic(wall3->hitbox.body);

    auto* wall4 = new Sprite3d();
    create_cuboid(&this->simulation, wall4, 1, 50, 50, 1);
    set_position(&wall4->hitbox, 0, 4, -24);
    dBodySetRotation(wall4->hitbox.body, wall_rotation);
    dBodySetKinematic(wall4->hitbox.body);

    Texture plasma = LoadTexture("resources/plasma.png");
    Texture brick = LoadTexture("resources/brick.png");

    this->loaded_textures = (std::vector<Texture*>) {&plasma, &brick};

    ball->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    floor->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall1->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall2->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall3->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;
    wall4->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = plasma;

    this->sprites = (std::vector<Sprite3d*>) {player, floor, ball, wall1, wall2, wall3, wall4};

    this->shader = LoadShader(nullptr, "resources/color_limit.fs");
    this->lighting_shader = LoadShader("resources/lighting.vs",
                                       "resources/fog.fs");

    this->lighting_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(this->lighting_shader, "viewPos");

    int fogDensityLoc = GetShaderLocation(this->lighting_shader, "fog_density");
    SetShaderValue(this->lighting_shader, fogDensityLoc, &config->fog_density, SHADER_UNIFORM_FLOAT);

    this->player_light = CreateLight(LIGHT_POINT, (Vector3) {0, 5, 0}, ZERO_ZERO_ZERO, (Color) {10, 5, 0}, this->lighting_shader);

    for (Sprite3d* sprite: this->sprites) {
        sprite->model.materials[0].shader = this->lighting_shader;
    }
}

void Debug0::unload() {
    dCloseODE();

    for (Texture* loaded_texture : this->loaded_textures) {
        UnloadTexture(*loaded_texture);
    }
}

std::string Debug0::id() {
    return "debug0";
}

void Debug0::tick(double frame_time) {
    Sprite3d* player = this->sprites[0];

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

    const float angle = Vector2Angle((Vector2) {0, 1},{this->camera.target.x, this->camera.target.z});
    force = Vector2Rotate(force, angle);
    force = Vector2Normalize(force);

    dBodyAddForce(player->hitbox.body, force.x * PLAYER_MOVEMENT_SPEED * movement_speed_multiplier, 0, force.y * PLAYER_MOVEMENT_SPEED * movement_speed_multiplier);
    dSpaceCollide(simulation.space, &this->simulation, &nearCallback);
    dWorldStep(simulation.world, frame_time);
    dJointGroupEmpty(simulation.contacts);

    Vector2 mouse_delta = GetMouseDelta();
    UpdateCameraPro(&this->camera, ZERO_ZERO_ZERO, (Vector3) {mouse_delta.x * config->sensitivity, mouse_delta.y * config->sensitivity, 0}, 0);

    const double *player_hitbox_position = dBodyGetPosition(player->hitbox.body);

    this->camera.position.x = (float) player_hitbox_position[0];
    this->camera.position.y = (float) player_hitbox_position[1] + 0.5f;
    this->camera.position.z = (float) player_hitbox_position[2];

    this->player_light.position = this->camera.position;
    UpdateLightValues(this->lighting_shader, this->player_light);
    float camera_position[3] =  {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(this->lighting_shader, this->lighting_shader.locs[SHADER_LOC_VECTOR_VIEW], camera_position, SHADER_UNIFORM_VEC3);
}
