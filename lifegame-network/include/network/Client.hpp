#pragma once

#include <network/Sockets.hpp>
#include <network/event/Event.hpp>
#include <memory>
#include <string>

namespace network
{
	namespace tcp
	{
		/*
		* @brief A peer client.
		*/
		class Client
		{
		public:
			Client();
			Client(const Client& client) = delete;
			Client& operator=(const Client& client) = delete;
			Client(Client&& client) noexcept;
			Client& operator=(Client&& client) noexcept;
			~Client();

			/*
			* @return the client unique id.
			*/
			int64_t id() const;

			/*
			* @brief Link a socket to the client.
			*
			* @param the socket to link.
			*
			* @return whether (nor not) the socket could be linked.
			*/
			bool initialize(SOCKET&& socket);

			/*
			* @brief Connect the client to a remote peer.
			*
			* @param address the remote peer address.
			* @param port the remote peer port.
			*
			* @return whether (or not) the client has been connected.
			*/
			bool connect(const std::string& address, unsigned short port);

			/*
			* @brief Disconnect the client from the remote peer.
			*/
			void disconnect();

			/*
			* @brief Enqueue a packet to be send.
			*
			* @param packet the packet.
			* @param length the length of the packet.
			*
			* @return whether (or not) the packet can be sent.
			*/
			bool send(const PacketUnit* packet, unsigned int length);

			/*
			* @brief Process message sending & reception for the client.
			*
			* @return the received message. 'nullptr' is no message has been received.
			*/
			std::unique_ptr<event::Event> process();

		private:
			class ClientImpl;
			std::unique_ptr<ClientImpl> impl_;

		};
	}
}
