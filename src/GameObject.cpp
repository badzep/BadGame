#include "GameObject.h"

void Billboard::on_load() {
    this->texture = LoadTexture(this->texture_path.c_str());
}

void Billboard::on_unload() {
    UnloadTexture(this->texture);
}

void Billboard::render(Camera3D* _camera) {
    if (!this->do_render) {
        return;
    }
    DrawBillboard(*_camera, this->texture, this->position, 1, BLACK);
}

void Text::on_load() {
}

void Text::on_unload() {
}

void Text::render(Camera3D* _camera) {
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
//    this->camera->position = this.
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

void GameObject::load() {
    this->on_load();
    this->loaded = true;
}

void GameObject::unload() {
    this->on_unload();
    this->loaded = false;
}

bool GameObject::is_loaded() const {
    return this->loaded;
}
