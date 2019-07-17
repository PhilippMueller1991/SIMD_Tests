#pragma once

#include <immintrin.h>
#include <smmintrin.h>

#define VECTOR_CALL __vectorcall

class NoiseAVX2
{
public:
	static void CreateImage(int width, int height, int iterations);
	static void RunTests();
private:
	static inline __m256 Lerp(__m256 a, __m256 b, __m256 t) noexcept;
	static inline __m256 Dot(__m256 a, __m256 b) noexcept;
	static __m256 Hash(__m256& v) noexcept;
	static __m256 PerlinNoise(const __m256& v) noexcept;
	static void Print(const __m256& v);
};

