#pragma once
#include <memory>

#if defined(_WIN32)
#include <network/win32/Sockets.hpp>
#elif defined(__linux__)
#include <network/unix/Sockets.hpp>
#endif

namespace network
{
	using PacketUnit = uint8_t;
	bool startup();
	void shutdown();
	void close(SOCKET socket);
	bool nonBlocking(SOCKET socket);
	bool reuseAddress(SOCKET socket);
	namespace error {
		int latest();
	}
	namespace tcp {
		using HeaderType = uint16_t;
		static const unsigned int HeaderSize = sizeof(HeaderType);
	}
}