#ifndef MATH_H
#define MATH_H

#include "core/api.h"
#include "core/types/typedefs.h"

#undef PI
#undef FLOAT32_MAX
#undef FLOAT32_MIN
#undef FLOAT64_MAX
#undef FLOAT64_MIN
#undef INT32_MAX
#undef INT32_MIN
#undef UINT32_MAX
#undef INT64_MAX
#undef INT64_MIN
#undef UINT64_MAX

#define FLOAT32_MAX 3.402823466e+38F
#define FLOAT32_MIN -3.402823466e+38F
#define FLOAT64_MAX 1.7976931348623158e+308
#define FLOAT64_MIN -1.7976931348623158e+308

#define INT32_MAX 2147483647
#define INT32_MIN -2147483648
#define UINT32_MAX 4294967295
#define INT64_MAX 9223372036854775807
#define INT64_MIN -9223372036854775808
#define UINT64_MAX 18446744073709551615

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

LS_EXPORT float32 math_sqrtf(float32 x);

LS_EXPORT float32 math_deg_to_rad(float32 degrees);
LS_EXPORT float32 math_rad_to_deg(float32 radians);

#endif // MATH_H