#include "Chunk.h"
#include "GameObject.h"


#include <raylib.h>
#include <chrono>
#include <raymath.h>


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

void Chunk::render() {
    BeginTextureMode(this->target);
        ClearBackground(BLACK);
        BeginMode3D(this->camera);
            for (GameObject* game_object: this->game_objects) {
                if (!game_object->is_loaded()) {
                    continue;
                }
                game_object->render();
            }
        EndMode3D();
    EndTextureMode();

    const float scale = MIN((float) GetScreenWidth() / GAME_WIDTH, (float) GetScreenHeight() / GAME_HEIGHT);
    BeginDrawing();
        ClearBackground(BLACK);
        BeginShaderMode(this->shader);
            DrawTexturePro(this->target.texture,
                           (Rectangle) {0.0f, 0.0f, (float) this->target.texture.width, (float) -this->target.texture.height},
                           (Rectangle) {((float) GetScreenWidth() - ((float) GAME_WIDTH * scale)) * 0.5f,
                                        ((float) GetScreenHeight() - ((float) GAME_HEIGHT * scale)) * 0.5f,
                                        (float) GAME_WIDTH * scale, (float) GAME_HEIGHT * scale},
                           ZERO_ZERO,
                           0.0f,
                           BLACK);
        EndShaderMode();
        if (this->config->show_fps) {
            DrawFPS((int) (.9 * GetScreenWidth()), (int) (.02 * GetScreenHeight()));
        }

    EndDrawing();
}

bool Chunk::run(Config* _config) {
    bool shutdown = false;
    this->config = _config;

    printf("Loading chunk %s\n", this->id().c_str());
    this->load();
    printf("Loaded chunk %s\n", this->id().c_str());

    double frame_time = 1.0 / 60;
    while (true) {
        std::chrono::system_clock::time_point frame_start = std::chrono::high_resolution_clock::now();

        this->tick(frame_time);

        this->render();

        if (WindowShouldClose()) {
            printf("Shutdown Starting from chunk %s\n", this->id().c_str());
            shutdown = true;
            break;
        }

        if (this->done) {
            break;
        }

        std::chrono::system_clock::time_point frame_end = std::chrono::high_resolution_clock::now();
        frame_time = MAX(((double)(std::chrono::duration_cast<std::chrono::nanoseconds>(frame_end - frame_start).count())) / 1e9, 1e-9);
    }

    printf("Unloading chunk %s\n", this->id().c_str());
    this->unload();
    printf("Unloaded chunk %s\n", this->id().c_str());

    return shutdown;
}


void MainScreen::load() {
    this->done = false;

    HideCursor();
    DisableCursor();

    this->target = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);

    this->camera = (Camera3D) {{0, 0, 0},
                               {9999, 1, 0},
                               {0, 1, 0},
                               config->fov,
                               CAMERA_PERSPECTIVE};


    this->shader = LoadShader(nullptr, "resources/color_limit.fs");

    this->lighting_shader = LoadShader("resources/lighting.vs",
                                       "resources/fog.fs");

    this->lighting_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(this->lighting_shader, "viewPos");

    int fogDensityLoc = GetShaderLocation(this->lighting_shader, "fog_density");
    this->fog_density = 0.09f;
    SetShaderValue(this->lighting_shader, fogDensityLoc, &this->fog_density, SHADER_UNIFORM_FLOAT);

    this->light = CreateLight(LIGHT_POINT, Vector3{0, 2, 0}, ZERO_ZERO_ZERO, Color{100, 50, 0}, this->lighting_shader);

    MainScreenWall* main_screen_wall = new MainScreenWall();
    main_screen_wall->factory();
    MainScreenText* main_screen_text = new MainScreenText();
    main_screen_text->factory();

    this->game_objects = (std::vector<GameObject*>) {main_screen_wall, main_screen_text};

    for (GameObject* game_object: this->game_objects) {
        if (game_object->is_loaded()) {
            continue;
        }
        game_object->load();
    }

    main_screen_wall->apply_shader(&this->lighting_shader);

    UpdateLightValues(this->lighting_shader, this->light);
    float camera_position[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(this->lighting_shader, this->lighting_shader.locs[SHADER_LOC_VECTOR_VIEW], camera_position, SHADER_UNIFORM_VEC3);
}

void MainScreen::unload() {
    for (GameObject* game_object: this->game_objects) {
        if (!game_object->is_loaded()) {
            continue;
        }
        game_object->unload();
    }
    UnloadShader(this->lighting_shader);
    UnloadShader(this->shader);
    UnloadRenderTexture(this->target);
}

void MainScreen::tick(double frame_time) {
    if (IsKeyDown(KEY_SPACE) | IsKeyDown(KEY_SPACE)) {
        this->done = true;
    }
}

std::string MainScreen::id() {
    return "main_screen";
}

