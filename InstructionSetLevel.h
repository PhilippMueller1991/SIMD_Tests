#pragma once

// Reference implementation https://gist.github.com/rygorous/4172889

#include <immintrin.h>
#include <stdlib.h>

union Vec4f {
	float m[4];
	__m128 row;
};

#pragma region Implementations

void VecAddRef(Vec4f& out, const Vec4f& A, const Vec4f& B)
{
	for (int i = 0; i < 4; i++)
		out.m[i] = A.m[i] + B.m[i];
}

void VecAddSSE(Vec4f& out, const Vec4f& A, const Vec4f& B)
{
	out.row = _mm_add_ps(A.row, B.row);
}

#pragma endregion


#pragma region Rand

static float Randf()
{
	return (rand() - 16384.0f) / 1024.0f;
}

static void RandVec(Vec4f& M)
{
	for (int i = 0; i < 4; i++)
		M.m[i] = Randf();
}

#pragma endregion

#pragma region Run

int gAntiOptimizationMask = 0; // Global so the compiler can't be sure what its value is for opt.

static void RunRef(Vec4f* out, const Vec4f* A, const Vec4f* B, int count)
{
	for (int i = 0; i < count; i++)
	{
		int j = i & gAntiOptimizationMask; // Only take first element
		VecAddRef(out[j], A[j], B[j]);
	}
}

static void RunSSE(Vec4f* out, const Vec4f* A, const Vec4f* B, int count)
{
	for (int i = 0; i < count; i++)
	{
		int j = i & gAntiOptimizationMask; // Only take first element
		VecAddSSE(out[j], A[j], B[j]);
	}
}

#pragma endregion

static int RunCorrectnessTest()
{
	srand(1234); // Fixed seed

	for (int i = 0; i < 1000000; i++)
	{
		Vec4f A, B, out, ref_out;
		RandVec(A);
		RandVec(B);

		VecAddRef(ref_out, A, B);
		VecAddSSE(out, A, B);

		if (memcmp(&out, &ref_out, sizeof(out)) != 0)
		{
			fprintf(stderr, "SSE test failed.\n");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

static void RunPerformanceTest()
{
	static constexpr struct {
		const char* name;
		void (*Run)(Vec4f* out, const Vec4f* A, const Vec4f* B, int count);
	} perfVariants[] = {
		{ "ref",  RunRef },
		{ "SSE",  RunSSE },
	};
	static constexpr int nPerfVars = (int)(sizeof(perfVariants) / sizeof(*perfVariants));

	Vec4f Aperf, Bperf, out;
	RandVec(Aperf);
	RandVec(Bperf);

	for (int i = 0; i < nPerfVars; i++)
	{
		static constexpr int nRuns = 4096;
		static constexpr int addsPerRun = 4096;

		unsigned long long bestTime = ~0ull;
		unsigned long long sumTime = 0ull;

		for (int run = 0; run < nRuns; run++)
		{
			unsigned long long time = __rdtsc(); // The processor time stamp records the number of clock cycles since the last reset
			perfVariants[i].Run(&out, &Aperf, &Bperf, addsPerRun);
			time = __rdtsc() - time;

			sumTime += time;

			if (time < bestTime)
				bestTime = time;
		}

		double avgTime = (double)sumTime / (double)(addsPerRun * nRuns);
		double cyclesPerRun = (double)bestTime / (double)addsPerRun;
		printf("%12s: %.2f average cycles | %.2f lowest cycles\n", perfVariants[i].name, avgTime, cyclesPerRun);
	}
}

static int RunTests()
{
	if (RunCorrectnessTest())
		return EXIT_FAILURE;
	
	printf("All tests correct.\n");

	RunPerformanceTest();

	return EXIT_SUCCESS;
}