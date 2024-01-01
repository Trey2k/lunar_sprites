#ifndef MATH_H
#define MATH_H

#include "core/api.h"
#include "core/types/typedefs.h"

#define FLOAT32_MAX 3.402823466e+38F
#define FLOAT32_MIN -3.402823466e+38F

#define PI 3.14159265358979323846f

_FORCE_INLINE_ int32 math_max(int32 a, int32 b) { return a > b ? a : b; }
_FORCE_INLINE_ int32 math_min(int32 a, int32 b) { return a < b ? a : b; }

_FORCE_INLINE_ int32 math_clamp(int32 value, int32 min, int32 max) { return math_max(min, math_min(max, value)); }

// Angle in radians
LS_EXPORT float32 math_tanf(float32 x);
// Taylor series, angle in radians
LS_EXPORT float32 math_sinf(float32 x);
// Taylor series, angle in radians
LS_EXPORT float32 math_cosf(float32 x);
LS_EXPORT float32 math_pow(float32 base, int32 exponent);
LS_EXPORT int32 math_factorial(int32 n);

LS_EXPORT float32 math_deg_to_rad(float32 degrees);
LS_EXPORT float32 math_rad_to_deg(float32 radians);

#endif // MATH_H