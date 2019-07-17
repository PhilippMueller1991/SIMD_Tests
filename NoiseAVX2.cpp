#include "NoiseAVX2.h"

#include <iostream>
#include <chrono>
#include "Image.h"
#include "NoiseRef.h"

void NoiseAVX2::CreateImage(int width, int height, int iterations)
{
	int widthAVX = width;
	if (width % 4 != 0)
	{
		widthAVX += width % 4;
	}

	float* data = new float[widthAVX * height];

	std::chrono::nanoseconds minTime = std::chrono::nanoseconds::max();
	std::chrono::nanoseconds averageTime = std::chrono::nanoseconds(0);
	for (int i = 0; i < iterations; i++)
	{
		auto time = std::chrono::high_resolution_clock::now();

		const float fh = 8.0f / static_cast<float>(height);
		const float fw = 8.0f / static_cast<float>(widthAVX);

		const __m256 factor = _mm256_set_ps(fh,fw,fh,fw,fh,fw,fh,fw);

		const __m256 half = _mm256_set1_ps(0.5f);

		__m256 v;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < widthAVX; x += 4)
			{
				v = _mm256_set_ps(
					y, x + 3, 
					y, x + 2,
					y, x + 1,
					y, x);
				v = _mm256_mul_ps(v, factor);
				v = PerlinNoise(v);

				v = _mm256_add_ps(half, _mm256_mul_ps(half, v));

				data[y * widthAVX + x + 0] = v.m256_f32[0];
				data[y * widthAVX + x + 1] = v.m256_f32[2];
				data[y * widthAVX + x + 2] = v.m256_f32[4];
				data[y * widthAVX + x + 3] = v.m256_f32[6];
			}
		}

		auto timeDelta = std::chrono::high_resolution_clock::now() - time;
		if (timeDelta < minTime)
			minTime = timeDelta;

		averageTime += timeDelta;
	}
	std::cout << "min: " << minTime.count() << " ns\t";
	std::cout << "avg: " << averageTime.count() / iterations << "ns\n";

	// Write data
	Image::SaveBitmap("NoiseAVX2", widthAVX, height, data);

	// Cleanup
	delete[] data;
}

