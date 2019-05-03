#include <iostream>
#include <string>
#include <thread>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "../Server/GameMessages.h"

RakNet::RakPeerInterface* m_peerInterface;
const char* IP = "127.0.0.1";
const unsigned short PORT = 5456;

void InitialiseClientConnection();

void HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	InitialiseClientConnection();
}

void InitialiseClientConnection()
{
	RakNet::SocketDescriptor sd;

	m_peerInterface->Startup(1, &sd, 1);

	printf("Connecting to server at: %s\n", IP);

	RakNet::ConnectionAttemptResult result = m_peerInterface->Connect(IP, PORT, nullptr, 0);

	if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("Unable to connect, id: %i", result);
	}
}

void HandleNetworkMessages()
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

		default:
			printf("Unknown id: %i\n", packet->data[0]);
			break;
		}
	}
}

void SendMessageToServer()
{
	while (true)
	{
		std::string input;
		std::getline(std::cin, input);

		if (!input.empty())
		{
			RakNet::BitStream bs;
			bs.Write((RakNet::MessageID)GameMessages::ID_CHAT_TEXT_MESSAGE);

			bs.Write(input.c_str());

			m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
		}
	}
}

int main()
{
	HandleNetworkConnection();

	std::thread inputThread(SendMessageToServer);

	while (true)
	{
		HandleNetworkMessages();
	}

	return 0;
}