#ifndef TEST_APP_H
#define TEST_APP_H
#include "ls_api.gen.h"

LS_EXPORT ApplicationInterface *register_application();

const LSWindow *test_app_init(LSCore *core, Renderer *renderer, void *user_data);
void test_app_start(void *user_data);
void test_app_deinit(void *user_data);

void test_app_update(float64 delta_time, void *user_data);
bool test_app_should_stop(void *user_data);

#endif // TEST_APP_H