#include "Application2D.h"
#include "Texture.h"
#include "Font.h"
#include "Input.h"

Application2D::Application2D() 
{

}

Application2D::~Application2D() 
{

}

bool Application2D::startup()
{	
	m_2dRenderer = new aie::Renderer2D();

	m_cameraX = 0;
	m_cameraY = 0;
	m_timer = 0;

	HandleNetworkConnection();

	return true;
}

void Application2D::shutdown() 
{	
	delete m_2dRenderer;
}

void Application2D::update(float deltaTime) 
{
	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();

	HandleNetworkMessages();

	// exit the application
	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();
}

void Application2D::draw() 
{
	// wipe the screen to the background colour
	clearScreen();

	// set the camera position before we begin rendering
	m_2dRenderer->setCameraPos(m_cameraX, m_cameraY);

	// begin drawing sprites
	m_2dRenderer->begin();


	// done drawing sprites
	m_2dRenderer->end();
}

void Application2D::HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	InitialiseClientConnection();
}

void Application2D::InitialiseClientConnection()
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

void Application2D::HandleNetworkMessages()
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

		default:
			printf("Unknown id: %i\n", packet->data[0]);
			break;
		}
	}
}