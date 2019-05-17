#include <network/event/Disconnection.hpp>

namespace network
{
	namespace event
	{
		Disconnection::Reason Disconnection::reason() const
		{
			return reason_;
		}
	}
}