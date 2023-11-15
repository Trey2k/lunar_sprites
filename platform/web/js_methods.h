#ifndef JS_METHODS_H
#define JS_METHODS_H

#include "core/types/string.h"
#include "core/types/typedefs.h"

void new_canvas(String canvas_id, int32 width, int32 height, int32 x, int32 y);
void delete_canvas(String canvas_id);
void set_canvas_focused(String canvas_id, bool focused);
void set_canvas_size(String canvas_id, int32 width, int32 height);
void set_canvas_position(String canvas_id, int32 x, int32 y);
void set_canvas_visibility(String canvas_id, bool visible);
void set_canvas_fullscreen(String canvas_id, bool fullscreen);

#endif // JS_METHODS_H