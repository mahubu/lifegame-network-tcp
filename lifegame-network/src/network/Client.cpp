#include <network/Client.hpp>
#include <network/handler/ConnectionHandler.hpp>
#include <network/handler/ReceptionHandler.hpp>
#include <network/handler/SendingHandler.hpp>
#include <network/message/Disconnection.hpp>
#include <assert.h>
#include <iostream>

namespace network
{
	namespace tcp
	{

		/////////////////////////////////////////////////////////////////////////////////////

		class Client::ClientImpl
		{
			enum class State {
				Connecting,
				Connected,
				Disconnected,
			};

		public:
			ClientImpl();
			ClientImpl(const ClientImpl& other) = delete;
			ClientImpl& operator=(const ClientImpl& other) = delete;
			ClientImpl(ClientImpl&& other) = delete;
			ClientImpl& operator=(ClientImpl&& other) = delete;
			~ClientImpl();

			uint64_t id() const;
			bool initialize(SOCKET&& socket);
			bool connect(const std::string& address, unsigned short port);
			void disconnect();
			bool send(const char* data, unsigned int length);
			std::unique_ptr<message::Message> process();

		private:
			handler::ConnectionHandler connectionHandler_;
			handler::SendingHandler sendingHandler_;
			handler::ReceptionHandler receivingHandler_;
			SOCKET socket_ = INVALID_SOCKET;
			State state_ = State::Disconnected;

		};

		/////////////////////////////////////////////////////////////////////////////////////
		Client::ClientImpl::ClientImpl() = default;

		Client::ClientImpl::~ClientImpl()
		{
			disconnect();
		}

		uint64_t Client::ClientImpl::id() const
		{
			return static_cast<uint64_t>(socket_);
		}

		bool Client::ClientImpl::initialize(SOCKET&& socket) {
			assert(socket != INVALID_SOCKET);
			if (socket == INVALID_SOCKET) {
				return false;
			}

			assert(state_ == State::Disconnected);
			assert(socket_ == INVALID_SOCKET);
			socket_ = socket;
			if (!nonBlocking(socket_))
			{
				disconnect();
				return false;
			}

			sendingHandler_.initialize(socket_);
			receivingHandler_.initialize(socket_);
			state_ = State::Connected;
			return true;
		}

		bool Client::ClientImpl::connect(const std::string& address, unsigned short port)
		{
			assert(state_ == State::Disconnected);
			assert(socket_ == INVALID_SOCKET);
			if (socket_ != INVALID_SOCKET) {
				disconnect();
			}
			socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (socket_ == INVALID_SOCKET)
			{
				return false;
			}
			else if (!nonBlocking(socket_))
			{
				disconnect();
				return false;
			}
			if (connectionHandler_.connect(socket_, address, port))
			{
				state_ = State::Connecting;
				return true;
			}
			return false;
		}

		void Client::ClientImpl::disconnect()
		{
			if (socket_ != INVALID_SOCKET)
			{
				close(socket_);
			}
			socket_ = INVALID_SOCKET;
			state_ = State::Disconnected;
		}

		bool Client::Client::ClientImpl::send(const char* data, unsigned int length)
		{
			return sendingHandler_.enqueue(data, length);
		}

		std::unique_ptr<message::Message> Client::ClientImpl::process()
		{
			switch (state_)
			{
			case State::Connecting:
			{
				auto msg = connectionHandler_.fetch();
				if (msg)
				{
					if (msg->state() == message::Connection::State::Successfull)
					{
						sendingHandler_.initialize(socket_);
						receivingHandler_.initialize(socket_);
						state_ = State::Connected;
					}
					else
					{
						disconnect();
					}
				}
				return msg;
			} break;
			case State::Connected:
			{
				sendingHandler_.send();
				auto msg = receivingHandler_.receive();
				if (msg && msg->is<message::Disconnection>())
				{
					disconnect();
				}
				return msg;
			} break;
			case State::Disconnected:
			{
				/// Do nothing
			} break;
			}
			return nullptr;
		}

		/////////////////////////////////////////////////////////////////////////////////////
		Client::Client() = default;

		Client::Client(Client&& other) noexcept : impl_(std::move(other.impl_))
		{
		}

		Client& Client::operator=(Client&& other) noexcept
		{
			impl_ = std::move(other.impl_);
			return *this;
		}

		Client::~Client() = default;

		int64_t Client::id() const
		{
			return impl_ ? impl_->id() : (uint64_t)(-1);
		}

		bool Client::initialize(SOCKET && socket) {
			if (!impl_)
			{
				impl_ = std::make_unique<ClientImpl>();
			}
			return impl_ && impl_->initialize(std::move(socket));
		}

		bool Client::connect(const std::string & ipaddress, unsigned short port)
		{
			if (!impl_)
			{
				impl_ = std::make_unique<ClientImpl>();
			}
			return impl_ && impl_->connect(ipaddress, port);
		}

		void Client::disconnect()
		{
			if (impl_)
			{
				impl_->disconnect();
			}
			impl_.reset();
		}

		bool Client::send(const char* data, unsigned int length)
		{
			return impl_ && impl_->send(data, length);
		}

		std::unique_ptr<message::Message> Client::process()
		{
			return impl_ ? impl_->process() : nullptr;
		}

	}
}
