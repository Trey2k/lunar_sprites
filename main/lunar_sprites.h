#ifndef MAIN_H
#define MAIN_H

#include "core/core.h"
#include "main/application.h"

typedef void (*OnUpdateCallback)(float64 delta_time);

void ls_main_init(int32 argc, char *argv[]);
int32 ls_main_deinit();

void ls_main_loop();

bool ls_should_stop();

// Ownership of the application interface is transferred to the main module.
LS_EXPORT void ls_set_application_interface(ApplicationInterface application_interface);

// Registers a callback to be called every frame.
// The call occurs at the start of the frame, unless end_of_frame is true.
// When end_of_frame is true the call occurs at the end of the frame.
LS_EXPORT void ls_register_update_callback(OnUpdateCallback callback, bool end_of_frame);

// Does not call exit() directly, but instead completes deinitialization and returns the exit code from the entry point.
LS_EXPORT void ls_exit(int32 exit_code);

#endif // MAIN_H
