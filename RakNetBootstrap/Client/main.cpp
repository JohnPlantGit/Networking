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
#include "Client.h"

int main()
{
	std::this_thread::sleep_for(std::chrono::seconds(2));

	Client* client = new Client();
	client->StartConnectionLoop();

	while (true) { std::this_thread::sleep_for(std::chrono::seconds(10)); }

	return 0;
}
