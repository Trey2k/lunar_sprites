#ifndef UI_BUTTON_H
#define UI_BUTTON_H

#include "core/core.h"

#include "modules/ui/elements.h"
#include "modules/ui/theme.h"

typedef struct {
	UIElement *label;

	UIElementTheme default_theme;
	UIElementTheme hover_theme;
	UIElementTheme click_theme;
	UIElementTheme disabled_theme;

	bool pressed;
	bool hovering;

	bool enabled;

	UIElementOnClickCallback on_click;

	void *user_data;
} UIButton;

void ui_button_draw(UIElement *element);
void ui_button_destroy(UIButton *element);
void ui_button_calculate_size(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds);

void ui_button_set_layout(UIElement *element, UILayout layout);

#endif // UI_BUTTON_H