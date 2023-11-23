// Small wrapper used when dlink is enabled for wasm builds.
// Main modules must export all symbols which can add to size a lot.
// Further more, side modules that link against libc doon't include it.
// So this acts as the main module and links against libc.

extern int web_main(int argc, char *argv[]);

int main(int argc, char *argv[]) {
	return web_main(argc, argv);
}