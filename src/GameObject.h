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
    void render() override {
        if (!this->visible) {
            return;
        }
        this->update_model_rotation();
        DrawModel(this->model, this->get_position(), 1, WHITE);
    }

    void load() override {
        this->base_rotation_matrix = this->model.transform;
        this->texture = LoadTexture(texture_path.c_str());
        SetMaterialTexture(&this->model.materials[0], MATERIAL_MAP_DIFFUSE, this->texture);
        this->loaded = true;
    }
    void unload() override {
        this->loaded = false;
        UnloadTexture(this->texture);
    }

    void apply_shader(Shader* shader) const {
        this->model.materials[0].shader = *shader;
    }
};

class Ghost3d: public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation {
protected:
    Vector3 position;
public:
    Vector3 get_position() override {
        return this->position;
    }
    void set_position(Vector3 _position) override {
        this->position = _position;
    }
};

class Ghost3dModelRotationLink: public virtual Ghost3d, public virtual Model3d {
protected:
    void update_model_rotation() override {}
public:
    void set_rotation(Vector3 euler_rotation) override {
        this->model.transform = QuaternionToMatrix(QuaternionFromEuler(euler_rotation.x, euler_rotation.y, euler_rotation.z));
    }
};

class Tangible3d: public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation {
protected:
    Hitbox3d hitbox;
public:
    Vector3 get_position() override {
        const double* position = dBodyGetPosition(this->hitbox.body);
        return (Vector3) {(float) position[0], (float) position[1], (float) position[2]};
    }
    void set_position(Vector3 _position) override {
        dBodySetPosition(this->hitbox.body, _position.x, _position.y, _position.z);
    }
    void apply_force(Vector3 force) const {
        dBodyAddForce(this->hitbox.body, force.x, force.y, force.z);
    }
    void set_linear_dampening(double strength) const {
        dBodySetLinearDamping(this->hitbox.body, strength);
    }
    void set_angular_dampening(double strength) const {
        dBodySetAngularDamping(this->hitbox.body, strength);
    }
    void set_kinematic() const {
        dBodySetKinematic(this->hitbox.body);
    }
    void set_dynamic() const {
        dBodySetDynamic(this->hitbox.body);
    }
    void enable_gravity() {
        dBodySetGravityMode(this->hitbox.body, 1);
    }
    void disable_gravity() {
        dBodySetGravityMode(this->hitbox.body, 0);
    }
};

class TangibleModelRotationLink: public virtual Tangible3d, public virtual Model3d {
protected:
    void update_model_rotation() override {
        const double* rotation = dBodyGetRotation(this->hitbox.body);
        this->model.transform.m0 = (float) rotation[0];
        this->model.transform.m1 = (float) rotation[4];
        this->model.transform.m2 = (float) rotation[8];
        this->model.transform.m3 = 0;
        this->model.transform.m4 = (float) rotation[1];
        this->model.transform.m5 = (float) rotation[5];
        this->model.transform.m6 = (float) rotation[9];
        this->model.transform.m7 = 0;
        this->model.transform.m8 = (float) rotation[2];
        this->model.transform.m9 = (float) rotation[6];
        this->model.transform.m10 = (float) rotation[10];
        this->model.transform.m11 = 0;
        this->model.transform.m12 = 0;
        this->model.transform.m13 = 0;
        this->model.transform.m14 = 0;
        this->model.transform.m15 = 1;
    }
public:
    void set_rotation(Vector3 euler_rotation) override {
        dMatrix3 rotation;
        dRFromEulerAngles(rotation, euler_rotation.x, euler_rotation.y, euler_rotation.z);
        dBodySetRotation(this->hitbox.body, rotation);
    }
};



class TestObject: virtual public Model3d, virtual public Tangible3d, public TangibleModelRotationLink {
public:
    void factory(Simulation* simulation) {
        this->model = LoadModelFromMesh(GenMeshCube(1, 5, 2));
        this->texture_path = "resources/brick.png";

        cuboid_hitbox(simulation, &this->hitbox, 10, 1, 5, 2);
        this->set_position({6, 3, 0});
        this->set_rotation({0, PI/4, 0});
    }
};
class MainScreenWall2: virtual public Model3d, virtual public Ghost3d, public Ghost3dModelRotationLink {
public:
    void factory(Simulation* simulation) {
        this->model = LoadModelFromMesh(GenMeshCube(1, 5, 5));
        this->set_position({5, 1, 0});
        this->set_rotation({0, PI/5, 0});
        this->texture_path = "resources/brick.png";
    }
};

