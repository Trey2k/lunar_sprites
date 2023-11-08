#ifndef OS_H
#define OS_H

#include "core/types/typedefs.h"

#include "platform/os.h"

#include "core/input/input_manager.h"

typedef struct OS OS;

OS *ls_create_os();
void ls_destroy_os(OS *os);

void *os_get_native_display(const OS *os);

PlatformOS *os_get_platform_os(const OS *os);

// Defined in core/lunar_sprites.c
const OS *ls_get_os();

#endif // OS_H