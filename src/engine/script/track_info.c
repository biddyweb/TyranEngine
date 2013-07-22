#include "track_info.h"
#include "bit_array.h"

void nimbus_track_info_init(nimbus_track_info* self, struct tyran_memory* memory, tyran_symbol type_symbol)
{
	self->type_symbol = type_symbol;
	nimbus_bit_array_init(&self->bit_array, memory, 1024);
}

int nimbus_track_info_get_free_index(nimbus_track_info* self)
{
	return nimbus_bit_array_reserve_free_index(&self->bit_array);
}
