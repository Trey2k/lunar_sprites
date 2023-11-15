#include "main/lunar_sprites.h"

int32 main(int32 argc, char *argv[]) {
	ls_main_init(argc, argv);

	while (!ls_should_quit()) {
		ls_main_loop();
	}

	ls_main_deinit();
	return 0;
}
