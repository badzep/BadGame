#ifndef BADGAME_SCENE_H
#define BADGAME_SCENE_H

#include "Constants.h"
#include "Sprite.h"


#include <iostream>
#include <raylib.h>
#include <vector>


class Chunk {
public:
    virtual void load() = 0;
    virtual void unload() = 0;
    virtual void run(RenderTexture2D* target);

protected:
    virtual char* id() = 0;
    virtual void tick(double frame_time) = 0;
    virtual void render();
    std::vector<Sprite3d*> sprites;
    Camera3D camera;
};

class MainScreen: public Chunk {
public:
    void load() override;
    void unload() override;
protected:
    char* id() override;
    void tick(double frame_time) override;
};


typedef struct {
    Camera3D* camera;
    std::vector<Sprite3d*> sprites;
} Scene3d;


void render(Scene3d* scene);

#endif //BADGAME_SCENE_H
