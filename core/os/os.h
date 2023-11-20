#ifndef OS_H
#define OS_H

#include "core/api.h"
#include "core/input/input_manager.h"
#include "core/types/typedefs.h"

#include "platform/os.h"

typedef struct OS OS;

OS *ls_create_os(InputManager *input_manager);
void ls_destroy_os(OS *os);

LSNativeDisplayType os_get_native_display(const OS *os);

const PlatformOS *os_get_platform_os(const OS *os);

#endif // OS_H