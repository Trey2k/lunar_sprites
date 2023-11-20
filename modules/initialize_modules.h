#ifndef REGISTER_MODULE_TYPES_H
#define REGISTER_MODULE_TYPES_H

#include "core/core.h"

typedef enum {
	MODULE_INITIALIZATION_LEVEL_CORE = 0,
	MODULE_INITIALIZATION_LEVEL_RENDER,
	MODULE_INITIALIZATION_LEVEL_MAIN,
} ModuleInitializationLevel;

void initialize_modules(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_modules(ModuleInitializationLevel p_level);

#endif // REGISTER_MODULE_TYPES_H
