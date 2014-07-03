#include <tyran_core/platform/posix/log.h>
#include <stdio.h>

void nimbus_log_posix(enum tyran_log_type type, const char* string)
{
	tyran_fputs(string, stderr);
	tyran_fprintf(stderr, "\n");
	tyran_fflush(stderr);
}
