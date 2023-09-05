#include "GameObject.h"


#include <raylib.h>
#include <raymath.h>

bool GameObject::is_loaded() const {
    return this->loaded;
}

void Model3d::render() {
    if (!this->visible | !this->is_loaded()) {
        return;
    }
    this->update_model_rotation();
    DrawModel(this->model, this->get_position(), this->scale, WHITE);
}

void Model3d::load() {
    if (this->is_loaded()) {
        return;
    }
    this->load_texture();
    SetMaterialTexture(&this->model.materials[0], MATERIAL_MAP_DIFFUSE, *this->get_texture());
    this->loaded = true;
}
void Model3d::unload() {
    if (!this->is_loaded()) {
        return;
    }
    this->release_texture();
    this->loaded = false;
}

void Model3d::apply_shader(Shader *shader) const {
    this->model.materials[0].shader = *shader;
}

void Model3d::set_scale(float _scale) {
    this->scale = _scale;
}

Vector3 Ghost3d::get_position() {
    return this->position;
}

void Ghost3d::set_position(const Vector3 _position) {
    this->position = _position;
}

void Ghost3dModelRotationLink::update_model_rotation() {}

void Ghost3dModelRotationLink::set_rotation(const Vector3 euler_rotation) {
    this->model.transform = QuaternionToMatrix(QuaternionFromEuler(euler_rotation.x, euler_rotation.y, euler_rotation.z));
}

Vector3 Tangible3d::get_position() {
    const double* position = dBodyGetPosition(this->hitbox.body);
    return (Vector3) {(float) position[0], (float) position[1], (float) position[2]};
}

void Tangible3d::set_position(const Vector3 _position) {
    dBodySetPosition(this->hitbox.body, _position.x, _position.y, _position.z);
}

void Tangible3d::apply_force(const Vector3 force) const {
    dBodyAddForce(this->hitbox.body, force.x, force.y, force.z);
}

void Tangible3d::set_linear_dampening(const double strength) const {
    dBodySetLinearDamping(this->hitbox.body, strength);
}

void Tangible3d::set_angular_dampening(const double strength) const {
    dBodySetAngularDamping(this->hitbox.body, strength);
}

void Tangible3d::set_kinematic() const {
    dBodySetKinematic(this->hitbox.body);
}

void Tangible3d::set_dynamic() const {
    dBodySetDynamic(this->hitbox.body);
}

void Tangible3d::enable_gravity() const {
    dBodySetGravityMode(this->hitbox.body, 1);
}

void Tangible3d::disable_gravity() const {
    dBodySetGravityMode(this->hitbox.body, 0);
}

void TangibleModelRotationLink::update_model_rotation() {
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

void TangibleModelRotationLink::set_rotation(const Vector3 euler_rotation) {
    dMatrix3 rotation;
    dRFromEulerAngles(rotation, euler_rotation.x, euler_rotation.y, euler_rotation.z);
    dBodySetRotation(this->hitbox.body, rotation);
}

void CameraLink::set_camera(Camera *_camera) {
    this->camera = _camera;
}

Vector3 CameraLink::get_camera_linear_offset() {
    return this->camera_linear_offset;
}

void CameraLink::set_camera_linear_offset(const Vector3 linear_offset) {
    this->camera_linear_offset = linear_offset;
}

void CameraLink::update_camera() {
    Vector3 object_position = this->get_position();
    Vector3 offset = this->get_camera_linear_offset();
    this->camera->position = Vector3Add(object_position, offset);
}

void Player::load() {
    this->loaded = true;
}

void Player::unload() {
    this->loaded = false;
}

void Player::set_rotation(Vector3 euler_rotation) {
    dMatrix3 rotation;
    dRFromEulerAngles(rotation, euler_rotation.x, euler_rotation.y, euler_rotation.z);
    dBodySetRotation(this->hitbox.body, rotation);
}

void Player::factory(Simulation *simulation, Camera *camera) {
    cuboid_hitbox(simulation, &this->hitbox, 87.72f, .25f, 2.0f, .25f);
    this->set_position({0, 0, 0});
    this->set_rotation({0, 0, 0});
    this->set_angular_dampening(1);

    this->set_camera(camera);
    this->set_camera_linear_offset({0, 0.5, 0});
    this->update_camera();
}

void Player::render() {

}

void MainScreenWall::factory() {
    this->model = LoadModelFromMesh(GenMeshCube(1, 20, 20));

    this->set_position({5, 0, 0});
    this->set_scale(1);
    this->set_rotation({0, PI/8, 0});
    this->texture_path = "resources/brick.png";
}

void MainScreenText::factory() {
    this->model = LoadModel("resources/mainscreentext.obj");
    this->set_position({3, 0, .3});
    this->set_rotation({-PI/2, -PI/2 + PI/8, 0});
    this->set_scale(1);
    this->texture_path = "resources/red.png";
}

void Rat::custom(Simulation *simulation, const Vector3 position, const Vector3 rotation) {
    this->model = LoadModel("resources/rat.glb");
    this->texture_path = "resources/red.png";
    cuboid_hitbox(simulation, &this->hitbox, 10, .2, .5, .2);
    this->set_position(position);
    this->set_rotation(Vector3Add(rotation,{-PI/2, -PI, PI/2}));
}

void Structure::custom(Simulation* simulation, const Vector3 position, const Vector3 rotation, const Vector3 size, Texture* external_texture) {
    this->model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
    this->share_texture(external_texture);
    cuboid_hitbox(simulation, &this->hitbox, 100, size.x, size.y, size.z);
    this->set_position(position);
    this->set_rotation(rotation);
    this->set_kinematic();
}

void Block::custom(Simulation *simulation, const Vector3 position, const Vector3 rotation, const Vector3 size, const float mass, Texture* external_texture) {
    this->model = LoadModelFromMesh(GenMeshCube(size.x, size.y, size.z));
    this->share_texture(external_texture);
    cuboid_hitbox(simulation, &this->hitbox, mass, size.x, size.y, size.z);
    this->set_position(position);
    this->set_rotation(rotation);
}

void Ball::custom(Simulation *simulation, const Vector3 position, const Vector3 rotation, const float radius, const float mass, Texture* external_texture) {
    this->model = LoadModelFromMesh(GenMeshSphere(radius, 10, 50));
    this->share_texture(external_texture);
    sphere_hitbox(simulation, &this->hitbox, mass, radius);
    this->set_position(position);
    this->set_rotation(rotation);
}

void PhysicalCylinder::custom(Simulation *simulation, const Vector3 position, const Vector3 rotation, const float radius, const float length, const float mass, Texture *external_texture) {

}

void Billboard::load() {
    if (this->is_loaded()) {
        return;
    }
    this->load_texture();
    this->loaded = true;
}

void Billboard::unload() {
    if (this->is_loaded()) {
        return;
    }
    this->load_texture();
    this->loaded = true;
}

void Billboard::set_camera(Camera *_camera) {
    this->camera = _camera;
}

void Billboard::render() {
    DrawBillboard(*this->camera, *this->get_texture(), this->get_position(), this->size, this->color);
}

Vector3 Billboard::get_position() {
    return this->position;
}

void Billboard::set_position(Vector3 _position) {
    this->position = _position;
}

void SharedTexture::share_texture(Texture *texture) {
    this->shared_texture = texture;
}

void SharedTexture::load_texture() {

}

void SharedTexture::release_texture() {

}

Texture *SharedTexture::get_texture() {
    return this->shared_texture;
}

Color Colored::get_color() {
    return this->color;
}

void Colored::set_color(Color _color) {
    this->color = _color;
}
