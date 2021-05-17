#pragma once

enum GPPacketType : unsigned char { PT_TEST_ECHO,
				PT_USER = 10, PT_MSG, PT_USER_LOGIN, PT_USER_LOGOUT, PT_USER_READY,
				PT_PLAYER = 20, PT_PLAYER_START, PT_PLAYER_UPDATE,
				PT_SERVER = 30, PT_BE_HOST,
				PT_GAME_START = 40, PT_GAME_END,
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

#define MAX_ID_LEN  24
#define MAX_DAT_SIZ 1024
#define MAX_PKT_SIZ sizeof(PacketH) + MAX_DAT_SIZ

#define GP_PORT 9000
