#ifndef SCENE_H
#define SCENE_H

#include "core/object/object.h"
#include "core/types/bstring.h"

typedef struct Scene Scene;

// Creates a new scene instance.
LS_EXPORT Scene *scene_create();
// Destroys the scene instance.
LS_EXPORT void scene_destroy(Scene *scene);

// Parses the scene file at the given path.
LS_EXPORT void scene_parse(Scene *scene, BString path);
// Saves the scene to the given path.
LS_EXPORT void scene_save(Scene *scene, BString path);

LS_EXPORT void scene_add_object(Scene *scene, Object *object);
LS_EXPORT uint64 scene_get_object_count(Scene *scene);
LS_EXPORT Object *scene_get_object(Scene *scene, uint64 index);
LS_EXPORT void scene_remove_object(Scene *scene, uint64 index);

LS_EXPORT void scene_update(Scene *scene, float64 delta_time);
LS_EXPORT void scene_draw(Scene *scene, float64 delta_time);

#endif // SCENE_H