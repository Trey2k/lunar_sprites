#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include "core/core.h"
#include "theme.h"

typedef struct UIElement UIElement;

void ui_draw_element(UIElement *element);
void ui_element_handle_event(UIElement *element, Event *event);

// Element Anchors
#define UI_ANCHOR_TOP 0x01 // 0000 0001
#define UI_ANCHOR_BOTTOM 0x02 // 0000 0010
#define UI_ANCHOR_LEFT 0x04 // 0000 0100
#define UI_ANCHOR_RIGHT 0x08 // 0000 1000
#define UI_ANCHOR_CENTER 0x10 // 0001 0000
#define UI_ANCHOR_FILL 0x0F // 0000 1111

typedef enum {
	UI_LAYOUT_MODE_NONE,
	UI_LAYOUT_MODE_POSITION,
	UI_LAYOUT_MODE_ANCHOR,
	UI_LAYOUT_MODE_CONTAINER,
} UILayoutMode;

typedef enum {
	UI_TEXT_WRAP_NONE,
	UI_TEXT_WRAP_WORD,
	UI_TEXT_WRAP_CHAR,
} UITextWrapMode;

typedef enum {
	UI_ALIGNMENT_BEGIN,
	UI_ALIGNMENT_CENTER,
	UI_ALIGNMENT_END,
} UIAllignment;

typedef struct {
	UILayoutMode mode;
	union {
		Vector2 position;
		uint32 anchors;
		Vector2u container_size;
	};
} UILayout;

// Destroys the given UI element.
LS_EXPORT void ui_element_destroy(UIElement *element);
// Returns the size of the element.
// Elements recalculate their size when drawn.
LS_EXPORT Vector2u ui_element_get_size(const UIElement *element);
// Returns the position of the element, relative to the top-left corner of its bounds.
LS_EXPORT Vector2 ui_element_get_position(const UIElement *element);
// Set the minimum size of the element.
LS_EXPORT void ui_element_set_min_size(UIElement *element, Vector2u min_size);
// Get the minimum size of the element.
LS_EXPORT Vector2u ui_element_get_min_size(const UIElement *element);
// Set the maximum size of the element.
LS_EXPORT void ui_element_set_max_size(UIElement *element, Vector2u max_size);
// Get the maximum size of the element.
LS_EXPORT Vector2u ui_element_get_max_size(const UIElement *element);
// Sets the layout of the element.
LS_EXPORT void ui_element_set_layout(UIElement *element, UILayout layout);
// Calculates the minimum size and position of the element based on the outer and inner bounds and anchor points.
LS_EXPORT void ui_element_calculate_position(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds);

// Label
// A label is a UI element that draws text within bounds, wrapping the text if it does not fit.
LS_EXPORT UIElement *ui_label_create(const Font *font, String text, UITextWrapMode wrap_mode);
// Sets the text of the label.
LS_EXPORT void ui_label_set_text(UIElement *element, String text);
// Sets the theme of the label. The theme is copied.
LS_EXPORT void ui_label_set_theme(UIElement *element, const UIElementTheme *theme);

// VerticalContaienr
// A VerticalContaienr is a UI element that arranges its children vertically.
LS_EXPORT UIElement *ui_vertical_container_create(uint32 spacing, UIAllignment alignment);
// Adds a child to the VerticalContaienr.
LS_EXPORT void ui_vertical_container_add_child(UIElement *element, UIElement *child);
// Removes a child from the VerticalContaienr.
LS_EXPORT void ui_vertical_container_remove_child(UIElement *element, UIElement *child);

// HorizontalContainer
// A HorizontalContainer is a UI element that arranges its children horizontally.
LS_EXPORT UIElement *ui_horizontal_container_create(uint32 spacing, UIAllignment alignment);
// Adds a child to the HorizontalContainer.
LS_EXPORT void ui_horizontal_container_add_child(UIElement *element, UIElement *child);
// Removes a child from the HorizontalContainer.
LS_EXPORT void ui_horizontal_container_remove_child(UIElement *element, UIElement *child);

typedef void (*UIElementOnClickCallback)(UIElement *element, void *user_data);

// Button
// A button is a UI element that can be clicked.
LS_EXPORT UIElement *ui_button_create(const Font *font, String text, UIElementOnClickCallback on_click, void *user_data);
// Sets the default theme of the button. The theme is copied.
LS_EXPORT void ui_button_set_theme(UIElement *element, const UIElementTheme *theme);
// Sets the theme of the button when it is hovered over. The theme is copied.
LS_EXPORT void ui_button_set_hover_theme(UIElement *element, const UIElementTheme *theme);
// Sets the theme of the button when it is clicked. The theme is copied.
LS_EXPORT void ui_button_set_click_theme(UIElement *element, const UIElementTheme *theme);
// Sets the theme of the button when it is disabled. The theme is copied.
LS_EXPORT void ui_button_set_disabled_theme(UIElement *element, const UIElementTheme *theme);
// Sets the enabled state of the button.
LS_EXPORT void ui_button_set_enabled(UIElement *element, bool enabled);

#endif // UI_ELEMENTS_H