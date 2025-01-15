#ifndef MAIN_H
#define MAIN_H

#include "core/core.h"
#include "main/application.h"
#include "main_loop.h"

typedef void (*OnUpdateCallback)(float64 delta_time);

void ls_main_init(int32 argc, char *argv[]);
int32 ls_main_deinit();

bool ls_should_stop();

void ls_update(float64 delta_time);

// Ownership of the application interface is transferred to the main module.
LS_EXPORT void ls_set_application_interface(ApplicationInterface application_interface);

// Registers a callback to be called every frame in the update loop.
LS_EXPORT void ls_register_update_callback(OnUpdateCallback callback);

// Does not call exit() directly, but instead completes deinitialization and returns the exit code from the entry point.
LS_EXPORT void ls_exit(int32 exit_code);

#endif // MAIN_H
