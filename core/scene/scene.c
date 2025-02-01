#include "core/scene/scene.h"

#include "core/json.h"
#include "core/log.h"
#include "core/memory.h"
#include "core/object/object.h"
#include "core/os/os.h"
#include "core/types/bstring.h"
#include "core/types/color.h"
#include "core/types/dictionary.h"
#include "core/types/variant.h"

struct Scene {
	Object **ojbects;
	uint64 object_count;
};

static Variant parse_vector2(const Dictionary *dict);
static Variant parse_vector2i(const Dictionary *dict);
static Variant parse_vector3(const Dictionary *dict);
static Variant parse_vector3i(const Dictionary *dict);
static Variant parse_color(const Dictionary *dict);
static Variant parse_resource(BString path);

static Dictionary *encode_object(const Object *object);
static Variant encode_value(Variant value);

static const Variant type_str = BSCVL("type", 4);
static const Variant properties_str = BSCVL("properties", 10);
static const Variant children_str = BSCVL("children", 8);

static void scene_parse_data(Scene *scene, const Array *scene_data);

Scene *scene_create() {
	Scene *scene = ls_malloc(sizeof(Scene));
	scene->ojbects = NULL;
	scene->object_count = 0;

	return scene;
}

void scene_destroy(Scene *scene) {
	for (size_t i = 0; i < scene->object_count; i++) {
		object_unref(scene->ojbects[i]);
	}

	if (scene->ojbects) {
		ls_free(scene->ojbects);
	}

	ls_free(scene);
}

void scene_parse(Scene *scene, BString path) {
	if (!bstring_ends_with(path, BSC(".scene"))) {
		ls_log(LOG_LEVEL_ERROR, "Invalid scene file: %S\n", path);
		return;
	}

	if (scene->object_count > 0) {
		ls_log(LOG_LEVEL_WARNING, "Scene already contains objects, clearing them\n");
		for (size_t i = 0; i < scene->object_count; i++) {
			object_unref(scene->ojbects[i]);
		}

		ls_free(scene->ojbects);
		scene->ojbects = NULL;
		scene->object_count = 0;
	}

	BString json_data = bstring_read_file(path);

	Variant result = json_parse(json_data);
	if (result.type != VARIANT_TYPE_ARRAY) {
		ls_log(LOG_LEVEL_ERROR, "Invalid scene file: %S\n", path);
		bstring_unref(json_data);
		return;
	}

	Array *scene_data = result.ARRAY;
	scene_parse_data(scene, scene_data);

	array_unref(scene_data);
	bstring_unref(json_data);
}

void scene_save(Scene *scene, BString path) {
	Array *scene_data = array_create(scene->object_count);
	for (size_t i = 0; i < scene->object_count; i++) {
		Object *object = scene->ojbects[i];
		Dictionary *object_data = encode_object(object);
		array_append(scene_data, VARIANT_DICTIONARY(object_data));
		dictionary_unref(object_data);
	}

	BString json_data = json_stringify_pretty(VARIANT_ARRAY(scene_data), 4);
	array_unref(scene_data);

	char *cstr = bstring_encode_utf8(path);
	LSFile file = os_open_file(cstr, "w");
	ls_free(cstr);
	if (!file) {
		ls_log(LOG_LEVEL_ERROR, "Failed to open file: %S\n", path);
		bstring_unref(json_data);
		return;
	}

	bstring_fwrite(file, json_data);
	os_close_file(file);
	bstring_unref(json_data);
}

void scene_add_object(Scene *scene, Object *object) {
	scene->ojbects = ls_realloc(scene->ojbects, sizeof(Object *) * (scene->object_count + 1));
	scene->ojbects[scene->object_count] = object;
	scene->object_count++;
}

uint64 scene_get_object_count(Scene *scene) {
	return scene->object_count;
}

Object *scene_get_object(Scene *scene, uint64 index) {
	return scene->ojbects[index];
}

void scene_remove_object(Scene *scene, uint64 index) {
	object_unref(scene->ojbects[index]);

	for (size_t i = index; i < scene->object_count - 1; i++) {
		scene->ojbects[i] = scene->ojbects[i + 1];
	}

	scene->object_count--;
}

