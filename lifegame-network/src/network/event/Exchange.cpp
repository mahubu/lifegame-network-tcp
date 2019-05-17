#include <network/event/Exchange.hpp>

namespace network
{
	namespace event
	{
		std::vector<PacketUnit> Exchange::packet() const
		{
			return packet_;
		}
	}
}
