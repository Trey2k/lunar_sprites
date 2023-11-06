#ifndef OS_H
#define OS_H

#include "core/typedefs.h"

bool os_init();
void os_deinit();

void *os_get_native_display();

#endif // OS_H