void NoiseAVX2::RunTests()
{
	float vals[8];
	__m256 a;
	__m256 b;
	__m256 v;

	// Lerp
	vals[0] = NoiseRef::Lerp(8.0f, 10.0f, 0.5f);
	vals[1] = NoiseRef::Lerp(7.0f, 13.0f, 0.5f);
	vals[2] = NoiseRef::Lerp(6.0f, 16.0f, 0.5f);
	vals[3] = NoiseRef::Lerp(5.0f, 19.0f, 0.5f);
	vals[4] = NoiseRef::Lerp(8.0f, 10.0f, 0.25f);
	vals[5] = NoiseRef::Lerp(7.0f, 13.0f, 0.75f);
	vals[6] = NoiseRef::Lerp(6.0f, 16.0f, 0.15f);
	vals[7] = NoiseRef::Lerp(5.0f, 19.0f, 0.95f);
	
	a = _mm256_set_ps(5.0f, 6.0f, 7.0f, 8.0f, 5.0f, 6.0f, 7.0f, 8.0f);
	b = _mm256_set_ps(19.0f, 16.0f, 13.0f, 10.0f, 19.0f, 16.0f, 13.0f, 10.0f);
	v = _mm256_set_ps(0.95f, 0.15f, 0.75f, 0.25f, 0.5f, 0.5f, 0.5f, 0.5f);
	v = Lerp(a, b, v);

	for (int i = 0; i < 8; i++)
		if (v.m256_f32[i] != vals[i])
			std::cerr << "Lerp AVX2 incorrect!" << v.m256_f32[i] << " != " << vals[i] << "\n";

	// Hash
	NoiseRef::Hash(1.0f, 2.0f, vals[0], vals[1]);
	NoiseRef::Hash(3.0f, 4.0f, vals[2], vals[3]);
	NoiseRef::Hash(5.0f, 6.0f, vals[4], vals[5]);
	NoiseRef::Hash(7.0f, 8.0f, vals[6], vals[7]);

	v = _mm256_set_ps(8.0, 7.0, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f);
	v = Hash(v);

	for (int i = 0; i < 8; i++)
		if (v.m256_f32[i] != vals[i])
			std::cerr << "Hash AVX2 incorrect!" << v.m256_f32[i] << " != " << vals[i] << "\n";

	// Dot
	vals[0] = NoiseRef::Dot(4.0f, 3.0f, 0.25f, 1.0f / 3.0f);
	vals[1] = NoiseRef::Dot(2.0f, 1.0f, -0.5f, -1.0f);
	vals[2] = NoiseRef::Dot(4.0f, 3.0f, 0.25f, 1.0f / 3.0f);
	vals[3] = NoiseRef::Dot(2.0f, 1.0f, -0.5f, -1.0f);

	a = _mm256_set_ps(1.0f, 2.0f, 3.0f, 4.0f, 1.0f, 2.0f, 3.0f, 4.0f);
	b = _mm256_set_ps(-1.0f, -0.5f, 1.0f / 3.0f, 0.25f, -1.0f, -0.5f, 1.0f / 3.0f, 0.25f);
	v = Dot(a,b);

	for (int i = 0; i < 4; i++)
		if (v.m256_f32[i] != vals[i])
			std::cerr << "Dot AVX2 incorrect!" << v.m256_f32[i] << " != " << vals[i] << "\n";

	// Perlin
	vals[0] = NoiseRef::PerlinNoise(2.8f, 1.75f);
	vals[1] = NoiseRef::PerlinNoise(4.1f, 3.25f);
	vals[2] = NoiseRef::PerlinNoise(6.5f, 1.25f);
	vals[3] = NoiseRef::PerlinNoise(0.1f, 6.25f);
	
	vals[4] = 0.0f;
	vals[5] = 0.0f;
	vals[6] = 0.0f;
	vals[7] = 0.0f;

	v = _mm256_set_ps(6.25f, 0.1, 1.25f, 6.5f, 3.25f, 4.1f, 1.75f, 2.8f);

	v = PerlinNoise(v);

	for (int i = 0; i < 4; i++)
		if (v.m256_f32[i*2] != vals[i])
			std::cerr << "Dot AVX2 incorrect!" << v.m256_f32[i*2] << " != " << vals[i] << "\n";
}

