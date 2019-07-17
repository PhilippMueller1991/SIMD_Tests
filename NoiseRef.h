#pragma once

class NoiseRef
{
public:
	static void CreateImage(int width, int height, int iterations);

	static inline float Fade(float t) noexcept;

	static inline float Lerp(float a, float b, float t) noexcept;

	static inline float Dot(float ax, float ay, float bx, float by) noexcept;

	static inline float Fract(float x) noexcept;

	static void Hash(float x, float y, float& out_x, float& out_y) noexcept;

	static float PerlinNoise(float x, float y) noexcept;
};