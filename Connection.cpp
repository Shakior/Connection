#include "Connection.h"

#ifdef __unix__ 

Connection::Connection(const char* const srv_host, const char* const srv_port)
	:srv_host{ srv_host },
	srv_port{ srv_port }
{
	addrinfo hostInfo, *hostInfoList;
	std::memset(&hostInfo, 0, sizeof(hostInfo));
	hostInfo.ai_family = AF_INET;//IPv4
	hostInfo.ai_socktype = SOCK_STREAM;
	if (getaddrinfo(srv_host, srv_port, &hostInfo, &hostInfoList) != 0) {//0 norm
		printf("getaddrinfo error\n");
		return;
	}

	//sock = socket(hostInfoList->ai_family, hostInfoList->ai_socktype, 0);
	if ((sock = socket(hostInfoList->ai_family, hostInfoList->ai_socktype, 0)) == -1) {
		printf("socket error\n");
		return;
	}

	if (connect(sock, hostInfoList->ai_addr, hostInfoList->ai_addrlen) != -1) {
		//connected = true;
	}
}

Connection::~Connection() {
	if (sock != -1) {
		close(sock);
	}
}

int Connection::send_data(const void* const msg, const size_t msg_size) {
	return send(sock, msg, msg_size, 0);
}

int Connection::get_data(void* const buffer, const size_t buf_size) {
	return recv(sock, buffer, buf_size, MSG_DONTWAIT);
}

int Connection::close_connection() {
	if (sock != -1) {
		return close(sock);
	}
	return 0;
}

#elif defined(_WIN32) || defined(WIN32)

Connection::Connection(const char* const srv_host, const char* const srv_port)
	:srv_host{ srv_host },
	srv_port{ srv_port }
{
	WSADATA ws;
	//по идее должна быть вызвана только 1 раз
	if (FAILED(WSAStartup(MAKEWORD(1, 1), &ws))) {
		printf("WSAStartup ERROR\n");
		return;
	}

	if (INVALID_SOCKET == (sock = socket(AF_INET, SOCK_STREAM, 0))) {
		printf("INVALID_SOCKET ERROR\n");
		return;
	}
	
	sockaddr_in sock_addr;
	ZeroMemory(&sock_addr, sizeof(sock_addr));
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr.S_un.S_addr = inet_addr(srv_host);
	sock_addr.sin_port = htons(atoi(srv_port));

	if (SOCKET_ERROR == (connect(sock, (sockaddr *)&sock_addr, sizeof(sock_addr)))) {
		printf("connection error\n");
		return;
	}

	//чтобы сокет не блокировался при чтении
	BOOL l = TRUE;
	if (SOCKET_ERROR == ioctlsocket(sock, FIONBIO, (unsigned long*)&l))
	{
		//int res = WSAGetLastError();
		printf("ioctlsocket error(nonblock)\n");
		return;
	}
}

Connection::~Connection() {
	if (sock != -1) {
		closesocket(sock);
	}
}

int Connection::send_data(const void* const msg, const size_t msg_size) {
	return send(sock, (const char* const)msg, msg_size, 0);
}

int Connection::get_data(void* const buffer, const size_t buf_size) {
	return recv(sock, (char* const)buffer, buf_size, 0);
}

int Connection::close_connection() {
	if (sock != -1) {
		return closesocket(sock);
	}
	return 0;
}

#endif