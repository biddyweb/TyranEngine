#ifndef tyran_log_h
#define tyran_log_h

#include <tyranscript/tyran_clib.h>

#include <stdio.h>
#include <stdlib.h>

enum tyran_log_type { TYRAN_LOG_TYPE_LOG };

typedef struct tyran_log {
	void (*log)(enum tyran_log_type type, const char* string);
} tyran_log;

extern tyran_log g_log;

#if defined TYRAN_CONFIGURATION_DEBUG

#define TYRAN_LOG(...)                                                                                                           \
	{                                                                                                                            \
		char _temp_str[1024];                                                                                                    \
		sprintf(_temp_str, __VA_ARGS__);                                                                                         \
		g_log.log(TYRAN_LOG_TYPE_LOG, _temp_str);                                                                                \
	}
#define TYRAN_LOG_NO_LF(...)                                                                                                     \
	{                                                                                                                            \
		TYRAN_LOG(__VA_ARGS__);                                                                                                  \
	}
#define TYRAN_SOFT_ERROR(...) TYRAN_LOG(__VA_ARGS__);
#define TYRAN_BREAK abort()
#define TYRAN_ERROR(...)                                                                                                         \
	TYRAN_LOG(__VA_ARGS__);                                                                                                      \
	TYRAN_BREAK;
#define TYRAN_ASSERT(expression, ...)                                                                                            \
	if (!(expression)) {                                                                                                         \
		TYRAN_LOG(__VA_ARGS__);                                                                                                  \
		TYRAN_BREAK;                                                                                                             \
	}

#else

#define TYRAN_LOG(...)
#define TYRAN_LOG_NO_LF(...)
#define TYRAN_SOFT_ERROR(...)
#define TYRAN_BREAK abort()
#define TYRAN_ERROR(...)
#define TYRAN_ASSERT(expression, ...)

#endif

#define TYRAN_OUTPUT(...)                                                                                                        \
	{                                                                                                                            \
		tyran_fprintf(stdout, __VA_ARGS__);                                                                                      \
		tyran_fprintf(stdout, "\n");                                                                                             \
		tyran_fflush(stdout);                                                                                                    \
	}
#define TYRAN_OUTPUT_NO_LF(...)                                                                                                  \
	{                                                                                                                            \
		tyran_fprintf(stdout, __VA_ARGS__);                                                                                      \
		tyran_fflush(stdout);                                                                                                    \
	}

#endif
