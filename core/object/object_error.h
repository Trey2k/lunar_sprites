#ifndef OBJECT_ERROR_H
#define OBJECT_ERROR_H

#include "core/types/bstring.h"
#include "core/types/typedefs.h"
#include "core/types/variant.h"

typedef enum {
	OBJ_ERROR_NONE,
	// The provided object is invalid.
	OBJ_ERROR_INVALID,
	// The provided method does not exist.
	OBJ_ERR_CALL_NO_METHOD,
	// The provided argument is not the correct type.
	OBJ_ERR_CALL_ARG_TYPE,
	// Expected more arguments.
	OBJ_ERR_CALL_TOO_FEW_ARGS,
	// Expected fewer arguments.
	OBJ_ERR_CALL_TOO_MANY_ARGS,
	// The provided property does not exist.
	OBJ_ERR_PROPERTY_NOT_FOUND,
	// The provided property is not the correct type.
	OBJ_ERR_PROPERTY_TYPE,
} ObjectErrorType;

typedef struct {
	ObjectErrorType type;
	// The name of the object type.
	BString type_name;
	union {
		struct {
			BString method_name;
			BString arg_name;
			VariantType expected_type;
			VariantType actual_type;
			uint16 expected_args;
			uint16 actual_args;
		} call;
		struct {
			BString property_name;
			VariantType expected_type;
			VariantType actual_type;
		} property;
	};

} ObjectError;

_FORCE_INLINE_ bool check_object_error(ObjectError *error) {
	return error->type != OBJ_ERROR_NONE;
}

_FORCE_INLINE_ BString object_error_to_string(ObjectError *error);

#endif // OBJECT_ERROR_H