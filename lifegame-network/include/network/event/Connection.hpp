#pragma once

#include <network/event/Event.hpp>

namespace network
{
	namespace event
	{
		/*
		* @brief An event bearing the connection state of a peer.
		*/
		class Connection : public Event
		{
		public:
			static const Event::Type STATIC_TYPE = Event::Type::Connection;
			enum class State
			{
				Successfull,
				Failed,
			};
			Connection(const State state) : Event(Type::Connection), state_(state)
			{
			}
			State state() const;

		private:
			State state_;

		};
	}

}