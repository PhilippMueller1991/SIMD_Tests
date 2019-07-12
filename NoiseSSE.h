#pragma once

#include <immintrin.h>
#include <smmintrin.h>

class NoiseSSE
{
public:
	static void CreateImage(int width, int height);

private:
	static inline __m128 Fade(__m128 t) noexcept;

	static inline float Fade(float t) noexcept;

	static inline __m128 Lerp(__m128 a, __m128 b, __m128 t) noexcept;

	static inline float Lerp(float a, float b, float t) noexcept;
	
	static inline __m128 Dot(__m128 a, __m128 b) noexcept;

	static inline float Dot(float ax, float ay, float bx, float by) noexcept;

	static inline __m128 Fract(const __m128& x) noexcept;

	static inline float Fract(float x) noexcept;

	static void Hash(__m128 x, __m128& out);

	static void Hash(float x, float y, float& out_x, float& out_y);

	static float PerlinNoise(float x, float y);

	static __m128 PerlinNoise(const __m128& v);

	static void Print(__m128 x);

	static void RunTests();
};