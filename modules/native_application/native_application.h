#ifndef NATIVE_APPLICATION_H
#define NATIVE_APPLICATION_H

#include "core/core.h"

#include "main/application.h"

typedef ApplicationInterface *(*NativeApplicationRegisterFunction)();

extern const unsigned char *LS_API_HEADER;

void native_application_init(LSCore *core);
void native_application_start();
void native_application_deinit();

#endif // NATIVE_APPLICATION_H