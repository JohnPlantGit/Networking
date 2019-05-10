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
	Client(std::string ip = "127.0.0.1", unsigned short port = 5457);
	~Client();

	void InitializeConnection();
	void HandleNetworkConnection();
	void HandleNetworkMessages();

	void StartConnectionLoop();
	void StopConnectionLoop();

	void RequestServerList();
	bool ServerListUpdate() { return m_updateServerList; }
	std::vector<std::string> GetServerList();

private:

	RakNet::RakPeerInterface* m_peerInterface;
	std::string m_ip;
	unsigned short m_port = 5457;

	bool m_connected = false;
	bool m_connectionLoop = true;

	std::vector<std::string> m_serverList;
	bool m_updateServerList = false;

	std::thread m_networkLoop;
};

