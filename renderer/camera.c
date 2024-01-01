#include "renderer/camera.h"

struct Camera {
	Vector3 position;
	Vector3 rotation;

	Matrix4 projection;
};

static const Camera *current_camera = NULL;

const Camera *get_current_camera() {
	return current_camera;
}

Camera *camera_create(float32 fov, float32 aspect, float32 near, float32 far) {
	Camera *camera = ls_malloc(sizeof(Camera));
	camera->position = vec3(0.0, 0.0, 0.0);
	camera->rotation = vec3(0.0, 0.0, 0.0);

	camera->projection = mat4_perspective(fov, aspect, near, far);

	return camera;
}

void camera_destroy(Camera *camera) {
	ls_free(camera);
}

void camera_set_position(Camera *camera, Vector3 position) {
	camera->position = position;
}

void camera_set_rotation(Camera *camera, Vector3 rotation) {
	camera->rotation = rotation;
}

void camera_set_projection(Camera *camera, float32 fov, float32 aspect, float32 near, float32 far) {
	camera->projection = mat4_perspective(fov, aspect, near, far);
}

void camera_move(Camera *camera, Vector3 delta) {
	camera->position = vec3_add(camera->position, delta);
}

void camera_rotate(Camera *camera, Vector3 delta) {
	camera->rotation = vec3_add(camera->rotation, delta);
}

void camera_set_active(const Camera *camera) {
	current_camera = camera;
}

Matrix4 camera_get_view_matrix(const Camera *camera) {
	Matrix4 view = mat4_translate(vec3_negate(camera->position));
	view = mat4_multiply(view, mat4_rotate(camera->rotation.x, vec3(1.0, 0.0, 0.0)));
	view = mat4_multiply(view, mat4_rotate(camera->rotation.y, vec3(0.0, 1.0, 0.0)));
	view = mat4_multiply(view, mat4_rotate(camera->rotation.z, vec3(0.0, 0.0, 1.0)));

	return view;
}

Matrix4 camera_get_projection_matrix(const Camera *camera) {
	return camera->projection;
}