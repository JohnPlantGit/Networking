#pragma once

#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <mutex>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "../Server/GameMessages.h"
class Client
{
public:
	Client();
	~Client();

	void InitializeConnection();
	void HandleNetworkConnection();
	void HandleNetworkMessages();
	void StartConnectionLoop();

	void SendMessageToServer();

	std::vector<std::string> RequestServerList();

private:

	RakNet::RakPeerInterface* m_peerInterface;
	const char* m_ip = "127.0.0.1";
	const unsigned short m_port = 5457;

	std::vector<std::string> m_serverList;
	std::mutex m_reconnecting;
	bool m_connected = false;
	bool m_connectionLoop = true;

	bool m_waitingForServerList = false;

	std::thread m_networkLoop;
	std::thread m_messageLoop;
};

