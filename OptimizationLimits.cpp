#include "OptimizationLimits.h"

#include <iostream>
#include <chrono>
#include <immintrin.h>

void OptimizationLimits::TestImplementations(int iter)
{
	constexpr int size = 64;

	float a[size];
	float b[size];

	for (int i = 1; i < iter; i++)
	{
		a[i] = a[i - 1] + b[i - 1];

		if (a[i] < 5)
			break;
	}
}