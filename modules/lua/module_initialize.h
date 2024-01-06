#ifndef LUA_MODULE_INITIALIZE_H
#define LUA_MODULE_INITIALIZE_H
#include "modules/initialize_modules.h"

void initialize_lua_module(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_lua_module(ModuleInitializationLevel p_level);

#endif // LUA_MODULE_INITIALIZE_H