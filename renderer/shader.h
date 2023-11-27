#ifndef SHADER_H
#define SHADER_H

#include "renderer/renderer.h"

typedef struct Shader Shader;

// Creates a shader from a vertex and fragment source. Returns 0 if the shader failed to compile.
// The shader source is expected to be compatible with the backend.
LS_EXPORT Shader *renderer_create_shader(const Renderer *renderer, String vertex_source, String fragment_source);
// Destroys a shader.
LS_EXPORT void shader_destroy(Shader *shader);
// Binds the shader for use in the current rendering context.
LS_EXPORT void shader_bind(const Shader *shader);
// Unbinds the shader from the current rendering context.
LS_EXPORT void shader_unbind(const Shader *shader);

// Sets a uniform in the shader. The shader must be bound before calling this function.
LS_EXPORT void shader_set_uniform_int(const Shader *shader, String name, int32 value);
// Sets a uniform in the shader. The shader must be bound before calling this function.
LS_EXPORT void shader_set_uniform_float(const Shader *shader, String name, float32 value);
// Sets a uniform in the shader. The shader must be bound before calling this function.
LS_EXPORT void shader_set_uniform_vec2(const Shader *shader, String name, Vector2 value);
// Sets a uniform in the shader. The shader must be bound before calling this function.
LS_EXPORT void shader_set_uniform_vec3(const Shader *shader, String name, Vector3 value);

// Returns the location of a uniform in the shader. The shader must be bound before calling this function.
LS_EXPORT int32 shader_get_uniform_location(const Shader *shader, String name);
// Returns the location of an attribute in the shader. The shader must be bound before calling this function.
LS_EXPORT uint32 shader_get_attrib_location(const Shader *shader, String name);

#endif // SHADER_H