#ifndef BADGAME_GAMEOBJECT_H
#define BADGAME_GAMEOBJECT_H


#include <raylib.h>
#include <string>
#include "Hitbox.h"


class GameObject {
private:
    bool loaded;
    virtual void on_load() = 0;
    virtual void on_unload() = 0;
public:
    unsigned char do_render;
    void load();
    void unload();
    bool is_loaded() const;
    virtual void render(Camera3D* _camera) = 0;
    virtual Vector3 get_position() = 0;
    virtual void set_position(const Vector3 _position) = 0;
};

class Ghost: public GameObject{
protected:
    Vector3 position;
public:
    Vector3 get_position() override;
    void set_position(const Vector3 _position) override;
};

class Tangible: public GameObject{
public:
    Hitbox3d hitbox;
    Vector3 get_position() override;
    void set_position(const Vector3 _position) override;
};

class CameraLink {
protected:
    Vector3 camera_linear_offset = (Vector3) {0, 0, 0};
    Camera* camera;
public:
    void set_camera_linear_offset(const Vector3 linear_offset);
    void set_camera(Camera* _camera);
    virtual void update_camera() = 0;
};

class Billboard: public Ghost {
public:
    void on_load() override;
    void on_unload() override;
    void render(Camera3D* _camera) override;
protected:
    Texture2D texture;
    std::string texture_path;
};

class Text: public Ghost {
protected:
    Font font;
    float font_size;
    Color color;
public:
    std::string text;
    void on_load() override;
    void on_unload() override;
    void render(Camera3D* _camera) override;
};


#endif //BADGAME_GAMEOBJECT_H
