#ifndef UI_ELEMENTS_H
#define UI_ELEMENTS_H

#include "core/core.h"
#include "theme.h"

typedef struct UIElement UIElement;

void ui_draw_element(UIElement *element, Vector2u outer_bounds, Vector2u inner_bounds);

LS_EXPORT void ui_element_destroy(UIElement *element);

LS_EXPORT Vector2u ui_element_get_size(const UIElement *element);
LS_EXPORT Vector2u ui_element_get_position(const UIElement *element);

LS_EXPORT const UIElementTheme *ui_element_get_theme(const UIElement *element);

// Label
//
LS_EXPORT UIElement *ui_label_create(const UIElementTheme *theme, String text, Vector2u position);

LS_EXPORT void ui_label_set_text(UIElement *element, String text);

#endif // UI_ELEMENTS_H