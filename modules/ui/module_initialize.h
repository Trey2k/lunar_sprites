#ifndef UI_MODULE_INITIALIZE_H
#define UI_MODULE_INITIALIZE_H

#include "modules/initialize_modules.h"

void initialize_ui_module(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_ui_module(ModuleInitializationLevel p_level);

#endif // UI_MODULE_INITIALIZE_H