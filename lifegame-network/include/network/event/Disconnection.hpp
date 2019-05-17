#pragma once

#include <network/event/Event.hpp>

namespace network
{
	namespace event
	{
		/*
		* @brief An event bearing the disconnection reason of a peer.
		*/
		class Disconnection : public Event
		{
		public:
			static const Event::Type STATIC_TYPE = Event::Type::Disconnection;
			enum class Reason
			{
				Intentional,
				Lost,
			};
			Disconnection(const Reason reason) : Event(Type::Disconnection), reason_(reason)
			{
			}
			Reason reason() const;

		private:
			Reason reason_;

		};
	}
}
