#include <network/message/Connection.hpp>

namespace network
{
	namespace message
	{
		Connection::State Connection::state() const
		{
			return state_;
		}
	}
}
