#pragma once

#include "Application.h"
#include "Renderer2D.h"

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

class Application2D : public aie::Application 
{
public:

	Application2D();
	virtual ~Application2D();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();

	void HandleNetworkConnection();
	void InitialiseClientConnection();

	void HandleNetworkMessages();

protected:

	aie::Renderer2D*	m_2dRenderer;

	float m_cameraX, m_cameraY;
	float m_timer;

	RakNet::RakPeerInterface* m_peerInterface;
	const char* IP = "127.0.0.1";
	const unsigned short PORT = 5456;
};