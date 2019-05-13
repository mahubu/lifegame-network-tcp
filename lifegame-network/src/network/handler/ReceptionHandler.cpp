#include <network/handler/ReceptionHandler.hpp>
#include <network/message/Disconnection.hpp>
#include <network/message/Raw.hpp>
#include <assert.h>

namespace network
{
	namespace handler
	{
		void ReceptionHandler::initialize(SOCKET socket)
		{
			assert(socket != INVALID_SOCKET);
			socket_ = socket;
			prepareHeaderReception();
		}

		std::unique_ptr<message::Message> ReceptionHandler::receive()
		{
			assert(socket_ != INVALID_SOCKET);
			int length = buffer_.size() - received_;
			char* buffer = buffer_.data() + received_;
			int received = ::recv(socket_, buffer, length, 0);
			if (received > 0) // Reception.
			{
				received_ += received;
				if (received_ == buffer_.size())
				{
					if (state_ == State::Body)
					{
						std::unique_ptr<message::Message> message = std::make_unique<message::Raw>(std::move(buffer_));
						prepareHeaderReception();
						return message;
					}
					else
					{
						prepareDataReception();
						return receive();
					}
				}
				return nullptr;
			}
			else if (received == 0) // Disconnection.
			{
				return std::make_unique<message::Disconnection>(message::Disconnection::Reason::Intentional);
			}
			else // received < 0
			{
				int error = error::latest();
				// No message to receive.
				if (error == static_cast<int>(error::WOULDBLOCK) || error == static_cast<int>(error::TRYAGAIN))
				{
					return nullptr;
				}
				else
				{
					return std::make_unique<message::Disconnection>(message::Disconnection::Reason::Lost);
				}
			}
		}

		void ReceptionHandler::prepareHeaderReception()
		{
			prepareReception(tcp::HeaderSize, State::Header);
		}

		void ReceptionHandler::prepareDataReception()
		{
			assert(buffer_.size() == sizeof(tcp::HeaderType));
			tcp::HeaderType networkDataLength;
			memcpy(&networkDataLength, buffer_.data(), sizeof(networkDataLength));
			const auto dataLength = ntohs(networkDataLength);
			prepareReception(dataLength, State::Body);
		}

		void ReceptionHandler::prepareReception(unsigned int dataLength, const State state)
		{
			received_ = 0;
			buffer_.clear();
			buffer_.resize(dataLength, 0);
			state_ = state;
		}
	}
}
