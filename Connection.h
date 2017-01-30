#pragma once

#ifdef __unix__ 

#include <netdb.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <stdio.h>
#include <unistd.h>

#elif defined(_WIN32) || defined(WIN32)

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <stdio.h>

#endif

class Connection {
public:
	Connection(const char* const srv_host, const char* const srv_port);
	virtual ~Connection();
	int send_data(const void* const msg, const size_t msg_size);
	int get_data(void* const buffer, const size_t size);
	int close_connection();
private:
	int sock = -1;
	const char* const srv_host;
	const char* const srv_port;
};

