#pragma once

#include "MessageIdentifiers.h"

enum GameMessages
{
	ID_SERVER_TEXT_MESSAGE = ID_USER_PACKET_ENUM + 1,
	ID_CHAT_TEXT_MESSAGE,
	ID_SERVER_LIST_REGISTER,
	ID_CLIENT_LIST_REQUEST,
	ID_CLIENT_LIST_RESPONSE
};