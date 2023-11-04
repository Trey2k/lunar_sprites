#ifndef OS_H
#define OS_H

#include "core/event/event_manager.h"

typedef struct OS OS;

OS *os_create(const EventManager *event_manager);
void os_destroy(OS *os);

#endif // OS_H