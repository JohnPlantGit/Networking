#include <vector>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "../Server/GameMessages.h"
#include "ServerList.h"

//std::vector<std::string> m_servers;
//
//void HandleNetworkMessages(RakNet::RakPeerInterface* peerInterface)
//{
//	RakNet::Packet* packet = nullptr;
//
//	while (true)
//	{
//		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
//		{
//			switch (packet->data[0])
//			{
//			case ID_NEW_INCOMING_CONNECTION:
//				printf("Connection incoming\n");
//				break;
//			case ID_DISCONNECTION_NOTIFICATION:
//				printf("Client disconnected\n");
//				break;
//			case ID_CONNECTION_LOST:
//				printf("Client lost connection\n");
//				break;
//			case ID_SERVER_LIST:
//				m_servers.push_back(std::string(packet->systemAddress.ToString()));
//				break;
//			case ID_CLIENT_LIST_REQUEST:
//			{
//				RakNet::BitStream bs;
//				bs.Write((RakNet::MessageID)GameMessages::ID_CLIENT_LIST_RESPONSE);
//				bs.Write((int)m_servers.size());
//				for (int i = 0; i < m_servers.size(); i++)
//				{
//					bs.Write(m_servers[i].c_str());
//				}
//
//				peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
//				break;
//			}
//			default:
//				printf("Unknown id: %i from %s\n", packet->data[0], packet->systemAddress.ToString());
//				break;
//			}
//		}
//	}
//}

int main()
{
	/*const unsigned short PORT = 5457;
	RakNet::RakPeerInterface* peerInterface = nullptr;

	peerInterface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd(PORT, 0);

	peerInterface->Startup(10, &sd, 1);
	peerInterface->SetMaximumIncomingConnections(10);

	HandleNetworkMessages(peerInterface);*/

	ServerList* serverList = new ServerList();
	serverList->StartConnectionLoop();

	while (true) {}

	return 0;
}