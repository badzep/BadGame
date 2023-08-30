#include "GameObject.h"


#include <raylib.h>
#include <raymath.h>


void Billboard::do_render(Camera3D* _camera) {
    DrawBillboard(*_camera, this->texture, this->position, 1, BLACK);
}

void Text::load() {
}

void Text::unload() {
}

void Text::do_render(Camera3D* _camera) {
    Vector3 _position = this->get_position();
    DrawTextEx(this->font, this->text.c_str(), (Vector2) {_position.x, _position.y}, this->font_size, 1, this->color);
}

void CameraLink::set_camera_linear_offset(const Vector3 linear_offset) {
    this->camera_linear_offset = linear_offset;
}

void CameraLink::set_camera(Camera *_camera) {
    this->camera = _camera;
}

void CameraLink::update_camera() {
    this->camera->position = this->get_position();
}

Vector3 Tangible::get_position() {
    const double* const position = dBodyGetPosition(this->hitbox.body);
    return (Vector3) {(float) position[0], (float) position[1], (float) position[2]};
}

void Tangible::set_position(const Vector3 _position) {
    dBodySetPosition(this->hitbox.body, _position.x, _position.y, _position.z);
}

Vector3 Ghost::get_position() {
    return this->position;
}

void Ghost::set_position(const Vector3 _position) {
    this->position = _position;
}

void GameObject::do_load() {
    this->load();
    this->loaded = true;
}

void GameObject::do_unload() {
    this->unload();
    this->loaded = false;
}

bool GameObject::is_loaded() const {
    return this->loaded;
}

void GameObject::render(Camera3D* _camera) {
    if (this->visible) {
        printf("Confirmed visible\n");
        this->do_render(_camera);
    }
}

void Sprite::do_render(Camera3D *_camera) {
    printf("Attempting to rotate\n");
    this->update_model_rotation();
    printf("Attempting to draw\n");
    DrawModel(this->model, this->get_position(), 1, WHITE);
    printf("done drawing\n");
}

void GhostSprite::update_model_rotation() {
    Vector3 euler = QuaternionToEuler(QuaternionFromMatrix(this->get_rotation_matrix()));
    this->model.transform = MatrixMultiply(MatrixRotateXYZ(euler), this->base_rotation_matrix);
}

Matrix GhostSprite::get_rotation_matrix() {
    return this->rotation_matrix;
}

void GhostSprite::set_rotation_matrix(Vector3 euler_rotation) {
    this->rotation_matrix = QuaternionToMatrix(QuaternionFromEuler(euler_rotation.x, euler_rotation.y, euler_rotation.z));
}

void TangibleSprite::update_model_rotation() {
    const double* rotation = dBodyGetRotation(this->hitbox.body);

    Vector3 euler = QuaternionToEuler(QuaternionFromMatrix((Matrix) {(float) rotation[0], (float) rotation[1], (float) rotation[2], (float) rotation[3],
                                                                     (float) rotation[4], (float) rotation[5], (float) rotation[6], (float) rotation[7],
                                                                     (float) rotation[8], (float) rotation[9], (float) rotation[10], (float) rotation[11],
                                                                     (float) rotation[12], (float) rotation[13], (float) rotation[14], (float) rotation[15]}));
    this->model.transform = MatrixMultiply(MatrixRotateXYZ(euler), this->base_rotation_matrix);
}

Matrix TangibleSprite::get_rotation_matrix() {
    const double* rotation = dBodyGetRotation(this->hitbox.body);
    return (Matrix) {(float) rotation[0], (float) rotation[1], (float) rotation[2], (float) rotation[3],
              (float) rotation[4], (float) rotation[5], (float) rotation[6], (float) rotation[7],
              (float) rotation[8], (float) rotation[9], (float) rotation[10], (float) rotation[11],
              (float) rotation[12], (float) rotation[13], (float) rotation[14], (float) rotation[15]};
}

void TangibleSprite::set_rotation_matrix(Vector3 euler_rotation) {
    dMatrix3 rotation;
    dRFromEulerAngles(rotation, euler_rotation.x, euler_rotation.y, euler_rotation.z);
    dBodySetRotation(this->hitbox.body, rotation);
}

void SimpleTexture::load() {
    this->texture = LoadTexture(this->texture_path.c_str());
}

void SimpleTexture::unload() {
    UnloadTexture(this->texture);
}

void TexturedSprite::load() {
    this->texture = LoadTexture(this->texture_path.c_str());
//    this->model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = this->texture;
}

void TexturedSprite::unload() {
    UnloadTexture(this->texture);
}

void TexturedSprite::apply_shader(Shader *shader) {
//    this->model.materials[0].shader = *shader;
}

TestWall TestWall::factory() {
    TestWall test_wall = TestWall();
    test_wall.model = LoadModelFromMesh(GenMeshCube(1, 5, 5));
    test_wall.set_position({5, 0, 0});
    test_wall.set_rotation_matrix({PI, 0, 0});
    test_wall.texture_path = "resources/brick.png";
    return test_wall;
}
