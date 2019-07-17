#include <iostream>
#include <string.h>

#include "InstructionSetLevel.h"
#include "NoiseRef.h"
#include "NoiseSSE.h"
#include "NoiseAVX2.h"
#include "NoiseCS.h"
#include "NoiseCL.h"

int iterations = 100;

int start = 16;
int end = 4096;


// Good read about the topic of optimization
// https://stackoverflow.com/questions/44773286/manual-simd-code-affordability
// Where compiler vectorization fails
// https://deplinenoise.files.wordpress.com/2015/03/gdc2015_afredriksson_simd.pdf
// Further discussions about when and why to use CPU SIMD
// https://softwareengineering.stackexchange.com/questions/266388/maintenance-cost-of-simd-programming-code-base

int main(int argc, char** argv)
{
	// Write console output to file
	FILE* logFile;
	freopen_s(&logFile, "output.txt", "w", stdout);

	std::cout << "\nReference implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseRef::CreateImage(i, i, iterations);

	std::cout << "\nSSE2 implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseSSE::CreateImage(i, i, iterations);

	std::cout << "\nAVX2 implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseAVX2::CreateImage(i, i, iterations);

	std::cout << "\nCS implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseCS::CreateImage(i, i, iterations);

	std::cout << "\nCL implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseCL::CreateImage(i, i, iterations);

	fclose(logFile);
	_fcloseall();

	// Tests
	//std::cout << "SSE tests...\n";
	//NoiseSSE::RunTests();

	//std::cout << "AVX2 tests...\n";
	//NoiseAVX2::RunTests();

	return EXIT_SUCCESS;
}