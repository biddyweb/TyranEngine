#include "endian.h"

#if defined TORNADO_OS_MAC_OS_X || defined TORNADO_OS_IOS || defined TORNADO_OS_NACL
#include <netinet/in.h>
#elif defined TORNADO_OS_LINUX
#include <netinet/in.h>
#elif defined TORNADO_OS_WINDOWS
#include <Winsock2.h>
#else
#error "No platform"
#endif

u16t nimbus_endian_u16_to_network(u16t data)
{
	return htons(data);
}

u16t nimbus_endian_u16_from_network(u16t data)
{
	return ntohs(data);
}

u32t nimbus_endian_u32_to_network(u32t data)
{
	return htonl(data);
}

u32t nimbus_endian_u32_from_network(u32t data)
{
	return ntohl(data);
}
