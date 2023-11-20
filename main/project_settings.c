#include "main/project_settings.h"

#include "parser.h"

#include <stdio.h>

struct ProjectSettings {
	char *project_name;

	Hashtable *namespaces;
};

static void json_callback(const char *const key, const char *const value, const char *const parentKey, void *object);
static bool project_settings_load_json(ProjectSettings *settings, String path);
static bool project_settings_save_json(ProjectSettings *settings, String path);

ProjectSettings *project_settings_create() {
	ProjectSettings *settings = ls_malloc(sizeof(ProjectSettings));
	settings->project_name = NULL;
	settings->namespaces = hashtable_create(HASHTABLE_KEY_STRING, 16, true);
	return settings;
}

void project_settings_destroy(ProjectSettings *settings) {
	hashtable_destroy(settings->namespaces);
	if (settings->project_name) {
		ls_free(settings->project_name);
	}

	ls_free(settings);
}

bool project_settings_load(ProjectSettings *settings, String path) {
	LS_ASSERT(settings);
	LS_ASSERT(path);

	String ext = ls_path_get_extension(path);
	if (!ext) {
		ls_log(LOG_LEVEL_ERROR, "Path '%s' has no extension.", path);
		return false;
	}

	if (ls_str_equals(ext, "json")) {
		return project_settings_load_json(settings, path);
	}

	ls_log(LOG_LEVEL_ERROR, "Unsupported file extension '%s'.", ext);
	return false;
}

bool project_settings_save(ProjectSettings *settings, String path) {
	LS_ASSERT(settings);
	LS_ASSERT(path);

	String ext = ls_path_get_extension(path);
	if (!ext) {
		ls_log(LOG_LEVEL_ERROR, "Path '%s' has no extension.", path);
		return false;
	}

	if (ls_str_equals(ext, "json")) {
		return project_settings_save_json(settings, path);
	}

	ls_log(LOG_LEVEL_ERROR, "Unsupported file extension '%s'.", ext);
	return false;
}

void project_settings_register_namespace(ProjectSettings *settings, String name, NameSpace ns) {
	LS_ASSERT(settings);
	LS_ASSERT(name);

	NameSpace *ns_ptr = ls_malloc(sizeof(NameSpace));
	*ns_ptr = ns;

	hashtable_set(settings->namespaces, HASH_KEY(str, name), HASH_VAL(ptr, ns_ptr));
}

void project_settings_set_name(ProjectSettings *settings, String name) {
	LS_ASSERT(settings);

	ls_free(settings->project_name);
	settings->project_name = ls_str_copy(name);
}

static void json_callback(const char *const key, const char *const value, const char *const parent_key, void *user_data) {
	LS_ASSERT(key);
	LS_ASSERT(value);

	ProjectSettings *settings = user_data;
	LS_ASSERT(settings);

	String namespace = parent_key;

	// Root project settings.
	if (!parent_key) {
		if (ls_str_equals(key, "project_name")) {
			settings->project_name = ls_str_copy(value);
		} else {
			ls_log(LOG_LEVEL_ERROR, "Unknown project setting '%s'.", key);
		}
		return;
	}

	// Namespace settings.
	if (!hashtable_contains(settings->namespaces, HASH_KEY(str, namespace))) {
		ls_log(LOG_LEVEL_ERROR, "Namespace '%s' does not exist.", namespace);
		return;
	}

	NameSpace *ns = hashtable_get(settings->namespaces, HASH_KEY(str, namespace)).ptr;
	LS_ASSERT(ns);

	if (!ns->set_setting(key, value, ns->user_data)) {
		ls_log(LOG_LEVEL_ERROR, "Failed to set setting '%s' in namespace '%s': %s", key, namespace, ns->get_error(ns->user_data));
		return;
	}
}

static bool project_settings_load_json(ProjectSettings *settings, String path) {
	LS_ASSERT(settings);
	LS_ASSERT(path);

	FILE *file = fopen(path, "r");

	if (!file) {
		ls_log(LOG_LEVEL_ERROR, "Failed to open file '%s'.", path);
		return false;
	}

	fseek(file, 0, SEEK_END);
	int32 length = ftell(file);

	if (length == -1) {
		ls_log(LOG_LEVEL_ERROR, "Failed to get length of file '%s'.", path);
		fclose(file);
		return false;
	}

	fseek(file, 0, SEEK_SET);
	char *buffer = ls_malloc(length + 1);
	fread(buffer, 1, length, file);
	fclose(file);

	buffer[length] = '\0';

	nanojsonc_parse_object(buffer, json_callback, NULL, settings);
}

static bool project_settings_save_json(ProjectSettings *settings, String path) {
	LS_ASSERT(settings);
	LS_ASSERT(path);

	FILE *file = fopen(path, "w");

	if (!file) {
		ls_log(LOG_LEVEL_ERROR, "Failed to open file '%s'.", path);
		return false;
	}

	fprintf(file, "{\n");

	if (settings->project_name) {
		fprintf(file, "\t\"project_name\": \"%s\",\n", settings->project_name);
	}

	fprintf(file, "\n");

	Slice *namespaces = hashtable_get_keys(settings->namespaces);

	for (size_t i = 0; i < slice_get_size(namespaces); i++) {
		const HashtableKey *key = slice_get(namespaces, i).ptr;
		String namespace = key->str;

		fprintf(file, "\t\"%s\": {\n", namespace);

		NameSpace *ns = hashtable_get(settings->namespaces, HASH_KEY(str, namespace)).ptr;
		LS_ASSERT(ns);

		size_t count;
		Setting *settings = ns->get_settings(&count, ns->user_data);

		for (size_t j = 0; j < count; j++) {
			Setting *setting = &settings[j];
			fprintf(file, "\t\t\"%s\": \"%s\"", setting->name, setting->value);

			if (j < count - 1) {
				fprintf(file, ",");
			}

			fprintf(file, "\n");
		}

		fprintf(file, "\t}");

		if (i < slice_get_size(namespaces) - 1) {
			fprintf(file, ",");
		}

		fprintf(file, "\n");

		ls_free(settings);
	}

	fprintf(file, "}\n");

	fclose(file);
	slice_destroy(namespaces);

	return true;
}