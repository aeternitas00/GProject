#pragma once

enum GPPacketType : unsigned char { PT_MSG, 
				PT_USER = 10, PT_USER_LOGIN, PT_USER_LOGOUT,
				PT_TEST_ECHO = 20,
				PT_GAME_START = 30, PT_GAME_END,
				PT_PLAYER_START = 40, PT_PLAYER_UPDATE,
				PT_MAX
			  }; //

typedef struct PAKET_HEADER
{
	unsigned short	size;
	GPPacketType	type;
}PacketH;

typedef struct PACKET
{
	PacketH	header;
	void*	data;
}Packet;

#define MAX_ID_SIZ  24
#define MAX_DAT_SIZ 1024
#define MAX_PKT_SIZ sizeof(PacketH) + MAX_DAT_SIZ

#define GP_PORT 9000
