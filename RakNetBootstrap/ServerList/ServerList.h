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
class ServerList
{
public:
	ServerList(unsigned short port = 5457, unsigned int maxConnections = 10);
	~ServerList();

	void HandleNetworkConnection();
	void HandleNetworkMessages();
	void StartConnectionLoop();

private:
	RakNet::RakPeerInterface* m_peerInterface;
	unsigned short m_port;
	unsigned int m_maxConnections;

	std::vector<std::string> m_serverList;
	bool m_connectionLoop = true;

	std::thread m_networkLoop;
};

