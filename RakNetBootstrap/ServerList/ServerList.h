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
	/*
		@brief start the server list
		@param the port to use
		@param the maximum amount of connections
	*/
	ServerList(unsigned short port = 5457, unsigned int maxConnections = 10);
	~ServerList();

	/*
		@brief Start the peer interface
	*/
	void HandleNetworkConnection();
	/*
		@brief Processes packets recieved by the peer interface
	*/
	void HandleNetworkMessages();
	/*
		@brief Runs HandleNetworkMessages on loop in a seperate thread
	*/
	void StartConnectionLoop();

private:
	RakNet::RakPeerInterface* m_peerInterface;
	unsigned short m_port;
	unsigned int m_maxConnections;

	std::vector<std::string> m_serverList;
	bool m_connectionLoop = true;

	std::thread m_networkLoop;
};

