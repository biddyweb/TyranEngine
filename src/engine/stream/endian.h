#ifndef nimbus_engine_endian_h
#define nimbus_engine_endian_h

u16t nimbus_endian_u16t_to_network(u16t data);
u16t nimbus_endian_u16t_from_network(u16t data);
u32t nimbus_endian_u32t_to_network(u32t data);
u32t nimbus_endian_u32t_from_network(u32t data);

#endif