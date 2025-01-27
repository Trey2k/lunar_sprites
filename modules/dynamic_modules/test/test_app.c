#include "test_app.h"
#include "ls_api.gen.h"
#include "modules/dynamic_modules/test/ls_api.gen.h"

typedef struct {
	LSCore *core;

	InputManager *input_manager;

	LSWindow *root_window;

	bool should_stop;

	Object *sprite;
	Camera *camera;

	float64 timer;

	UIElement *title_label;

	UIElement *start_button;
	UIElement *coming_soon_button;
	UIElement *exit_button;

	UIElement *vertical_container;
	UIElement *horizontal_container;

	Sound *chirp_sound;
	Sound *fox_sound;

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

LSWindow *test_app_init(LSCore *core, void *user_data) {
	TestApplication *test_application = user_data;
	test_application->core = core;
	test_application->should_stop = false;

	test_application->chirp_sound = sound_create("chirp_test.wav");
	test_application->fox_sound = sound_create("quick_fox.wav");

	const WindowConfig ROOT_WINDOW_CONFIG = {
		.position = vec2u(0, 0),
		.size = vec2u(1200, 800),
		.min_size = vec2u(0, 0),
		.max_size = vec2u(0, 0),
		.title = "Lunar Sprites Test Application",
		.fullscreen = false,
		.hidden = false,
		.root_window = true,
	};

	test_application->root_window = renderer_create_window(ROOT_WINDOW_CONFIG);
	test_application->input_manager = core_get_input_manager(core);

	test_application->fps_text = ls_str_format("FPS: %f", 60.0);

	return test_application->root_window;
}

static void on_button_click(UIElement *element, void *user_data) {
	TestApplication *test_application = (TestApplication *)user_data;
	sound_play(test_application->fox_sound);

	sound_play(test_application->chirp_sound);
	ls_printf("Button clicked!\n");
}

static void on_exit_button_click(UIElement *element, void *user_data) {
	TestApplication *test_application = user_data;
	test_application->should_stop = true;
}

void test_app_start(void *user_data) {
	TestApplication *test_application = user_data;

	EventManager *event_handler = core_get_event_manager(test_application->core);
	event_manager_add_handler(event_handler, input_handler, test_application);

	renderer_set_clear_color(0.0, 0.0, 0.0, 1.0);

	test_application->font = font_create("default.ttf");

	LS_ASSERT(test_application->font);

	UIElementTheme title_theme = { 0 };
	title_theme.font = test_application->font;
	title_theme.font_size = 128;
	title_theme.text_alignment = UI_TEXT_ALIGN_CENTER;
	title_theme.font_color = COLOR_LIGHT_YELLOW;
	title_theme.background_color = COLOR_TRANSPARENT;
	title_theme.border_color = COLOR_TRANSPARENT;
	title_theme.border_size = 0;
	title_theme.radius = 0;
	UILayout title_layout = { 0 };
	title_layout.mode = UI_LAYOUT_MODE_ANCHOR;
	title_layout.anchors = UI_ANCHOR_TOP | UI_ANCHOR_CENTER;
	test_application->title_label = ui_label_create(test_application->font, "Lunar Sprites Test App!", UI_TEXT_WRAP_NONE);
	ui_element_set_layout(test_application->title_label, title_layout);

	ui_label_set_theme(test_application->title_label, &title_theme);
	ui_add_element(test_application->title_label);

	test_application->start_button = ui_button_create(test_application->font, "Start", on_button_click, test_application);
	test_application->coming_soon_button = ui_button_create(test_application->font, "Coming Soon", on_button_click, test_application);
	ui_button_set_enabled(test_application->coming_soon_button, false);
	test_application->exit_button = ui_button_create(test_application->font, "Exit", on_exit_button_click, test_application);

	test_application->vertical_container = ui_vertical_container_create(10, UI_ALIGNMENT_CENTER);

	UILayout hcontainer_layout = { 0 };
	hcontainer_layout.mode = UI_LAYOUT_MODE_ANCHOR;
	hcontainer_layout.anchors = UI_ANCHOR_LEFT | UI_ANCHOR_RIGHT | UI_ANCHOR_CENTER;
	test_application->horizontal_container = ui_horizontal_container_create(10, UI_ALIGNMENT_CENTER);
	ui_element_set_layout(test_application->horizontal_container, hcontainer_layout);

	ui_vertical_container_add_child(test_application->vertical_container, test_application->start_button);
	ui_vertical_container_add_child(test_application->vertical_container, test_application->coming_soon_button);
	ui_vertical_container_add_child(test_application->vertical_container, test_application->exit_button);

	ui_horizontal_container_add_child(test_application->horizontal_container, test_application->vertical_container);

	ui_add_element(test_application->horizontal_container);

	test_application->sprite = object_create(BSC("Sprite"));
	Resource *texture = resource_create(BSC("moon.png"));
	object_set_property(test_application->sprite, BSC("position"), VARIANT_VECTOR2I(vec2i(0, 0)));
	object_set_property(test_application->sprite, BSC("scale"), VARIANT_VECTOR2(vec2(0.15, 0.15)));
	object_set_property(test_application->sprite, BSC("rotation"), VARIANT_FLOAT(0.0));
	object_set_property(test_application->sprite, BSC("texture"), VARIANT_RESOURCE(texture));

	Vector2u viewport_size = renderer_get_viewport_size();
	test_application->camera = camera_create(math_deg_to_rad(100), (float32)viewport_size.y / (float32)viewport_size.x, 0.1, 100.0);
	camera_set_active(test_application->camera);
}

void test_app_deinit(void *user_data) {
	TestApplication *test_application = user_data;

	object_unref(test_application->sprite);
	camera_destroy(test_application->camera);
	font_destroy(test_application->font);

	ls_free(test_application->fps_text);

	sound_destroy(test_application->chirp_sound);
	sound_destroy(test_application->fox_sound);

	ls_free(test_application);
}

void test_app_update(float64 delta_time, void *user_data) {
	TestApplication *test_application = user_data;

	Vector2u viewport_size = renderer_get_viewport_size();

	object_set_property(test_application->sprite, BSC("position"), VARIANT_VECTOR2I(vec2i(viewport_size.x / 2, viewport_size.y / 2)));

	Variant rotation = object_get_property(test_application->sprite, BSC("rotation"));
	rotation.FLOAT += 0.01;
	if (rotation.FLOAT >= PI) {
		rotation.FLOAT = -PI;
	}

	object_set_property(test_application->sprite, BSC("rotation"), rotation);

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

	font_draw_text(test_application->font, 24, COLOR_WHITE, test_application->fps_text, vec2(10, 10));

	test_application->timer += delta_time;
	check_input(test_application);

	object_draw(test_application->sprite, delta_time);

	if (test_application->timer > 0.25) {
		Variant test = json_parse(BSC("{\"test\": [1, 2.5, 3, {\"bool\": false}], \"test2\": {\"test3\": \"test4\"}}"));
		ls_printf("Test: %V\n", test);
		BString json = json_stringify(test);
		ls_printf("JSON: %S\n", json);
		bstring_unref(json);
		variant_unref(test);

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