#include "Server.h"
#include <fstream>

Server::Server(unsigned short hostPort, std::string serverListIp, unsigned short serverListPort)
{
	m_hostPort = hostPort;
	m_serverListIp = serverListIp;
	m_serverListPort = serverListPort;

	std::fstream file;
	std::string filename = std::to_string(m_hostPort) + ".txt";
	file.open(filename, std::fstream::in);
	if (file.is_open())
	{
		while (file.good())
		{
			std::string systemAddress;
			std::getline(file, systemAddress, ',');
			std::string username;
			std::getline(file, username);
			if (!systemAddress.empty() && !username.empty())
				m_usernames[systemAddress] = username;
		}
	}

	file.close();

	HandleNetworkConnection();
}

Server::~Server()
{
	std::fstream file;
	std::string filename = std::to_string(m_hostPort) + ".txt";
	file.open(filename, std::fstream::out);

	if (file.is_open())
	{
		for (auto &pair : m_usernames)
			file << pair.first << ',' << pair.second << '\n';
	}

	file.close();
}

void Server::InitializeConnection()
{
	m_peerInterface->Connect(m_serverListIp.c_str(), m_serverListPort, nullptr, 0);

	std::thread registerThread([this]
	{
		while (!m_connected) {}

		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_LIST_REGISTER);

		RakNet::SystemAddress sa;
		std::string address = m_serverListIp + '|' + std::to_string(m_serverListPort);
		sa.FromString(address.c_str());

		m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sa, false);
	});

	registerThread.detach();
}

void Server::HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	
	RakNet::SocketDescriptor sd(m_hostPort, 0);

	m_peerInterface->Startup(33, &sd, 1);
	m_peerInterface->SetMaximumIncomingConnections(32);

	InitializeConnection();
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
		case ID_CHAT_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			RakNet::BitStream bsOut;
			RakNet::RakString message;
			bsIn.Read(message);

			if (message[0] == '/')
			{
				ParseCommand(std::string(message.C_String()), packet->systemAddress);
			}

			if (m_usernames.find(packet->systemAddress.ToString()) != m_usernames.end())
			{
				message.Set("%s says: %s", m_usernames[packet->systemAddress.ToString()].c_str(), message.C_String());
			}
			else
			{
				message.Set("%s says: %s", packet->systemAddress.ToString(), message.C_String());
			}
			bsOut.Write((RakNet::MessageID)GameMessages::ID_SERVER_TEXT_MESSAGE);
			bsOut.Write(message.C_String());
			m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, /*packet->systemAddress*/RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

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
	std::stringstream ss;
	ss << input;
	std::vector<std::string> parts;

	while (ss.good())
	{
		std::string substr;
		std::getline(ss, substr, ' ');
		parts.push_back(substr);
	}

	if (parts[0] == "/" + m_commands[0])
	{
		SetUserName(parts, sa);
	}
}

void Server::SetUserName(std::vector<std::string> parts, RakNet::SystemAddress sa)
{
	if (parts.size() > 1)
	{
		m_usernames[sa.ToString()] = parts[1];

		std::fstream file;
		std::string filename = std::to_string(m_hostPort) + ".txt";
		file.open(filename, std::fstream::out);

		if (file.is_open())
		{
			for (auto &pair : m_usernames)
				file << pair.first << ',' << pair.second << '\n';
		}

		file.close();
	}
}