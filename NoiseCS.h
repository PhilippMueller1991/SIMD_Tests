#pragma once

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class NoiseCS
{
public:
	static void CreateImage(int width, int height, int iterations);

private:
	bool static ShaderErrorInfo(GLuint shader);
};