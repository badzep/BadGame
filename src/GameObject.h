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
};

//class TwoDimensional {
//public:
//    virtual Vector2 get_position() = 0;
//    virtual void set_position(Vector2 _position) = 0;
//};

class ThreeDimensionalLinearity {
public:
    virtual Vector3 get_position() = 0;
    virtual void set_position(Vector3 _position) = 0;
};

class ThreeDimensionalRotation {
protected:
    Matrix base_rotation_matrix;
    virtual void set_rotation(Vector3 euler_rotation) = 0;
};

class Model3d: public GameObject, public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation {
protected:
    Model model;
    Texture texture;
    std::string texture_path;
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
    void set_linear_dampening(double strength) const;
    void set_angular_dampening(double strength) const;
    void set_kinematic() const;
    void set_dynamic() const;
    void enable_gravity() const;
    void disable_gravity() const;
};

class TangibleModelRotationLink: public virtual Tangible3d, public virtual Model3d {
protected:
    void update_model_rotation() override;
public:
    void set_rotation(Vector3 euler_rotation) override;
};



class TestObject: virtual public Model3d, virtual public Tangible3d, public TangibleModelRotationLink {
public:
    void factory(Simulation* simulation);
};
class MainScreenWall2: virtual public Model3d, virtual public Ghost3d, public Ghost3dModelRotationLink {
public:
    void factory();
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
    void render() override {

    }
    void set_rotation(Vector3 euler_rotation) override;
    void factory(Simulation* simulation, Camera* camera);
};


class MainScreenWall: virtual public Model3d, virtual public Ghost3d, public Ghost3dModelRotationLink {
public:
    void factory(Simulation* simulation);
};

class Structure: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink {
public:
    void custom(Simulation* simulation, const Vector3 position, const Vector3 rotation, const Vector3 size, const std::string& _texture_path);

};

class Block: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink {
public:
    void custom(Simulation* simulation, const Vector3 position, const Vector3 rotation, const Vector3 size, const float mass, const std::string& _texture_path);
};

class Ball: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink {
public:
    void custom(Simulation* simulation, const Vector3 position, const Vector3 rotation, const float radius, const float mass, const std::string& _texture_path);
};

#endif //BADGAME_GAMEOBJECT_H
