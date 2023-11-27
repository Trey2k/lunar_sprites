#include "renderer/shader.h"

#include "renderer/opengl/shader.h"
#include "renderer/renderer.h"

#if defined(OPENGL_ENABLED)

#define SHADER_CALL(shader, method, ...)                                     \
	switch (shader->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                        \
		} break;                                                             \
                                                                             \
		case RENDERER_BACKEND_OPENGL: {                                      \
			opengl_shader_##method(shader->opengl, ##__VA_ARGS__);           \
		} break;                                                             \
                                                                             \
		default:                                                             \
			ls_log_fatal("Unknown renderer backend: %d\n", shader->backend); \
	};

// SHADER_CALL_R is for functions that return a value
// The macro will only return if the call is successful
#define SHADER_CALL_R(shader, method, ...)                                   \
	switch (shader->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                        \
		} break;                                                             \
                                                                             \
		case RENDERER_BACKEND_OPENGL: {                                      \
			return opengl_shader_##method(shader->opengl, ##__VA_ARGS__);    \
		} break;                                                             \
                                                                             \
		default:                                                             \
			ls_log_fatal("Unknown renderer backend: %d\n", shader->backend); \
	};

#else

#define SHADER_CALL(shader, method, ...)                                     \
	switch (shader->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                        \
		} break;                                                             \
                                                                             \
		default:                                                             \
			ls_log_fatal("Unknown renderer backend: %d\n", shader->backend); \
	};

#define SHADER_CALL_R(shader, method, ...) SHADER_CALL(shader, method, ##__VA_ARGS__)

#endif

struct Shader {
	RendererBackend backend;

	union {
#if defined(OPENGL_ENABLED)
		OpenGLShader *opengl;
#endif
	};
};

Shader *renderer_create_shader(const Renderer *renderer, String vertex_source, String fragment_source) {
	Shader *shader = ls_malloc(sizeof(Shader));
	shader->backend = renderer_get_backend(renderer);

	switch (shader->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

		case RENDERER_BACKEND_OPENGL: {
#if defined(OPENGL_ENABLED)
			shader->opengl = opengl_shader_create(vertex_source, fragment_source);
			if (!shader->opengl) {
				ls_free(shader);
				return NULL;
			}
#endif

		} break;

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", shader->backend);
	}

	return shader;
}

void shader_destroy(Shader *shader) {
	SHADER_CALL(shader, destroy);
	ls_free(shader);
}

void shader_bind(const Shader *shader) {
	SHADER_CALL(shader, bind);
}

void shader_unbind(const Shader *shader) {
	SHADER_CALL(shader, unbind);
}

void shader_set_uniform_int(const Shader *shader, String name, int32 value) {
	SHADER_CALL(shader, set_uniform_int, name, value);
}

void shader_set_uniform_float(const Shader *shader, String name, float value) {
	SHADER_CALL(shader, set_uniform_float, name, value);
}

void shader_set_uniform_vec2(const Shader *shader, String name, Vector2 value) {
	SHADER_CALL(shader, set_uniform_vec2, name, value);
}

void shader_set_uniform_vec3(const Shader *shader, String name, Vector3 value) {
	SHADER_CALL(shader, set_uniform_vec3, name, value);
}

int32 shader_get_uniform_location(const Shader *shader, String name) {
	SHADER_CALL_R(shader, get_uniform_location, name);
	return -1;
}

uint32 shader_get_attrib_location(const Shader *shader, String name) {
	SHADER_CALL_R(shader, get_attrib_location, name);
	return -1;
}
