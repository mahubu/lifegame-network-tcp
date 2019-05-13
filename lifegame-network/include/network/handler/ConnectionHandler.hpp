#pragma once

#include <network/Sockets.hpp>
#include <network/message/Connection.hpp>
#include <network/message/Message.hpp>
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
			* @brief Verify the connection request.
			*
			* @return a message with the connection state. If successfull, the connection has succeeded.
			*/
			std::unique_ptr<message::Connection> fetch();

		private:
			std::string address_;
			unsigned short port_ = 0;
			pollfd descriptor_{ 0 };

		};
	}
}