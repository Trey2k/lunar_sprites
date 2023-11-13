#include "main/lunar_sprites.h"

// Reduce the size of the windows header.
#define WIN32_LEAN_AND_MEAN

#include <windows.h>

// Enable high performance NVidia GPU on windows systems with both integrated and discrete GPUs.
// https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// Enable high performance AMD GPU on windows systems with both integrated and discrete GPUs.
// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;

int main(int argc, char *argv[]) {
	lunar_spriates_main(argc, argv);
	return 0;
}