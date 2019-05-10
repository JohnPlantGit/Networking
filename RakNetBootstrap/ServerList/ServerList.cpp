#include "ServerList.h"

ServerList::ServerList(unsigned short port, unsigned int maxConnections)
{
	m_port = port;
	m_maxConnections = maxConnections;
	HandleNetworkConnection();
}

ServerList::~ServerList()
{
	m_connectionLoop = false;
	m_networkLoop.join();

	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
}

void ServerList::HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd(m_port, 0);

	m_peerInterface->Startup(m_maxConnections, &sd, 1);
	m_peerInterface->SetMaximumIncomingConnections(m_maxConnections);
}

void ServerList::HandleNetworkMessages()
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
		case ID_SERVER_LIST:
			m_serverList.push_back(std::string(packet->systemAddress.ToString()));
			break;
		case ID_CLIENT_LIST_REQUEST:
		{
			RakNet::BitStream bs;
			bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_LIST_RESPONSE);
			bs.Write((int)m_serverList.size());
			for (int i = 0; i < m_serverList.size(); i++)
			{
				bs.Write(m_serverList[i].c_str());
			}

			m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
			break;
		}
		default:
			printf("Unknown id: %i from %s\n", packet->data[0], packet->systemAddress.ToString());
			break;
		}
	}
}

void ServerList::StartConnectionLoop()
{
	m_networkLoop = std::thread([this]
	{
		while (m_connectionLoop)
		{
			HandleNetworkMessages();
		}
	});
}