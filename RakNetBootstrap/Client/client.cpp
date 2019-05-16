#include "Client.h"

Client::Client(std::string ip, unsigned short port)
{
	m_ip = ip;
	m_port = port;
	HandleNetworkConnection();
}

Client::~Client()
{
	m_connectionLoop = false; // to stop the connection loop from running again

	m_networkLoop.join(); // waits for connection loop thread to stop

	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface); // destroys the peer interface
}

void Client::InitializeConnection()
{
	printf("Connecting to server at: %s\n", m_ip.c_str());

	RakNet::ConnectionAttemptResult result = m_peerInterface->Connect(m_ip.c_str(), m_port, nullptr, 0); // connects the peer interface to the stored ip and port

	if (result != RakNet::CONNECTION_ATTEMPT_STARTED)
	{
		printf("Unable to connect, id: %i", result);
	}
}

void Client::HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd;

	m_peerInterface->Startup(1, &sd, 1); // starts the peer interface as a pure client

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
		case ID_SERVER_TEXT_MESSAGE: // Receiving a text message from the server -OLD
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			RakNet::RakString message;
			bsIn.Read(message); // read the data as a string

			printf("%s\n", message.C_String()); // prints the message

			break;
		}
		case ID_CLIENT_LIST_RESPONSE: // Receiving the server list
		{
			RakNet::BitStream bsIn(packet->data, packet->length, false);
			bsIn.IgnoreBytes(sizeof(RakNet::MessageID));

			//bsIn.Read((char*)&m_servers, sizeof(std::vector<std::string>));
			int count;
			bsIn.Read(count); // reads the amount of servers
			for (int i = 0; i < count; i++)
			{
				RakNet::RakString newString;
				bsIn.Read(newString); // reads data as a string
				m_serverList.push_back(std::string(newString.C_String())); // stores the string in the server list
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
	if (m_networkLoop.joinable())
		m_networkLoop.join(); // checks that the network loop isn't already running

	m_connectionLoop = true;
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

void Client::StopConnectionLoop()
{
	m_connectionLoop = false;
}

void Client::RequestServerList()
{
	m_serverList.clear(); // clears the old server list

	RakNet::BitStream bsOut;
	bsOut.Write((RakNet::MessageID)GameMessages::ID_CLIENT_LIST_REQUEST);

	m_peerInterface->Send(&bsOut, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true); // sends a packet just containing the ID_CLIENT_LIST_REQUEST Message 
	printf("Sending Request\n");
}

std::vector<std::string> Client::GetServerList()
{
	m_updateServerList = false;
	return m_serverList;
}