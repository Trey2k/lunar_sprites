#include "dynamic_modules.h"

#include "core/core.h"

#include "main/lunar_sprites.h"

#if defined(DEBUG_ENABLED)
#define SUFFIX ".debug"
#else
#define SUFFIX ".release"
#endif

#if defined(WINDOWS_ENABLED)
#define PLATFORM "windows" SUFFIX
#elif defined(WEB_ENABLED)
#define PLATFORM "web" SUFFIX
#else
#define PLATFORM "linuxbsd" SUFFIX
#endif

#define DYNAMIC_MODULE_CONFIG_EXTENSION "dynamic_module"

typedef void (*LSApiInitFunc)(LSAPIInterface *p_interface);

typedef struct {
	FlagValue *dump_api;
	FlagValue *dump_api_file;
	Slice64 *interfaces;
} DynamicModules;

static DynamicModules dynamic_modules;

static void load_modules(Slice64 *interfaces, String path, int32 depth);

void dynamic_modules_init(LSCore *core) {
	api_interface_init();

	FlagManager *flag_manager = core_get_flag_manager(core);

	dynamic_modules.dump_api = flag_manager_register(flag_manager, "dump-api", FLAG_TYPE_BOOL, FLAG_VAL(b, false), "Extract the native API header file.");
	dynamic_modules.dump_api_file = flag_manager_register(flag_manager, "api-file", FLAG_TYPE_STRING, FLAG_VAL(str, "ls_api.h"), "If dump-api is true, extract the native API to this file.");

	dynamic_modules.interfaces = slice64_create(16, true);

	char *dir = os_get_working_directory();
	load_modules(dynamic_modules.interfaces, dir, 16);
}

void dynamic_modules_start() {
	if (dynamic_modules.dump_api->b) {
		ls_printf("Dumping native API to %s\n", dynamic_modules.dump_api_file->str);
		ls_printf("This file is automatically generated. Do not edit it manually.\n");

		char *file_name = ls_str_copy(dynamic_modules.dump_api_file->str);

		char *temp = file_name;
		file_name = ls_str_replace(file_name, ".h", "");
		ls_free(temp);
		temp = file_name;
		file_name = ls_str_replace(file_name, ".c", "");
		ls_free(temp);

		char *header_file = ls_str_format("%s.h", file_name);
		char *source_file = ls_str_format("%s.c", file_name);
		ls_free(file_name);

		char *api_source = ls_str_replace((String)LS_API_SOURCE, "@API_HEADER@", header_file);

		if (!os_write_file(source_file, api_source, ls_str_length(api_source))) {
			ls_log_fatal("Failed to write file %s\n", source_file);
		}

		ls_free(api_source);

		if (!os_write_file(header_file, (String)LS_API_HEADER, ls_str_length((String)LS_API_HEADER))) {
			ls_log_fatal("Failed to write file %s\n", header_file);
		}

		ls_printf("Wrote native API header to %s and %s\n", header_file, source_file);

		ls_free(header_file);
		ls_free(source_file);

		ls_exit(0);
	}
}

void dynamic_modules_deinit() {
	slice64_destroy(dynamic_modules.interfaces);
}

static DynamicModuleInterface *load_dynamic_module(String file_path) {
	Hashtable *config = read_config(file_path);

	if ((!hashtable_contains(config, HASH_KEY(str, PLATFORM))) ||
			!hashtable_contains(config, HASH_KEY(str, "name")) ||
			!hashtable_contains(config, HASH_KEY(str, "entry_point"))) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load dynamic module %s\n", file_path);
		return NULL;
	}

	String module_name = hashtable_get(config, HASH_KEY(str, "name")).str;
	String entry_point = hashtable_get(config, HASH_KEY(str, "entry_point")).str;
	String lib_path = hashtable_get(config, HASH_KEY(str, PLATFORM)).str;

	ls_log(LOG_LEVEL_INFO, "Loading dynamic module %s\n", module_name);

	char *lib_path_abs = os_path_to_absolute(lib_path);
	void *handle = os_open_library(lib_path_abs);
	ls_free(lib_path_abs);
	if (!handle) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load dynamic module %s\n", module_name);
		return NULL;
	}

	LSApiInitFunc init_function = os_get_library_symbol(handle, "ls_api_init");
	if (!init_function) {
		ls_log(LOG_LEVEL_INFO, "Failed to load dynamic module %s\n", module_name);
	}

	init_function(ls_api_interface);

	DynamicModuleRegisterFunc register_function = os_get_library_symbol(handle, entry_point);
	if (!register_function) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load dynamic module %s\n", module_name);
		return NULL;
	}

	DynamicModuleInterface interface = register_function();
	if (!interface.init || !interface.deinit) {
		ls_log(LOG_LEVEL_ERROR, "Failed to load dynamic module %s\n", module_name);
		return NULL;
	}

	DynamicModuleInterface *interface_ptr = ls_malloc(sizeof(DynamicModuleInterface));
	*interface_ptr = interface;

	hashtable_destroy(config);

	return interface_ptr;
}

static void load_modules(Slice64 *interfaces, String path, int32 depth) {
	if (depth <= 0) {
		LS_ASSERT_MSG(false, "%s", "Maximum module search depth reached.\n");
		return;
	}

	Slice64 *files = os_list_directory(path);
	for (size_t i = 0; i < slice64_get_size(files); i++) {
		String file = slice64_get(files, i).str;
		if (os_path_is_directory(file)) {
			load_modules(interfaces, file, depth - 1);
			continue;
		}

		char *ext = os_path_get_extension(slice64_get(files, i).str);
		if (ext && ls_str_equals(ext, DYNAMIC_MODULE_CONFIG_EXTENSION)) {
			DynamicModuleInterface *interface = load_dynamic_module(file);
			if (interface) {
				slice64_append(interfaces, SLICE_VAL64(ptr, interface));
			}
		}
		continue;
	}

	slice64_destroy(files);
}

void initialize_dynamic_modules(ModuleInitializationLevel p_level, void *p_arg) {
	for (size_t i = 0; i < slice64_get_size(dynamic_modules.interfaces); i++) {
		DynamicModuleInterface *interface = slice64_get(dynamic_modules.interfaces, i).ptr;
		interface->init(p_level, p_arg);
	}
}

void uninitialize_dynamic_modules(ModuleInitializationLevel p_level) {
	for (size_t i = 0; i < slice64_get_size(dynamic_modules.interfaces); i++) {
		DynamicModuleInterface *interface = slice64_get(dynamic_modules.interfaces, i).ptr;
		interface->deinit(p_level);
	}
}