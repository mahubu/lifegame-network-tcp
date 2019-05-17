#include <server/Server.hpp>
#include <network/Client.hpp>
#include <network/event/Connection.hpp>
#include <network/event/Disconnection.hpp>
#include <network/event/Exchange.hpp>
#include <map>
#include <list>
#include <assert.h>

namespace network
{
	namespace tcp
	{

		/////////////////////////////////////////////////////////////////////////////////////

		class Server::ServerImpl
		{
		public:
			ServerImpl();
			ServerImpl(const ServerImpl& server) = delete;
			ServerImpl& operator=(const ServerImpl& server) = delete;
			ServerImpl(ServerImpl&& server) = delete;
			ServerImpl& operator=(ServerImpl&& server) = delete;
			~ServerImpl();

			bool startup(unsigned short port);
			void shutdown();
			void process(std::map<uint64_t, std::unique_ptr<event::Event>>& events);
			bool send(uint64_t clientid, const PacketUnit* packet, unsigned int length);
			bool send(const PacketUnit* packet, unsigned int length);

		private:
			std::map<uint64_t, Client> clients_;
			SOCKET socket_ = INVALID_SOCKET;

		};

		/////////////////////////////////////////////////////////////////////////////////////

		Server::ServerImpl::ServerImpl() = default;

		Server::ServerImpl::~ServerImpl()
		{
			shutdown();
		}

		bool Server::ServerImpl::startup(unsigned short port)
		{
			assert(socket_ == INVALID_SOCKET);
			if (socket_ != INVALID_SOCKET)
			{
				shutdown();
			}
			socket_ = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (socket_ == INVALID_SOCKET)
			{
				return false;
			}

			if (!reuseAddress(socket_) || !nonBlocking(socket_))
			{
				shutdown();
				return false;
			}

			sockaddr_in address;
			address.sin_addr.s_addr = INADDR_ANY;
			address.sin_port = htons(port);
			address.sin_family = AF_INET;
			if (bind(socket_, reinterpret_cast<sockaddr*>(&address), sizeof(address)) != 0)
			{
				shutdown();
				return false;
			}
			if (listen(socket_, SOMAXCONN) != 0)
			{
				shutdown();
				return false;
			}
			return true;
		}

		void Server::ServerImpl::shutdown()
		{
			for (auto& client : clients_)
			{
				client.second.disconnect();
			}
			clients_.clear();
			if (socket_ != INVALID_SOCKET)
			{
				close(socket_);
			}
			socket_ = INVALID_SOCKET;
		}

		void Server::ServerImpl::process(std::map<uint64_t, std::unique_ptr<event::Event>>& events)
		{
			if (socket_ == INVALID_SOCKET)
			{
				return;
			}

			// Processing message reception & send for each client. 
			for (auto itClient = clients_.begin(); itClient != clients_.end(); )
			{
				auto& client = *itClient;
				auto event = client.second.process();
				if (event)
				{
					auto id = client.first;
					if (event->is<event::Disconnection>())
					{
						itClient = clients_.erase(itClient);
					}
					else
					{
						++itClient;
					}
					events[id] = std::move(event);
				}
				else
				{
					++itClient;
				}
			}

			// Listening to new clients.
			/// TODO use in another thread instead -> can block when new clients don't stop coming.
			while (true)
			{
				sockaddr_in addr = { 0 };
				socklen_t addrlen = sizeof(addr);
				SOCKET clientSocket = accept(socket_, reinterpret_cast<sockaddr*>(&addr), &addrlen);
				if (clientSocket == INVALID_SOCKET)
				{
					break;
				}
				Client client;
				if (client.initialize(std::move(clientSocket)))
				{
					auto id = client.id();
					auto connection = std::make_unique<event::Connection>(event::Connection::State::Successfull);
					events[id] = std::move(connection);
					clients_[id] = std::move(client);
				}
			}

		}

		bool Server::ServerImpl::send(uint64_t clientid, const PacketUnit* packet, unsigned int length)
		{
			auto itClient = clients_.find(clientid);
			return itClient != clients_.end() && itClient->second.send(packet, length);
		}

		bool Server::ServerImpl::send(const PacketUnit* packet, unsigned int length)
		{
			bool sent = true;
			for (auto& client : clients_)
			{
				sent &= client.second.send(packet, length);
			}
			return sent;
		}

		/////////////////////////////////////////////////////////////////////////////////////

		Server::Server() = default;

		Server::~Server() = default;

		Server::Server(Server&& other) noexcept : impl_(std::move(other.impl_))
		{
		}

		Server& Server::operator=(Server&& other) noexcept
		{
			impl_ = std::move(other.impl_);
			return *this;
		}

		bool Server::startup(unsigned short port)
		{
			if (!impl_)
			{
				impl_ = std::make_unique<ServerImpl>();
			}
			return impl_ && impl_->startup(port);
		}

		void Server::shutdown()
		{
			if (impl_)
			{
				impl_->shutdown();
			}
		}

		void Server::process(std::map<uint64_t, std::unique_ptr<event::Event>>& events)
		{
			if (impl_)
			{
				impl_->process(events);
			}
		}

		bool Server::send(uint64_t clientid, const PacketUnit* packet, unsigned int length)
		{
			return impl_ ? impl_->send(clientid, packet, length) : false;
		}

		bool Server::send(const PacketUnit* packet, unsigned int length)
		{
			return impl_ ? impl_->send(packet, length) : false;
		}
	}
}