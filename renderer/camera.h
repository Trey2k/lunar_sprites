#ifndef CMARA_H
#define CMARA_H

#include "core/core.h"

typedef struct Camera Camera;

LS_EXPORT const Camera *get_current_camera();

LS_EXPORT Camera *camera_create(float32 fov, float32 aspect, float32 near, float32 far);
LS_EXPORT void camera_destroy(Camera *camera);

LS_EXPORT void camera_set_position(Camera *camera, Vector3 position);
LS_EXPORT Vector3 camera_get_position(const Camera *camera);
LS_EXPORT void camera_set_rotation(Camera *camera, Vector3 rotation);
LS_EXPORT Vector3 camera_get_rotation(const Camera *camera);

LS_EXPORT void camera_set_projection(Camera *camera, float32 fov, float32 aspect, float32 near, float32 far);

LS_EXPORT void camera_move(Camera *camera, Vector3 delta);
LS_EXPORT void camera_rotate(Camera *camera, Vector3 delta);

// Set the current camera to be used for rendering
LS_EXPORT void camera_set_active(const Camera *camera);

LS_EXPORT Matrix4 camera_get_view_matrix(const Camera *camera);
LS_EXPORT Matrix4 camera_get_projection_matrix(const Camera *camera);

#endif // CMARA_H