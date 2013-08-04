#ifndef nimbus_object_decorator_h
#define nimbus_object_decorator_h

#include <tyranscript/tyran_symbol.h>

struct tyran_memory;
struct tyran_object;

struct nimbus_object_info* nimbus_decorate_object(struct tyran_object* o, struct tyran_memory* memory);

#endif
