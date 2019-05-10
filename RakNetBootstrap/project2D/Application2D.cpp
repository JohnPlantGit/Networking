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

	m_font = new aie::Font("font/consolas.ttf", 20);

	m_client = new Client();
	m_client->StartConnectionLoop();

	m_refreshButton = new Button(glm::vec2(1200, 650), glm::vec2(50,50));

	return true;
}

void Application2D::shutdown() 
{	
	delete m_2dRenderer;
	delete m_client;
	delete m_refreshButton;
}

void Application2D::update(float deltaTime) 
{
	m_timer += deltaTime;

	// input example
	aie::Input* input = aie::Input::getInstance();

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

	for (int i = 0; i < m_servers.size(); i++)
	{
		m_serverButtons[i]->Draw(m_2dRenderer);
		m_2dRenderer->setRenderColour(0, 0, 0);
		m_2dRenderer->drawText(m_font, m_servers[i].c_str(), m_serverButtons[i]->GetPosition().x - 100, m_serverButtons[i]->GetPosition().y - 10);
		m_2dRenderer->setRenderColour(1, 1, 1);

	}


	m_refreshButton->Draw(m_2dRenderer);

	// done drawing sprites
	m_2dRenderer->end();
}