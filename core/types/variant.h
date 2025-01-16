#ifndef VARIANT_H
#define VARIANT_H

#include "core/math/vector.h"
#include "core/object/object.h"
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
} VariantType;

typedef struct {
	VariantType type;

	union {
		void *NIL;

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
	} val;
} Variant;

#define TO_VARIANT(val, type) ((Variant){ .type = VARIANT_TYPE_##type, .val.type = val })

#endif // VARIANT_H