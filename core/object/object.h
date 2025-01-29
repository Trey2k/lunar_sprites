#ifndef OBJECT_H
#define OBJECT_H

#include "core/api.h"
#include "core/math/vector.h"
#include "core/types/bstring.h"
#include "core/types/typedefs.h"

typedef struct Object Object;

LS_EXPORT Object *object_create(BString type_name);
LS_EXPORT Object *object_create_from_data(BString type_name, void *data);
LS_EXPORT void object_ref(Object *object);
LS_EXPORT void object_unref(Object *object);

LS_EXPORT void object_draw(Object *object, float64 delta_time);

LS_EXPORT bool object_has_property(Object *object, BString name);
LS_EXPORT Variant object_get_property(Object *object, BString name);
LS_EXPORT void object_set_property(Object *object, BString name, Variant value);

LS_EXPORT bool object_has_method(Object *object, BString name);
LS_EXPORT Variant object_call_method(Object *object, BString name, Variant *args, size_t n_args);

LS_EXPORT void object_set_position(Object *object, Vector2i position);
LS_EXPORT Vector2i object_get_position(Object *object);
LS_EXPORT Vector2i object_get_global_position(Object *object);

LS_EXPORT void object_add_child(Object *parent, Object *child);
LS_EXPORT void object_remove_child(Object *parent, Object *child);

LS_EXPORT int32 object_get_child_count(Object *object);
LS_EXPORT Object *object_get_child(Object *object, int32 index);

LS_EXPORT Object *object_get_parent(Object *object);

LS_EXPORT bool object_equals(Object *a, Object *b);

LS_EXPORT void *object_get_data(Object *object);

LS_EXPORT uint64 object_get_type_id(Object *object);
LS_EXPORT BString object_get_type_name(Object *object);
LS_EXPORT BString object_to_string(Object *object);

#endif // OBJECT_H