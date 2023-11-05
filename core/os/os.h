#ifndef OS_H
#define OS_H

#include "core/input/input.h"

typedef struct OS OS;

OS *os_create(Input *input);
void os_destroy(OS *os);

#endif // OS_H