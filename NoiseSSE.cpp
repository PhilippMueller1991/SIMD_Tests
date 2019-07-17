#include "NoiseSSE.h"

#include <iostream>
#include <math.h>
#include <chrono>
#include "Image.h"
#include "NoiseRef.h"

void NoiseSSE::CreateImage(int width, int height, int iterations)
{
	//RunTests();

	int widthSSE = width;
	if (width % 2 != 0)
	{
		widthSSE++;
	}

	float* data = new float[widthSSE * height];

	std::chrono::nanoseconds minTime = std::chrono::nanoseconds::max();
	std::chrono::nanoseconds averageTime = std::chrono::nanoseconds(0);
	for (int i = 0; i < iterations; i++)
	{
		auto time = std::chrono::high_resolution_clock::now();

		const __m128 factor = _mm_set_ps(
			8.0f / static_cast<float>(height),
			8.0f / static_cast<float>(widthSSE),
			8.0f / static_cast<float>(height),
			8.0f / static_cast<float>(widthSSE)
		);

		__m128 v;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < widthSSE; x += 2)
			{
				v = _mm_mul_ps(_mm_set_ps((float)y, (float)(x + 1), (float)y, (float)x), factor);
				v = PerlinNoise(v);
				data[y * widthSSE + x] = 0.5f + 0.5f * v.m128_f32[0];
				data[y * widthSSE + x + 1] = 0.5f + 0.5f * v.m128_f32[2];
			}
		}

		auto timeDelta = std::chrono::high_resolution_clock::now() - time;
		if (timeDelta < minTime)
			minTime = timeDelta;

		averageTime += timeDelta;
	}
	std::cout << "min: " << minTime.count() << " ns\t";
	std::cout << "avg: " << averageTime.count() / iterations << " ns\n";

	// Write data
	Image::SaveBitmap("NoiseSSE", widthSSE, height, data);

	// Cleanup
	delete[] data;
}

inline __m128 NoiseSSE::Fade(__m128 t) noexcept
{
	return _mm_mul_ps(_mm_mul_ps(t, t), _mm_sub_ps(_mm_set1_ps(3.0f), _mm_mul_ps(_mm_set1_ps(2.0f), t)));
}

inline __m128 NoiseSSE::Lerp(__m128 a, __m128 b, __m128 t) noexcept
{
	return _mm_add_ps(a, _mm_mul_ps(t, _mm_sub_ps(b, a)));
}

inline __m128 NoiseSSE::Dot(__m128 a, __m128 b) noexcept
{
	__m128 dp = _mm_mul_ps(a, b);
	return _mm_hadd_ps(dp, dp);
}

inline __m128 NoiseSSE::Fract(const __m128& x) noexcept
{
	return _mm_sub_ps(x, _mm_floor_ps(x));
}

void NoiseSSE::Hash(__m128 x, __m128& out) noexcept
{
	const __m128 k = _mm_set_ps(0.3678794f, 0.3183099f, 0.3678794f, 0.3183099f);

	x = _mm_add_ps(_mm_mul_ps(x, k), _mm_shuffle_ps(k, k, _MM_SHUFFLE(0, 1, 2, 3)));

	__m128 mul = _mm_mul_ps(x, _mm_shuffle_ps(x, x, _MM_SHUFFLE(2, 3, 0, 1)));
	__m128 add = _mm_hadd_ps(x, x);
	add = _mm_shuffle_ps(add, add, _MM_SHUFFLE(3, 1, 2, 0));

	__m128 tmp = Fract(_mm_mul_ps(mul, add));
	tmp = Fract(_mm_mul_ps(_mm_set1_ps(16.0f),_mm_mul_ps(k, tmp)));

	out = _mm_add_ps(_mm_set1_ps(-1.0f), _mm_mul_ps(_mm_set1_ps(2.0f), tmp));
}

