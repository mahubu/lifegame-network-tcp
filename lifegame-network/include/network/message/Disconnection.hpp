#pragma once

#include <network/message/Message.hpp>

namespace network
{
	namespace message
	{
		/*
		* @brief A message bearing the disconnection reason of a peer.
		*/
		class Disconnection : public Message
		{
		public:
			static const Message::Type STATIC_TYPE = Message::Type::Disconnection;
			enum class Reason
			{
				Intentional,
				Lost,
			};
			Disconnection(const Reason reason) : Message(Type::Disconnection), reason_(reason)
			{
			}
			Reason reason() const;

		private:
			Reason reason_;

		};
	}
}
