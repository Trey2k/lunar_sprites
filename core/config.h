#ifndef CONFIG_H
#define CONFIG_H

#include "types/hashtable.h"
#include "types/string.h"

// Hashtable ownership is not transferred
// All keys and values must be Strings
LS_EXPORT void write_config(String path, const Hashtable *config);

// Returns a new Hashtable. Ownership is transferred.
// Hashtable is a simple String to String mapping
LS_EXPORT Hashtable *read_config(String path);

#endif // CONFIG_H