#ifndef OPENGL_RENDERER_INTERFACE_H
#define OPENGL_RENDERER_INTERFACE_H

#include "core/core.h"
#include "renderer/renderer.h"

void opengl_set_clear_color(float32 r, float32 g, float32 b, float32 a);
void opengl_clear();

uint32 opengl_create_shader(String vertex_source, String fragment_source);
void opengl_destroy_shader(uint32 shader);
void opengl_bind_shader(uint32 shader);
void opengl_set_uniform_int(uint32 shader, String name, int32 value);
void opengl_set_uniform_float(uint32 shader, String name, float32 value);
void opengl_set_uniform_vec2(uint32 shader, String name, Vector2 value);
void opengl_set_uniform_vec3(uint32 shader, String name, Vector3 value);
int32 opengl_get_uniform_location(uint32 shader, String name);
uint32 opengl_get_attrib_location(uint32 shader, String name);

uint32 opengl_create_vertex_buffer(size_t size, void *data, UsageHint usage_hint);
void opengl_destroy_vertex_buffer(uint32 vertex_buffer);
void opengl_bind_vertex_buffer(TargetHint target, uint32 vertex_buffer);

uint32 opengl_create_vertex_array();
void opengl_destroy_vertex_array(uint32 vertex_array);
void opengl_bind_vertex_array(uint32 vertex_array);
void opengl_set_vertex_array_uniform(uint32 vertex_array, int32 uniform_position, uint32 size, DataType data_type, bool normalized, size_t stride, size_t offset);

void opengl_enable_vertex_attrib_array(uint32 index);
void opengl_draw_arrays(DrawMode draw_mode, size_t first, size_t count);

#endif // OPENGL_RENDERER_INTERFACE_H