#ifndef APPLICATION_H
#define APPLICATION_H

#include "core/core.h"
#include "renderer/renderer.h"

typedef LSWindow *(*ApplicationInitFunction)(LSCore *core, void *user_data);
typedef void (*ApplicationStartFunction)(void *user_data);
typedef void (*ApplicationDeinitFunction)(void *user_data);

typedef void (*ApplicationUpdateFunction)(float64 delta_time, void *user_data);
typedef bool (*ApplicationShouldStopFunction)(void *user_data);

typedef struct {
	ApplicationInitFunction init;
	ApplicationStartFunction start;
	ApplicationDeinitFunction deinit;

	ApplicationUpdateFunction update;
	ApplicationShouldStopFunction should_stop;

	void *user_data;
} ApplicationInterface;

#endif // APPLICATION_H