#include <network/message/Raw.hpp>

namespace network
{
	namespace message
	{
		std::vector<char> Raw::raw() const
		{
			return raw_;
		}
	}
}
