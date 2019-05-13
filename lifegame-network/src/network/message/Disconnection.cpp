#include <network/message/Disconnection.hpp>

namespace network
{
	namespace message
	{
		Disconnection::Reason Disconnection::reason() const
		{
			return reason_;
		}
	}
}