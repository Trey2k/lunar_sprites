#ifndef DYNAMIC_MODULES_INITALIZE_H
#define DYNAMIC_MODULES_INITALIZE_H
#include "modules/initialize_modules.h"

void initialize_dynamic_modules_module(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_dynamic_modules_module(ModuleInitializationLevel p_level);

#endif // DYNAMIC_MODULES_INITALIZE_H