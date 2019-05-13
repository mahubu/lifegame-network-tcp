#include <network/handler/SendingHandler.hpp>
#include <assert.h>
#include <iostream>
#include <numeric>

namespace network
{
	namespace handler
	{
		void SendingHandler::initialize(SOCKET socket)
		{
			queueingBuffers_.clear();
			sendingBuffer_.clear();
			state_ = State::Idle;
			socket_ = socket;
		}

		bool SendingHandler::enqueue(const char* data, unsigned int length)
		{
			if (length > (std::numeric_limits<tcp::HeaderType>::max)()) {
				return false;
			}
			queueingBuffers_.emplace_back(data, data + length);
			return true;
		}

		void SendingHandler::send()
		{
			assert(socket_ != INVALID_SOCKET);
			if (state_ == State::Idle && !queueingBuffers_.empty())
			{
				prepareHeaderSending();
			}
			while (state_ != State::Idle && sendBuffer())
			{

				if (state_ == State::Header)
				{
					prepareBodySending();
				}
				else
				{
					if (!queueingBuffers_.empty())
					{
						prepareHeaderSending();
					}
					else
					{
						state_ = State::Idle;
					}
				}
			}
		}

		bool SendingHandler::sendBuffer()
		{
			if (sendingBuffer_.empty()) {
				return true;
			}

			// Send data based on previous send.
			int sent = ::send(socket_, sendingBuffer_.data(), sendingBuffer_.size(), 0);
			if (sent > 0)
			{
				if (sent == sendingBuffer_.size())
				{
					// All data have been sent.
					sendingBuffer_.clear();
					return true;
				}
				else
				{
					// Partial data send.
					memmove(sendingBuffer_.data() + sent, sendingBuffer_.data(), sent);
					sendingBuffer_.erase(sendingBuffer_.cbegin() + sent, sendingBuffer_.cend());
				}
			}
			return false;
		}

		void SendingHandler::prepareHeaderSending()
		{
			assert(!queueingBuffers_.empty());
			auto header = static_cast<tcp::HeaderType>(queueingBuffers_.front().size());
			const auto networkHeader = htons(header);
			sendingBuffer_.clear();
			sendingBuffer_.resize(tcp::HeaderSize);
			memcpy(sendingBuffer_.data(), &networkHeader, sizeof(tcp::HeaderType));
			state_ = State::Header;
		}

		void SendingHandler::prepareBodySending()
		{
			assert(!queueingBuffers_.empty());
			sendingBuffer_.swap(queueingBuffers_.front());
			queueingBuffers_.pop_front();
			state_ = State::Body;
		}

		size_t SendingHandler::queueSize() const
		{
			size_t size = std::accumulate(
				queueingBuffers_.cbegin(),
				queueingBuffers_.cend(),
				static_cast<size_t>(0),
				[](size_t n, const std::vector<char> & queuedItem) {
					return n + queuedItem.size() + tcp::HeaderSize;
				}
			);
			if (state_ == State::Body) {
				size += sendingBuffer_.size();
			}
			return size;
		}
	}
}