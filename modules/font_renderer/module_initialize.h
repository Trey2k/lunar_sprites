#ifndef FONT_MODULE_INITIALIZE_H
#define FONT_MODULE_INITIALIZE_H

#include "modules/initialize_modules.h"

void initialize_font_renderer_module(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_font_renderer_module(ModuleInitializationLevel p_level);

#endif // FONT_MODULE_INITIALIZE_H