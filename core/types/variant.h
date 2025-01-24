#ifndef VARIANT_H
#define VARIANT_H

#include "core/debug.h"
#include "core/math/vector.h"
#include "core/object/object.h"
#include "core/resource/resource.h"
#include "core/types/color.h"
#include "core/types/string.h"
#include "core/types/typedefs.h"

typedef enum {
	VARIANT_TYPE_NIL,

	VARIANT_TYPE_INT,
	VARIANT_TYPE_FLOAT,
	VARIANT_TYPE_BOOL,
	VARIANT_TYPE_STRING,

	VARIANT_TYPE_VECTOR2,
	VARIANT_TYPE_VECTOR2I,
	VARIANT_TYPE_VECTOR3,
	VARIANT_TYPE_VECTOR3I,

	VARIANT_TYPE_COLOR,

	VARIANT_TYPE_OBJECT,
	VARIANT_TYPE_RESOURCE,
	VARIANT_TYPE_VARIANT
} VariantType;

struct Variant {
	VariantType type;

	union {
		int32 INT;
		float32 FLOAT;
		bool BOOL;
		String STRING;

		Vector2 VECTOR2;
		Vector2i VECTOR2I;
		Vector3 VECTOR3;
		Vector3i VECTOR3I;

		Color COLOR;

		Object *OBJECT;
		Resource *RESOURCE;
	};
};

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
#define VARIANT_OBJECT(val) ((Variant){ .type = VARIANT_TYPE_OBJECT, .OBJECT = val })
#define VARIANT_RESOURCE(val) ((Variant){ .type = VARIANT_TYPE_RESOURCE, .RESOURCE = val })

_FORCE_INLINE_ int32 variant_to_int(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_INT);
	return val.INT;
}

_FORCE_INLINE_ float32 variant_to_float(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_FLOAT);
	return val.FLOAT;
}

_FORCE_INLINE_ bool variant_to_bool(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_BOOL);
	return val.BOOL;
}

_FORCE_INLINE_ String variant_to_string(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_STRING);
	return val.STRING;
}

_FORCE_INLINE_ Vector2 variant_to_vector2(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR2);
	return val.VECTOR2;
}

_FORCE_INLINE_ Vector2i variant_to_vector2i(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR2I);
	return val.VECTOR2I;
}

_FORCE_INLINE_ Vector3 variant_to_vector3(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR3);
	return val.VECTOR3;
}

_FORCE_INLINE_ Vector3i variant_to_vector3i(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_VECTOR3I);
	return val.VECTOR3I;
}

_FORCE_INLINE_ Color variant_to_color(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_COLOR);
	return val.COLOR;
}

_FORCE_INLINE_ Object *variant_to_object(Variant val) {
	LS_ASSERT(val.type == VARIANT_TYPE_OBJECT);
	return val.OBJECT;
}

_FORCE_INLINE_ Resource *variant_to_resource(Variant val) {
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
		case VARIANT_TYPE_BOOL: {
			return a.INT == b.INT;
		} break;

		case VARIANT_TYPE_STRING: {
			return ls_str_equals(a.STRING, b.STRING);
		} break;

		case VARIANT_TYPE_OBJECT: {
			return object_equals(a.OBJECT, b.OBJECT);
		} break;

		case VARIANT_TYPE_RESOURCE: {
			return object_equals(a.OBJECT, b.OBJECT);
		} break;

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

_FORCE_INLINE_ String variant_type_to_string(VariantType type) {
	switch (type) {
		case VARIANT_TYPE_NIL:
			return "NIL";
		case VARIANT_TYPE_INT:
			return "int";
		case VARIANT_TYPE_FLOAT:
			return "float";
		case VARIANT_TYPE_BOOL:
			return "bool";
		case VARIANT_TYPE_STRING:
			return "String";
		case VARIANT_TYPE_VECTOR2:
			return "Vector2";
		case VARIANT_TYPE_VECTOR2I:
			return "Vector2i";
		case VARIANT_TYPE_VECTOR3:
			return "Vector3";
		case VARIANT_TYPE_VECTOR3I:
			return "Vector3i";
		case VARIANT_TYPE_COLOR:
			return "Color";
		case VARIANT_TYPE_OBJECT:
			return "Object";
		case VARIANT_TYPE_RESOURCE:
			return "Resource";
		case VARIANT_TYPE_VARIANT:
			return "Variant";
		default:
			return "Unknown";
	};
}

#endif // VARIANT_H