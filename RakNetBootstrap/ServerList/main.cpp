#include <vector>
#include <string>

#include <RakPeerInterface.h>
#include <MessageIdentifiers.h>
#include <BitStream.h>

#include "../Server/GameMessages.h"
#include "ServerList.h"

int main()
{
	ServerList* serverList = new ServerList();
	serverList->StartConnectionLoop();

	while (true) { std::this_thread::sleep_for(std::chrono::seconds(10)); }

	return 0;
}