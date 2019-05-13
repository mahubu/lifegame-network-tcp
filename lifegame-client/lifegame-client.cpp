#include <network/Client.hpp>
#include <network/Sockets.hpp>
#include <network/message/Connection.hpp>
#include <network/message/Disconnection.hpp>
#include <network/message/Message.hpp>
#include <network/message/Raw.hpp>
#include <iostream>

int main()
{
	std::cout << "Client.\n";

	if (!network::startup())
	{
		std::cerr << "Socket initialization error: " << network::error::latest() << std::endl;
		return EXIT_FAILURE;
	}

	network::tcp::Client client;
	bool connected = client.connect("127.0.0.1", 11000);
	if (!connected)
	{
		std::cerr << "Client connection error: " << network::error::latest() << std::endl;
		return EXIT_FAILURE;
	}
	else
	{
		while (true)
		{
			// TODO time out when 'connecting' never become 'connected'.
			while (auto msg = client.process())
			{
				if (msg->is<network::message::Connection>())
				{
					auto connection = msg->as<network::message::Connection>();
					if (connection->state() == network::message::Connection::State::Successfull)
					{
						std::cout << "Client connected." << std::endl;
						std::string phrase("Connected!");
						if (!client.send(phrase.c_str(), static_cast<unsigned int>(phrase.length())))
						{
							std::cerr << "Client sending error: " << network::error::latest() << std::endl;
							break;
						}
					}
					else
					{
						std::cerr << "Client connection error: " << static_cast<int>(connection->state()) << std::endl;
						break;
					}
				}
				else if (msg->is<network::message::Raw>())
				{
					auto raw = msg->as<network::message::Raw>();
					std::string reply(reinterpret_cast<const char*>(raw->raw().data()), raw->raw().size());
					std::cout << "Server response: " << reply << std::endl;
					std::string phrase("Exchanging!");
					if (!client.send(phrase.c_str(), static_cast<unsigned int>(phrase.length())))
					{
						std::cerr << "Client sending error: " << network::error::latest() << std::endl;
						break;
					}
				}
				else if (msg->is<network::message::Disconnection>())
				{
					auto disconnection = msg->as<network::message::Disconnection>();
					std::cout << "Client disconnected: " << static_cast<int>(disconnection->reason()) << std::endl;
					break;
				}
			}
		}
	}

	network::shutdown();

	return EXIT_SUCCESS;
}