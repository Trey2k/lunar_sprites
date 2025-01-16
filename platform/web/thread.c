#include "core/os/thread.h"

#include "core/core.h"

#include <pthread.h>

typedef struct LSMutex {
	pthread_mutex_t mutex;
} LSMutex;

LSMutex *os_mutex_create() {
	LSMutex *mutex = ls_malloc(sizeof(LSMutex));
	pthread_mutex_init(&mutex->mutex, NULL);

	return mutex;
}

void os_mutex_destroy(LSMutex *mutex) {
	LS_ASSERT(mutex);

	pthread_mutex_destroy(&mutex->mutex);
	ls_free(mutex);
}

void os_mutex_lock(LSMutex *mutex) {
	LS_ASSERT(mutex);

	pthread_mutex_lock(&mutex->mutex);
}

void os_mutex_unlock(LSMutex *mutex) {
	LS_ASSERT(mutex);

	pthread_mutex_unlock(&mutex->mutex);
}

typedef struct LSThread {
	pthread_t thread;
} LSThread;

void *thread_function(void *data) {
	LSThreadFunction function = (LSThreadFunction)data;
	function(NULL);

	return NULL;
}

LSThread *os_thread_create(LSThreadFunction function, void *data) {
	LSThread *thread = ls_malloc(sizeof(LSThread));
	pthread_create(&thread->thread, NULL, thread_function, function);

	return thread;
}

void os_thread_destroy(LSThread *thread) {
	LS_ASSERT(thread);

	ls_free(thread);
}

void os_thread_join(LSThread *thread) {
	LS_ASSERT(thread);

	pthread_join(thread->thread, NULL);
}