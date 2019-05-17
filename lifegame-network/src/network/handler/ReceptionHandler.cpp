#include <network/handler/ReceptionHandler.hpp>
#include <network/event/Disconnection.hpp>
#include <network/event/Exchange.hpp>
#include <assert.h>

#include <iostream>

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

		std::unique_ptr<event::Event> ReceptionHandler::receive()
		{
			assert(socket_ != INVALID_SOCKET);
			int length = buffer_.size() - received_;
			PacketUnit* buffer = buffer_.data() + received_;
			int received = ::recv(socket_, reinterpret_cast<char*>(buffer), length, 0);
			if (received > 0) // Reception.
			{
				received_ += received;
				if (received_ == buffer_.size())
				{
					if (state_ == State::Body)
					{
						std::unique_ptr<event::Event> message = std::make_unique<event::Exchange>(std::move(buffer_));
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
				return std::make_unique<event::Disconnection>(event::Disconnection::Reason::Intentional);
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
					return std::make_unique<event::Disconnection>(event::Disconnection::Reason::Lost);
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
