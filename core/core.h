#ifndef CORE_H
#define CORE_H

#include "core/core_log.h"
#include "core/os/os.h"
#include "core/typedefs.h"

#include "renderer/renderer.h"

void core_init();
void core_deinit();

void *core_malloc(size_t size);
void *core_realloc(void *ptr, size_t size);
void core_free(void *ptr);

void core_fatal(String message, ...);

OS *core_get_os();

Renderer *core_get_renderer();

#endif // CORE_H
