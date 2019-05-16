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
	/*
		@brief creates the server and starts up the raknet peer. Reads the saved usernames from a file
		@param The port to host with
		@param The ip of the server list
		@param The port of the server list
	*/
	Server(unsigned short hostPort = 5555, std::string serverListIp = "127.0.0.1", unsigned short serverListPort = 5457);
	~Server();

	/*
		@brief Connects the peer interface to the server list and sends the packet to register the server
	*/
	void InitializeConnection();
	/*
		@brief starts up the peer interface
	*/
	void HandleNetworkConnection();
	/*
		@brief Processes packets recieved by the peer interface
	*/
	void HandleNetworkMessages();
	/*
		@brief runs HandleNetworkMessages in a loop on a seperate thread
	*/
	void StartConnectionLoop();

private:

	/*
		@brief Parses the input against the list of commands
		@param The input string
		@param The system address the input came from
	*/
	void ParseCommand(std::string input, RakNet::SystemAddress sa);
	/*
		@brief Sets the username of a specific system address
		@param The input string
		@param The system address the input came from
	*/
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

