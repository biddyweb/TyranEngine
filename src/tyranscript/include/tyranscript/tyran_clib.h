#ifndef TYRAN_CLIB_H
#define TYRAN_CLIB_H

#if !defined TORNADO_OS_NACL
#include <memory.h>
#else
#include <stdio.h>
#endif

#include <stdlib.h>

#if defined TORNADO_OS_IOS || defined TORNADO_OS_NACL
#include <string.h>
#endif

struct tyran_memory;

char* tyran_str_dup(struct tyran_memory* pool, const char* str);

#define tyran_malloc malloc
#define tyran_realloc realloc
#define tyran_free free

#define tyran_memcpy_type(T, dest, source, N) memcpy(dest, source, (N) * sizeof(T))
#define tyran_memcpy_type_n(dest, source, N) memcpy(dest, source, (N) * sizeof(*dest))
#define tyran_memmove_type(T, dest, source, N) memmove(dest, source, (N) * sizeof(T))

#define tyran_memcpy_octets(dest, source, N) memcpy(dest, source, (N))

#define tyran_memcmp memcmp
#define tyran_memset_type(T, V) memset(T, V, sizeof(*T))
#define tyran_memset_type_n(T, V, N) memset(T, V, sizeof(*T) * (N))
#define tyran_mem_clear_type_n(T, N) tyran_memset_type_n(T, 0, N);
#define tyran_mem_clear(D, N) memset(D, 0, N)

#if defined WIN32
// #pragma warning( disable : 4100 )

#define tyran_sscanf sscanf_s
#define tyran_snprintf sprintf_s
#define tyran_strncpy(dest, dest_size, source, source_size) strncpy_s(dest, dest_size, source, source_size)
#define tyran_strcpy(dest, dest_size, source) strcpy_s(dest, dest_size, source)
#define tyran_fopen fopen_s
#define tyran_fread fread
#define tyran_fclose fclose
#define tyran_strncat strncat_s
#define tyran_strlen strlen

#else

#define tyran_sscanf sscanf
#define tyran_snprintf snprintf
#define tyran_strncpy(dest, dest_size, source, source_size) strncpy(dest, source, dest_size)
#define tyran_strcpy(dest, dest_size, source) strcpy(dest, source)
#define tyran_strncat strncat
#define tyran_strlen (int) strlen
#define tyran_fopen(F, N, M) *F = fopen(N, M)
#define tyran_fread fread
#define tyran_fclose fclose
#define tyran_strncat strncat

#endif

#define tyran_strcmp strcmp
#define tyran_strncmp strncmp
#define tyran_strstr strstr
#define tyran_fprintf fprintf
#define tyran_fputs fputs
#define tyran_fflush fflush
#define tyran_strdup(pool, str) tyran_str_dup(pool, str)
#define tyran_str_equal(a, b) (tyran_strcmp(a, b) == 0)
#define tyran_str_chr(a, b) strchr(a, b)

#define tyran_fmod fmod
#define tyran_pow pow

#endif
