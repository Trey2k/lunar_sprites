#include "core/time.h"

#include <sys/time.h>

int64 ls_get_time() {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64)tv.tv_sec * 1000000 + tv.tv_usec;
}