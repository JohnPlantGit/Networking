#include <iostream>
#include <string>
#include <thread>
#include <map>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameMessages.h"

std::map<std::string, std::string> m_usernames;

void HandleNetworkMessages(RakNet::RakPeerInterface* peerInterface)
{
	RakNet::Packet* packet = nullptr;

	while (true)
	{
		for (packet = peerInterface->Receive(); packet; peerInterface->DeallocatePacket(packet), packet = peerInterface->Receive())
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
			case ID_CHAT_TEXT_MESSAGE:
			{
				RakNet::BitStream bsIn(packet->data, packet->length, false);
				bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

				RakNet::BitStream bsOut;				
				RakNet::RakString message;
				bsIn.Read(message);	

				if (message.StrCmp("/Setusername") == 0)
				{
					m_usernames[packet->systemAddress.ToString()] = "Test";
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

				peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);

				printf("%s\n", message.C_String());		

				break;
			}
			default:
				printf("Unknown id: %i\n", packet->data[0]);
				break;
			}
		}
	}
}

void SendClientPing(RakNet::RakPeerInterface* peerInterface)
{
	while (true)
	{
		std::string input;
		std::getline(std::cin, input);

		if (!input.empty())
		{
			if (input[0] != '/')
			{
				RakNet::BitStream bs;
				bs.Write((RakNet::MessageID)GameMessages::ID_SERVER_TEXT_MESSAGE);
				//bs.Write("Ping");
				bs.Write(input.c_str());

				peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

				if (input == "Exit")
				{
					return;
				}
			}
		}

		//std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

int main()
{
	const unsigned short PORT = 5456;
	RakNet::RakPeerInterface* peerInterface = nullptr;

	printf("Starting server\n");

	peerInterface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd(PORT, 0);

	peerInterface->Startup(32, &sd, 1);
	peerInterface->SetMaximumIncomingConnections(32);

	std::thread ping(SendClientPing, peerInterface);
	
	HandleNetworkMessages(peerInterface);

	ping.join();

	return 0;
}