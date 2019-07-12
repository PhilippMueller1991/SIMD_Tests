#pragma once

#include <string>

class Image
{
private:
	static const int dpi = 72;

public:
	static void SaveBitmap(std::string filename, const int width, const int height, float* data);
};

#pragma once
