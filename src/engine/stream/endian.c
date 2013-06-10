#include "endian.h"

#import <netinet/in.h>

u16t nimbus_endian_u16t_to_network(u16t data)
{
    return htons(data);
}

u16t nimbus_endian_u16t_from_network(u16t data)
{
    return ntohs(data);
}

u32t nimbus_endian_u32t_to_network(u32t data)
{
    return htonl(data);
}

u32t nimbus_endian_u32t_from_network(u32t data)
{
    return ntohl(data);
}
