#pragma once

#include <math.h>
#include <chrono>
#include "Settings.h"
#include "Image.h"

class NoiseRef
{
public:
	static void CreateImage(int width, int height)
	{
		float* data = new float[width * height];

		std::chrono::nanoseconds bestTime = std::chrono::nanoseconds::max();
		for(int i=0; i<Settings::iterations; i++)
		{
			auto time = std::chrono::high_resolution_clock::now();

			const float factorx = 8.0f / static_cast<float>(width);
			const float factory = 8.0f / static_cast<float>(height);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					float xres = static_cast<float>(x) * factorx;
					float yres = static_cast<float>(y) * factory;

					data[y * width + x] = 0.5f + 0.5f * PerlinNoise(xres, yres);
				}
			}

			auto timeDelta = std::chrono::high_resolution_clock::now() - time;
			if (timeDelta < bestTime)
				bestTime = timeDelta;
		}
		printf("Reference implementation executed in: %.4f ms\n\n", std::chrono::duration_cast<std::chrono::microseconds>(bestTime).count() / 1000.0);

		// Write data
		Image::SaveBitmap("NoiseRef", width, height, data);
			
		// Cleanup
		delete[] data;
	}

private:
	static inline float Fade(float t) noexcept
	{
		return t * t * (3.0f - 2.0f * t);
	}

	static inline float Lerp(float a, float b, float t) noexcept
	{
		return a + t * (b - a);
	}

	static inline float Dot(float ax, float ay, float bx, float by) noexcept
	{
		return ax * bx + ay * by;
	}

	static inline float Fract(float x) noexcept
	{
		return x - floorf(x);
	}

	static void Hash(float x, float y, float& out_x, float& out_y)
	{
		constexpr float kx = 0.3183099f;
		constexpr float ky = 0.3678794f;

		x = x * kx + ky;
		y = y * ky + kx;

		float tmp = Fract(x*y  * (x + y));

		out_x = -1.0f + 2.0f * Fract(16.0f * kx * tmp);
		out_y = -1.0f + 2.0f * Fract(16.0f * ky * tmp);
	}

	static float PerlinNoise(float x, float y)
	{
		float xi = floorf(x);
		float yi = floorf(y);

		float xf = x - xi;
		float yf = y - yi;

		float u = Fade(xf);
		float v = Fade(yf);

		float ll_x;
		float ll_y;
		Hash(xi, yi, ll_x, ll_y);

		float lr_x;
		float lr_y;
		Hash(xi+1, yi, lr_x, lr_y);

		float ul_x;
		float ul_y;
		Hash(xi, yi+1, ul_x, ul_y);

		float ur_x;
		float ur_y;
		Hash(xi+1, yi+1, ur_x, ur_y);

		return Lerp( Lerp( Dot( ll_x, ll_y, xf,			yf ),
						   Dot( lr_x, lr_y, xf - 1.0f,	yf ),		 u ),
					 Lerp( Dot( ul_x, ul_y, xf,			yf - 1.0f),
						   Dot( ur_x, ur_y, xf - 1.0f,	yf - 1.0f), u ), v );
	}
};