#include <network/event/Connection.hpp>

namespace network
{
	namespace event
	{
		Connection::State Connection::state() const
		{
			return state_;
		}
	}
}
