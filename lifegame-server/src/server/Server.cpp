#include <server/Server.hpp>
#include <network/Client.hpp>
#include <network/message/Connection.hpp>
#include <network/message/Disconnection.hpp>
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
			std::map<uint64_t, std::unique_ptr<message::Message>> process();
			bool send(uint64_t clientid, const char* data, unsigned int length);
			bool send(const char* data, unsigned int length);

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

		std::map<uint64_t, std::unique_ptr<message::Message>> Server::ServerImpl::process()
		{
			std::map<uint64_t, std::unique_ptr<message::Message>> messages;
			if (socket_ == INVALID_SOCKET)
			{
				return messages;
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
					clients_[client.id()] = std::move(client);
					//auto message = std::make_unique<message::Connection>(message::Connection::State::Successfull);
					//messages[client.id()] = std::move(message);
				}
			}

			// Processing message reception & send for each client. 
			for (auto itClient = clients_.begin(); itClient != clients_.end(); )
			{
				auto& client = *itClient;
				auto msg = client.second.process();
				if (msg)
				{
					if (msg->is<message::Disconnection>())
					{
						itClient = clients_.erase(itClient);
					}
					else
					{
						messages[client.first] = std::move(msg);
						++itClient;
					}
				}
				else
				{
					++itClient;
				}
			}

			return messages;
		}

		bool Server::ServerImpl::send(uint64_t clientid, const char* data, unsigned int length)
		{
			auto itClient = clients_.find(clientid);
			return itClient != clients_.end() && itClient->second.send(data, length);
		}

		bool Server::ServerImpl::send(const char* data, unsigned int length)
		{
			bool sent = true;
			for (auto& client : clients_)
			{
				sent &= client.second.send(data, length);
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

		std::map<uint64_t, std::unique_ptr<message::Message>> Server::process()
		{
			return impl_ ? impl_->process() : std::map<uint64_t, std::unique_ptr<message::Message>>();
		}

		bool Server::send(uint64_t clientid, const char* data, unsigned int length)
		{
			return impl_ ? impl_->send(clientid, data, length) : false;
		}

		bool Server::send(const char* data, unsigned int length)
		{
			return impl_ ? impl_->send(data, length) : false;
		}
	}
}