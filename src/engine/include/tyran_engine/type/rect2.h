#ifndef nimbus_rect_h
#define nimbus_rect_h

#include <tyranscript/tyran_types.h>
#include <tyran_engine/type/vector2.h>
#include <tyran_engine/type/size2.h>

typedef struct nimbus_rect {
	nimbus_vector2 vector;
	nimbus_size2 size;
} nimbus_rect;

#endif
