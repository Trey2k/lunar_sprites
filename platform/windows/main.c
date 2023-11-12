#include "main/lunar_sprites.h"

// Reduce the size of the windows header.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// Enable high performance GPU on windows systems with both integrated and discrete GPUs.
// https://developer.nvidia.com/optimus
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

int main(int argc, char *argv[]) {
	lunar_spriates_main(argc, argv);
	return 0;
}