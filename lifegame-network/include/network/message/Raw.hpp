#pragma once

#include <network/message/Message.hpp>
#include <vector>

namespace network
{
	namespace message
	{
		/*
		* @brief A message bearing raw data.
		*/
		class Raw : public Message
		{
		public:
			static const Message::Type STATIC_TYPE = Message::Type::Raw;
			///Raw(std::vector<char>&& raw): Message(Type::Raw), raw_(std::move(raw))
			Raw(const std::vector<char>& raw) : Message(Type::Raw), raw_(raw)
			{
			}
			std::vector<char> raw() const;

		private:
			std::vector<char> raw_;

		};
	}

}