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

	m_networkLoop.join();

	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
}

void Client::InitializeConnection()
{
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

	RakNet::SocketDescriptor sd;

	m_peerInterface->Startup(1, &sd, 1);

	InitializeConnection();
}

void Client::HandleNetworkMessages()
{
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
		case ID_CONNECTION_ATTEMPT_FAILED:
			printf("Failed to connect. Retrying\n");
			InitializeConnection();
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

			m_updateServerList = true;;
			break;
		}

		default:
			printf("Unknown id: %i\n", packet->data[0]);
			break;
		}
	}
}

void Client::StartConnectionLoop()
{
	m_connectionLoop = true;
	if (m_networkLoop.joinable())
		m_networkLoop.join();
	m_networkLoop = std::thread([this]
	{
		while (m_connectionLoop)
		{
			HandleNetworkMessages();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		}
	});
}

void Client::StopConnectionLoop()
{
	m_connectionLoop = false;
}

void Client::RequestServerList()
{
	m_serverList.clear();

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_LIST_REQUEST);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
	printf("Sending Request\n");
}

std::vector<std::string> Client::GetServerList()
{
	m_updateServerList = false;
	return m_serverList;
}