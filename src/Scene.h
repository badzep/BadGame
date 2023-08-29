#ifndef BADGAME_SCENE_H
#define BADGAME_SCENE_H


#include "Constants.h"
#include "Sprite.h"


#include <iostream>
#include <raylib.h>
#include <vector>


typedef struct {
    int type;
    bool enabled;
    Vector3 position;
    Vector3 target;
    Color color;
    float attenuation;
    int enabledLoc;
    int typeLoc;
    int positionLoc;
    int targetLoc;
    int colorLoc;
    int attenuationLoc;
} Light;

typedef enum {
    LIGHT_DIRECTIONAL = 0,
    LIGHT_POINT
} LightType;


Light CreateLight(int type, Vector3 position, Vector3 target, Color color, Shader shader);
void UpdateLightValues(Shader shader, Light light);


class Chunk {
public:
    virtual void run(RenderTexture2D* target, Config* _config);
protected:
    virtual void load() = 0;
    virtual void unload() = 0;
    virtual std::string id() = 0;
    virtual void tick(double frame_time) = 0;
    virtual void render(RenderTexture2D* target);
    std::vector<Texture*> loaded_textures;
    std::vector<Sprite3d*> sprites;
    Config* config;
    Camera3D camera;
    Shader lighting_shader;
    Shader shader;
};

//class MainScreen: public Chunk {
//public:
//    void load() override;
//    void unload() override;
//protected:
//    std::string id() override;
//    void tick(double frame_time) override;
//};


class Debug0: public Chunk {
protected:
    void load() override;
    void unload() override;
    std::string id() override;
    void tick(double frame_time) override;
    Simulation simulation;
    Light player_light;
};

#endif //BADGAME_SCENE_H

