#include <network/Sockets.hpp>
#include <network/message/Connection.hpp>
#include <network/message/Disconnection.hpp>
#include <network/message/Message.hpp>
#include <network/message/Raw.hpp>
#include <server/Server.hpp>
#include <iostream>
#include <map>

int main()
{
	std::cerr << "Server.\n";

	if (!network::startup())
	{
		std::cout << "Socket initialization error: " << network::error::latest();
		return EXIT_FAILURE;
	}

	network::tcp::Server server;
	if (!server.startup(11000))
	{
		std::cerr << "Server connection error: " << network::error::latest();
		return EXIT_FAILURE;
	}

	while (true)
	{
		std::map<uint64_t, std::unique_ptr<network::message::Message>> messages = server.process();
		for (auto& msg : messages)
		{
			if (msg.second->is<network::message::Connection>())
			{
				std::cout << "Client connected: " << msg.first << std::endl;
			}
			else if (msg.second->is<network::message::Disconnection>())
			{
				std::cout << "Client disconnected: " << msg.first << std::endl;
			}
			else if (msg.second->is<network::message::Raw>())
			{
				auto raw = msg.second->as<network::message::Raw>();
				std::string reply(reinterpret_cast<const char*>(raw->raw().data()), raw->raw().size());
				std::cout << "Client response: " << reply << std::endl;
				server.send(raw->raw().data(), static_cast<unsigned int>(raw->raw().size()));
			}
		}
	}

	server.shutdown();

	network::shutdown();

	return EXIT_SUCCESS;
}