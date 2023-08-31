#ifndef BADGAME_GAMEOBJECT_H
#define BADGAME_GAMEOBJECT_H


#include "Hitbox.h"


#include <raylib.h>
#include <string>


class GameObject {
protected:
    bool loaded = false;
public:
    virtual void render() = 0;
    virtual void load() = 0;
    virtual void unload() = 0;
    bool is_loaded() const;
};

class Colored {
protected:
    Color color;
public:
    Color get_color();
    void set_color(Color _color);
};

class Textured {
protected:
    virtual void load_texture() = 0;
    virtual void release_texture() = 0;
    virtual Texture* get_texture() = 0;
};

class IndependentTexture: public virtual Textured {
protected:
    Texture texture;
    std::string texture_path;
protected:
    void load_texture() override {
        this->texture = LoadTexture(texture_path.c_str());
    }
    void release_texture() override {
        UnloadTexture(this->texture);
    }
    Texture* get_texture() override {
        return &this->texture;
    }
};

class SharedTexture: public virtual Textured {
protected:
    Texture* shared_texture;
    void load_texture() override;
    void release_texture() override;
    Texture* get_texture() override;
public:
    void share_texture(Texture* texture);
};

class TwoDimensionalLinearity {
protected:
    Vector2 position;
public:
    Vector2 get_position() {
        return this->position;
    }
    void set_position(Vector2 _position) {
        this->position = _position;
    }
};

class Text: public GameObject, public TwoDimensionalLinearity, public Colored {
protected:
    Font font;
    float font_size;
    float spacing;
    std::string text;
public:
    void load() override {
        this->loaded = true;
    }
    void unload() override {
        this->loaded = false;
    }
    void render() override {
        DrawTextEx(this->font, this->text.c_str(), this->get_position(), this->font_size, this->spacing, this->color);
    }
};

class ThreeDimensionalLinearity {
public:
    virtual Vector3 get_position() = 0;
    virtual void set_position(Vector3 _position) = 0;
};

class ThreeDimensionalRotation {
public:
    virtual void set_rotation(Vector3 euler_rotation) = 0;
};

class Billboard: public virtual GameObject, public ThreeDimensionalLinearity, public virtual Textured, public Colored {
protected:
    Vector3 position;
    float size;
    Camera* camera;
public:
    void load() override;
    void unload() override;
    void render() override;
    void set_camera(Camera* _camera);
};

class Model3d: public GameObject, public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation, public virtual Textured {
protected:
    Model model;
    bool visible = true;
    virtual void update_model_rotation() = 0;
public:
    void render() override;
    void load() override;
    void unload() override;
    void apply_shader(Shader* shader) const;
};

class Ghost3d: public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation {
protected:
    Vector3 position;
public:
    Vector3 get_position() override;
    void set_position(Vector3 _position) override;
};

class Ghost3dModelRotationLink: public virtual Ghost3d, public virtual Model3d {
protected:
    void update_model_rotation() override;
public:
    void set_rotation(Vector3 euler_rotation) override;
};

class Tangible3d: public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation {
protected:
    Hitbox3d hitbox;
public:
    Vector3 get_position() override;
    void set_position(Vector3 _position) override;
    void apply_force(Vector3 force) const;
    void set_linear_dampening(const double strength) const;
    void set_angular_dampening(const double strength) const;
    void set_kinematic() const;
    void set_dynamic() const;
    void enable_gravity() const;
    void disable_gravity() const;
};

class TangibleModelRotationLink: public virtual Tangible3d, public virtual Model3d {
protected:
    void update_model_rotation() override;
public:
    void set_rotation(const Vector3 euler_rotation) override;
};

class CameraLink: public virtual ThreeDimensionalLinearity {
protected:
    Camera* camera;
    Vector3 camera_linear_offset;
public:
    void set_camera(Camera* _camera);
    Vector3 get_camera_linear_offset();
    void set_camera_linear_offset(const Vector3 linear_offset);
    void update_camera();
};

class Player: public GameObject, public Tangible3d, public CameraLink {
public:
    void load() override;
    void unload() override;
    void render() override;
    void set_rotation(Vector3 euler_rotation) override;
    void factory(Simulation* simulation, Camera* camera);
};

class MainScreenWall: virtual public Model3d, virtual public Ghost3d, public Ghost3dModelRotationLink, public IndependentTexture {
public:
    void factory();
};

class Structure: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink, public SharedTexture {
public:
    void custom(Simulation* simulation, const Vector3 position, const Vector3 rotation, const Vector3 size, Texture* external_texture);
};

class Block: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink, public SharedTexture  {
public:
    void custom(Simulation* simulation, const Vector3 position, const Vector3 rotation, const Vector3 size, const float mass, Texture* external_texture);
};

class Ball: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink, public SharedTexture  {
public:
    void custom(Simulation* simulation, const Vector3 position, const Vector3 rotation, const float radius, const float mass, Texture* external_texture);
};

#endif //BADGAME_GAMEOBJECT_H
