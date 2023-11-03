#ifndef MATH_H
#define MATH_H

#include "core/typedefs.h"

_FORCE_INLINE_ int32 math_max(int32 a, int32 b) { return a > b ? a : b; }
_FORCE_INLINE_ int32 math_min(int32 a, int32 b) { return a < b ? a : b; }

_FORCE_INLINE_ int32 math_clamp(int32 value, int32 min, int32 max) { return math_max(min, math_min(max, value)); }

#endif // MATH_H