__m256 NoiseAVX2::PerlinNoise(const __m256& v) noexcept
{
	__m256 i = _mm256_floor_ps(v);
	__m256 f = _mm256_sub_ps(v, i);

	__m256 u = _mm256_sub_ps(_mm256_set1_ps(3.0f), _mm256_mul_ps(_mm256_set1_ps(2.0f), f));
	u = _mm256_mul_ps(_mm256_mul_ps(f, f), u);

	__m256 lower = _mm256_set_ps(0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f);
	__m256 upper = _mm256_set_ps(1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 0.0f);

	__m256 tmp = _mm256_add_ps(_mm256_shuffle_ps(i, i, _MM_SHUFFLE(1, 0, 1, 0)), lower);
	__m256 hash0_l = Hash(tmp);

	tmp = _mm256_add_ps(_mm256_shuffle_ps(i, i, _MM_SHUFFLE(1, 0, 1, 0)), upper);
	__m256 hash0_u = Hash(tmp);

	tmp = _mm256_add_ps(_mm256_shuffle_ps(i, i, _MM_SHUFFLE(3, 2, 3, 2)), lower);
	__m256 hash1_l = Hash(tmp);

	tmp = _mm256_add_ps(_mm256_shuffle_ps(i, i, _MM_SHUFFLE(3, 2, 3, 2)), upper);
	__m256 hash1_u = Hash(tmp);

	__m256 dotL0 = Dot(hash0_l, _mm256_sub_ps(_mm256_shuffle_ps(f, f, _MM_SHUFFLE(1, 0, 1, 0)), lower));
	__m256 dotU0 = Dot(hash0_u, _mm256_sub_ps(_mm256_shuffle_ps(f, f, _MM_SHUFFLE(1, 0, 1, 0)), upper));
	__m256 dot0 = _mm256_shuffle_ps(dotL0, dotU0, _MM_SHUFFLE(3, 2, 1, 0));

	__m256 dotL1 = Dot(hash1_l, _mm256_sub_ps(_mm256_shuffle_ps(f, f, _MM_SHUFFLE(3, 2, 3, 2)), lower));
	__m256 dotU1 = Dot(hash1_u, _mm256_sub_ps(_mm256_shuffle_ps(f, f, _MM_SHUFFLE(3, 2, 3, 2)), upper));
	__m256 dot1 = _mm256_shuffle_ps(dotL1, dotU1, _MM_SHUFFLE(3, 2, 1, 0));

	__m256 lerp_horizontal = Lerp(
		_mm256_shuffle_ps(dot0, dot1, _MM_SHUFFLE(2, 0, 2, 0)),
		_mm256_shuffle_ps(dot0, dot1, _MM_SHUFFLE(3, 1, 3, 1)),
		_mm256_shuffle_ps(u, u, _MM_SHUFFLE(2, 2, 0, 0)));

	__m256 lerp_vertical = Lerp(
		_mm256_shuffle_ps(lerp_horizontal, lerp_horizontal, _MM_SHUFFLE(2, 2, 0, 0)),
		_mm256_shuffle_ps(lerp_horizontal, lerp_horizontal, _MM_SHUFFLE(3, 3, 1, 1)),
		_mm256_shuffle_ps(u, u, _MM_SHUFFLE(3, 3, 1, 1)));

	return lerp_vertical;
}

inline __m256 NoiseAVX2::Lerp(__m256 a, __m256 b, __m256 t) noexcept
{
	return _mm256_add_ps(a, _mm256_mul_ps(t, _mm256_sub_ps(b, a)));
}

inline __m256 NoiseAVX2::Dot(__m256 a, __m256 b) noexcept
{
	__m256 dp = _mm256_mul_ps(a, b);
	dp = _mm256_hadd_ps(dp, dp);
	return dp;
}

__m256 NoiseAVX2::Hash(__m256& v) noexcept
{
	const __m256 k = _mm256_set_ps(
		0.3678794f, 0.3183099f,	// y0, x0
		0.3678794f, 0.3183099f,	// y1, x1
		0.3678794f, 0.3183099f,	// y2, x2
		0.3678794f, 0.3183099f);// y3, x3

	__m256 x = _mm256_add_ps(_mm256_mul_ps(v, k), _mm256_shuffle_ps(k, k, _MM_SHUFFLE(0, 1, 2, 3)));

	__m256 f = _mm256_mul_ps(x, _mm256_shuffle_ps(x, x, _MM_SHUFFLE(2, 3, 0, 1)));
	__m256 add = _mm256_hadd_ps(x, x);
	add = _mm256_shuffle_ps(add, add, _MM_SHUFFLE(3, 1, 2, 0));

	f = _mm256_mul_ps(f, add);
	f = _mm256_sub_ps(f, _mm256_floor_ps(f));
	f = _mm256_mul_ps(_mm256_set1_ps(16.0f), _mm256_mul_ps(k, f));
	f = _mm256_sub_ps(f, _mm256_floor_ps(f));
	
	f = _mm256_mul_ps(_mm256_set1_ps(2.0f), f);
	f = _mm256_sub_ps(f, _mm256_set1_ps(1.0f));

	return f;
}

void NoiseAVX2::Print(const __m256& v)
{
	std::cout << "(";
	for (int i = 0; i < 7; i++)
	{
		std::cout << v.m256_f32[i] << ",";
	}
	std::cout << v.m256_f32[7] << ")";
}
