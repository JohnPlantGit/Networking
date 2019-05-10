#include <iostream>
#include <string>
#include <thread>
#include <map>
#include <vector>
#include <sstream>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "GameMessages.h"
#include "Server.h"

int main()
{
	std::string input;
	std::getline(std::cin, input);

	unsigned short port = std::stoi(input);

	Server* server = new Server(port);
	server->StartConnectionLoop();

	while (true) { std::this_thread::sleep_for(std::chrono::seconds(10)); }

	return 0;
}