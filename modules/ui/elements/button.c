#include "button.h"

#include "core/input/input_manager.h"
#include "core/types/color.h"
#include "elements.h"
#include "modules/ui/elements.h"
#include "modules/ui/theme.h"
#include "modules/ui/ui.h"

UIElement *ui_button_create(const Font *font, String text, UIElementOnClickCallback on_click, void *user_data) {
	UIElementTheme default_theme = { 0 };
	default_theme.background_color = color_create(0.18f, 0.18f, 0.18f, 1.0f);
	default_theme.border_color = color_create_hex(0x323ca5);
	default_theme.font_color = color_create_hex(0xe1e1ff);
	default_theme.border_size = 2;
	default_theme.radius = 7;
	default_theme.font_size = 64;
	default_theme.text_alignment = UI_TEXT_ALIGN_CENTER;
	default_theme.font = font;

	UIElementTheme hover_theme = { 0 };
	hover_theme.background_color = color_create(0.25f, 0.25f, 0.25f, 1.0f);
	hover_theme.border_color = color_create_hex(0x3943ac);
	hover_theme.font_color = color_create_hex(0xffffff);
	hover_theme.border_size = 2;
	hover_theme.radius = 7;
	hover_theme.font_size = 64;
	hover_theme.text_alignment = UI_TEXT_ALIGN_CENTER;
	hover_theme.font = font;

	UIElementTheme click_theme = { 0 };
	click_theme.background_color = color_create(0.12f, 0.12f, 0.12f, 1.0f);
	click_theme.border_color = color_create_hex(0x2c369f);
	click_theme.font_color = color_create_hex(0xe1e1ff);
	click_theme.border_size = 2;
	click_theme.radius = 7;
	click_theme.font_size = 64;
	click_theme.text_alignment = UI_TEXT_ALIGN_CENTER;
	click_theme.font = font;

	UIElementTheme disabled_theme = { 0 };
	disabled_theme.background_color = color_create(0.08f, 0.08f, 0.08f, 1.0f);
	disabled_theme.border_color = color_create_hex(0x232d96);
	disabled_theme.font_color = color_create_hex(0xc8c8cd);
	disabled_theme.border_size = 2;
	disabled_theme.radius = 7;
	disabled_theme.font_size = 64;
	disabled_theme.text_alignment = UI_TEXT_ALIGN_CENTER;
	disabled_theme.font = font;

	UIElement *element = ls_malloc(sizeof(UIElement));
	element->type = UI_ELEMENT_TYPE_BUTTON;
	element->button.label = ui_label_create(font, text, UI_TEXT_WRAP_CHAR);

	ui_label_set_theme(element->button.label, &default_theme);
	element->button.default_theme = default_theme;
	element->button.hover_theme = hover_theme;
	element->button.click_theme = click_theme;
	element->button.disabled_theme = disabled_theme;

	element->button.pressed = false;
	element->button.hovering = false;
	element->button.enabled = true;
	element->button.user_data = user_data;

	element->button.on_click = on_click;
	return element;
}

void ui_button_destroy(UIButton *button) {
	ui_element_destroy(button->label);
}

void ui_button_draw(UIElement *element) {
	UIButton *button = &element->button;

	button->label->position = element->position;
	button->label->size = element->size;
	InputManager *input_manager = ui_get_input_manager();
	Vector2u mouse_pos = input_get_mouse_position(input_manager);

	if (button->hovering) {
		if (mouse_pos.x <= element->position.x || mouse_pos.x >= element->position.x + element->size.x ||
				mouse_pos.y <= element->position.y || mouse_pos.y >= element->position.y + element->size.y) {
			button->hovering = false;
			ui_label_set_theme(button->label, &button->default_theme);
		}
	}

	ui_draw_element(button->label);
}

void ui_button_calculate_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds) {
	UIButton *button = &element->button;
	ui_element_calculate_position(button->label, outer_bounds, inner_bounds);

	element->size = button->label->size;
	element->position = button->label->position;
}

void ui_button_handle_event(UIElement *element, Event *event) {
	UIButton *button = &element->button;

	if (event->type != EVENT_MOUSE || !button->enabled) {
		return;
	}

	if (event->mouse.type == EVENT_MOUSE_PRESSED) {
		button->pressed = true;
		ui_label_set_theme(element->button.label, &button->click_theme);
	} else if (event->mouse.type == EVENT_MOUSE_RELEASED) {
		if (button->pressed) {
			button->on_click(element, button->user_data);
			ui_label_set_theme(element->button.label, &button->hover_theme);
		}

		button->pressed = false;
	} else if (event->mouse.type == EVENT_MOUSE_MOVED) {
		if (!button->hovering) {
			button->hovering = true;
			ui_label_set_theme(element->button.label, &button->hover_theme);
		}
	}
}

void ui_button_set_theme(UIElement *element, const UIElementTheme *theme) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_BUTTON);

	ui_label_set_theme(element->button.label, theme);
	element->button.default_theme = *theme;
}

void ui_button_set_hover_theme(UIElement *element, const UIElementTheme *theme) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_BUTTON);

	element->button.hover_theme = *theme;
}

void ui_button_set_click_theme(UIElement *element, const UIElementTheme *theme) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_BUTTON);

	element->button.click_theme = *theme;
}

void ui_button_set_disabled_theme(UIElement *element, const UIElementTheme *theme) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_BUTTON);

	element->button.disabled_theme = *theme;
}

void ui_button_set_enabled(UIElement *element, bool enabled) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_BUTTON);

	element->button.enabled = enabled;

	if (enabled) {
		if (element->button.hovering) {
			ui_label_set_theme(element->button.label, &element->button.hover_theme);
		} else {
			ui_label_set_theme(element->button.label, &element->button.default_theme);
		}
	} else {
		ui_label_set_theme(element->button.label, &element->button.disabled_theme);
	}
}

void ui_button_set_layout(UIElement *element, UILayout layout) {
	LS_ASSERT(element->type == UI_ELEMENT_TYPE_BUTTON);

	ui_element_set_layout(element->button.label, layout);
	element->layout = layout;
}