#ifndef NATIVE_API_H
#define NATIVE_API_H

#include "core/api.h"
#include "core/types/typedefs.h"

typedef enum {
	API_CLIENT_INITIALIZATION_LEVEL_CORE = 0,
	API_CLIENT_INITIALIZATION_LEVEL_RENDER,
	API_CLIENT_INITIALIZATION_LEVEL_MAIN,
} APIClientInitializationLevel;

typedef void (*APIClientInitializationFunction)(APIClientInitializationLevel p_level);

typedef struct {
	APIClientInitializationFunction initialize;
	APIClientInitializationFunction uninitialize;
} APIClientInterface;

typedef APIClientInterface (*APIClientRegisterFunction)();

extern const unsigned char *LS_API_HEADER;

void native_api_init();

void native_api_init_clients(APIClientInitializationLevel p_level);
void native_api_uninit_clients(APIClientInitializationLevel p_level);

void native_api_start();

void native_api_deinit();

#endif // NATIVE_API_H