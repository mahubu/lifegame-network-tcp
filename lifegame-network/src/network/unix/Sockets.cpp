#if defined(__linux__)
#include <network/unix/Sockets.hpp>
namespace network
{
	bool startup()
	{
		return true;
	}
	void shutdown()
	{
	}
	void close(SOCKET socket)
	{
		::close(socket);
	}
	bool nonBlocking(SOCKET socket)
	{
		return ::fcntl(socket, F_SETFL, O_NONBLOCK) >= 0;
	}
	bool reuseAddress(SOCKET socket)
	{
		int optval = 1;
		return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == 0;
	}
	namespace error {
		int latest()
		{
			return errno;
		}
	}
}
#endif