class CameraLink: public virtual ThreeDimensionalLinearity {
protected:
    Camera* camera;
    Vector3 camera_linear_offset;
public:
    void set_camera(Camera* _camera) {
        this->camera = _camera;
    }
    Vector3 get_camera_linear_offset() {
        return this->camera_linear_offset;
    }
    void set_camera_linear_offset(const Vector3 linear_offset) {
        this->camera_linear_offset = linear_offset;
    }
    void update_camera() {
        Vector3 object_position = this->get_position();
        Vector3 offset = this->get_camera_linear_offset();
        this->camera->position = Vector3Add(object_position, offset);
    }
};


class Player: public GameObject, public Tangible3d, public CameraLink {
public:
    void load() override {
        this->loaded = true;
    }
    void unload() override {
        this->loaded = false;
    }
    void render() override {

    }
    void set_rotation(Vector3 euler_rotation) override {
        dMatrix3 rotation;
        dRFromEulerAngles(rotation, euler_rotation.x, euler_rotation.y, euler_rotation.z);
        dBodySetRotation(this->hitbox.body, rotation);
    }
    void factory(Simulation* simulation, Camera* camera) {
        cuboid_hitbox(simulation, &this->hitbox, 87.72f, .25f, 2.0f, .25f);
        this->set_position({0, 0, 0});
        this->set_rotation({0, 0, 0});
        this->set_angular_dampening(1);

        this->set_camera(camera);
        this->set_camera_linear_offset({0, 0.5, 0});
        this->update_camera();
    }
};

//class Player: public GameObject, public Tangible3d {
//public:
//    void load() override {
//        this->loaded = true;
//    }
//    void unload() override {
//        this->loaded = false;
//    }
//    void render() override {
//
//    }
//    void set_rotation(Vector3 euler_rotation) override {
//        dMatrix3 rotation;
//        dRFromEulerAngles(rotation, euler_rotation.x, euler_rotation.y, euler_rotation.z);
//        dBodySetRotation(this->hitbox.body, rotation);
//    }
//    void factory(Simulation* simulation, Camera* camera) {
//        cuboid_hitbox(simulation, &this->hitbox, 87.72f, .25f, 2.0f, .25f);
//        this->set_position({0, 3, 0});
//        this->set_rotation({0, 0, 0});
//        this->set_angular_dampening(1);
//    }
//};

class MainScreenWall: virtual public Model3d, virtual public Ghost3d, public Ghost3dModelRotationLink {
public:
    void factory(Simulation* simulation) {
        this->model = LoadModelFromMesh(GenMeshCube(1, 5, 5));
        this->set_position({5, 0, 0});
        this->set_rotation({0, PI/5, 0});
        this->texture_path = "resources/brick.png";
    }
};

class Structure: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink {
public:
    void custom(Simulation* simulation, Vector3 position, Vector3 rotation, Vector3 size, std::string _texture_path) {
        this->model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
        this->texture_path = std::move(_texture_path);
        cuboid_hitbox(simulation, &this->hitbox, 100, size.x, size.y, size.z);
        this->set_position(position);
        this->set_rotation(rotation);
        this->set_kinematic();
    }
};

class Block: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink {
public:
    void custom(Simulation* simulation, Vector3 position, Vector3 rotation, Vector3 size, std::string _texture_path) {
        this->model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
        this->texture_path = std::move(_texture_path);
        cuboid_hitbox(simulation, &this->hitbox, 100, size.x, size.y, size.z);
        this->set_position(position);
        this->set_rotation(rotation);
    }
};

class Ball: virtual public Model3d, public virtual Tangible3d, public TangibleModelRotationLink {
public:
    void custom(Simulation* simulation, Vector3 position, Vector3 rotation, float radius, std::string _texture_path) {
        this->model = LoadModelFromMesh(GenMeshSphere(radius, 10, 50));
        this->texture_path = std::move(_texture_path);
        sphere_hitbox(simulation, &this->hitbox, 100, radius);
        this->set_position(position);
        this->set_rotation(rotation);
    }
};

#endif //BADGAME_GAMEOBJECT_H
