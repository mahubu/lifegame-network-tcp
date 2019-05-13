#pragma once
#include <memory>

#if defined(_WIN32)
#include <network/win32/Sockets.hpp>
#elif defined(__linux__)
#include <network/unix/Sockets.hpp>
#endif

namespace network
{
	bool startup();
	void shutdown();
	void close(SOCKET socket);
	bool nonBlocking(SOCKET socket);
	bool reuseAddress(SOCKET socket);
	namespace error {
		int latest();
	}
	namespace tcp {
		typedef uint16_t HeaderType;
		static const unsigned int HeaderSize = sizeof(HeaderType);
	}
}