void Debug0::load() {
    this->done = false;
    dInitODE2(0);

    HideCursor();
    DisableCursor();

    initialize_simulation(&this->simulation);

    this->target = LoadRenderTexture(RENDER_WIDTH, RENDER_HEIGHT);

    this->camera = (Camera3D) {Vector3{0, 1, 0},
                               Vector3{9999, 1, 0},
                               Vector3{0, 1, 0},
                               config->fov,
                               CAMERA_PERSPECTIVE};


    this->player.factory(&this->simulation, &this->camera);

    this->brick = LoadTexture("resources/brick.png");
    this->cement = LoadTexture("resources/cement.png");
    this->padded = LoadTexture("resources/padded_walls.png");

//    this->block.custom(&this->simulation, Vector3{0.0f, 5.0f, 0.0f}, Vector3{0, 0, 0}, Vector3{2, 2, 2}, 100, &brick);
    this->block.custom(&this->simulation, Vector3{0.0f, 3.0f, 5.0f}, Vector3{0, 0, 0}, Vector3{1, 2, 2}, 100, &brick);

    Ball* ball = new Ball();
    ball->custom(&this->simulation, Vector3{1, 1, 1}, Vector3{0, 0, 0}, .5, 100, &brick);

    Rat* rat = new Rat();
    rat->custom(&this->simulation, Vector3{2, 5, 0}, Vector3{0, 0, 0});

    Structure* floor = new Structure();
    floor->custom(&this->simulation, Vector3{0, -2, 0}, Vector3{0, 0, 0}, {50, 2, 50}, &brick);

    Structure* wall1 = new Structure();
    wall1->custom(&this->simulation, Vector3{24, 4, 0}, Vector3{0, 0, 0}, Vector3{1, 50, 50}, &padded);

    Structure* wall2 = new Structure();
    wall2->custom(&this->simulation, Vector3{-24, 4, 0}, Vector3{0, 0, 0}, Vector3{1, 50, 50}, &padded);

    Structure* wall3 = new Structure();
    wall3->custom(&this->simulation, Vector3{0, 4, 24}, Vector3{0, 0, 0}, Vector3{50, 50, 1}, &padded);

    Structure* wall4 = new Structure();
    wall4->custom(&this->simulation, Vector3{0, 4, -24}, Vector3{0, 0, 0}, Vector3{ 50, 50, 1}, &padded);


    this->game_objects = (std::vector<GameObject*>) {&player, &block, ball, floor, wall1, wall2, wall3, wall4, rat};

    this->shader = LoadShader(nullptr, "resources/color_limit.fs");
    this->lighting_shader = LoadShader("resources/lighting.vs",
                                       "resources/fog.fs");

    this->lighting_shader.locs[SHADER_LOC_VECTOR_VIEW] = GetShaderLocation(this->lighting_shader, "viewPos");

    int fogDensityLoc = GetShaderLocation(this->lighting_shader, "fog_density");
    this->fog_density = 0.002f;
    SetShaderValue(this->lighting_shader, fogDensityLoc, &this->fog_density, SHADER_UNIFORM_FLOAT);

//    this->player_light = CreateLight(LIGHT_POINT, (Vector3) {0, 5, 0}, ZERO_ZERO_ZERO, (Color) {10, 10, 10}, this->lighting_shader);
    this->player_light = CreateLight(LIGHT_POINT, (Vector3) {0, 5, 0}, ZERO_ZERO_ZERO, (Color) {2, 1, 0}, this->lighting_shader);

    int ambientLoc = GetShaderLocation(this->lighting_shader, "ambient");
    float ambient_light[4]{.06,.03,.0,1};

    SetShaderValue(this->lighting_shader, ambientLoc, ambient_light, SHADER_UNIFORM_VEC4);

    for (GameObject* game_object: this->game_objects) {
        game_object->load();
    }

    this->block.apply_shader(&this->lighting_shader);
    ball->apply_shader(&this->lighting_shader);
    floor->apply_shader(&this->lighting_shader);
    wall1->apply_shader(&this->lighting_shader);
    wall2->apply_shader(&this->lighting_shader);
    wall3->apply_shader(&this->lighting_shader);
    wall4->apply_shader(&this->lighting_shader);


    UpdateLightValues(this->lighting_shader, this->player_light);
    float camera_position[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(this->lighting_shader, this->lighting_shader.locs[SHADER_LOC_VECTOR_VIEW], camera_position, SHADER_UNIFORM_VEC3);
}

void Debug0::unload() {
    dCloseODE();
    UnloadShader(this->lighting_shader);
    UnloadShader(this->shader);
    UnloadRenderTexture(this->target);
    UnloadTexture(this->brick);
    UnloadTexture(this->padded);
    UnloadTexture(this->cement);
}

void Debug0::tick(double frame_time) {
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
    force = Vector2Rotate(force, -angle + PI / 2);
//    force = Vector2Rotate(force, angle);
    force = Vector2Normalize(force);

    this->player.apply_force({force.x * PLAYER_MOVEMENT_SPEED * movement_speed_multiplier, 0, force.y * PLAYER_MOVEMENT_SPEED * movement_speed_multiplier});

    dSpaceCollide(simulation.space, &this->simulation, &nearCallback);
    dWorldStep(simulation.world, frame_time);
    dJointGroupEmpty(simulation.contacts);

    Vector2 mouse_delta = GetMouseDelta();
    UpdateCameraPro(&this->camera, ZERO_ZERO_ZERO, (Vector3) {mouse_delta.x * config->sensitivity, mouse_delta.y * config->sensitivity, 0}, 0);

//    printf("%f, %f, %f\n", this->player.get_position().x, this->player.get_position().y, this->player.get_position().z);
    this->player.update_camera();

    this->player_light.position = this->camera.position;

    UpdateLightValues(this->lighting_shader, this->player_light);
    float camera_position[3] = {camera.position.x, camera.position.y, camera.position.z};
    SetShaderValue(this->lighting_shader, this->lighting_shader.locs[SHADER_LOC_VECTOR_VIEW], camera_position, SHADER_UNIFORM_VEC3);
}

std::string Debug0::id() {
    return "debug0";
}
