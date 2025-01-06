#ifndef UI_H
#define UI_H

#include "core/core.h"
#include "core/input/input_manager.h"
#include "renderer/renderer.h"

#include "elements.h"

void ui_init(Renderer *renderer, LSCore *core, const LSWindow *window);
void ui_deinit();

void ui_draw(const Texture *texture, Color color, const float32 *vertices, const float32 *tcoords, const uint32 *indices, size_t nverts, size_t nindices, uint32 radius, Vector2u element_size);
void ui_draw_rect(const Texture *texture, Color color, uint32 radius, Vector2u position, Vector2u size);

Vector2u ui_draw_text(const UIElementTheme *theme, String text, Vector2u position);
Vector2u ui_get_text_size(const UIElementTheme *theme, String text);

InputManager *ui_get_input_manager();

// Adds an element to the UI.
// The UI will take ownership of the element and free it when it is removed.
LS_EXPORT void ui_add_element(UIElement *element);
LS_EXPORT void ui_remove_element(const UIElement *element);

#endif // UI_H