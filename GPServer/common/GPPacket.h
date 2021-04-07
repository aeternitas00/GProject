#pragma once

enum GPPacketType { PT_NONE = -1, PT_MSG, 
				PT_USER = 10, PT_USER_LOGIN, PT_USER_LOGOUT,
				PT_SERVER = 20
			  }; //

typedef struct PAKET_HEADER
{
	int				size;
	GPPacketType	type;
}PacketH;


typedef struct PACKET
{
	PacketH	header;
	void*	data;
}Packet;

#define MAX_DAT_SIZ 256
#define MAX_PKT_SIZ sizeof(PacketH) + MAX_DAT_SIZ

#define GP_PORT 9000
