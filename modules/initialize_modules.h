#ifndef REGISTER_MODULE_TYPES_H
#define REGISTER_MODULE_TYPES_H

#include "core/core.h"

typedef enum {
	// MODULE_INITIALIZATION_LEVEL_CORE is triggered after core initialization. p_arg is a pointer to the LSCore.
	MODULE_INITIALIZATION_LEVEL_CORE = 0,
	// MODULE_INITIALIZATION_LEVEL_RENDER is triggered after renderer initialization. p_arg is a pointer to the Renderer.
	MODULE_INITIALIZATION_LEVEL_RENDER,
	// MODULE_INITIALIZATION_LEVEL_FLAGS is triggered after flag have been parsed. p_arg is NULL.
	MODULE_INITIALIZATION_LEVEL_FLAGS,
	// MODULE_INITIALIZATION_LEVEL_MAIN is triggered after main initialization. p_arg is NULL.
	MODULE_INITIALIZATION_LEVEL_MAIN,
} ModuleInitializationLevel;

void initialize_modules(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_modules(ModuleInitializationLevel p_level);

#endif // REGISTER_MODULE_TYPES_H
