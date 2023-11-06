#ifndef CORE_H
#define CORE_H

#include "core/core_log.h"
#include "core/events/events.h"
#include "core/os/os.h"
#include "core/typedefs.h"
#include "core/types/string.h"
#include "renderer/renderer.h"

void core_init(RendererBackend renderer_backend);
void core_poll();
void core_deinit();

void *core_malloc(size_t size);
void *core_realloc(void *ptr, size_t size);
void core_free(void *ptr);

void core_fatal(String message, ...);

#endif // CORE_H