__m128 NoiseSSE::PerlinNoise(const __m128& v) noexcept
{
	__m128 i = _mm_floor_ps(v);

	__m128 f = _mm_sub_ps(v, i);

	__m128 u = Fade(f);

	__m128 hash0_l;
	Hash(_mm_add_ps(_mm_shuffle_ps(i, i, _MM_SHUFFLE(1, 0, 1, 0)), _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f)), hash0_l);

	__m128 hash0_u;
	Hash(_mm_add_ps(_mm_shuffle_ps(i, i, _MM_SHUFFLE(1, 0, 1, 0)), _mm_set_ps(1.0f, 1.0f, 1.0f, 0.0f)), hash0_u);

	__m128 hash1_l;
	Hash(_mm_add_ps(_mm_shuffle_ps(i, i, _MM_SHUFFLE(3, 2, 3, 2)), _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f)), hash1_l);

	__m128 hash1_u;
	Hash(_mm_add_ps(_mm_shuffle_ps(i, i, _MM_SHUFFLE(3, 2, 3, 2)), _mm_set_ps(1.0f, 1.0f, 1.0f, 0.0f)), hash1_u);

	__m128 dotL0 = Dot(hash0_l, _mm_sub_ps(_mm_shuffle_ps(f, f, _MM_SHUFFLE(1, 0, 1, 0)), _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f)));
	__m128 dotU0 = Dot(hash0_u, _mm_sub_ps(_mm_shuffle_ps(f, f, _MM_SHUFFLE(1, 0, 1, 0)), _mm_set_ps(1.0f, 1.0f, 1.0f, 0.0f)));
	__m128 dot0 = _mm_shuffle_ps(dotL0, dotU0, _MM_SHUFFLE(3, 2, 1, 0));

	__m128 dotL1 = Dot(hash1_l, _mm_sub_ps(_mm_shuffle_ps(f, f, _MM_SHUFFLE(3, 2, 3, 2)), _mm_set_ps(0.0f, 1.0f, 0.0f, 0.0f)));
	__m128 dotU1 = Dot(hash1_u, _mm_sub_ps(_mm_shuffle_ps(f, f, _MM_SHUFFLE(3, 2, 3, 2)), _mm_set_ps(1.0f, 1.0f, 1.0f, 0.0f)));
	__m128 dot1 = _mm_shuffle_ps(dotL1, dotU1, _MM_SHUFFLE(3, 2, 1, 0));

	__m128 lerp_horizontal = Lerp(
		_mm_shuffle_ps(dot0, dot1, _MM_SHUFFLE(2, 0, 2, 0)),
		_mm_shuffle_ps(dot0, dot1, _MM_SHUFFLE(3, 1, 3, 1)),
		_mm_shuffle_ps(u, u, _MM_SHUFFLE(2, 2, 0, 0)));

	__m128 lerp_vertical = Lerp(
		_mm_shuffle_ps(lerp_horizontal, lerp_horizontal, _MM_SHUFFLE(2, 2, 0, 0)),
		_mm_shuffle_ps(lerp_horizontal, lerp_horizontal, _MM_SHUFFLE(3, 3, 1, 1)),
		_mm_shuffle_ps(u, u, _MM_SHUFFLE(3, 3, 1, 1)));

	return lerp_vertical;
}

void NoiseSSE::Print(__m128 x)
{
	std::cout << "(" << x.m128_f32[0] << "," << x.m128_f32[1] << "," << x.m128_f32[2] << "," << x.m128_f32[3] << ")";
}

