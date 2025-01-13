#ifndef THREAD_H
#define THREAD_H

#include "core/api.h"

typedef struct LSMutex LSMutex;

LS_EXPORT LSMutex *os_mutex_create();
LS_EXPORT void os_mutex_destroy(LSMutex *mutex);

LS_EXPORT void os_mutex_lock(LSMutex *mutex);
LS_EXPORT void os_mutex_unlock(LSMutex *mutex);

typedef struct LSThread LSThread;

typedef void (*LSThreadFunction)(void *data);

LS_EXPORT LSThread *os_thread_create(LSThreadFunction function, void *data);
LS_EXPORT void os_thread_destroy(LSThread *thread);
LS_EXPORT void os_thread_join(LSThread *thread);

#endif // THREAD_H