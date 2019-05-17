#include <network/Sockets.hpp>
#include <network/Client.hpp>
#include <network/handler/ConnectionHandler.hpp>
#include <network/handler/ReceptionHandler.hpp>
#include <network/handler/SendingHandler.hpp>
#include <network/event/Disconnection.hpp>
#include <assert.h>

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
			bool send(const PacketUnit* packet, unsigned int length);
			std::unique_ptr<event::Event> process();

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

		bool Client::Client::ClientImpl::send(const PacketUnit* packet, unsigned int length)
		{
			return sendingHandler_.enqueue(packet, length);
		}

		std::unique_ptr<event::Event> Client::ClientImpl::process()
		{
			switch (state_)
			{
			case State::Connecting:
			{
				auto event = connectionHandler_.ready();
				if (event)
				{
					if (event->state() == event::Connection::State::Successfull)
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
				return event;
			} break;
			case State::Connected:
			{
				sendingHandler_.send();
				auto event = receivingHandler_.receive();
				if (event && event->is<event::Disconnection>())
				{
					disconnect();
				}
				return event;
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

		uint64_t Client::id() const
		{
			// TODO unsigned -1 ???? -> use int64_t instead
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

		bool Client::send(const PacketUnit* packet, unsigned int length)
		{
			return impl_ && impl_->send(packet, length);
		}

		std::unique_ptr<event::Event> Client::process()
		{
			return impl_ ? impl_->process() : nullptr;
		}

	}
}
