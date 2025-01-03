#include "test_app.h"
#include "ls_api.gen.h"
#include "modules/dynamic_modules/test/ls_api.gen.h"

typedef struct {
	LSCore *core;
	Renderer *renderer;

	InputManager *input_manager;

	LSWindow *root_window;

	bool should_stop;

	Sprite *sprite;
	Camera *camera;

	float64 timer;

	UIElement *label;
	Font *font;

	char *fps_text;
} TestApplication;

static void check_input(TestApplication *test_application);
static void input_handler(Event *event, void *user_data);

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

	const WindowConfig ROOT_WINDOW_CONFIG = {
		.position = vec2i(0, 0),
		.size = vec2i(800, 600),
		.title = "Lunar Sprites Test Application",
		.fullscreen = false,
		.hidden = false,
		.root_window = true,
	};

	test_application->root_window = renderer_create_window(renderer, ROOT_WINDOW_CONFIG);
	test_application->input_manager = core_get_input_manager(core);

	test_application->fps_text = ls_str_format("FPS: %f", 0.0);

	return test_application->root_window;
}

void test_app_start(void *user_data) {
	TestApplication *test_application = user_data;

	EventManager *event_handler = core_get_event_manager(test_application->core);
	event_manager_add_handler(event_handler, input_handler, test_application);

	renderer_set_clear_color(test_application->renderer, 0.0, 0.0, 0.0, 1.0);

	test_application->font = font_create("default.ttf");

	LS_ASSERT(test_application->font);

	test_application->label = ui_label_create(test_application->font, "Hello, World!", vec2u(0, 0));

	ui_add_element(test_application->label);

	test_application->sprite = renderer_create_sprite(test_application->renderer, "moon.png", vec2(0, 0), vec2(0.25, 0.25), 0.0);
	Vector2u viewport_size = window_get_size(test_application->root_window);
	test_application->camera = camera_create(math_deg_to_rad(100), (float32)viewport_size.y / (float32)viewport_size.x, 0.1, 100.0);
	camera_set_active(test_application->camera);
}

void test_app_deinit(void *user_data) {
	TestApplication *test_application = user_data;

	sprite_destroy(test_application->sprite);
	camera_destroy(test_application->camera);
	font_destroy(test_application->font);
	ui_element_destroy(test_application->label);

	ls_free(test_application->fps_text);

	ls_free(test_application);
}

void test_app_update(float64 delta_time, void *user_data) {
	TestApplication *test_application = user_data;

	float32 rotation = sprite_get_rotation(test_application->sprite);
	rotation += 0.01 * delta_time;
	if (rotation >= PI * 2) {
		rotation = 0.0;
	}
	sprite_set_rotation(test_application->sprite, rotation);

	if (input_is_key_pressed(test_application->input_manager, LS_KEY_W)) {
		camera_move(test_application->camera, vec3(0.0, 0.01, 0.0));
	}

	if (input_is_key_pressed(test_application->input_manager, LS_KEY_S)) {
		camera_move(test_application->camera, vec3(0.0, -0.01, 0.0));
	}

	if (input_is_key_pressed(test_application->input_manager, LS_KEY_A)) {
		camera_move(test_application->camera, vec3(-0.01, 0.0, 0.0));
	}

	if (input_is_key_pressed(test_application->input_manager, LS_KEY_D)) {
		camera_move(test_application->camera, vec3(0.01, 0.0, 0.0));
	}

	test_application->timer += delta_time;
	check_input(test_application);

	sprite_draw(test_application->sprite);

	if (test_application->timer > 1.0) {
		ls_printf("FPS: %f\n", 1.0 / delta_time);
		ls_free(test_application->fps_text);
		test_application->fps_text = ls_str_format("FPS: %f", 1.0 / delta_time);
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
		case EVENT_WINDOW: {
			switch (e->window.type) {
				case EVENT_WINDOW_CLOSE: {
					test_application->should_stop = true;
				} break;
				default: {
				} break;
			}
		} break;
		default: {
		} break;
	}
}