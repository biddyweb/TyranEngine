#ifndef nimbus_track_info_h
#define nimbus_track_info_h

struct tyran_memory;

#include <tyranscript/tyran_symbol.h>
#include "bit_array.h"

typedef struct nimbus_track_info {
	tyran_symbol type_symbol;
	nimbus_bit_array bit_array;
} nimbus_track_info;

void nimbus_track_info_init(nimbus_track_info* self, struct tyran_memory* memory, tyran_symbol type_symbol);
int nimbus_track_info_get_free_index(nimbus_track_info* self);
void nimbus_track_info_delete_index(nimbus_track_info* self, int index);

#endif
