#ifndef OS_H
#define OS_H

typedef struct OS OS;

OS *os_create();
void os_destroy(OS *os);

#endif // OS_H