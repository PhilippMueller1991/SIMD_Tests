#include "OptimizationLimits.h"

#include <iostream>
#include <chrono>
#include <immintrin.h>

inline float Func(float ax, float ay, float bx, float by) noexcept
{
	return ax * ay + bx * by;
}

void OptimizationLimits::TestImplementations()
{
	constexpr int iter = 64;

	float a[iter];
	float b[iter];
	float c[iter];

	for (int i = 0; i < iter-1; i++)
		c[i] = Func(a[i], a[i + 1], b[i], b[i + 1]);
}