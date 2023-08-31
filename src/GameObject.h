#ifndef BADGAME_GAMEOBJECT_H
#define BADGAME_GAMEOBJECT_H


#include "Hitbox.h"


#include <raylib.h>
#include <string>


class BaseGameObject {
public:
    bool loaded = false;
    virtual void render() = 0;
    virtual void load() = 0;
    virtual void unload() = 0;
};

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

//class TwoDimensional {
//public:
//    virtual Vector2 get_position() = 0;
//    virtual void set_position(Vector2 _position) = 0;
//};

class Tangible3d: public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation {
protected:
    Hitbox3d hitbox;
public:

    Vector3 get_position() override {
        const double* const position = dBodyGetPosition(this->hitbox.body);
        return (Vector3) {(float) position[0], (float) position[1], (float) position[2]};
    }
    void set_position(Vector3 _position) override {
        dBodySetPosition(this->hitbox.body, _position.x, _position.y, _position.z);
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

class WithModel: public BaseGameObject, public virtual ThreeDimensionalLinearity, public virtual ThreeDimensionalRotation {
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

    void apply_shader(Shader* shader) {
        this->model.materials[0].shader = *shader;
    }
};

class Ghost3dModelRotationLink: public virtual Ghost3d, public virtual WithModel {
protected:
    void update_model_rotation() override {
        // nothing needed
    }
public:
    void set_rotation(Vector3 euler_rotation) override {
        this->model.transform = QuaternionToMatrix(QuaternionFromEuler(euler_rotation.x, euler_rotation.y, euler_rotation.z));
    }
};


class TangibleModelRotationLink: public virtual Tangible3d, public virtual WithModel {
protected:
    void update_model_rotation() override {
        const double* rotation = dBodyGetRotation(this->hitbox.body);

        Vector3 euler = QuaternionToEuler(QuaternionFromMatrix((Matrix) {(float) rotation[0], (float) rotation[1], (float) rotation[2], (float) rotation[3],
                                                                         (float) rotation[4], (float) rotation[5], (float) rotation[6], (float) rotation[7],
                                                                         (float) rotation[8], (float) rotation[9], (float) rotation[10], (float) rotation[11],
                                                                         (float) rotation[12], (float) rotation[13], (float) rotation[14], (float) rotation[15]}));
        euler.x *= -1;
        euler.y *= -1;
        euler.z *= -1;
        this->model.transform = MatrixMultiply(MatrixRotateXYZ(euler), this->base_rotation_matrix);
    }
public:
    void set_rotation(Vector3 euler_rotation) override {
        dMatrix3 rotation;
        dRFromEulerAngles(rotation, euler_rotation.x, euler_rotation.y, euler_rotation.z);
        dBodySetRotation(this->hitbox.body, rotation);
    }
};

class TestObject: virtual public WithModel, virtual public Tangible3d, public TangibleModelRotationLink {
public:
    void factory(Simulation* simulation) {
        cuboid_hitbox(simulation, &this->hitbox, 10, 1, 5, 2);
        this->model = LoadModelFromMesh(GenMeshCube(1, 5, 2));
        this->set_position({6, 3, 0});
        this->set_rotation({0, PI/4, 0});
        this->texture_path = "resources/brick.png";
    }
};

class MainScreenWall: virtual public WithModel, public Ghost3dModelRotationLink {
public:
    void factory(Simulation* simulation) {
        this->model = LoadModelFromMesh(GenMeshCube(1, 5, 5));
        this->set_position({5, 0, 0});
        this->set_rotation({0, PI/5, 0});
        this->texture_path = "resources/brick.png";
    }
};

//
//class Tangible: public virtual BaseGameObject {
//protected:
//    Hitbox3d hitbox;
//    Model model;
//    Texture texture;
//    bool visible = false;
//    std::string texture_path;
//public:
//    Vector3 get_position();
//    void set_position(Vector3 _position);
//    void render() override {
//        if (!this->visible) {
//            return;
//        }
//        DrawModel(this->model, this->get_position(), 1, WHITE);
//    }
//
//    void load() override {
//        this->texture = LoadTexture(texture_path.c_str());
//        SetMaterialTexture(&this->model.materials[0], MATERIAL_MAP_DIFFUSE, this->texture);
//        this->loaded = true;
//    }
//    void unload() override {
//        this->loaded = false;
//        UnloadTexture(this->texture);
//    }
//};





//void load() override {
//this->loaded = true;
//}
//void unload() override {
//this->loaded = false;
//}
//
//class GameObject {
//private:
//    bool visible = true;
//    bool loaded;
//    virtual void load() = 0;
//    virtual void unload() = 0;
//protected:
//    virtual void do_render(Camera* _camera) = 0;
//public:
//    void render(Camera* _camera);
//    void do_load();
//    void do_unload();
//    bool is_loaded() const;
//    virtual Vector3 get_position() = 0;
//    virtual void set_position(const Vector3 _position) = 0;
//};
//
//// does not automatically apply to model
//class SimpleTexture: public virtual GameObject {
//protected:
//    Texture texture;
//    std::string texture_path;
//
//    void load() override;
//    void unload() override;
//};
//
//class Ghost: public virtual GameObject {
//protected:
//    Vector3 position;
//    Matrix rotation_matrix;
//public:
//    Vector3 get_position() override;
//    void set_position(const Vector3 _position) override;
//};
//
//
//
//class Sprite: public virtual GameObject {
//public:
//    Model model;
//    Matrix base_rotation_matrix;
//    virtual void update_model_rotation() = 0;
//    virtual Matrix get_rotation_matrix() = 0;
//    virtual void set_rotation_matrix(Vector3 euler_rotation) = 0;
//    void do_render(Camera3D* _camera) override;
//};
//
//class TexturedSprite: public virtual GameObject {
//public:
//    void apply_shader(Shader* shader);
//protected:
//    Texture texture;
//    std::string texture_path;
//
//    void load() override;
//    void unload() override;
//};
//
//
//class CameraLink: public virtual GameObject {
//protected:
//    Vector3 camera_linear_offset = (Vector3) {0, 0, 0};
//    Camera* camera;
//public:
//    void set_camera_linear_offset(const Vector3 linear_offset);
//    void set_camera(Camera* _camera);
//    void update_camera();
//};
//
//class GhostSprite: public Ghost, public Sprite {
//protected:
//    void update_model_rotation() override;
//    Matrix get_rotation_matrix() override;
//    void set_rotation_matrix(Vector3 euler_rotation) override;
//};
//
//class TangibleSprite: public virtual Tangible, public virtual Sprite {
//protected:
//    void update_model_rotation() override;
//    Matrix get_rotation_matrix() override;
//    void set_rotation_matrix(Vector3 euler_rotation) override;
//};
//
//class TestWall: public GhostSprite, public TexturedSprite {
//public:
//    static TestWall factory();
//};
//
//class Billboard: public virtual Ghost, public virtual SimpleTexture {
//public:
//    void do_render(Camera3D* _camera) override;
//};
//
//class Text: public virtual Ghost {
//protected:
//    Font font;
//    float font_size;
//    Color color;
//public:
//    std::string text;
//    void load() override;
//    void unload() override;
//    void do_render(Camera3D* _camera) override;
//};


#endif //BADGAME_GAMEOBJECT_H
