#include <network/handler/ConnectionHandler.hpp>
#include <assert.h>

namespace network
{
	namespace handler
	{
		bool ConnectionHandler::connect(SOCKET socket, const std::string& address, unsigned short port)
		{
			assert(socket != INVALID_SOCKET);
			address_ = address;
			port_ = port;
			descriptor_.fd = socket;
			descriptor_.events = POLLOUT;
			sockaddr_in server;
			inet_pton(AF_INET, address_.c_str(), &server.sin_addr.s_addr);
			server.sin_family = AF_INET;
			server.sin_port = htons(port_);
			if (::connect(socket, (const sockaddr*)& server, sizeof(server)) != 0)
			{
				int error = error::latest();
				if (error != static_cast<int>(error::INPROGRESS) && error != static_cast<int>(error::WOULDBLOCK))
				{
					return false;
				}
			}
			return true;
		}

		std::unique_ptr<event::Connection> ConnectionHandler::ready()
		{
			int result = ::poll(&descriptor_, 1, 0);
			if (result < 0)
			{
				return std::make_unique<event::Connection>(event::Connection::State::Failed);
			}
			else if (result > 0)
			{
				short revents = descriptor_.revents;
				if (revents & POLLOUT)
				{
					return std::make_unique<event::Connection>(event::Connection::State::Successfull);
				}
				else if (revents & (POLLHUP | POLLNVAL))
				{
					return std::make_unique<event::Connection>(event::Connection::State::Failed);
				}
				else if (revents & POLLERR)
				{
					return std::make_unique<event::Connection>(event::Connection::State::Failed);
				}
				return nullptr;
			}
			return nullptr;
		}

	}

}
