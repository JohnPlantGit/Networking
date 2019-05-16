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
	/*
		@brief Stores the ip and port and runs the HandleNetworkConnection Function
		@param The ip to connect to
		@param The port to connect to
	*/
	LIBRARY Client(std::string ip = "127.0.0.1", unsigned short port = 5457);
	/*
		@brief Shuts down the connection loop and destroys the peer interface
	*/
	LIBRARY ~Client();

	/*
		@brief Starts up the peer interface before calling the InitializeConnection function
	*/
	LIBRARY void HandleNetworkConnection();
	/*
		@brief Connects the peer interface to the stored ip and port
	*/
	LIBRARY void InitializeConnection();
	/*
		@brief Processes packets recieved by the peer interface
	*/
	LIBRARY void HandleNetworkMessages();
	 
	/*
		@brief Runs the function HandleNetworkMessages on loop in a seperate thread
	*/
	LIBRARY void StartConnectionLoop();
	/*
		@brief Stops the connection loop
	*/
	LIBRARY void StopConnectionLoop();
	 
	/*
		@brief Sends a packet requesting the server list
	*/
	LIBRARY void RequestServerList();
	/*
		@brief Checks if the server list has updated
		@return if the server list has updated
	*/
	LIBRARY bool ServerListUpdate() { return m_updateServerList; }
	/*
		@brief Gets the server list and sets it's update status to false
		@return The vector containing the server list
	*/
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
