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

// Library
#ifdef STATIC
#define LIBRARY
#else
#ifdef DLL
#define LIBRARY __declspec(dllexport)
#else
#define LIBRARY __declspec(dllimport)
#endif // DLL
#endif // STATIC
//

class Client
{
public:
	LIBRARY Client(std::string ip = "127.0.0.1", unsigned short port = 5457);
	LIBRARY ~Client();
	 
	LIBRARY void InitializeConnection();
	LIBRARY void HandleNetworkConnection();
	LIBRARY void HandleNetworkMessages();
	 
	LIBRARY void StartConnectionLoop();
	LIBRARY void StopConnectionLoop();
	 
	LIBRARY void RequestServerList();
	LIBRARY bool ServerListUpdate() { return m_updateServerList; }
	LIBRARY std::vector<std::string> GetServerList();

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
