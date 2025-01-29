#ifndef DYNAMIC_MODULES_H
#define DYNAMIC_MODULES_H

#include "core/core.h"
#include "modules/initialize_modules.h"

typedef void (*DynamicModuleInitalizeFunc)(ModuleInitializationLevel p_level, void *p_arg);
typedef void (*DynamicModuleDeinitalizeFunc)(ModuleInitializationLevel p_levela);

typedef struct {
	DynamicModuleInitalizeFunc init;
	DynamicModuleDeinitalizeFunc deinit;
} DynamicModuleInterface;

typedef struct LSAPIInterface LSAPIInterface;

typedef DynamicModuleInterface (*DynamicModuleRegisterFunc)();

extern LSAPIInterface *ls_api_interface;
extern const unsigned char *LS_API_HEADER;
extern const unsigned char *LS_API_SOURCE;

void api_interface_init();
void dynamic_modules_init();
void dynamic_modules_start();
void dynamic_modules_deinit();

void initialize_dynamic_modules(ModuleInitializationLevel p_level, void *p_arg);
void uninitialize_dynamic_modules(ModuleInitializationLevel p_level);

#endif // DYNAMIC_MODULES_H