static void update_object_children(Object *object, float64 delta_time) {
	for (size_t i = 0; i < object_get_child_count(object); i++) {
		Object *child = object_get_child(object, i);
		object_update(child, delta_time);
		update_object_children(child, delta_time);
	}
}

void scene_update(Scene *scene, float64 delta_time) {
	for (size_t i = 0; i < scene->object_count; i++) {
		object_update(scene->ojbects[i], delta_time);
		update_object_children(scene->ojbects[i], delta_time);
	}
}

static void draw_object_children(Object *object, float64 delta_time) {
	for (int32 i = object_get_child_count(object) - 1; i >= 0; i--) {
		Object *child = object_get_child(object, i);
		draw_object_children(child, delta_time);
		object_draw(child, delta_time);
	}
}

void scene_draw(Scene *scene, float64 delta_time) {
	for (int32 i = scene->object_count - 1; i >= 0; i--) {
		Object *object = scene->ojbects[i];
		draw_object_children(object, delta_time);
		object_draw(object, delta_time);
	}
}

static void object_parse_properties(Object *object, const Dictionary *properties) {
	Array *keys = dictionary_keys(properties);

	for (size_t j = 0; j < array_get_size(keys); j++) {
		Variant key_var = array_get(keys, j);
		BString key = key_var.STRING;
		Variant value = dictionary_get(properties, key_var);

		if (!object_has_property(object, key)) {
			ls_log(LOG_LEVEL_ERROR, "Invalid property '%S' for object of type '%S'\n", key, object_get_type_name(object));
			continue;
		}
		VariantType prop_type = object_get_property_type(object, key);
		if (value.type == prop_type) {
			object_set_property(object, key, value);
			continue;
		}

		switch (prop_type) {
			case VARIANT_TYPE_VECTOR2I: {
				object_set_property(object, key, parse_vector2i(value.DICTIONARY));
			} break;

			case VARIANT_TYPE_VECTOR2: {
				object_set_property(object, key, parse_vector2(value.DICTIONARY));
			} break;

			case VARIANT_TYPE_VECTOR3I: {
				object_set_property(object, key, parse_vector3i(value.DICTIONARY));
			} break;

			case VARIANT_TYPE_VECTOR3: {
				object_set_property(object, key, parse_vector3(value.DICTIONARY));
			} break;

			case VARIANT_TYPE_COLOR: {
				object_set_property(object, key, parse_color(value.DICTIONARY));
			} break;

			case VARIANT_TYPE_RESOURCE: {
				object_set_property(object, key, parse_resource(value.STRING));
			} break;

			default: {
				ls_log(LOG_LEVEL_ERROR, "Invalid property '%S' for object of type '%S'\n", key, object_get_type_name(object));
			} break;
		}
	}

	array_unref(keys);
}

static Object *parse_object_data(const Dictionary *object_data) {
	if (!dictionary_contains(object_data, type_str)) {
		ls_log(LOG_LEVEL_ERROR, "Invalid object data in scene file\n");
		return NULL;
	}
	BString type_name = dictionary_get(object_data, type_str).STRING;
	Object *object = object_create(type_name);

	if (dictionary_contains(object_data, properties_str)) {
		Variant properties = dictionary_get(object_data, properties_str);
		if (properties.type != VARIANT_TYPE_DICTIONARY) {
			ls_log(LOG_LEVEL_ERROR, "Invalid properties data in scene file\n");
			object_unref(object);
			return NULL;
		}

		object_parse_properties(object, properties.DICTIONARY);
	}

	if (dictionary_contains(object_data, children_str)) {
		Variant children = dictionary_get(object_data, children_str);
		if (children.type != VARIANT_TYPE_ARRAY) {
			ls_log(LOG_LEVEL_ERROR, "Invalid children data in scene file\n");
			object_unref(object);
			return NULL;
		}

		Array *children_data = children.ARRAY;
		for (size_t i = 0; i < array_get_size(children_data); i++) {
			Variant child_data = array_get(children_data, i);
			if (child_data.type != VARIANT_TYPE_DICTIONARY) {
				ls_log(LOG_LEVEL_ERROR, "Invalid child data in scene file\n");
				continue;
			}

			Object *child = parse_object_data(child_data.DICTIONARY);
			if (child) {
				object_add_child(object, child);
			}
		}

		array_unref(children_data);
	}

	return object;
}

