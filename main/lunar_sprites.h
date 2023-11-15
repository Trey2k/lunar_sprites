#ifndef MAIN_H
#define MAIN_H

#include "core/flags.h"

void ls_main_init(int32 argc, char *argv[]);
void ls_main_deinit();

void ls_main_loop();

bool ls_should_quit();

#endif // MAIN_H
