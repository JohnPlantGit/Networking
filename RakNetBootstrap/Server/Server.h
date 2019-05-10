#pragma once

#include <string>
#include <thread>
#include <vector>
#include <sstream>
#include <mutex>
#include <map>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameMessages.h"

class Server
{
public:
	Server(unsigned short hostPort = 5555, std::string serverListIp = "127.0.0.1", unsigned short serverListPort = 5457);
	~Server();

	void InitializeConnection();
	void HandleNetworkConnection();
	void HandleNetworkMessages();
	void StartConnectionLoop();

private:

	void ParseCommand(std::string input, RakNet::SystemAddress sa);
	void SetUserName(std::vector<std::string> parts, RakNet::SystemAddress sa);

	RakNet::RakPeerInterface* m_peerInterface;
	unsigned short m_hostPort;
	std::string m_serverListIp;
	unsigned short m_serverListPort;

	bool m_connected = false;

	std::thread m_networkLoop;
	bool m_connectionLoop = true;

	std::map<std::string, std::string> m_usernames;
	std::vector<std::string> m_commands =
	{
		"setusername"
	};
};

