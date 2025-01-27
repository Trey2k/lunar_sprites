#include "core/types/variant.h"

#include "core/object/object.h"
#include "core/resource/resource.h"
#include "core/types/array.h"

BString variant_to_string(Variant val) {
	switch (val.type) {
		case VARIANT_TYPE_INT:
			return bstring_format("%lld", val.INT);
		case VARIANT_TYPE_FLOAT:
			return bstring_format("%lf", val.FLOAT);
		case VARIANT_TYPE_BOOL:
			return val.BOOL ? BSC("true") : BSC("false");
		case VARIANT_TYPE_STRING:
			return bstring_format("\"%S\"", val.STRING);
		case VARIANT_TYPE_OBJECT:
			return object_to_string(val.OBJECT);
		case VARIANT_TYPE_RESOURCE:
			return resource_to_string(val.RESOURCE);
		case VARIANT_TYPE_DICTIONARY:
			return dictionary_to_string(val.DICTIONARY);
		case VARIANT_TYPE_ARRAY:
			return array_to_string(val.ARRAY);
		case VARIANT_TYPE_VECTOR2:
			return bstring_format("%v2f", val.VECTOR2);
		case VARIANT_TYPE_VECTOR2I:
			return bstring_format("%v2i", val.VECTOR2I);
		case VARIANT_TYPE_VECTOR3:
			return bstring_format("%v3f", val.VECTOR3);
		case VARIANT_TYPE_VECTOR3I:
			return bstring_format("%v3i", val.VECTOR3I);
		case VARIANT_TYPE_COLOR:
			return bstring_format("%C", val.COLOR);
		case VARIANT_TYPE_NIL:
			return BSC("nil");
		default:
			return BSC("Unknown");
	};
}