#ifndef JSON_H
#define JSON_H

#include "core/types/bstring.h"
#include "core/types/dictionary.h"

// Parses a JSON string into a Variant.
// Does not perform any allocations for strings. Any string key, or value will be a sub string of the input.
// Returns a Variant of type VARIANT_TYPE_DICTIONARY if the JSON is an object, or VARIANT_TYPE_ARRAY if the JSON is an array.
// Number values will be assumed to be integers if they do not contain a decimal point.
LS_EXPORT Variant json_parse(const BString json);
// Converts a Variant into a JSON string.
LS_EXPORT BString json_stringify(Variant value);
// Converts a Variant into a JSON string with pretty formatting.
LS_EXPORT BString json_stringify_pretty(Variant value, uint32 spaces);

#endif // JSON_H