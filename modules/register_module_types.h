#ifndef REGISTER_MODULE_TYPES_H
#define REGISTER_MODULE_TYPES_H

typedef enum {
	MODULE_INITIALIZATION_LEVEL_CORE = 0,
} ModuleInitializationLevel;

void initialize_modules(ModuleInitializationLevel p_level);
void uninitialize_modules(ModuleInitializationLevel p_level);

#endif // REGISTER_MODULE_TYPES_H
