#pragma once
#include <sstream> ///

//todo namespace?

enum GPPacketType : unsigned char { 
				PT_NONE,
				PT_TEST_ECHO, PT_MSG,
				PT_USER, 
				PT_USER_LOGIN, PT_USER_LOGOUT, PT_USER_SIGNUP, PT_USER_READY,
				PT_PLAYER, 
				PT_PLAYER_START, PT_PLAYER_UPDATE,
				PT_SERVER, 
				PT_SERVER_LOGIN_GOOD, PT_SERVER_LOGIN_FAIL, PT_SERVER_SINGUP_GOOD, PT_SERVER_SINGUP_FAIL,
				PT_BE_HOST,
				PT_GAME, 
				PT_GAME_START, PT_GAME_END,
				PT_GOOD,
				PT_MAX
			  }; //	//using enum GPPacketType; //enum class c++20

#pragma pack(push, gp, 1)
struct PacketH
{
	unsigned short	size;
	GPPacketType	type;
};

#define MAX_ID_LEN  24
#define MAX_DAT_SIZ 1024 * 2
#define MAX_PKT_SIZ sizeof(PacketH) + MAX_DAT_SIZ

struct Packet
{
	PacketH	header;
	void*	data;

	void WriteData(std::stringstream& ss) {
		ss.write((char*)&data, header.size - sizeof(header));//
	}
};
#pragma pack(pop, gp)

//struct GPSerializable
//{
//	virtual bool Serialize() = 0;
//	virtual bool DeSerialize() = 0;
//};

struct GameObject
{
	float X;
	float Y;
	float Z;

	float Yaw;
	float Pitch;
	float Roll;

	GameObject(float InX = 0.f, float InY = 0.f, float InZ = 0.f,
			   float InYaw = 0.f, float InPitch = 0.f, float InRoll = 0.f) 
			 : X(InX), Y(InY), Z(InZ), Yaw(InYaw), Pitch(InPitch), Roll(InRoll) {}
	virtual ~GameObject() {}

	friend auto& operator<<(std::ostream& os, const GameObject& go)
	{
		return os
			<< go.X << " " << go.Y << " " << go.Z << " \n"
			<< go.Yaw << " " << go.Pitch << " " << go.Roll << "\n";
	}

	friend auto& operator>>(std::istream& is, GameObject& go)
	{
		return is
			>> go.X >> go.Y >> go.Z
			>> go.Yaw >> go.Pitch >> go.Roll;
	}
};


//template<typename Stream>
//bool Serialize(Stream& stream, GameObject& go)
//{
//	return stream << go;
//}
//
//template<typename Stream>
//bool Deserialize(Stream& stream, GameObject& go)
//{
//	return stream >> go;
//}

#define GP_PORT 9000
#define GP_AUTH GP_PORT + 1
#define GP_MATCH GP_PORT + 2