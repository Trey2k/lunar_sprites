#ifndef VARIANT_H
#define VARIANT_H

#include "core/debug.h"
#include "core/math/vector.h"
#include "core/object/object.h"
#include "core/resource/resource.h"
#include "core/types/array.h"
#include "core/types/bstring.h"
#include "core/types/color.h"
#include "core/types/dictionary.h"
#include "core/types/typedefs.h"

#define VARIANT_TYPE_NIL 0

#define VARIANT_TYPE_INT 1
#define VARIANT_TYPE_FLOAT 2
#define VARIANT_TYPE_BOOL 3
#define VARIANT_TYPE_STRING 4

#define VARIANT_TYPE_VECTOR2 5
#define VARIANT_TYPE_VECTOR2I 6
#define VARIANT_TYPE_VECTOR3 7
#define VARIANT_TYPE_VECTOR3I 8

#define VARIANT_TYPE_COLOR 9

#define VARIANT_TYPE_ARRAY 10
#define VARIANT_TYPE_DICTIONARY 11

#define VARIANT_TYPE_OBJECT 12
#define VARIANT_TYPE_RESOURCE 13
#define VARIANT_TYPE_VARIANT 14

struct Variant {
	VariantType type;

	union {
		int64 INT;
		float64 FLOAT;
		bool BOOL;
		BString STRING;

		Vector2 VECTOR2;
		Vector2i VECTOR2I;
		Vector3 VECTOR3;
		Vector3i VECTOR3I;

		Color COLOR;

		Array *ARRAY;
		Dictionary *DICTIONARY;

		Object *OBJECT;
		Resource *RESOURCE;
	};
};

BString variant_to_string(Variant val);

#define VARIANT_NIL ((Variant){ .type = VARIANT_TYPE_NIL })
#define VARIANT_INT(val) ((Variant){ .type = VARIANT_TYPE_INT, .INT = val })
#define VARIANT_FLOAT(val) ((Variant){ .type = VARIANT_TYPE_FLOAT, .FLOAT = val })
#define VARIANT_BOOL(val) ((Variant){ .type = VARIANT_TYPE_BOOL, .BOOL = val })
#define VARIANT_STRING(val) ((Variant){ .type = VARIANT_TYPE_STRING, .STRING = val })
#define VARIANT_VECTOR2(val) ((Variant){ .type = VARIANT_TYPE_VECTOR2, .VECTOR2 = val })
#define VARIANT_VECTOR2I(val) ((Variant){ .type = VARIANT_TYPE_VECTOR2I, .VECTOR2I = val })
#define VARIANT_VECTOR3(val) ((Variant){ .type = VARIANT_TYPE_VECTOR3, .VECTOR3 = val })
#define VARIANT_VECTOR3I(val) ((Variant){ .type = VARIANT_TYPE_VECTOR3I, .VECTOR3I = val })
#define VARIANT_COLOR(val) ((Variant){ .type = VARIANT_TYPE_COLOR, .COLOR = val })
#define VARIANT_DICTIONARY(val) ((Variant){ .type = VARIANT_TYPE_DICTIONARY, .DICTIONARY = val })
#define VARIANT_ARRAY(val) ((Variant){ .type = VARIANT_TYPE_ARRAY, .ARRAY = val })
#define VARIANT_OBJECT(val) ((Variant){ .type = VARIANT_TYPE_OBJECT, .OBJECT = val })
#define VARIANT_RESOURCE(val) ((Variant){ .type = VARIANT_TYPE_RESOURCE, .RESOURCE = val })

_FORCE_INLINE_ int32 variant_get_int(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_INT);
	return val.INT;
}

_FORCE_INLINE_ float32 variant_get_float(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_FLOAT);
	return val.FLOAT;
}

_FORCE_INLINE_ bool variant_get_bool(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_BOOL);
	return val.BOOL;
}

_FORCE_INLINE_ BString variant_get_string(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_STRING);
	return val.STRING;
}

_FORCE_INLINE_ Vector2 variant_get_vector2(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR2);
	return val.VECTOR2;
}

_FORCE_INLINE_ Vector2i variant_get_vector2i(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR2I);
	return val.VECTOR2I;
}

_FORCE_INLINE_ Vector3 variant_get_vector3(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR3);
	return val.VECTOR3;
}

_FORCE_INLINE_ Vector3i variant_get_vector3i(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR3I);
	return val.VECTOR3I;
}

_FORCE_INLINE_ Color variant_get_color(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_COLOR);
	return val.COLOR;
}

_FORCE_INLINE_ Dictionary *variant_get_dictionary(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_DICTIONARY);
	return val.DICTIONARY;
}

_FORCE_INLINE_ Array *variant_get_array(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_ARRAY);
	return val.ARRAY;
}

_FORCE_INLINE_ Object *variant_get_object(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_OBJECT);
	return val.OBJECT;
}

_FORCE_INLINE_ Resource *variant_get_resource(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_RESOURCE);
	return val.RESOURCE;
}

_FORCE_INLINE_ bool variant_is(Variant val, VariantType type) {
	return val.type == type;
}

_FORCE_INLINE_ bool variant_is_nil(Variant val) {
	return val.type == VARIANT_TYPE_NIL;
}

