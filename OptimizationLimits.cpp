#include "OptimizationLimits.h"

#include <iostream>
#include <chrono>

static float Randf()
{
	return (rand() - 16384.0f) / 1024.0f;
}

void OptimizationLimits::TestImplementations()
{
	volatile int iterations = 10000;
	constexpr int size = 1024;

	float A[size];
	for (int i = 0; i < size; i++)
	{
		A[i] = Randf();
	}

	float B[size];
	for (int i = 0; i < size; i++)
	{
		B[i] = Randf();
	}

	float C[size] = { 0 };

	TestImplementationA(A, B, C, size, iterations);
	TestImplementationB(A, B, C, size, iterations);
}

void OptimizationLimits::TestImplementationA(const float* A, const float* B, float* C, volatile int size, volatile int iterations)
{
	std::chrono::nanoseconds bestTime = std::chrono::nanoseconds::max();
	auto time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; i++)
	{
		for (int j = 0; j < size; ++j)
			C[j] = A[j] * B[j];
	}

	bestTime = std::chrono::high_resolution_clock::now() - time;
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(bestTime).count() << "ns\n";
}

void OptimizationLimits::TestImplementationB(const float* A, const float* B, float* C, volatile int size, volatile int iterations)
{
	std::chrono::nanoseconds bestTime = std::chrono::nanoseconds::max();
	auto time = std::chrono::high_resolution_clock::now();

	for (int i = 0; i < iterations; i++)
	{
		for (int j = 0; j < size; ++j)
			C[j] = A[j] * B[j];
	}

	bestTime = std::chrono::high_resolution_clock::now() - time;
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(bestTime).count() << "ns\n";
}
