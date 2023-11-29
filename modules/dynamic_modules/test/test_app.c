#include "test_app.h"
#include "modules/dynamic_modules/test/ls_api.gen.h"

static const char TRI_VERT[] = "#version 300 es\n"
							   "in vec3 pos;\n"
							   "void main() {\n"
							   "	gl_Position = vec4(pos, 1.0);\n"
							   "}\n";

static const char TRI_FRAG[] = "#version 300 es\n"
							   "precision mediump float;\n"
							   "out vec4 FragColor;\n"
							   "void main() {\n"
							   "	FragColor = vec4(1.0, 0, 0, 1);\n"
							   "}\n";

typedef struct {
	LSCore *core;
	Renderer *renderer;

	InputManager *input_manager;

	LSWindow *root_window;

	bool should_stop;

	Shader *tri_shader;
	VertexArray *tri_vao;

	float64 timer;
} TestApplication;

static void check_input(TestApplication *test_application);
static void input_handler(Event *event, void *user_data);

static const WindowConfig ROOT_WINDOW_CONFIG = {
	.position = vec2i(0, 0),
	.size = vec2i(800, 600),
	.title = "Lunar Sprites Test Application",
	.fullscreen = false,
	.hidden = false,
	.root_window = true,
};

ApplicationInterface register_application() {
	ApplicationInterface application_interface;
	application_interface.init = test_app_init;
	application_interface.start = test_app_start;
	application_interface.deinit = test_app_deinit;
	application_interface.update = test_app_update;
	application_interface.should_stop = test_app_should_stop;

	application_interface.user_data = ls_calloc(1, sizeof(TestApplication));

	return application_interface;
}

const LSWindow *test_app_init(LSCore *core, Renderer *renderer, void *user_data) {
	TestApplication *test_application = user_data;
	test_application->core = core;
	test_application->renderer = renderer;
	test_application->should_stop = false;
	test_application->root_window = renderer_create_window(renderer, ROOT_WINDOW_CONFIG);
	test_application->input_manager = core_get_input_manager(core);

	return test_application->root_window;
}

void test_app_start(void *user_data) {
	TestApplication *test_application = user_data;
	core_add_event_handler(test_application->core, input_handler, test_application);

	renderer_set_clear_color(test_application->renderer, 0.0, 1.0, 0.0, 1.0);

	float32 rect_verticies[] = {
		-0.5, -0.5, 0.0,
		0.5, -0.5, 0.0,
		0.5, 0.5, 0.0,
		-0.5, 0.5, 0.0
	};

	uint32 rect_indecies[] = {
		0, 1, 2,
		2, 3, 0
	};

	VertexBuffer *tri_vbo = renderer_create_vertex_buffer(test_application->renderer, rect_verticies, sizeof(rect_verticies));
	BufferElement BufferLayoutElms[] = {
		{ SHADER_DATA_TYPE_FLOAT3, "pos", false },
	};

	BufferLayout *tri_layout = buffer_layout_create(BufferLayoutElms, 1);
	vertex_buffer_set_layout(tri_vbo, tri_layout);

	IndexBuffer *tri_ibo = renderer_create_index_buffer(test_application->renderer, rect_indecies, sizeof(rect_indecies));

	test_application->tri_shader = renderer_create_shader(test_application->renderer, TRI_VERT, TRI_FRAG);

	LS_ASSERT(test_application->tri_shader);

	test_application->tri_vao = renderer_create_vertex_array(test_application->renderer);

	vertex_arrray_add_vertex_buffer(test_application->tri_vao, tri_vbo);
	vertex_array_set_index_buffer(test_application->tri_vao, tri_ibo);

	vertex_array_bind(test_application->tri_vao);

	shader_bind(test_application->tri_shader);
}

void test_app_deinit(void *user_data) {
	TestApplication *test_application = user_data;

	shader_destroy(test_application->tri_shader);

	vertex_array_destroy(test_application->tri_vao);

	ls_free(test_application);
}

void test_app_update(float64 delta_time, void *user_data) {
	TestApplication *test_application = user_data;
	test_application->timer += delta_time;
	check_input(test_application);

	vertex_array_draw(test_application->tri_vao);

	if (test_application->timer > 1.0) {
		ls_printf("FPS: %f\n", 1.0 / delta_time);
		test_application->timer = 0.0;
	}
}

bool test_app_should_stop(void *user_data) {
	TestApplication *test_application = user_data;
	return test_application->should_stop;
}

static void check_input(TestApplication *test_application) {
	if (input_is_key_just_released(test_application->input_manager, LS_KEY_ESCAPE) ||
			input_is_key_just_released(test_application->input_manager, LS_KEY_Q)) {
		test_application->should_stop = true;
	}
}

static void input_handler(Event *e, void *user_data) {
	LS_ASSERT(e);

	TestApplication *test_application = user_data;

	switch (e->type) {
		case EVENT_WINDOW_CLOSE: {
			test_application->should_stop = true;
		} break;

		case EVENT_KEY: {
			switch (e->key.type) {
				case EVENT_KEY_PRESSED: {
					if (!e->key.repeat) {
						ls_printf("Key pressed: %s\n", keycode_to_string(e->key.keycode));
					} else {
						ls_printf("Key repeated: %s\n", keycode_to_string(e->key.keycode));
					}
				} break;

				case EVENT_KEY_RELEASED: {
					ls_printf("Key released: %s\n", keycode_to_string(e->key.keycode));
				} break;
			}
		} break;

		case EVENT_MOUSE: {
			switch (e->mouse.type) {
				case EVENT_MOUSE_PRESSED: {
					ls_printf("Mouse pressed: %s\n", mouse_button_to_string(e->mouse.button));
				} break;

				case EVENT_MOUSE_RELEASED: {
					ls_printf("Mouse released: %s\n", mouse_button_to_string(e->mouse.button));
				} break;

				case EVENT_MOUSE_MOVED: {
					ls_printf("Mouse moved: %d, %d\n", e->mouse.position.x, e->mouse.position.y);
				} break;

				case EVENT_MOUSE_ENTERED: {
					ls_printf("Mouse entered: %d, %d\n", e->mouse.position.x, e->mouse.position.y);
				} break;

				case EVENT_MOUSE_LEFT: {
					ls_printf("Mouse left: %d, %d\n", e->mouse.position.x, e->mouse.position.y);
				} break;
			}
		} break;

		default: {
		} break;
	}
}