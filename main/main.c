#include "main/main.h"

#include "core/core.h"

#include "modules/register_module_types.h"

void main_start() {
	initialize_core();
	initialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	uninitialize_modules(MODULE_INITIALIZATION_LEVEL_CORE);
	uninitialize_core();
}
