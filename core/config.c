#include "core/config.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/os.h"
#include "core/types/hashtable.h"
#include "core/types/string.h"

enum ConfigParserState {
	CONFIG_STATE_NONE,
	CONFIG_STATE_KEY,
	CONFIG_STATE_VALUE,
};

static void parse_config(Hashtable *config, char current_char, enum ConfigParserState *state);

void write_config(String path, Hashtable *config) {
	LS_ASSERT(path);
	LS_ASSERT(config);

	LSFile file = os_open_file(path, "w");
	if (!file) {
		ls_log(LOG_LEVEL_ERROR, "Failed to open file %s", path);
		return;
	}

	Slice *keys = hashtable_get_keys(config);

	for (size_t i = 0; i < slice_get_size(keys); i++) {
		String key = slice_get(keys, i).str;
		String value = hashtable_get(config, HASH_KEY(str, key)).str;

		char *line = ls_str_format("%s: %s\n", key, value);
		os_write_file_data(file, line, ls_str_length(line));
		ls_free(line);
	}

	os_close_file(file);
	slice_destroy(keys);
}

Hashtable *read_config(String path) {
	LS_ASSERT(path);

	Hashtable *config = hashtable_create(HASHTABLE_KEY_STRING, 16, false);

	LSFile file = os_open_file(path, "r");
	if (!file) {
		ls_log(LOG_LEVEL_ERROR, "Failed to open file %s", path);
		return config;
	}

	size_t file_size = os_get_file_size(file);
	char *file_data = ls_alloca(file_size + 1);
	os_read_file_data(file, file_data, file_size);
	file_data[file_size] = '\0';
	enum ConfigParserState state = CONFIG_STATE_NONE;

	while (*file_data) {
		parse_config(config, *file_data, &state);
		file_data++;
	}

	return config;
}

static void parse_config(Hashtable *config, char current_char, enum ConfigParserState *state) {
}