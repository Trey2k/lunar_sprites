#include "main/lunar_sprites.h"

int32 main(int32 argc, char *argv[]) {
	ls_main_init(argc, argv);

	while (!ls_should_stop()) {
		ls_main_loop();
	}

	return ls_main_deinit();
}
