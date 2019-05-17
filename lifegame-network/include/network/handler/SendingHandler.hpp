#pragma once

#include <network/Sockets.hpp>
#include <network/event/Event.hpp>
#include <list>
#include <vector>

namespace network
{
	namespace handler
	{
		/*
		* @brief Handle message send for a peer.
		*/
		class SendingHandler
		{
			/*
			* @brief Define the sending state :
			* * 'Idle' when no message are sent.
			* * 'Header' when sending message header.
			* * 'Body' when sending message body.
			*/
			enum class State {
				Idle,
				Header,
				Body,
			};

		public:
			/*
			* @brief Link the handler to a socket.
			*
			* @param socket the socket to link.
			*/
			void initialize(SOCKET socket);

			/*
			* @brief Enqueue a packet to be send.
			*
			* @param packet the packet.
			* @param length the length of the packet.
			*
			* @return whether (or not) the data can be sent.
			*/
			bool enqueue(const PacketUnit* packet, unsigned int length);

			/*
			* @brief Send the enqueued data.
			*/
			void send();

			/*
			* @return the size of the data queue to be sent.
			*/
			size_t queueSize() const;

		private:
			// TODO remove these private methods from the header ?
			bool sendBuffer();
			void prepareHeaderSending();
			void prepareBodySending();

			std::list<std::vector<PacketUnit>> queueingBuffers_;
			std::vector<PacketUnit> sendingBuffer_;
			SOCKET socket_ = INVALID_SOCKET;
			State state_ = State::Idle;

		};
	}
}