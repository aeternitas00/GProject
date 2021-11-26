#pragma once
#include <sstream> ///
#include <map>


enum GPPacketType : unsigned char { 
				PT_NONE,
				PT_TEST_ECHO, PT_MSG,
				PT_USER, 
				PT_USER_HOST, PT_USER_JOIN, PT_USER_READY,
				PT_USER_LOGIN, PT_USER_LOGOUT, PT_USER_SIGNUP, 
				PT_PLAYER, 
				PT_PLAYER_START, PT_PLAYER_UPDATE,
				PT_SERVER, 
				PT_SERVER_LOGIN_GOOD, PT_SERVER_LOGIN_FAIL, PT_SERVER_SINGUP_GOOD, PT_SERVER_SINGUP_FAIL,
				PT_BE_HOST,
				PT_GAME, 
				PT_GAME_START, PT_GAME_END,
				PT_GOOD, PT_MORE,
				PT_MAX
			  }; //	//todo using enum GPPacketType; //enum class c++20


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
		ss.write((char*)&data, header.size - sizeof(header));//todo endian
	}
};
#pragma pack(pop, gp)

/* serialize / deserialize
 * 이후에는 std의 스트림을 쓴다면 std::ios::binary 와 read() / write()를 쓰게 바꾸거나  
 * 현재 ClientConsole에서 테스트 중인 커스텀 GPBitWriter로 바꾸어야함.
*/


struct GPVector
{
	float X;
	float Y;
	float Z;

	friend auto& operator<<(std::ostream& os, const GPVector& v)
	{
		return os << v.X << ' ' << v.Y << ' ' << v.Z;
	}

	friend auto& operator>>(std::istream& is, GPVector& v)
	{
		return is >> v.X >> v.Y >> v.Z;
	}
};

struct GameObject
{
	typedef std::map<int, GameObject> GOMap;//

	GPVector Location;

	float Pitch;
	float Yaw;
	float Roll;

	GPVector Velocity;

	
	virtual ~GameObject() {}

	friend auto& operator<<(std::ostream& os, const GameObject& go)
	{
		return os << go.Location << ' '
			<< go.Pitch << ' ' << go.Yaw << ' ' << go.Roll << ' '
			<< go.Velocity;
	}

	friend auto& operator>>(std::istream& is, GameObject& go)
	{
		return is >> go.Location
			>> go.Pitch >> go.Yaw >> go.Roll
			>> go.Velocity;
	}

	friend auto& operator<<(std::ostream& os, const GOMap& gom)
	{
		for (const auto& pair : gom) {
			os << pair.first << ' ' << pair.second << '\n';
		}
		return os;
	}

	friend auto& operator>>(std::istream& is, GOMap& gom)
	{
		for (std::pair<int, GameObject>&& pair : gom) {
			is >> pair.first >> pair.second;
		}
		return is;
	}
};


#define GP_PORT 9000
#define GP_AUTH GP_PORT + 1
#define GP_MATCH GP_PORT + 2