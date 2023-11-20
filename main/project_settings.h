#ifndef PROJECT_SETTINGS_H
#define PROJECT_SETTINGS_H

#include "core/core.h"

typedef struct {
	char *name;
	char *value;
} Setting;

// Should return true if the setting was set successfully, false otherwise.
// raw_value is the raw string value of the setting.
typedef bool (*NameSpaceSetSetting)(String key, String raw_value, void *user_data);

// Should return the error message if NameSpaceSetSetting returned false.
// The returned string should be allocated.
// The returned string will be freed by the caller.
typedef char *(*NameSpaceGetError)(void *user_data);

// Should return an array of settings. The array should be allocated.
// The array will be freed by the caller.
// The count should be set to the number of settings in the array.
typedef Setting *(*NameSpaceGetSettings)(size_t *count, void *user_data);

typedef struct {
	void *user_data;

	NameSpaceSetSetting set_setting;
	NameSpaceGetSettings get_settings;
	NameSpaceGetError get_error;
} NameSpace;

typedef struct ProjectSettings ProjectSettings;

ProjectSettings *project_settings_create();
void project_settings_destroy(ProjectSettings *settings);

bool project_settings_load(ProjectSettings *settings, String path);
bool project_settings_save(ProjectSettings *settings, String path);

void project_settings_set_name(ProjectSettings *settings, String name);

LS_EXPORT void project_settings_register_namespace(ProjectSettings *settings, String name, NameSpace ns);

#endif // PROJECT_SETTINGS_H