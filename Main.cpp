#include <iostream>
#include <string.h>

#include "InstructionSetLevel.h"
#include "NoiseRef.h"
#include "NoiseSSE.h"
#include "NoiseCS.h"

int start = 16;
int end = 4096;

int main(int argc, char** argv)
{
	// Write console output to file
	FILE* logFile;
	freopen_s(&logFile, "output.txt", "w", stdout);

	std::cout << "Start resolution " << start << " to " << end << " res\n";

	std::cout << "\nReference implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseRef::CreateImage(i, i);

	std::cout << "\nSSE implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseSSE::CreateImage(i, i);

	std::cout << "\nCS implementation:\n";
	for (int i = start; i <= end; i *= 2)
		NoiseCS::CreateImage(i, i);

	fclose(logFile);

	return EXIT_SUCCESS;
}