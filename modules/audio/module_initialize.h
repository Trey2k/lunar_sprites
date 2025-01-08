#ifndef AUDIO_MODULE_INITIALIZE_H
#define AUDIO_MODULE_INITIALIZE_H
#include "modules/initialize_modules.h"

void initialize_audio_module(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_audio_module(ModuleInitializationLevel p_level);

#endif // PNG_MODULE_INITIALIZE_H