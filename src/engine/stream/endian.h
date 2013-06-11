#ifndef nimbus_engine_endian_h
#define nimbus_engine_endian_h

#include <tyranscript/tyran_types.h>

u16t nimbus_endian_u16_to_network(u16t data);
u16t nimbus_endian_u16_from_network(u16t data);
u32t nimbus_endian_u32_to_network(u32t data);
u32t nimbus_endian_u32_from_network(u32t data);

#endif