static void scene_parse_data(Scene *scene, const Array *scene_data) {
	for (size_t i = 0; i < array_get_size(scene_data); i++) {
		Variant val = array_get(scene_data, i);
		if (val.type != VARIANT_TYPE_DICTIONARY) {
			ls_log(LOG_LEVEL_ERROR, "Invalid object data in scene file\n");
			continue;
		}

		Dictionary *object_data = val.DICTIONARY;
		Object *object = parse_object_data(object_data);
		if (object) {
			scene_add_object(scene, object);
		}
	}
}

static Variant parse_vector2(const Dictionary *dict) {
	const Variant x_str = BSCV("x");
	const Variant y_str = BSCV("y");

	if (!dictionary_contains(dict, x_str) || !dictionary_contains(dict, y_str)) {
		ls_log(LOG_LEVEL_ERROR, "Invalid Vector2 data in scene file\n");
		return VARIANT_NIL;
	}

	return VARIANT_VECTOR2(vec2(dictionary_get(dict, x_str).FLOAT, dictionary_get(dict, y_str).FLOAT));
}

static Variant parse_vector2i(const Dictionary *dict) {
	const Variant x_str = BSCV("x");
	const Variant y_str = BSCV("y");

	if (!dictionary_contains(dict, x_str) || !dictionary_contains(dict, y_str)) {
		ls_log(LOG_LEVEL_ERROR, "Invalid Vector2i data in scene file\n");
		return VARIANT_NIL;
	}

	return VARIANT_VECTOR2I(vec2i((int32)dictionary_get(dict, x_str).INT, (int32)dictionary_get(dict, y_str).INT));
}

static Variant parse_vector3(const Dictionary *dict) {
	const Variant x_str = BSCV("x");
	const Variant y_str = BSCV("y");
	const Variant z_str = BSCV("z");

	if (!dictionary_contains(dict, x_str) || !dictionary_contains(dict, y_str) || !dictionary_contains(dict, z_str)) {
		ls_log(LOG_LEVEL_ERROR, "Invalid Vector3 data in scene file\n");
		return VARIANT_NIL;
	}

	return VARIANT_VECTOR3(vec3(dictionary_get(dict, x_str).FLOAT, dictionary_get(dict, y_str).FLOAT, dictionary_get(dict, z_str).FLOAT));
}

static Variant parse_vector3i(const Dictionary *dict) {
	const Variant x_str = BSCV("x");
	const Variant y_str = BSCV("y");
	const Variant z_str = BSCV("z");

	if (!dictionary_contains(dict, x_str) || !dictionary_contains(dict, y_str) || !dictionary_contains(dict, z_str)) {
		ls_log(LOG_LEVEL_ERROR, "Invalid Vector3i data in scene file\n");
		return VARIANT_NIL;
	}

	return VARIANT_VECTOR3I(vec3i((int32)dictionary_get(dict, x_str).INT, (int32)dictionary_get(dict, y_str).INT, (int32)dictionary_get(dict, z_str).INT));
}

static Variant parse_color(const Dictionary *dict) {
	const Variant r_str = BSCV("r");
	const Variant g_str = BSCV("g");
	const Variant b_str = BSCV("b");
	const Variant a_str = BSCV("a");

	if (!dictionary_contains(dict, r_str) || !dictionary_contains(dict, g_str) || !dictionary_contains(dict, b_str) || !dictionary_contains(dict, a_str)) {
		ls_log(LOG_LEVEL_ERROR, "Invalid Color data in scene file\n");
		return VARIANT_NIL;
	}

	return VARIANT_COLOR(color_create(dictionary_get(dict, r_str).FLOAT, dictionary_get(dict, g_str).FLOAT, dictionary_get(dict, b_str).FLOAT, dictionary_get(dict, a_str).FLOAT));
}

