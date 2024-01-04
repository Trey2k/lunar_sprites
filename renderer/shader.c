#include "renderer/shader.h"
#include "renderer/renderer.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/shader.h"
#endif

// We cannot use if with the preprocessor, so we use this macro instead
#if defined(OPENGL_ENABLED)
#define GL_SHADER_CALL(shader, method, ...) \
	opengl_shader_##method(shader->opengl, ##__VA_ARGS__);
#else
#define GL_SHADER_CALL(shader, method, ...) \
	ls_log_fatal("OpenGL is not enabled\n");
#endif // OPENGL_ENABLED

// SHADER_CALL is for functions that do not return a value
#define SHADER_CALL(shader, method, ...)                                     \
	switch (shader->backend) {                                               \
		case RENDERER_BACKEND_NONE: {                                        \
		} break;                                                             \
                                                                             \
		case RENDERER_BACKEND_OPENGL: {                                      \
			GL_SHADER_CALL(shader, method, ##__VA_ARGS__);                   \
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
			return GL_SHADER_CALL(shader, method, ##__VA_ARGS__);            \
		} break;                                                             \
                                                                             \
		default:                                                             \
			ls_log_fatal("Unknown renderer backend: %d\n", shader->backend); \
	};

struct Shader {
	RendererBackend backend;

	union {
#if defined(OPENGL_ENABLED)
		OpenGLShader *opengl;
#endif
	};
};

enum ShaderParserState {
	STATE_NONE,
	STATE_PREPROCESSOR,
	STATE_OPNGL_VERTEX,
	STATE_OPNGL_FRAGMENT,
};

static bool handle_preprocessor_command(String command, enum ShaderParserState *state) {
	if (ls_str_equals(command, "opengl_vertex")) {
		*state = STATE_OPNGL_VERTEX;
	} else if (ls_str_equals(command, "opengl_fragment")) {
		*state = STATE_OPNGL_FRAGMENT;
	} else {
		ls_log(LOG_LEVEL_ERROR, "Unknown preprocessor command: %s\n", command);
		return false;
	}

	return true;
}

static bool is_preprocessor_command(String cur_char) {
	return *cur_char == '#' && *(cur_char + 1) == 'L' && *(cur_char + 2) == 'S' && *(cur_char + 3) == ' ';
}

Shader *renderer_create_shader(const Renderer *renderer, String source, size_t source_size) {
	Slice8 *opengl_vertex_source = slice8_create(source_size / 2);
	Slice8 *opengl_fragment_source = slice8_create(source_size / 2);
	Slice8 *preprocessor_command = slice8_create(32);
	Shader *shader = NULL;

	enum ShaderParserState state = STATE_NONE;

	String cur_char = source;
	while (*cur_char != '\0') {
		switch (state) {
			case STATE_NONE: {
				if (is_preprocessor_command(cur_char)) {
					cur_char += 3;
					state = STATE_PREPROCESSOR;
				}
			} break;

			case STATE_PREPROCESSOR: {
				if ((*cur_char == '\n') || (*cur_char == '\r' && *(cur_char + 1) == '\n')) {
					if (*cur_char == '\r') {
						cur_char++;
					}

					slice8_append(preprocessor_command, SLICE_VAL8(chr, '\0'));
					if (!handle_preprocessor_command(slice8_get_data(preprocessor_command), &state)) {
						goto error;
					}
					slice8_clear(preprocessor_command);
				} else {
					slice8_append(preprocessor_command, SLICE_VAL8(chr, *cur_char));
				}
			} break;

			case STATE_OPNGL_VERTEX: {
				if (is_preprocessor_command(cur_char)) {
					cur_char += 3;
					state = STATE_PREPROCESSOR;
					break;
				}

				slice8_append(opengl_vertex_source, SLICE_VAL8(chr, *cur_char));
			} break;

			case STATE_OPNGL_FRAGMENT: {
				if (is_preprocessor_command(cur_char)) {
					cur_char += 3;
					state = STATE_PREPROCESSOR;
					break;
				}

				slice8_append(opengl_fragment_source, SLICE_VAL8(chr, *cur_char));
			} break;
		}
		cur_char++;
	}

	RendererBackend backend = renderer_get_backend(renderer);

	if (backend == RENDERER_BACKEND_OPENGL) {
		slice8_append(opengl_vertex_source, SLICE_VAL8(chr, '\0'));
		slice8_append(opengl_fragment_source, SLICE_VAL8(chr, '\0'));

		shader = renderer_create_shader_raw(renderer, slice8_get_data(opengl_vertex_source), slice8_get_data(opengl_fragment_source));
	}

error:
	slice8_destroy(opengl_vertex_source);
	slice8_destroy(opengl_fragment_source);
	slice8_destroy(preprocessor_command);

	return shader;
}

Shader *renderer_create_shader_file(const Renderer *renderer, String path) {
	size_t file_size = 0;
	char *file_data = os_read_file(path, &file_size);

	Shader *shader = renderer_create_shader(renderer, file_data, file_size);
	ls_free(file_data);

	return shader;
}

Shader *renderer_create_shader_raw(const Renderer *renderer, String vertex_source, String fragment_source) {
	Shader *shader = ls_malloc(sizeof(Shader));
	shader->backend = renderer_get_backend(renderer);

	switch (shader->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

		case RENDERER_BACKEND_OPENGL: {
#if defined(OPENGL_ENABLED)
			shader->opengl = opengl_create_shader(vertex_source, fragment_source);
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

void shader_set_uniform_mat4(const Shader *shader, String name, Matrix4 value) {
	SHADER_CALL(shader, set_uniform_mat4, name, value);
}

int32 shader_get_uniform_location(const Shader *shader, String name) {
	SHADER_CALL_R(shader, get_uniform_location, name);
	return -1;
}

uint32 shader_get_attrib_location(const Shader *shader, String name) {
	SHADER_CALL_R(shader, get_attrib_location, name);
	return -1;
}
