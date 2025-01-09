#include "core/math/math.h"

#define MAX_ITERATIONS 10

float32 math_tanf(float32 x) {
	float32 cos_val = math_cosf(x);
	if (cos_val == 0.0f) {
		// Handle division by zero error, return maximum float value
		return FLOAT32_MAX;
	}
	return math_sinf(x) / cos_val;
}

float32 math_sinf(float32 x) {
	float32 sin = 0;

	for (int32 i = 0; i < MAX_ITERATIONS; i++) {
		float32 term = math_pow(-1, i) * math_pow(x, 2 * i + 1) / math_factorial(2 * i + 1);
		sin += term;
	}

	return sin;
}

float32 math_cosf(float32 x) {
	float32 cos = 0;

	for (int i = 0; i < MAX_ITERATIONS; i++) {
		float32 term = math_pow(-1, i) * math_pow(x, 2 * i) / math_factorial(2 * i);
		cos += term;
	}

	return cos;
}

float32 math_pow(float32 base, int32 exponent) {
	float32 result = 1.0f;
	for (int32 i = 0; i < exponent; i++) {
		result *= base;
	}
	return result;
}

int32 math_factorial(int32 n) {
	int32 result = 1;
	for (int32 i = 1; i <= n; i++) {
		result *= i;
	}
	return result;
}

float32 math_sqrtf(float32 x) {
	float32 result = x;
	float32 xhalf = 0.5f * x;
	int32 i = *(int32 *)&result;
	i = 0x5f3759df - (i >> 1);
	result = *(float32 *)&i;
	result = result * (1.5f - xhalf * result * result);
	return 1.0f / result;
}

float32 math_deg_to_rad(float32 degrees) {
	return degrees * (PI / 180.0f);
}

float32 math_rad_to_deg(float32 radians) {
	return radians * (180.0f / PI);
}