static Variant parse_resource(BString path) {
	return VARIANT_RESOURCE(resource_create(path));
}

static Dictionary *encode_object(const Object *object) {
	Dictionary *object_data = dictionary_create(16);

	dictionary_set(object_data, type_str, VARIANT_STRING(object_get_type_name(object)));

	Array *properties = object_get_properties(object);
	Dictionary *properties_data = dictionary_create(array_get_size(properties));

	for (size_t i = 0; i < array_get_size(properties); i++) {
		BString key = array_get(properties, i).STRING;
		Variant value = encode_value(object_get_property(object, key));
		dictionary_set(properties_data, VARIANT_STRING(key), value);
		variant_unref(value);
	}

	if (array_get_size(properties) > 0) {
		dictionary_set(object_data, properties_str, VARIANT_DICTIONARY(properties_data));
	}

	array_unref(properties);

	Array *children = array_create(object_get_child_count(object));
	for (int32 i = object_get_child_count(object) - 1; i >= 0; i--) {
		const Object *child = object_get_child(object, i);
		Dictionary *child_data = encode_object(child);
		array_append(children, VARIANT_DICTIONARY(child_data));
		dictionary_unref(child_data);
	}

	if (array_get_size(children) > 0) {
		dictionary_set(object_data, children_str, VARIANT_ARRAY(children));
	}

	return object_data;
}

static Variant encode_value(Variant value) {
	switch (value.type) {
		case VARIANT_TYPE_VECTOR2: {
			Dictionary *dict = dictionary_create(2);
			dictionary_set(dict, BSCVL("x", 1), VARIANT_FLOAT(value.VECTOR2.x));
			dictionary_set(dict, BSCVL("y", 1), VARIANT_FLOAT(value.VECTOR2.y));
			return VARIANT_DICTIONARY(dict);
		} break;

		case VARIANT_TYPE_VECTOR2I: {
			Dictionary *dict = dictionary_create(2);
			dictionary_set(dict, BSCVL("x", 1), VARIANT_INT(value.VECTOR2I.x));
			dictionary_set(dict, BSCVL("y", 1), VARIANT_INT(value.VECTOR2I.y));
			return VARIANT_DICTIONARY(dict);
		} break;

		case VARIANT_TYPE_VECTOR3: {
			Dictionary *dict = dictionary_create(3);
			dictionary_set(dict, BSCVL("x", 1), VARIANT_FLOAT(value.VECTOR3.x));
			dictionary_set(dict, BSCVL("y", 1), VARIANT_FLOAT(value.VECTOR3.y));
			dictionary_set(dict, BSCVL("z", 1), VARIANT_FLOAT(value.VECTOR3.z));
			return VARIANT_DICTIONARY(dict);
		} break;

		case VARIANT_TYPE_VECTOR3I: {
			Dictionary *dict = dictionary_create(3);
			dictionary_set(dict, BSCVL("x", 1), VARIANT_INT(value.VECTOR3I.x));
			dictionary_set(dict, BSCVL("y", 1), VARIANT_INT(value.VECTOR3I.y));
			dictionary_set(dict, BSCVL("z", 1), VARIANT_INT(value.VECTOR3I.z));
			return VARIANT_DICTIONARY(dict);
		} break;

		case VARIANT_TYPE_COLOR: {
			Dictionary *dict = dictionary_create(4);
			dictionary_set(dict, BSCVL("r", 1), VARIANT_FLOAT(value.COLOR.r));
			dictionary_set(dict, BSCVL("g", 1), VARIANT_FLOAT(value.COLOR.g));
			dictionary_set(dict, BSCVL("b", 1), VARIANT_FLOAT(value.COLOR.b));
			dictionary_set(dict, BSCVL("a", 1), VARIANT_FLOAT(value.COLOR.a));
			return VARIANT_DICTIONARY(dict);
		} break;

		case VARIANT_TYPE_RESOURCE: {
			return VARIANT_STRING(resource_get_path(value.RESOURCE));
		} break;

		default: {
			return value;
		} break;
	}
}