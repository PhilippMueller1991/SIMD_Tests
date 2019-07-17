#include "NoiseCL.h"

#include <iostream>
#include <chrono>
#include "CL/cl.hpp"
#include "Image.h"

//http://simpleopencl.blogspot.com/2013/06/tutorial-simple-start-with-opencl-and-c.html

void NoiseCL::CreateImage(int width, int height, int iterations)
{
	static bool first_call = true;

	// Get all platforms (drivers)
	std::vector<cl::Platform> all_platforms;
	cl::Platform::get(&all_platforms);

	if (all_platforms.size() == 0) 
	{
		std::cerr << "No platforms found. Check OpenCL installation!\n";
		return;
	}

	cl::Platform default_platform = all_platforms[0];
	if (first_call)
		std::cout << "Using platform: " << default_platform.getInfo<CL_PLATFORM_NAME>() << "\n";

	// Get default device of the default platform
	std::vector<cl::Device> all_devices;
	default_platform.getDevices(CL_DEVICE_TYPE_ALL, &all_devices);
	if (all_devices.size() == 0) 
	{
		std::cerr << "No devices found. Check OpenCL installation!\n";
		return;
	}
	cl::Device default_device = all_devices[0];
	if(first_call)
		std::cout << "Using device: " << default_device.getInfo<CL_DEVICE_NAME>() << "\n";

	// Create device context
	cl::Context context({ default_device });

	// Similar to shader source
	cl::Program::Sources sources;

	// Kernel calculates for each element C=A+B
	std::string kernel_code =
		"	float2 hash2_2(float2 x)																			\n"
		"	{																									\n"
		"		const float2 k = (float2) (0.3183099f, 0.3678794f);												\n"
		"		x = x * k + k.yx;																				\n"
		"		float2 floorRes;																				\n"
		"		return -1.0f + 2.0f * fract(16.0f * k * fract(x.x * x.y * (x.x + x.y), &floorRes), &floorRes);	\n"
		"	}																									\n"
		"																										\n"
		"	float perlin_noise(float2 p)																		\n"
		"	{																									\n"
		"		float2 i;																						\n"
		"		float2 f = fract(p, &i);																		\n"
		"																										\n"
		"		float2 u = f * f * (3.0f - 2.0f * f);															\n"
		"																										\n"
		"		return mix( mix( dot( hash2_2(i + (float2)(0.0, 0.0)), f - (float2)(0.0, 0.0)),					\n"
		"						 dot( hash2_2(i + (float2)(1.0, 0.0)), f - (float2)(1.0, 0.0)), u.x),			\n"
		"					mix( dot( hash2_2(i + (float2)(0.0, 1.0)), f - (float2)(0.0, 1.0)),					\n"
		"						 dot( hash2_2(i + (float2)(1.0, 1.0)), f - (float2)(1.0, 1.0)), u.x), u.y);		\n"
		"	}																									\n"
		"																										\n"
		"   void kernel opencl_main(global float* data)															\n"
		"	{																									\n"
		"		float2 uv = { get_global_id(0), get_global_id(1) };												\n"
		"		uv /= (float2)( get_global_size(0), get_global_size(1) );										\n"
		"		uv *= 8.0f;																						\n"
		"																										\n"
		"		uint idx = get_global_id(0) + get_global_id(1) * get_global_size(0);							\n"
		"       data[idx] = 0.5f + 0.5f * perlin_noise(uv);														\n"
		"   }																									\n";

	//"		const float2 k = float2(0.3183099f, 0.3678794f);							"
	//"		x = x * k + k.yx;															"
	//"		return -1.0f + 2.0f * fract(16.0f * k * fract(x.x * x.y * (x.x + x.y)));	"
	sources.push_back({ kernel_code.c_str(), kernel_code.length() });

	// Create program, similar to shader program
	cl::Program program(context, sources);
	if (program.build({ default_device }) != CL_SUCCESS) 
	{
		std::cout << " Error building: \n" << program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(default_device) << "\n";
		return;
	}

	// Create buffers on the device
	const int element_count = width * height;
	const int buffer_size = width * height * sizeof(float);

	cl::Buffer buffer_data;
	cl::CommandQueue queue;

	std::chrono::nanoseconds minTime = std::chrono::nanoseconds::max();
	std::chrono::nanoseconds averageTime = std::chrono::nanoseconds(0);
	for (int i = 0; i < iterations; i++)
	{
		auto time = std::chrono::high_resolution_clock::now();

		buffer_data = cl::Buffer(context, CL_MEM_READ_WRITE, buffer_size);

		// Create queue to which we will push commands for the device (Similar to DirectX12 or Vulkan)
		queue = cl::CommandQueue(context, default_device);

		// Get function by name
		cl::Kernel opencl_main(program, "opencl_main");

		// Setup function parameter
		opencl_main.setArg(0, buffer_data);

		// Dispatch with workgroup parameter 
		queue.enqueueNDRangeKernel(opencl_main, cl::NullRange, cl::NDRange(width, height), cl::NDRange(32,32));

		auto timeDelta = std::chrono::high_resolution_clock::now() - time;
		if (timeDelta < minTime)
			minTime = timeDelta;

		averageTime += timeDelta;
	}
	std::cout << "min: " << minTime.count() << " ns\t";
	std::cout << "avg: " << averageTime.count() / iterations << "ns\n";

	// Results
	float* data = new float[element_count] { 0 };

	// Read result C from the device to array C
	queue.enqueueReadBuffer(buffer_data, CL_TRUE, 0, buffer_size, data);

	// Write data
	Image::SaveBitmap("NoiseCL", width, height, data);

	// Cleanup
	delete[] data;
	if (cl::flush() != CL_SUCCESS || cl::finish() != CL_SUCCESS)
	{
		std::cout << " Error by OpenCL shutdown.\n";
		return;
	}

	first_call = false;

	return;
}
