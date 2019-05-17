#pragma once

#include <network/Sockets.hpp>
#include <network/event/Event.hpp>
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
			* @param the messages to receive, associated with each client.
			*/
			void process(std::map<uint64_t, std::unique_ptr<event::Event>>& events);

			/*
			* @brief Enqueue a packet to send to a client.
			*
			* @param clientid the client.
			* @param packet the packet.
			* @param length the length of the packet.
			*
			* @return whether (or not) the data can be sent.
			*/
			bool send(uint64_t clientid, const PacketUnit* packet, unsigned int length);

			/*
			* @brief Enqueue a packet to send to all clients.
			*
			* @param packet the packet.
			* @param length the length of the packet.
			*
			* @return whether (or not) the data can be sent.
			*/
			bool send(const PacketUnit* packet, unsigned int length);

		private:
			class ServerImpl;
			std::unique_ptr<ServerImpl> impl_;

		};
	}
}

