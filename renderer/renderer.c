#include "renderer/renderer.h"

#include "renderer/renderer_interface.h"

#include "core/core.h"

#if defined(OPENGL_ENABLED)
#include "renderer/opengl/renderer.h"
#endif

struct Renderer {
	LSCore *core;

	RendererInterface interface;

	FlagValue *backend_flag;
	RendererBackend backend;
	union {
#if defined(OPENGL_ENABLED)
		OpenGLRenderer *opengl;
#endif
	};
};

static void check_flags(Renderer *renderer);

Renderer *renderer_create(LSCore *core) {
	Renderer *renderer = ls_malloc(sizeof(Renderer));
	renderer->core = core;

	renderer->backend_flag = flag_manager_register(core_get_flag_manager(core),
			"renderer-backend", FLAG_TYPE_STRING, FLAG_VAL(str, "OPENGL"),
			"The renderer backend to use. Valid values are NONE and OPENGL.");

	return renderer;
}

void renderer_start(Renderer *renderer) {
	check_flags(renderer);
	switch (renderer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

#if defined(OPENGL_ENABLED)
		case RENDERER_BACKEND_OPENGL: {
			renderer->opengl = opengl_renderer_create(renderer->core);
			opengl_register_methods(&renderer->interface);
		} break;
#endif

		default:
			ls_log_fatal("Unknown renderer backend: %d\n", renderer->backend);
	};
}

void renderer_destroy(Renderer *renderer) {
	switch (renderer->backend) {
		case RENDERER_BACKEND_NONE: {
		} break;

		case RENDERER_BACKEND_OPENGL: {
#if defined(OPENGL_ENABLED)
			opengl_renderer_destroy(renderer->opengl);
#endif

		} break;
	}
}

RendererBackend renderer_get_backend(const Renderer *renderer) {
	return renderer->backend;
}

LSCore *renderer_get_core(const Renderer *renderer) {
	return renderer->core;
}

#if defined(OPENGL_ENABLED)
OpenGLRenderer *renderer_get_opengl(const Renderer *renderer) {
	return renderer->opengl;
}
#endif

void renderer_set_clear_color(const Renderer *renderer, float32 r, float32 g, float32 b, float32 a) {
	renderer->interface.set_clear_color(r, g, b, a);
}

void renderer_clear(const Renderer *renderer) {
	renderer->interface.clear();
}

uint32 renderer_create_vertex_buffer(const Renderer *renderer, size_t size, void *data, UsageHint usage_hint) {
	return renderer->interface.create_vertex_buffer(size, data, usage_hint);
}

void renderer_bind_vertex_buffer(const Renderer *renderer, TargetHint target, uint32 vertex_buffer) {
	renderer->interface.bind_vertex_buffer(target, vertex_buffer);
}

void renderer_destroy_vertex_buffer(const Renderer *renderer, uint32 vertex_buffer) {
	renderer->interface.destroy_vertex_buffer(vertex_buffer);
}

uint32 renderer_create_vertex_array(const Renderer *renderer) {
	return renderer->interface.create_vertex_array();
}

void renderer_destroy_vertex_array(const Renderer *renderer, uint32 vertex_array) {
	renderer->interface.destroy_vertex_array(vertex_array);
}

void renderer_bind_vertex_array(const Renderer *renderer, uint32 vertex_array) {
	renderer->interface.bind_vertex_array(vertex_array);
}

void renderer_set_vertex_array_uniform(const Renderer *renderer, uint32 vertex_array, int32 uniform_position, uint32 size, DataType data_type, bool normalized, size_t stride, size_t offset) {
	renderer->interface.set_vertex_array_uniform(vertex_array, uniform_position, size, data_type, normalized, stride, offset);
}

void renderer_enable_vertex_attrib_array(const Renderer *renderer, uint32 index) {
	renderer->interface.enable_vertex_attrib_array(index);
}

void renderer_draw_arrays(const Renderer *renderer, DrawMode draw_mode, size_t first, size_t count) {
	renderer->interface.draw_arrays(draw_mode, first, count);
}

static void check_flags(Renderer *renderer) {
	ls_str_to_upper(renderer->backend_flag->str);

	if (ls_str_equals(renderer->backend_flag->str, "NONE")) {
		renderer->backend = RENDERER_BACKEND_NONE;
	} else if (ls_str_equals(renderer->backend_flag->str, "OPENGL")) {
		renderer->backend = RENDERER_BACKEND_OPENGL;
	} else {
		ls_log_fatal("Invalid renderer backend: %s\n", renderer->backend_flag->str);
	}
}