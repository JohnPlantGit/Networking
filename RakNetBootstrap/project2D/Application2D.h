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

	/*
		@brief Waits for the user to input a port to use and starts up the client's connection loop
		@return if the application should continue to start
	*/
	virtual bool startup();
	virtual void shutdown();

	/*
		@brief While in the lobby, allows the user to refresh the server list and choose a server to connect to
		@brief While in game, allows the user to send text messages to the server
	*/
	virtual void update(float deltaTime);
	/*
		@brief draws buttons and text
	*/
	virtual void draw();

	/*
		@brief starts up the peer interface and connects to a server user the ip and port
		@param the ip to connect to
		@param the port to connect to
	*/
	void InitializeConnection(std::string ip, unsigned short port);
	/*
		@brief Processes packets recieved by the peer interface
	*/
	void HandleNetworkMessages();
	/*
		@brief Shuts down the peer interface
	*/
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