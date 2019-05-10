#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <mutex>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "../Server/GameMessages.h"
#include "Client.h"

//RakNet::RakPeerInterface* m_peerInterface;
//const char* IP = "127.0.0.1";
//const unsigned short PORT = 5457;
//std::vector<std::string> m_servers;
//bool connected = false;
//std::mutex reconnecting;
//
//void InitialiseClientConnection();
//
//void HandleNetworkConnection()
//{
//	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
//	InitialiseClientConnection();
//}
//
//void InitialiseClientConnection()
//{
//	RakNet::SocketDescriptor sd;
//
//	m_peerInterface->Startup(1, &sd, 1);
//
//	printf("Connecting to server at: %s\n", IP);
//
//	RakNet::ConnectionAttemptResult result = m_peerInterface->Connect(IP, PORT, nullptr, 0);
//
//	if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
//	{
//		printf("Unable to connect, id: %i", result);
//	}
//}
//
//void HandleNetworkMessages()
//{
//	reconnecting.lock();
//
//	RakNet::Packet* packet = nullptr;
//
//	for (packet = m_peerInterface->Receive(); packet; m_peerInterface->DeallocatePacket(packet), packet = m_peerInterface->Receive())
//	{
//		switch (packet->data[0])
//		{
//		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
//			printf("Other client disconnected\n");
//			break;
//		case ID_REMOTE_CONNECTION_LOST:
//			printf("Other client lost connection\n");
//			break;
//		case ID_REMOTE_NEW_INCOMING_CONNECTION:
//			printf("Other client connected\n");
//			break;
//		case ID_CONNECTION_REQUEST_ACCEPTED:
//			printf("Connected\n");
//			connected = true;
//			break;
//		case ID_NO_FREE_INCOMING_CONNECTIONS:
//			printf("Server full\n");
//			break;
//		case ID_DISCONNECTION_NOTIFICATION:
//			printf("We have been disconnected\n");
//			break;
//		case ID_CONNECTION_LOST:
//			printf("Connection lost\n");
//			break;
//		case ID_SERVER_TEXT_MESSAGE:
//		{
//			RakNet::BitStream bsIn(packet->data, packet->length, false);
//			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//
//			RakNet::RakString message;
//			bsIn.Read(message);
//
//			printf("%s\n", message.C_String());
//
//			break;
//		}
//		case ID_CLIENT_LIST_RESPONSE:
//		{			
//			RakNet::BitStream bsIn(packet->data, packet->length, false);
//			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
//
//			//bsIn.Read((char*)&m_servers, sizeof(std::vector<std::string>));
//			int count;
//			bsIn.Read(count);
//			for (int i = 0; i < count; i++)
//			{
//				RakNet::RakString newString;
//				bsIn.Read(newString);
//				m_servers.push_back(std::string(newString.C_String()));
//			}
//
//			for (int i = 0; i < m_servers.size(); i++)
//			{
//				printf("%s\n", m_servers[i].c_str());				
//			}
//			break;
//		}
//
//		default:
//			printf("Unknown id: %i\n", packet->data[0]);
//			break;
//		}
//	}
//	reconnecting.unlock();
//}
//
//void SendMessageToServer()
//{
//	while (true)
//	{
//		std::string input;
//		std::getline(std::cin, input);
//
//		if (!input.empty())
//		{
//			if (input == "/r")
//			{
//				m_servers.clear();
//
//				RakNet::BitStream bsOut;
//				bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_LIST_REQUEST);
//
//				m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
//				printf("Sending Request\n");
//			}
//			else if (input[0] == '/')
//			{
//				std::stringstream ss;
//				ss << input;
//				std::vector<std::string> parts;
//
//				while (ss.good())
//				{
//					std::string substr;
//					std::getline(ss, substr, ' ');
//					parts.push_back(substr);
//				}
//
//				if (parts[0] == "/connect" && parts.size() > 1)
//				{
//					reconnecting.lock();
//
//					int serverIndex = std::stoi(parts[1]);
//
//					RakNet::SystemAddress saOld;
//					saOld.FromString("127.0.0.1|5457");
//					m_peerInterface->CloseConnection(saOld, true);
//					m_peerInterface->Shutdown(500);
//					RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
//
//					m_peerInterface = RakNet::RakPeerInterface::GetInstance();
//
//					std::stringstream tmp;
//					tmp << m_servers[serverIndex - 1];
//					std::vector<std::string> result;
//
//					while (tmp.good())
//					{
//						std::string substr;
//						std::getline(tmp, substr, '|');
//						result.push_back(substr);
//					}
//
//					RakNet::SocketDescriptor sd;
//					m_peerInterface->Startup(1, &sd, 1);
//
//					m_peerInterface->Connect(result[0].c_str(), std::stoi(result[1].c_str()), nullptr, 0);
//
//					reconnecting.unlock();
//				}
//				else
//				{
//					RakNet::BitStream bs;
//					bs.Write((RakNet::MessageID)GameMessages::ID_CHAT_TEXT_MESSAGE);
//
//					bs.Write(input.c_str());
//
//					m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
//				}
//			}
//			else
//			{
//				RakNet::BitStream bs;
//				bs.Write((RakNet::MessageID)GameMessages::ID_CHAT_TEXT_MESSAGE);
//
//				bs.Write(input.c_str());
//
//				m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
//			}
//		}
//	}
//}

int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));

	Client* client = new Client();
	client->StartConnectionLoop();

	/*HandleNetworkConnection();

	std::thread inputThread(SendMessageToServer);
	std::thread requestThread([]
	{
		while (!connected) {}
		RakNet::BitStream bsOut;
		bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_LIST_REQUEST);

		m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		printf("Sending Request\n");				
	});
	while (true)
	{
		HandleNetworkMessages();		
	}*/

	while (true) {}
	//std::this_thread::sleep_for(std::chrono::seconds(10));

	return 0;
}
