#ifndef BADGAME_SCENE_H
#define BADGAME_SCENE_H


#include "Constants.h"
#include "Sprite.h"
#include "GameObject.h"


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
    virtual bool run(Config* _config);
protected:
    RenderTexture2D target;
    Config* config;
    Camera3D camera;
    Simulation simulation;
    Shader lighting_shader;
    Shader shader;
    float fog_density;
    bool done;
    std::vector<GameObject*> game_objects;
    std::vector<Texture*> loaded_textures;
    std::vector<Sprite3d*> sprites;
    virtual void render();
    virtual std::string id() = 0;
    virtual void load() = 0;
    virtual void unload() = 0;
    virtual void tick(double frame_time) = 0;
};

class MainScreen: public Chunk {
private:
    std::string id() override;
protected:
    void load() override;
    void unload() override;
    void tick(double frame_time) override;
    Light light;
};


class Debug0: public Chunk {
protected:
    void load() override;
    void unload() override;
    void tick(double frame_time) override;
    Light player_light;
    std::string id() override;
//     player;
};

#endif //BADGAME_SCENE_H

