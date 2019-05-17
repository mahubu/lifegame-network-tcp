#pragma once

#include <network/Sockets.hpp>
#include <network/event/Connection.hpp>
#include <network/event/Event.hpp>
#include <memory>
#include <string>

namespace network
{
	namespace handler
	{
		/*
		* @brief Handle connections between peers.
		*/
		class ConnectionHandler
		{
		public:
			ConnectionHandler() = default;
			/*
			* @brief Make a connection request  to a remote peer on a socket.
			*
			* @param socket the socket to connect.
			* @param address the remote peer address.
			* @param port the remote peer port.
			*
			* @return whether (or not) the connection request has succeeded.
			*/
			bool connect(SOCKET socket, const std::string& address, unsigned short port);

			/*
			* @brief Verify whether (or not) the connection is ready.
			*
			* @return a message with the connection state. If successfull, the connection has succeeded.
			*/
			std::unique_ptr<event::Connection> ready();

		private:
			std::string address_;
			unsigned short port_ = 0;
			pollfd descriptor_{ 0 };

		};
	}
}