#include "NoiseCS.h"

#include <iostream>
#include <math.h>
#include <chrono>
#include "Settings.h"
#include "Image.h"

void NoiseCS::CreateImage(int width, int height)
{
	const char* shader_source = "																							\
			#version 430																									\n\
			#extension GL_ARB_compute_shader: enable																		\n\
			#extension GL_ARB_shader_storage_buffer_object: enable															\n\
																															\n\
			layout(local_size_x = 32, local_size_y = 32, local_size_z = 1)	in;												\n\
			layout(binding = 4) buffer data																					\n\
			{																												\n\
				float Values[];																								\n\
			};																												\n\
																															\n\
			// Hash function from iq's shader																				\n\
			// https://www.shadertoy.com/view/XdXGW8																		\n\
			vec2 hash2_2(vec2 x)																							\n\
			{																												\n\
				const vec2 k = vec2(0.3183099, 0.3678794);																	\n\
				x = x * k + k.yx;																							\n\
				return -1.0 + 2.0 * fract(16.0 * k * fract(x.x * x.y * (x.x + x.y)));										\n\
			}																												\n\
																															\n\
			float perlinNoise( in vec2 p )																					\n\
			{																												\n\
				vec2 i = floor(p); // Cube indices																			\n\
				vec2 f = fract(p); // Inner coordinate																		\n\
																															\n\
				vec2 u = f * f * (3.0 - 2.0 * f); // Cubic fade function													\n\
																															\n\
				return mix( mix( dot(hash2_2(i + vec2(0.0, 0.0)), f - vec2(0.0, 0.0)),										\n\
								 dot(hash2_2(i + vec2(1.0, 0.0)), f - vec2(1.0, 0.0)), u.x),								\n\
							mix( dot(hash2_2(i + vec2(0.0, 1.0)), f - vec2(0.0, 1.0)),										\n\
								 dot(hash2_2(i + vec2(1.0, 1.0)), f - vec2(1.0, 1.0)), u.x), u.y);							\n\
			}																												\n\
																															\n\
			void main()																										\n\
			{																												\n\
				vec2 uv = vec2(float(gl_GlobalInvocationID.x), float(gl_GlobalInvocationID.y));								\n\
				uv /= vec2(float(gl_NumWorkGroups.x * gl_WorkGroupSize.x),float(gl_NumWorkGroups.y * gl_WorkGroupSize.y));	\n\
				uv *= 8.0;																									\n\
																															\n\
				uint idx = gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_NumWorkGroups.x * gl_WorkGroupSize.x;		\n\
				Values[idx] = 0.5 + 0.5 * perlinNoise(uv);																	\n\
			}																												\n\
	";

	// OpenGL init
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Needed for glad initialization
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "Failed to initialize GLAD" << std::endl;
		return;
	}

	// Create shader
	GLuint shader = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(shader, 1, &shader_source, nullptr);
	glCompileShader(shader);
	ShaderErrorInfo(shader);

	GLuint program = glCreateProgram();
	glAttachShader(program, shader);
	glLinkProgram(program);
	glUseProgram(program);

	glDeleteShader(shader);

	int work_grp_cnt[3];
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &work_grp_cnt[0]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &work_grp_cnt[1]);
	glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &work_grp_cnt[2]);

	printf("max global (total) work group size x:%i y:%i z:%i\n", work_grp_cnt[0], work_grp_cnt[1], work_grp_cnt[2]);

	float* data;

	std::chrono::nanoseconds bestTime = std::chrono::nanoseconds::max();
	for (int i = 0; i < Settings::iterations; i++)
	{
		auto time = std::chrono::high_resolution_clock::now();

		// Generate buffer
		GLuint buffer;
		glGenBuffers(1, &buffer);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, buffer);
		glBufferData(GL_SHADER_STORAGE_BUFFER, width * height * sizeof(float), NULL, GL_STATIC_DRAW);

		// Dispatch
		glDispatchCompute((width + 31) / 32, (height + 31) / 32, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		// Read
		data = static_cast<float*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY));

		auto timeDelta = std::chrono::high_resolution_clock::now() - time;
		if (timeDelta < bestTime)
			bestTime = timeDelta;

		glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	}
	printf("Compute shader implementation executed in: %.4f ms\n\n", std::chrono::duration_cast<std::chrono::microseconds>(bestTime).count() / 1000.0);

	// Write to image
	Image::SaveBitmap("NoiseCS", width, height, data);

	glDeleteProgram(program);

	glfwTerminate();
}

bool NoiseCS::ShaderErrorInfo(GLuint shader)
{
	int success;
	char infoLog[512];
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	return success;
}