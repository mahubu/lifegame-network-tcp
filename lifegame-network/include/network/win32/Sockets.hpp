#pragma once

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using nfds_t = unsigned long;
using socklen_t = int;
inline int poll(pollfd fdarray[], nfds_t nfds, int timeout) { return WSAPoll(fdarray, nfds, timeout); };

namespace network
{
	namespace error
	{
		enum
		{
			WOULDBLOCK = WSAEWOULDBLOCK,
			INPROGRESS = WSAEINPROGRESS,
			TRYAGAIN = WSATRY_AGAIN,
			INTERRUPT = WSAEINTR,
		};
	}
}