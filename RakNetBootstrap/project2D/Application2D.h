#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "Client.h"
#include "Button.h"

class Application2D : public aie::Application 
{
public:

	Application2D();
	virtual ~Application2D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void InitializeConnection(std::string ip, unsigned short port);
	void HandleNetworkConnection();
	void HandleNetworkMessages();
	void Disconnect();

protected:

	aie::Renderer2D*	m_2dRenderer;
	aie::Font* m_font;

	Client* m_client;
	RakNet::RakPeerInterface* m_peerInterface = nullptr;
	unsigned short m_port;

	// lobby
	std::vector<std::string> m_servers;
	std::vector<Button*> m_serverButtons;
	Button* m_refreshButton;
	//

	// in game
	std::string m_inputString;
	std::vector<std::string> m_chatLog;
	//

	bool m_inLobby = true;

	float m_cameraX, m_cameraY;
	float m_timer;
};