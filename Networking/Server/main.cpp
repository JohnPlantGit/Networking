#include <iostream>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

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
				printf("Client lost connection");
				break;

			default:
				printf("Unknown id: %i\n", packet->data[0]);
				break;
			}
		}
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

	HandleNetworkMessages(peerInterface);

	return 0;
}