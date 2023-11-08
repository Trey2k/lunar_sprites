#ifndef HASHTABLE_INT_H
#define HASHTABLE_INT_H

#include "core/types/typedefs.h"

typedef struct HashtableInt HashtableInt;

HashtableInt *hashtable_int_create(size_t capacity);
void hashtable_int_destroy(HashtableInt *hashtable);

void hashtable_int_set(HashtableInt *hashtable, int32 key, void *data);
void *hashtable_int_get(const HashtableInt *hashtable, int32 key);
bool hashtable_int_has(const HashtableInt *hashtable, int32 key);
bool hashtable_int_remove(HashtableInt *hashtable, int32 key);

#endif // HASHTABLE_INT_H