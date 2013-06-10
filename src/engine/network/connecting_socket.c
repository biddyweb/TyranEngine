#include "connecting_socket.h"

#if defined TORNADO_OS_WINDOWS
    #include <Winsock2.h>
	#include <Ws2tcpip.h>
#else
	#include <sys/socket.h>
	#include <netdb.h>
	#include <netinet/tcp.h> 	
	#include <unistd.h>
#endif

#include <sys/types.h>
#include <tyranscript/tyran_clib.h>

int nimbus_engine_connecting_socket_connect(nimbus_engine_connecting_socket* self, const char* hostname, int port)
{
	self->socket_handle = socket(AF_INET, SOCK_STREAM, 0);

	int flag = 1;
	/* int ret = */ setsockopt(self->socket_handle, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(flag) );

	struct sockaddr_in sin;

#if 0 // defined TORNADO_OS_WINDOWS
	ADDRINFOA hints;
	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	PADDRINFOA found_address_info = 0;
	getaddrinfo(hostname.c_str(), 0, 0, &found_address_info);
	tornado_memcpy(&sin.sin_addr.s_addr, found_address_info->ai_addr, found_address_info->ai_addrlen);
#else
	struct hostent *host = gethostbyname(hostname);
	if (!host) {
		return -1;
	}
	tyran_memcpy_octets(&sin.sin_addr.s_addr, host->h_addr, host->h_length);
#endif
	
	sin.sin_family = AF_INET;
	sin.sin_port = htons(port);

	int result = connect(self->socket_handle, (const struct sockaddr*)&sin, sizeof(sin));
	if (result < 0) {
		return -2;
	}

	return 0;
}

nimbus_engine_connecting_socket* nimbus_engine_connecting_socket_new(tyran_memory* memory, const char* hostname, int port)
{
	nimbus_engine_connecting_socket* self = TYRAN_MEMORY_CALLOC_TYPE(memory, nimbus_engine_connecting_socket);
	
	nimbus_engine_connecting_socket_connect(self, hostname, port);
	
	return self;
}

void nimbus_engine_connecting_socket_write(nimbus_engine_connecting_socket* self, const u8t* data, int length)
{
    send(self->socket_handle, data, length, 0);
}

int nimbus_engine_connecting_socket_read(nimbus_engine_connecting_socket* self, u8t* data, int max_length)
{
    int octets_read = recv(self->socket_handle, data, max_length, 0);
    
    return octets_read;
}

void nimbus_engine_connecting_socket_close(nimbus_engine_connecting_socket* self)
{
#if defined TORNADO_OS_WINDOWS
	closesocket(self->socket_handle);
#else
	close(self->socket_handle);
#endif
	self->socket_handle = 0;
}
