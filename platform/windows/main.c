#include "main/lunar_sprites.h"

#include <windows.h>

#if defined(OPENGL_ENABLED)

// Enable high performance NVidia GPU on windows systems with both integrated and discrete GPUs.
// https://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;

// Enable high performance AMD GPU on windows systems with both integrated and discrete GPUs.
// https://gpuopen.com/learn/amdpowerxpressrequesthighperformance/
__declspec(dllexport) DWORD AmdPowerXpressRequestHighPerformance = 0x00000001;

#endif // OPENGL_ENABLED

int main(int argc, char *argv[]) {
	ls_main_init(argc, argv);

	while (!ls_should_stop()) {
		ls_main_loop();
	}

	return ls_main_deinit();
}