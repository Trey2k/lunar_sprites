#ifndef OPENGL_SHADER_H
#define OPENGL_SHADER_H

#include "core/core.h"

typedef struct OpenGLShader OpenGLShader;

OpenGLShader *opengl_create_shader(String vertex_source, String fragment_source);
void opengl_shader_destroy(OpenGLShader *shader);

void opengl_shader_bind(const OpenGLShader *shader);
void opengl_shader_unbind(const OpenGLShader *shader);

void opengl_shader_set_uniform_int(const OpenGLShader *shader, String name, int32 value);
void opengl_shader_set_uniform_float(const OpenGLShader *shader, String name, float32 value);
void opengl_shader_set_uniform_vec2(const OpenGLShader *shader, String name, Vector2 value);
void opengl_shader_set_uniform_vec3(const OpenGLShader *shader, String name, Vector3 value);
void opengl_shader_set_uniform_mat4(const OpenGLShader *shader, String name, Matrix4 value);
void opengl_shader_set_uniform_intv(const OpenGLShader *shader, String name, const int32 *value, size_t count);
void opengl_shader_set_uniform_floatv(const OpenGLShader *shader, String name, const float32 *value, size_t count);

int32 opengl_shader_get_uniform_location(const OpenGLShader *shader, String name);
uint32 opengl_shader_get_attrib_location(const OpenGLShader *shader, String name);

#endif // OPENGL_SHADER_H