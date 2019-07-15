#pragma once
class OptimizationLimits
{
public:
	static void TestImplementations();
private:
	static void TestImplementationA(const float* A, const float* B, float* C, int u, int iterations);
	static void TestImplementationB(const float* A, const float* B, float* C, int u, int iterations);
};

