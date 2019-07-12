#include <intrin.h>
#include <stdio.h>
#include <string.h>

#include "InstructionSetLevel.h"
#include "NoiseRef.h"
#include "NoiseSSE.h"
#include "NoiseCS.h"

int main(int argc, char** argv)
{
	constexpr int width = 512;
	constexpr int height = 512;

	NoiseRef::CreateImage(width, height);

	NoiseSSE::CreateImage(width, height);

	NoiseCS::CreateImage(width, height);

	return EXIT_SUCCESS;
}