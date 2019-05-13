#pragma once

#include <network/message/Message.hpp>
#include <map>
#include <memory>

namespace network
{
	namespace tcp
	{
		/*
		* @brief A server, handling with remote client.
		*/
		class Server
		{
		public:
			Server();
			Server(const Server& server) = delete;
			Server& operator=(const Server& server) = delete;
			Server(Server&& server) noexcept;
			Server& operator=(Server&& server) noexcept;
			~Server();

			/*
			* @brief Start up the server on given port.
			*
			* @param port the port listening to client connections.
			*
			* @return whether (or not) the server could be started.
			*/
			bool startup(unsigned short port);

			/*
			* @brief Shutdown the server.
			*/
			void shutdown();

			/*
			* @brief Process message sending & reception for the server.
			*
			* @return the received messages, associated with each client. 'nullptr' is no message has been received.
			*/
			std::map<uint64_t, std::unique_ptr<message::Message>> process();

			/*
			* @brief Enqueue data to send to a client.
			*
			* @param clientid the client to send to.
			* @param data the data send to send.
			* @param length the length of data to send.
			*
			* @return whether (or not) the data can be sent.
			*/
			bool send(uint64_t clientid, const char* data, unsigned int length);

			/*
			* @brief Enqueue data to send to all clients.
			*
			* @param data the data send to send.
			* @param length the length of data to send.
			*
			* @return whether (or not) the data can be sent.
			*/
			bool send(const char* data, unsigned int length);

		private:
			class ServerImpl;
			std::unique_ptr<ServerImpl> impl_;

		};
	}
}

