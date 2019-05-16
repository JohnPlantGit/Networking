#include "Server.h"
#include <fstream>

Server::Server(unsigned short hostPort, std::string serverListIp, unsigned short serverListPort)
{
	m_hostPort = hostPort;
	m_serverListIp = serverListIp;
	m_serverListPort = serverListPort;

	// opens the username file
	std::fstream file;
	std::string filename = std::to_string(m_hostPort) + ".txt";
	file.open(filename, std::fstream::in);
	if (file.is_open())
	{
		while (file.good()) // while there is more text in the file
		{
			std::string systemAddress;
			std::getline(file, systemAddress, ','); // get the system address
			std::string username;
			std::getline(file, username); // get the username
			if (!systemAddress.empty() && !username.empty()) // if they are both valid
				m_usernames[systemAddress] = username; // store the username in the map using the system address as the index
		}
	}

	file.close(); // close the file

	HandleNetworkConnection(); // start the peer interface
}

Server::~Server()
{
	std::fstream file;
	std::string filename = std::to_string(m_hostPort) + ".txt";
	file.open(filename, std::fstream::out); // open the username file

	if (file.is_open())
	{
		for (auto &pair : m_usernames)
			file << pair.first << ',' << pair.second << '\n'; // store the system address and the username 
	}

	file.close(); // close the file
}

void Server::InitializeConnection()
{
	m_peerInterface->Connect(m_serverListIp.c_str(), m_serverListPort, nullptr, 0); // connect to the server list

	// start a thread
	std::thread registerThread([this]
	{
		while (!m_connected) {} // wait until connected to the server list

		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_LIST_REGISTER);

		RakNet::SystemAddress sa;
		std::string address = m_serverListIp + '|' + std::to_string(m_serverListPort);
		sa.FromString(address.c_str());

		m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sa, false); // send the packet to the server list to register
	});

	registerThread.detach(); // let the thread keep running
}

void Server::HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	
	RakNet::SocketDescriptor sd(m_hostPort, 0);

	m_peerInterface->Startup(33, &sd, 1); // start up the server with 33 connections, 32 incoming - 1 outgoing for the server list
	m_peerInterface->SetMaximumIncomingConnections(32);

	InitializeConnection(); // connect to the server list
}

void Server::HandleNetworkMessages()
{
	RakNet::Packet* packet = nullptr;

	for (packet = m_peerInterface->Receive(); packet; m_peerInterface->DeallocatePacket(packet), packet = m_peerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_NEW_INCOMING_CONNECTION:
			printf("Connection incoming\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			printf("Client disconnected\n");
			break;
		case ID_CONNECTION_LOST:
			printf("Client lost connection\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			printf("Connected\n");
			m_connected = true;
			break;
		case ID_UNCONNECTED_PONG:
			printf("Pong\n");
			m_connected = true;
			break;
		case ID_CHAT_TEXT_MESSAGE: // When receiving a message from a user
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			RakNet::BitStream bsOut;
			RakNet::RakString message;
			bsIn.Read(message); // read the message

			if (message[0] == '/') // if the message starts with a '/'
			{
				ParseCommand(std::string(message.C_String()), packet->systemAddress); // parse the command
			}

			if (m_usernames.find(packet->systemAddress.ToString()) != m_usernames.end()) // if the system address has set a username
			{
				message.Set("%s says: %s", m_usernames[packet->systemAddress.ToString()].c_str(), message.C_String()); // use their username in the message
			}
			else // if they don't have a username
			{
				message.Set("%s says: %s", packet->systemAddress.ToString(), message.C_String()); // use their system address in the message
			}
			bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_TEXT_MESSAGE);
			bsOut.Write(message.C_String());
			m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, /*packet->systemAddress*/RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // forward the message to the users

			printf("%s\n", message.C_String());

			break;
		}
		default:
			printf("Unknown id: %i\n", packet->data[0]);
			break;
		}
	}
}

void Server::StartConnectionLoop()
{
	// runs handleNetworkMessages every 100 milliseconds in a seperate thread
	m_networkLoop = std::thread([this]
	{
		while (m_connectionLoop)
		{
			HandleNetworkMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	});
}

void Server::ParseCommand(std::string input, RakNet::SystemAddress sa)
{
	// split the input in strings separated but ' '
	std::stringstream ss;
	ss << input;
	std::vector<std::string> parts;

	while (ss.good())
	{
		std::string substr;
		std::getline(ss, substr, ' ');
		parts.push_back(substr);
	}

	if (parts[0] == "/" + m_commands[0]) // if the first substring is a valid command
	{
		SetUserName(parts, sa); // set the username
	}
}

void Server::SetUserName(std::vector<std::string> parts, RakNet::SystemAddress sa)
{
	if (parts.size() > 1)
	{
		m_usernames[sa.ToString()] = parts[1]; // store the username in the map

		std::fstream file;
		std::string filename = std::to_string(m_hostPort) + ".txt";
		file.open(filename, std::fstream::out); // open the username file

		if (file.is_open())
		{
			for (auto &pair : m_usernames)
				file << pair.first << ',' << pair.second << '\n'; // save each system address and username
		}

		file.close();// close the file
	}
}