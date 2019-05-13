#pragma once

#include <network/Sockets.hpp>
#include <network/message/Message.hpp>
#include <memory>
#include <vector>

namespace network
{
	namespace handler
	{
		/*
		* @brief Handle message reception for a peer.
		*/
		class ReceptionHandler
		{
			/*
			* @brief Define the reception state :
			* * 'Header' when receiving message header.
			* * 'Body' when receiving message body.
			*/
			enum class State {
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
			* @brief Receive the latest message on the linked socket.
			*
			* @return a message. 'nullptr' if no message has been received.
			*/
			std::unique_ptr<message::Message> receive();

		private:
			// TODO remove these private methods from the header ?
			void prepareHeaderReception();
			void prepareDataReception();
			void prepareReception(unsigned int expectedLength, State state);

			std::vector<char> buffer_;
			unsigned int received_ = 0;
			SOCKET socket_ = INVALID_SOCKET;
			State state_ = State::Header;

		};

	}
}