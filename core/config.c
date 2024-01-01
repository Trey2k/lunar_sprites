#include "core/config.h"
#include "core/debug.h"
#include "core/log.h"
#include "core/os.h"
#include "core/types/hashtable.h"
#include "core/types/slice.h"
#include "core/types/string.h"

enum ConfigParserState {
	CONFIG_STATE_NONE,
	CONFIG_STATE_KEY,
	CONFIG_STATE_VALUE,
};

void write_config(String path, const Hashtable *config) {
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

	Hashtable *config = hashtable_create(HASHTABLE_KEY_STRING, 16, true);

	LSFile file = os_open_file(path, "r");
	if (!file) {
		ls_log(LOG_LEVEL_ERROR, "Failed to open file %s", path);
		return config;
	}

	size_t file_size = os_get_file_size(file);
	char *file_data = ls_malloc(file_size + 1);
	os_read_file_data(file, file_data, file_size);
	file_data[file_size] = '\0';
	os_close_file(file);

	enum ConfigParserState state = CONFIG_STATE_NONE;

	Slice8 *current_key = slice8_create(32);
	Slice8 *current_value = slice8_create(32);
	char *cur_char = file_data;
	while (*cur_char) {
		switch (state) {
			case CONFIG_STATE_NONE: {
				if (*cur_char >= 'A' && *cur_char <= 'z') {
					state = CONFIG_STATE_KEY;
					cur_char--;
				} else if (*cur_char == '\n') {
					state = CONFIG_STATE_NONE;
				} else {
					ls_log(LOG_LEVEL_ERROR, "Invalid character '%c' in config file %s", *cur_char, path);
					return config;
				}
			} break;
			case CONFIG_STATE_KEY: {
				if (*cur_char == ':') {
					cur_char++;
					if (*cur_char != ' ') {
						cur_char--;
					}

					state = CONFIG_STATE_VALUE;
				} else if ((*cur_char >= 'A' && *cur_char <= 'z') ||
						(*cur_char >= '0' && *cur_char <= '9') ||
						*cur_char == '.') {
					slice8_append(current_key, SLICE_VAL8(chr, *cur_char));
				} else {
					ls_log(LOG_LEVEL_ERROR, "Invalid character '%c' in config file %s", *cur_char, path);
					return config;
				}
			} break;
			case CONFIG_STATE_VALUE: {
				if (*cur_char == '\n') {
					state = CONFIG_STATE_NONE;
					slice8_append(current_key, SLICE_VAL8(chr, '\0'));
					slice8_append(current_value, SLICE_VAL8(chr, '\0'));

					char *key = ls_str_copy(slice8_get_data(current_key));
					char *value = ls_str_copy(slice8_get_data(current_value));

					hashtable_set(config, HASH_KEY(str, key), HASH_VAL(str, value));
					slice8_clear(current_key);
					slice8_clear(current_value);
				} else if ((*cur_char >= ' ' && *cur_char <= '~')) {
					slice8_append(current_value, SLICE_VAL8(chr, *cur_char));
				} else {
					ls_log(LOG_LEVEL_ERROR, "Invalid character '%c' in config file %s", *cur_char, path);
					return config;
				}
			} break;
		};
		cur_char++;
	}

	return config;
}
