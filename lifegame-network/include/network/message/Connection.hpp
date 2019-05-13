#pragma once

#include <network/message/Message.hpp>

namespace network
{
	namespace message
	{
		/*
		* @brief A message bearing the connection state of a peer.
		*/
		class Connection : public Message
		{
		public:
			static const Message::Type STATIC_TYPE = Message::Type::Connection;
			enum class State
			{
				Successfull,
				Failed,
			};
			Connection(const State state) : Message(Type::Connection), state_(state)
			{
			}
			State state() const;

		private:
			State state_;

		};
	}

}