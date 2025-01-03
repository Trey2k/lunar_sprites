#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include "core/core.h"
#include "theme.h"

typedef struct UIElement UIElement;

void ui_draw_element(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds);

// Element Anchors
#define UI_ANCHOR_TOP 0x01 // 0000 0001
#define UI_ANCHOR_BOTTOM 0x02 // 0000 0010
#define UI_ANCHOR_LEFT 0x04 // 0000 0100
#define UI_ANCHOR_RIGHT 0x08 // 0000 1000
#define UI_ANCHOR_CENTER 0x10 // 0001 0000
#define UI_ANCHOR_FILL 0x0F // 0000 1111

// Destroys the given UI element.
LS_EXPORT void ui_element_destroy(UIElement *element);
// Returns the size of the element.
// Elements recalculate their size when drawn.
LS_EXPORT Vector2u ui_element_get_size(const UIElement *element);
// Returns the position of the element, relative to the top-left corner of its bounds.
LS_EXPORT Vector2u ui_element_get_position(const UIElement *element);
// Sets the anchor points of the element.
LS_EXPORT void ui_element_set_anchor(UIElement *element, uint32 anchors);

// Label
// A label is a UI element that draws text within bounds, wrapping the text if it does not fit.
LS_EXPORT UIElement *ui_label_create(const Font *font, String text, Vector2u position);
// Returns the theme of the label for modifying.
LS_EXPORT UIElementTheme *ui_label_get_theme(UIElement *element);
// Sets the text of the label.
LS_EXPORT void ui_label_set_text(UIElement *element, String text);

// VBoxContainer
// A VBoxContainer is a UI element that arranges its children vertically.
// LS_EXPORT UIElement *ui_container_vbox_create(Vector2u position);
// // Adds a child to the VBoxContainer.
// LS_EXPORT void ui_container_vbox_add_child(UIElement *element, UIElement *child);
// // Removes a child from the VBoxContainer.
// LS_EXPORT void ui_container_vbox_remove_child(UIElement *element, UIElement *child);

#endif // UI_ELEMENTS_H