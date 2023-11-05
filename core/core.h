#ifndef CORE_H
#define CORE_H

#include "core/core_log.h"
#include "core/event/event_manager.h"
#include "core/os/os.h"
#include "core/string.h"
#include "core/typedefs.h"

#include "renderer/renderer.h"

void core_init();
void core_poll();
void core_deinit();

void *core_malloc(size_t size);
void *core_realloc(void *ptr, size_t size);
void core_free(void *ptr);

void core_fatal(const String message, ...);

void core_add_event_handler(EventHandler handler, void *user_data);

const EventManager *core_get_event_manager();
const Input *core_get_input();
const OS *core_get_os();
const Renderer *core_get_renderer();

#endif // CORE_H
