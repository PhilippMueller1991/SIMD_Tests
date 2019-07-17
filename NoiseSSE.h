#pragma once

#include <immintrin.h>
#include <smmintrin.h>

class NoiseSSE
{
public:
	static void CreateImage(int width, int height, int iterations);

	static void RunTests();
private:
	static inline __m128 Fade(__m128 t) noexcept;

	static inline __m128 Lerp(__m128 a, __m128 b, __m128 t) noexcept;

	static inline __m128 Dot(__m128 a, __m128 b) noexcept;

	static inline __m128 Fract(const __m128& x) noexcept;

	static void Hash(__m128 x, __m128& out) noexcept;

	static __m128 PerlinNoise(const __m128& v) noexcept;

	static void Print(__m128 x);
};