#ifndef nimbus_track_info_h
#define nimbus_track_info_h

struct tyran_memory;

#include <tyranscript/tyran_symbol.h>
#include "bit_array.h"

struct tyran_object;

typedef struct nimbus_track_info {
	tyran_symbol type_symbol;
	nimbus_bit_array bit_array;
	struct tyran_object** objects;
} nimbus_track_info;

void nimbus_track_info_init(nimbus_track_info* self, struct tyran_memory* memory, tyran_symbol type_symbol);
void nimbus_track_info_free(nimbus_track_info* self);
int nimbus_track_info_get_free_index(nimbus_track_info* self, struct tyran_object* o);
void nimbus_track_info_delete_index(nimbus_track_info* self, int index);

#endif