void NoiseSSE::RunTests()
{
	// Fade
	std::cout << "Fade Reference: " << NoiseRef::Fade(0.65f) << "," << NoiseRef::Fade(0.5f) << "," << NoiseRef::Fade(0.35f) << "," << NoiseRef::Fade(0.1f);
	std::cout << std::endl;

	std::cout << "Fade SSE: ";
	Print(Fade(_mm_set_ps(0.1f, 0.35f, 0.5f, 0.65f)));	// note: direction is inverted for set
	std::cout << '\n' << std::endl;

	// Lerp
	std::cout << "Lerp Reference: " << NoiseRef::Lerp(8.0f, 10.0f, 0.5f) << ", " << NoiseRef::Lerp(7.0f, 13.0f, 0.5f) << ", " << NoiseRef::Lerp(6.0f, 16.0f, 0.5f) << ", " << NoiseRef::Lerp(5.0f, 19.0f, 0.5f);
	std::cout << std::endl;

	std::cout << "Lerp SSE: ";
	Print(Lerp(_mm_set_ps(5.0f, 6.0f, 7.0f, 8.0), _mm_set_ps(19.0f, 16.0f, 13.0f, 10.0f), _mm_set1_ps(0.5f))); // note: direction is inverted for set
	std::cout << '\n' << std::endl;

	// Dot
	float dot0 = NoiseRef::Dot(4.0f, 3.0f, 0.25f, 1.0f / 3.0f);
	float dot1 = NoiseRef::Dot(2.0f, 1.0f, -0.5f, -1.0f);
	std::cout << "Dot Reference: " << dot0 << ", " << dot1;
	std::cout << std::endl;

	std::cout << "Dot SSE: ";
	Print(Dot(_mm_set_ps(1.0f, 2.0f, 3.0f, 4.0f), _mm_set_ps(-1.0f, -0.5f, 1.0f / 3.0f, 0.25f))); // note: direction is inverted for set
	std::cout << '\n' << std::endl;

	// Fract
	std::cout << "Fract Reference: " << NoiseRef::Fract(4.567f) << ", " << NoiseRef::Fract(3.456f) << ", " << NoiseRef::Fract(2.345f) << ", " << NoiseRef::Fract(1.234f);
	std::cout << std::endl;

	std::cout << "Fract SSE: ";
	Print(Fract(_mm_set_ps(1.234f, 2.345f, 3.456f, 4.567f))); // note: direction is inverted for set
	std::cout << '\n' << std::endl;

	// Hash
	float resx0, resy0;
	NoiseRef::Hash(0.5f, 0.25f, resx0, resy0);
	std::cout << "Reference Hash: " << resx0 << "," << resy0 << ", ";
	float resx1, resy1;
	NoiseRef::Hash(0.1f, 0.125f, resx1, resy1);
	std::cout << resx1 << "," << resy1;
	std::cout << std::endl;

	__m128 out;
	Hash(_mm_set_ps(0.125f, 0.1f, 0.25f, 0.5f), out); // note: direction is inverted for set
	std::cout << "SSE Hash: ";
	Print(out);
	std::cout << '\n' << std::endl;

	// Perlin
	float perlin0;
	float perlin1;
	__m128 uv;

	std::cout << "Reference Perlin: ";
	perlin0 = NoiseRef::PerlinNoise(0.8f, 0.75f); // declare order here instead of an inline print otherwise the order of execution changes.
	perlin1 = NoiseRef::PerlinNoise(0.1f, 0.25f);
	std::cout << perlin0 << ", " << perlin1;
	std::cout << std::endl;

	std::cout << "SSE Perlin: ";
	uv = _mm_set_ps(0.25f, 0.1f, 0.75f, 0.8f);	// note: direction is inverted for set
	Print(PerlinNoise(uv));
	std::cout << '\n' << std::endl;


	std::cout << "Reference Perlin: ";
	perlin0 = NoiseRef::PerlinNoise(2.8f, 1.75f); // declare order here instead of an inline print otherwise the order of execution changes.
	perlin1 = NoiseRef::PerlinNoise(4.1f, 3.25f);
	std::cout << perlin0 << ", " << perlin1;
	std::cout << std::endl;

	std::cout << "SSE Perlin: ";
	uv = _mm_set_ps(3.25f, 4.1f, 1.75f, 2.8f);	// note: direction is inverted for set
	Print(PerlinNoise(uv));
	std::cout << '\n' << std::endl;
}