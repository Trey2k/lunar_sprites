#ifndef PNG_MODULE_INITIALIZE_H
#define PNG_MODULE_INITIALIZE_H
#include "modules/initialize_modules.h"

void initialize_png_module(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_png_module(ModuleInitializationLevel p_level);

#endif // PNG_MODULE_INITIALIZE_H