_FORCE_INLINE_ bool variant_equals(Variant a, Variant b) {
	if (a.type != b.type) {
		return false;
	}

	switch (a.type) {
		case VARIANT_TYPE_INT:
		case VARIANT_TYPE_FLOAT:
		// Only compare pointers for dictionaries.
		case VARIANT_TYPE_DICTIONARY:
		case VARIANT_TYPE_BOOL: {
			return a.INT == b.INT;
		} break;

		case VARIANT_TYPE_STRING: {
			return bstring_equals(a.STRING, b.STRING);
		} break;

		case VARIANT_TYPE_OBJECT: {
			return object_equals(a.OBJECT, b.OBJECT);
		} break;

		case VARIANT_TYPE_RESOURCE: {
			return object_equals(a.OBJECT, b.OBJECT);
		} break;

		case VARIANT_TYPE_ARRAY: {
			return array_equals(a.ARRAY, b.ARRAY);
		}

		case VARIANT_TYPE_VECTOR2: {
			return vec2_equals(a.VECTOR2, b.VECTOR2);
		} break;
		case VARIANT_TYPE_VECTOR2I: {
			return vec2i_equals(a.VECTOR2I, b.VECTOR2I);
		} break;
		case VARIANT_TYPE_VECTOR3: {
			return vec3_equals(a.VECTOR3, b.VECTOR3);
		} break;
		case VARIANT_TYPE_VECTOR3I: {
			return vec3i_equals(a.VECTOR3I, b.VECTOR3I);
		} break;

		case VARIANT_TYPE_COLOR: {
			return color_equals(a.COLOR, b.COLOR);
		} break;

		case VARIANT_TYPE_NIL: {
			return true;
		}

		default:
			return false;
	};

	return false;
}

_FORCE_INLINE_ BString variant_type_to_string(VariantType type) {
	switch (type) {
		case VARIANT_TYPE_NIL:
			return BSC("NIL");
		case VARIANT_TYPE_INT:
			return BSC("int");
		case VARIANT_TYPE_FLOAT:
			return BSC("float");
		case VARIANT_TYPE_BOOL:
			return BSC("bool");
		case VARIANT_TYPE_STRING:
			return BSC("String");
		case VARIANT_TYPE_VECTOR2:
			return BSC("Vector2");
		case VARIANT_TYPE_VECTOR2I:
			return BSC("Vector2i");
		case VARIANT_TYPE_VECTOR3:
			return BSC("Vector3");
		case VARIANT_TYPE_VECTOR3I:
			return BSC("Vector3i");
		case VARIANT_TYPE_COLOR:
			return BSC("Color");
		case VARIANT_TYPE_OBJECT:
			return BSC("Object");
		case VARIANT_TYPE_RESOURCE:
			return BSC("Resource");
		case VARIANT_TYPE_DICTIONARY:
			return BSC("Dictionary");
		case VARIANT_TYPE_ARRAY:
			return BSC("Array");
		case VARIANT_TYPE_VARIANT:
			return BSC("Variant");
		default:
			return BSC("Unknown");
	};
}

// https://en.wikipedia.org/wiki/Universal_hashing#Hashing_integers
#define VARIANT_HASH_RATIO 0x9E3779B9

_FORCE_INLINE_ uint64 variant_hash(Variant val) {
	switch (val.type) {
		case VARIANT_TYPE_INT:
			return (uint64)val.INT * VARIANT_HASH_RATIO;
		case VARIANT_TYPE_FLOAT:
			return (uint64)val.FLOAT * VARIANT_HASH_RATIO;
		case VARIANT_TYPE_BOOL:
			return (uint64)val.BOOL * VARIANT_HASH_RATIO;
		case VARIANT_TYPE_STRING:
			return bstring_hash(val.STRING);
		case VARIANT_TYPE_OBJECT:
			return (uint64)val.OBJECT * VARIANT_HASH_RATIO;
		case VARIANT_TYPE_RESOURCE:
			return (uint64)val.RESOURCE * VARIANT_HASH_RATIO;
		case VARIANT_TYPE_DICTIONARY:
			return dictionary_hash(val.DICTIONARY);
		case VARIANT_TYPE_ARRAY:
			return array_hash(val.ARRAY);
		case VARIANT_TYPE_VECTOR2:
			return vec2_hash(val.VECTOR2);
		case VARIANT_TYPE_VECTOR2I:
			return vec2i_hash(val.VECTOR2I);
		case VARIANT_TYPE_VECTOR3:
			return vec3_hash(val.VECTOR3);
		case VARIANT_TYPE_VECTOR3I:
			return vec3i_hash(val.VECTOR3I);
		case VARIANT_TYPE_COLOR:
			return color_hash(val.COLOR);
		case VARIANT_TYPE_NIL:
			return 0;
		default:
			return 0;
	};
}

_FORCE_INLINE_ void variant_ref(Variant val) {
	switch (val.type) {
		case VARIANT_TYPE_STRING:
			bstring_ref(val.STRING);
			break;
		case VARIANT_TYPE_OBJECT:
			object_ref(val.OBJECT);
			break;
		case VARIANT_TYPE_RESOURCE:
			resource_ref(val.RESOURCE);
			break;
		case VARIANT_TYPE_DICTIONARY:
			dictionary_ref(val.DICTIONARY);
			break;
		case VARIANT_TYPE_ARRAY:
			array_ref(val.ARRAY);
			break;
		default:
			break;
	};
}

_FORCE_INLINE_ void variant_unref(Variant val) {
	switch (val.type) {
		case VARIANT_TYPE_STRING:
			bstring_unref(val.STRING);
			break;
		case VARIANT_TYPE_OBJECT:
			object_unref(val.OBJECT);
			break;
		case VARIANT_TYPE_RESOURCE:
			resource_unref(val.RESOURCE);
			break;
		case VARIANT_TYPE_DICTIONARY:
			dictionary_unref(val.DICTIONARY);
			break;
		case VARIANT_TYPE_ARRAY:
			array_unref(val.ARRAY);
			break;
		default:
			break;
	};
}

#endif // VARIANT_H