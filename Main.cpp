#include <intrin.h>
#include <stdio.h>
#include <string.h>

#include "InstructionSetLevel.h"
#include "MakingSomeNoiseRef.h"
#include "MakingSomeNoiseSSE.h"
#include "MakingSomeNoiseCS.h"

int main(int argc, char** argv)
{
	constexpr int width = 512;
	constexpr int height = 512;

	Ref::MakingSomeNoiseRef::CreateImage(width, height);

	SSE::MakingSomeNoiseSSE::CreateImage(width, height);

	CS::MakingSomeNoiseCS::CreateImage(width, height);

	return EXIT_SUCCESS;
}