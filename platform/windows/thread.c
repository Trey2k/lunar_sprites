#include "core/os/thread.h"

#include "core/core.h"

#include <windows.h>

typedef struct LSMutex {
	CRITICAL_SECTION mutex;
} LSMutex;

LSMutex *os_mutex_create() {
	LSMutex *mutex = ls_malloc(sizeof(LSMutex));
	InitializeCriticalSection(&mutex->mutex);

	return mutex;
}

void os_mutex_destroy(LSMutex *mutex) {
	LS_ASSERT(mutex);

	DeleteCriticalSection(&mutex->mutex);
	ls_free(mutex);
}

void os_mutex_lock(LSMutex *mutex) {
	LS_ASSERT(mutex);

	EnterCriticalSection(&mutex->mutex);
}

void os_mutex_unlock(LSMutex *mutex) {
	LS_ASSERT(mutex);

	LeaveCriticalSection(&mutex->mutex);
}

typedef struct ThreadData {
	LSThreadFunction function;
	void *data;
} ThreadData;

typedef struct LSThread {
	HANDLE thread;
	ThreadData thread_data;
} LSThread;

DWORD WINAPI thread_function(void *data) {
	ThreadData *thread_data = (ThreadData *)data;
	thread_data->function(thread_data->data);

	return 0;
}

LSThread *os_thread_create(LSThreadFunction function, void *data) {
	LSThread *thread = ls_malloc(sizeof(LSThread));
	thread->thread_data.function = function;
	thread->thread_data.data = data;
	thread->thread = CreateThread(NULL, 0, thread_function, &thread->thread_data, 0, NULL);

	return thread;
}

void os_thread_destroy(LSThread *thread) {
	LS_ASSERT(thread);

	CloseHandle(thread->thread);
	ls_free(thread);
}

void os_thread_join(LSThread *thread) {
	LS_ASSERT(thread);

	WaitForSingleObject(thread->thread, INFINITE);
}