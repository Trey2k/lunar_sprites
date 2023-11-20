#ifndef NATIVE_APPLICATION_MODULE_REGISTER_H
#define NATIVE_APPLICATION_MODULE_REGISTER_H

#include "core/core.h"
#include "modules/initialize_modules.h"

void initialize_native_application_module(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_native_application_module(ModuleInitializationLevel p_level);

#endif // NATIVE_APPLICATION_MODULE_REGISTER_H
