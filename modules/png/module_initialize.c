#include "module_initialize.h"
#include "png_parser.h"

#include "renderer/texture.h"

void initialize_png_module(ModuleInitializationLevel p_level, void *p_arg) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_RENDER) {
		return;
	}

	texture_manager_register_parser("png", parse_png_texture);
}

void uninitialize_png_module(ModuleInitializationLevel p_level) {
	if (p_level != MODULE_INITIALIZATION_LEVEL_RENDER) {
		return;
	}
}