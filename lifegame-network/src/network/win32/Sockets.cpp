#if defined(_WIN32)
#include <network/win32/Sockets.hpp>
namespace network
{
	bool startup()
	{
		WSAData wsaData;
		return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
	}
	void shutdown()
	{
		WSACleanup();
	}
	void close(SOCKET socket)
	{
		::closesocket(socket);
	}
	bool nonBlocking(SOCKET socket)
	{
		u_long mode = 1;
		return ::ioctlsocket(socket, FIONBIO, &mode) == 0;
	}
	bool reuseAddress(SOCKET socket)
	{
		int optval = 1;
		return setsockopt(socket, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<const char*>(&optval), sizeof(optval)) == 0;
	}
	namespace error {
		int latest()
		{
			return WSAGetLastError();
		}
	}
}
#endif