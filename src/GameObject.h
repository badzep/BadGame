#ifndef BADGAME_GAMEOBJECT_H
#define BADGAME_GAMEOBJECT_H


#include "Hitbox.h"


#include <raylib.h>
#include <string>


class GameObject {
private:
    bool visible = true;
    bool loaded;
    virtual void load() = 0;
    virtual void unload() = 0;
protected:
    virtual void do_render(Camera3D* _camera) = 0;
public:
    virtual void render(Camera3D* _camera);
    void do_load();
    void do_unload();
    bool is_loaded() const;
    virtual Vector3 get_position() = 0;
    virtual void set_position(const Vector3 _position) = 0;
};

// does not automatically apply to model
class SimpleTexture: public virtual GameObject {
protected:
    Texture texture;
    std::string texture_path;

    void load() override;
    void unload() override;
};

class Ghost: public virtual GameObject {
protected:
    Vector3 position;
    Matrix rotation_matrix;
public:
    Vector3 get_position() override;
    void set_position(const Vector3 _position) override;
};

class Tangible: public virtual GameObject{
protected:
    Hitbox3d hitbox;
public:
    Vector3 get_position() override;
    void set_position(const Vector3 _position) override;
};

class Sprite: public virtual GameObject {
protected:
    Model model;
    Matrix base_rotation_matrix;
    virtual void update_model_rotation() = 0;
    virtual Matrix get_rotation_matrix() = 0;
    virtual void set_rotation_matrix(Vector3 euler_rotation) = 0;
    void do_render(Camera3D* _camera) override;
};

class TexturedSprite: public virtual GameObject {
public:
    void apply_shader(Shader* shader);
protected:
    Texture texture;
    std::string texture_path;

    void load() override;
    void unload() override;
};


class CameraLink: public virtual GameObject {
protected:
    Vector3 camera_linear_offset = (Vector3) {0, 0, 0};
    Camera* camera;
public:
    void set_camera_linear_offset(const Vector3 linear_offset);
    void set_camera(Camera* _camera);
    void update_camera();
};

class GhostSprite: public Ghost, public Sprite {
protected:
    void update_model_rotation() override;
    Matrix get_rotation_matrix() override;
    void set_rotation_matrix(Vector3 euler_rotation) override;
};

class TangibleSprite: public virtual Tangible, public virtual Sprite {
protected:
    void update_model_rotation() override;
    Matrix get_rotation_matrix() override;
    void set_rotation_matrix(Vector3 euler_rotation) override;
};

class TestWall: public GhostSprite, public TexturedSprite {
public:
    static TestWall factory();
};

class Billboard: public virtual Ghost, public virtual SimpleTexture {
public:
    void do_render(Camera3D* _camera) override;
};

class Text: public virtual Ghost {
protected:
    Font font;
    float font_size;
    Color color;
public:
    std::string text;
    void load() override;
    void unload() override;
    void do_render(Camera3D* _camera) override;
};


#endif //BADGAME_GAMEOBJECT_H
