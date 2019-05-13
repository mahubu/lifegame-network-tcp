#pragma once

#include <sys/socket.h>
#include <netinet/in.h> // sockaddr_in, IPPROTO_TCP
#include <arpa/inet.h> // hton*, ntoh*, inet_addr
#include <unistd.h>  // close
#include <cerrno> // errno
#include <poll> // poll
#define SOCKET int
#define INVALID_SOCKET ((int)-1)
#define SOCKET_ERROR (int(-1))

namespace network
{
	namespace error
	{
		enum
		{
			AGAIN = EAGAIN,
			WOULDBLOCK = EWOULDBLOCK,
			INPROGRESS = EINPROGRESS,
			INTR = EINTR,
		};

	}
}