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
	printf("Enter port\n");
	std::string input;
	std::getline(std::cin, input);

	m_port = std::stoi(input);

	m_2dRenderer = new aie::Renderer2D();

	m_cameraX = 0;
	m_cameraY = 0;
	m_timer = 0;

	m_font = new aie::Font("font/consolas.ttf", 20);

	m_client = new Client();
	m_client->StartConnectionLoop();

	m_refreshButton = new Button(glm::vec2(1200, 650), glm::vec2(50,50));

	return true;
}

void Application2D::shutdown() 
{	
	delete m_2dRenderer;
	delete m_font;
	delete m_client;
	delete m_refreshButton;
}

void Application2D::update(float deltaTime) 
{
	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();

	if (m_inLobby)
	{
		if (input->wasMouseButtonPressed(0))
		{
			glm::vec2 mousePos(input->getMouseX(), input->getMouseY());
			if (m_refreshButton->ContainsPoint(mousePos))
			{
				m_client->RequestServerList();
			}
			for (int i = 0; i < m_serverButtons.size(); i++)
			{
				if (m_serverButtons[i]->ContainsPoint(mousePos))
				{
					m_inLobby = false;
					
					std::stringstream tmp;
					tmp << m_servers[i];
					std::vector<std::string> result;

					while (tmp.good())
					{
						std::string substr;
						std::getline(tmp, substr, '|');
						result.push_back(substr);
					}

					if (result.size() >= 2)
						InitializeConnection(result[0], std::stoi(result[1].c_str()));

					m_client->StopConnectionLoop();
				}
			}
		}

		if (m_client->ServerListUpdate())
		{
			m_servers = m_client->GetServerList();
			for (int i = 0; i < m_serverButtons.size(); i++)
			{
				delete m_serverButtons[i];
			}
			m_serverButtons.clear();
			for (int i = 0; i < m_servers.size(); i++)
			{
				m_serverButtons.push_back(new Button(glm::vec2(150, 600 - (50 * i)), glm::vec2(200, 25)));
			}
		}
	}
	else
	{
		HandleNetworkMessages();

		std::vector<unsigned int> pressedChars = input->getPressedCharacters();

		if (!pressedChars.empty())
			m_inputString.push_back(pressedChars[0]);

		if (input->wasKeyPressed(aie::INPUT_KEY_BACKSPACE))
		{
			m_inputString.pop_back();
		}

		if (input->wasKeyPressed(aie::INPUT_KEY_ENTER) && input->wasKeyPressed(aie::INPUT_KEY_KP_ENTER) && !m_inputString.empty())
		{
			RakNet::BitStream bs;
			bs.Write((RakNet::MessageID)GameMessages::ID_CHAT_TEXT_MESSAGE);

			bs.Write(m_inputString.c_str());

			m_peerInterface->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);

			m_inputString.clear();
		}

		if (input->wasKeyPressed(aie::INPUT_KEY_ESCAPE))
		{
			m_inLobby = true;

			Disconnect();

			m_client->StartConnectionLoop();

			m_chatLog.clear();
		}
	}	

	// exit the application
	/*if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();*/
}

void Application2D::draw() 
{
	// wipe the screen to the background colour
	clearScreen();

	// set the camera position before we begin rendering
	m_2dRenderer->setCameraPos(m_cameraX, m_cameraY);

	// begin drawing sprites
	m_2dRenderer->begin();

	if (m_inLobby)
	{
		for (int i = 0; i < m_servers.size(); i++)
		{
			m_serverButtons[i]->Draw(m_2dRenderer);
			m_2dRenderer->setRenderColour(0, 0, 0);
			m_2dRenderer->drawText(m_font, m_servers[i].c_str(), m_serverButtons[i]->GetPosition().x - 100, m_serverButtons[i]->GetPosition().y - 10);
			m_2dRenderer->setRenderColour(1, 1, 1);

		}

		m_refreshButton->Draw(m_2dRenderer);
	}
	else
	{
		m_2dRenderer->drawText(m_font, m_inputString.c_str(), 100, 100);
		
		for (int i = 0; i < m_chatLog.size(); i++)
		{
			m_2dRenderer->drawText(m_font, m_chatLog[i].c_str(), 100, 140 + (40 * (m_chatLog.size() - i)));
		}
	}

	// done drawing sprites
	m_2dRenderer->end();
}

void Application2D::InitializeConnection(std::string ip, unsigned short port)
{
	if (m_peerInterface)
	{
		Disconnect();
	}

	m_peerInterface = RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd(m_port, "127.0.0.1");
	m_peerInterface->Startup(1, &sd, 1);

	m_peerInterface->Connect(ip.c_str(), port, nullptr, 0);
}

void Application2D::HandleNetworkConnection()
{
	m_peerInterface = RakNet::RakPeerInterface::GetInstance();

	RakNet::SocketDescriptor sd;

	m_peerInterface->Startup(1, &sd, 1);
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
		case ID_CONNECTION_ATTEMPT_FAILED:
			printf("Failed to connect\n");
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

			m_chatLog.push_back(std::string(message.C_String()));

			break;
		}
		default:
			printf("Unknown id: %i\n", packet->data[0]);
			break;
		}
	}
}

void Application2D::Disconnect()
{
	m_peerInterface->Shutdown(100);
	RakNet::RakPeerInterface::DestroyInstance(m_peerInterface);
	m_peerInterface = nullptr;
}