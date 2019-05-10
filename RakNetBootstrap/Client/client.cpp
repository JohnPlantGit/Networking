#include "Client.h"

Client::Client(std::string ip, unsigned short port)
{
	m_ip = ip;
	m_port = port;
	HandleNetworkConnection();
}

Client::~Client()
{
	m_connectionLoop = false;

	m_messageLoop.join();
	m_networkLoop.join();

	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
}

void Client::InitializeConnection()
{
	RakNet::SocketDescriptor sd;

	m_peerInterface->Startup(1, &sd, 1);

	printf("Connecting to server at: %s\n", m_ip.c_str());

	RakNet::ConnectionAttemptResult result = m_peerInterface->Connect(m_ip.c_str(), m_port, nullptr, 0);

	if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("Unable to connect, id: %i", result);
	}
}

void Client::HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	InitializeConnection();
}

void Client::HandleNetworkMessages()
{
	m_reconnecting.lock();

	RakNet::Packet* packet = nullptr;

	for (packet = m_peerInterface->Receive(); packet; m_peerInterface->DeallocatePacket(packet), packet = m_peerInterface->Receive())
	{
		switch (packet->data[0])
		{
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			printf("Other client disconnected\n");
			break;
		case ID_REMOTE_CONNECTION_LOST:
			printf("Other client lost connection\n");
			break;
		case ID_REMOTE_NEW_INCOMING_CONNECTION:
			printf("Other client connected\n");
			break;
		case ID_CONNECTION_REQUEST_ACCEPTED:
			printf("Connected\n");
			m_connected = true;
			break;
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			printf("Server full\n");
			break;
		case ID_DISCONNECTION_NOTIFICATION:
			printf("We have been disconnected\n");
			break;
		case ID_CONNECTION_LOST:
			printf("Connection lost\n");
			break;
		case ID_SERVER_TEXT_MESSAGE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			RakNet::RakString message;
			bsIn.Read(message);

			printf("%s\n", message.C_String());

			break;
		}
		case ID_CLIENT_LIST_RESPONSE:
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			//bsIn.Read((char*)&m_servers, sizeof(std::vector<std::string>));
			int count;
			bsIn.Read(count);
			for (int i = 0; i < count; i++)
			{
				RakNet::RakString newString;
				bsIn.Read(newString);
				m_serverList.push_back(std::string(newString.C_String()));
			}

			for (int i = 0; i < m_serverList.size(); i++)
			{
				printf("%s\n", m_serverList[i].c_str());
			}

			m_waitingForServerList = false;
			break;
		}

		default:
			printf("Unknown id: %i\n", packet->data[0]);
			break;
		}
	}
	m_reconnecting.unlock();
}

void Client::SendMessageToServer()
{
	std::string input;
	std::getline(std::cin, input);

	if (!input.empty())
	{
		if (input == "/r")
		{
			RequestServerList();
		}
		else if (input[0] == '/')
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

			if (parts[0] == "/connect" && parts.size() > 1)
			{
				m_reconnecting.lock();

				int serverIndex = std::stoi(parts[1]);
				if (serverIndex > 0 && serverIndex <= m_serverList.size())
				{
					RakNet::SystemAddress saOld;
					saOld.FromString("127.0.0.1|5457");
					m_peerInterface->CloseConnection(saOld, true);
					m_peerInterface->Shutdown(500);
					RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);

					m_peerInterface = RakNet::RakPeerInterface::GetInstance();

					std::stringstream tmp;
					tmp << m_serverList[serverIndex - 1];
					std::vector<std::string> result;

					while (tmp.good())
					{
						std::string substr;
						std::getline(tmp, substr, '|');
						result.push_back(substr);
					}

					RakNet::SocketDescriptor sd;
					m_peerInterface->Startup(1, &sd, 1);

					m_peerInterface->Connect(result[0].c_str(), std::stoi(result[1].c_str()), nullptr, 0);

				}
				else
				{
					printf("Invalid index\n");
				}

				m_reconnecting.unlock();
			}
			else
			{
				RakNet::BitStream bs;
				bs.Write((RakNet::MessageID)GameMessages::ID_CHAT_TEXT_MESSAGE);

				bs.Write(input.c_str());

				m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
			}
		}
		else
		{
			RakNet::BitStream bs;
			bs.Write((RakNet::MessageID)GameMessages::ID_CHAT_TEXT_MESSAGE);

			bs.Write(input.c_str());

			m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		}
	}
}

void Client::StartConnectionLoop()
{
	m_networkLoop = std::thread([this]
	{
		while (m_connectionLoop)
		{
			HandleNetworkMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	});

	m_messageLoop = std::thread([this]
	{
		while (m_connectionLoop)
		{
			SendMessageToServer();
		}
	});
}

void Client::StopConnectionLoop()
{
	m_connectionLoop = false;
}

std::vector<std::string> Client::RequestServerList()
{
	m_serverList.clear();

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_LIST_REQUEST);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	printf("Sending Request\n");

	m_waitingForServerList = true;
	//while (m_waitingForServerList) {}

	return m_serverList;
}