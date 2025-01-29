#ifndef UI_H
#define UI_H

#include "core/core.h"
#include "core/input/input_manager.h"
#include "renderer/renderer.h"

#include "elements.h"

void ui_init(const LSWindow *window);
void ui_deinit();

// Adds an element to the UI.
// The UI will take ownership of the element and free it when it is removed.
LS_EXPORT void ui_add_element(UIElement *element);
LS_EXPORT void ui_remove_element(const UIElement *element);

#endif // UI_H