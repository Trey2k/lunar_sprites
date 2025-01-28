#include "core/json.h"
#include "core/log.h"
#include "core/types/array.h"
#include "core/types/bstring.h"
#include "core/types/dictionary.h"
#include "core/types/variant.h"

static Variant json_parse_object(const BString json);
static Variant json_parse_array(const BString json);
static Variant json_parse_value(const BString json);

static void json_stringify_value(BString *string, Variant value);
static void json_stringify_object(BString *string, Dictionary *dict);
static void json_stringify_array(BString *string, Array *array);

static bool is_whitespace(char c) {
	return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

Variant json_parse(const BString json) {
	if (json.length == 0) {
		return VARIANT_NIL;
	}

	switch (bstring_get(json, 0)) {
		case '{':
			return json_parse_object(json);
		case '[':
			return json_parse_array(json);
		default:
			return json_parse_value(json);
	}
}

BString json_stringify(Variant value) {
	BString json = BSTRING_EMPTY;
	switch (value.type) {
		case VARIANT_TYPE_DICTIONARY: {
			json_stringify_object(&json, value.DICTIONARY);
		} break;

		case VARIANT_TYPE_ARRAY: {
			json_stringify_array(&json, value.ARRAY);
		} break;

		default: {
			json_stringify_value(&json, value);
		} break;
	}

	return json;
}

static Variant json_parse_object(const BString json) {
	if (json.length < 2 || bstring_get(json, 0) != '{' || bstring_get(json, json.length - 1) != '}') {
		ls_log(LOG_LEVEL_ERROR, "Invalid JSON object: %S\n", json);
		return VARIANT_NIL;
	}

	Dictionary *dict = dictionary_create(16);

	enum {
		STATE_NONE,
		STATE_KEY,
		STATE_VALUE,
		STATE_ARRAY,
		STATE_OBJECT
	} state = STATE_KEY;

	uint32 start = 0;
	BString key = BSTRING_EMPTY;

	for (uint32 i = 1; i < json.length; i++) {
		switch (state) {
			case STATE_NONE: {
				if (!is_whitespace(bstring_get(json, i))) {
					switch (bstring_get(json, i)) {
						case '{': {
							state = STATE_OBJECT;
							start = i;
						} break;

						case '[': {
							state = STATE_ARRAY;
							start = i;
						} break;

						// End of object.
						case '}':
							break;

						default: {
							state = STATE_VALUE;
							start = i;
						} break;
					}
				}
			} break;

			case STATE_KEY: {
				if (!is_whitespace(bstring_get(json, i))) {
					if (bstring_get(json, i) == '"') {
						if (start == 0) {
							start = i + 1;
						} else {
							key = bstring_substring(json, start, i);
							break;
						}
					}

					// Non-whitespace or quote character found.
					if (start == 0) {
						ls_log(LOG_LEVEL_ERROR, "Invalid JSON object. Expected '\"' but got '%c': %S\n", bstring_get(json, i), json);
						goto error;
					} else if (key.length > 0) {
						if (bstring_get(json, i) == ':') {
							state = STATE_NONE;
							start = 0;
						} else {
							// None : after key.
							ls_log(LOG_LEVEL_ERROR, "Invalid JSON object. Expected ':' after key '%S' but got '%c': %S\n", key, bstring_get(json, i), json);
							goto error;
						}
					}
				}
			} break;

			case STATE_VALUE: {
				if (!is_whitespace(bstring_get(json, i))) {
					if (bstring_get(json, i) == ',' || bstring_get(json, i) == '}') {
						BString value = bstring_substring(json, start, i);
						dictionary_set(dict, VARIANT_STRING(key), json_parse_value(value));
						key = BSTRING_EMPTY;
						start = 0;
						state = bstring_get(json, i) == ',' ? STATE_KEY : STATE_NONE;
					}
				}
			} break;

			case STATE_ARRAY: {
				if (bstring_get(json, i) == ']' && key.length > 0) {
					BString value = bstring_substring(json, start, i + 1);
					dictionary_set(dict, VARIANT_STRING(key), json_parse_array(value));
					key = BSTRING_EMPTY;
				} else if (key.length == 0) {
					start = 0;
					state = bstring_get(json, i) == ',' ? STATE_KEY : STATE_NONE;
				}
			} break;

			case STATE_OBJECT: {
				if (bstring_get(json, i) == '}' && key.length > 0) {
					BString value = bstring_substring(json, start, i + 1);
					dictionary_set(dict, VARIANT_STRING(key), json_parse_object(value));
					key = BSTRING_EMPTY;
				} else if (key.length == 0) {
					start = 0;
					state = bstring_get(json, i) == ',' ? STATE_KEY : STATE_NONE;
				}
			} break;
		}
	}

	if (state != STATE_NONE) {
		ls_log(LOG_LEVEL_ERROR, "Invalid JSON object. Unexpected end of object: %S\n", json);
		goto error;
	}

	return VARIANT_DICTIONARY(dict);
error:
	dictionary_unref(dict);
	return VARIANT_NIL;
}

static Variant json_parse_array(const BString json) {
	if (json.length < 2 || bstring_get(json, 0) != '[' || bstring_get(json, json.length - 1) != ']') {
		ls_log(LOG_LEVEL_ERROR, "Invalid JSON array: %S\n", json);
		return VARIANT_NIL;
	}

	Array *array = array_create(16);

	uint32 start = 0;

	for (uint32 i = 1; i < json.length; i++) {
		if (!is_whitespace(bstring_get(json, i))) {
			if (start == 0) {
				start = i;
			}

			if (bstring_get(json, i) == ',' || bstring_get(json, i) == ']') {
				BString value = bstring_substring(json, start, i);
				array_append(array, json_parse(value));
				start = 0;
			}
		}
	}

	return VARIANT_ARRAY(array);
}

static Variant json_parse_value(const BString json) {
	if (json.length == 0) {
		return VARIANT_NIL;
	}

	if (bstring_get(json, 0) == '"' && bstring_get(json, json.length - 1) == '"') {
		return VARIANT_STRING(bstring_substring(json, 1, json.length - 1));
	} else if (bstring_is_number(json)) {
		if (bstring_contains(json, BSC("."))) {
			return VARIANT_FLOAT(bstring_to_float(json));
		} else {
			return VARIANT_INT(bstring_to_int(json));
		}
	} else if (bstring_equals(json, BSC("true"))) {
		return VARIANT_BOOL(true);
	} else if (bstring_equals(json, BSC("false"))) {
		return VARIANT_BOOL(false);
	} else if (bstring_equals(json, BSC("null"))) {
		return VARIANT_NIL;
	} else {
		ls_log(LOG_LEVEL_ERROR, "Invalid JSON value: %S\n", json);
		return VARIANT_NIL;
	}
}

static void json_stringify_value(BString *string, Variant value) {
	switch (value.type) {
		case VARIANT_TYPE_INT: {
			BString val = bstring_format("%lld", value.INT);
			bstring_append(string, val);
			bstring_unref(val);
		} break;

		case VARIANT_TYPE_FLOAT: {
			BString val = bstring_format("%lf", value.FLOAT);
			bstring_append(string, val);
			bstring_unref(val);
		} break;

		case VARIANT_TYPE_BOOL: {
			BString val = value.BOOL ? BSC("true") : BSC("false");
			bstring_append(string, val);
		} break;

		case VARIANT_TYPE_STRING: {
			BString val = bstring_format("\"%S\"", value.STRING);
			bstring_append(string, val);
			bstring_unref(val);
		} break;

		case VARIANT_TYPE_DICTIONARY: {
			json_stringify_object(string, value.DICTIONARY);
		} break;

		case VARIANT_TYPE_ARRAY: {
			json_stringify_array(string, value.ARRAY);
		} break;

		default: {
			BString val = variant_to_string(value);
			BString val_formatted = bstring_format("\"%S\"", val);
			bstring_append(string, val_formatted);
			bstring_unref(val);
			bstring_unref(val_formatted);
		} break;
	}
}

static void json_stringify_object(BString *string, Dictionary *dict) {
	bstring_append_cstr(string, "{", 1);

	Array *keys = dictionary_keys(dict);
	for (size_t i = 0; i < array_get_size(keys); i++) {
		Variant key = array_get(keys, i);
		if (key.type != VARIANT_TYPE_STRING) {
			ls_log(LOG_LEVEL_ERROR, "Invalid key type, expected String got %S\n", variant_type_to_string(key.type));
			continue;
		}

		Variant value = dictionary_get(dict, key);
		BString key_str = bstring_format("\"%S\":", key.STRING);
		bstring_append(string, key_str);
		bstring_unref(key_str);

		json_stringify_value(string, value);

		if (i < array_get_size(keys) - 1) {
			bstring_append_cstr(string, ",", 1);
		}
	}

	bstring_append_cstr(string, "}", 1);
}

static void json_stringify_array(BString *string, Array *array) {
	bstring_append_cstr(string, "[", 1);

	for (size_t i = 0; i < array_get_size(array); i++) {
		Variant value = array_get(array, i);
		json_stringify_value(string, value);

		if (i < array_get_size(array) - 1) {
			bstring_append_cstr(string, ",", 1);
		}
	}

	bstring_append_cstr(string, "]", 1);
}