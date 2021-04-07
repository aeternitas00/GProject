#pragma once
#include "GPPacket.h"

class GPServerManager;

//accepted client session
class ClientSession : OVERLAPPED
{
public:
	
	~ClientSession() {}

	bool IsConnected() const { return mConnected; }

	bool Send(const char* buf, int len); // 동기식. @ WSASend 비동기로 쓰면 IOCP스레드에서 완료 통지를 계속 받아 반복될 것이기 때문에 일단 쓰지 않음.
	//bool Send(); //TODO 현재 버프(mBuf)로 Send 하려면 동기화 문제 때문에 Recv/Send 별개의 buf 및 bytes 변수들이 필요할 것.

	bool Recv(); //비동기식. TODO 예외 처리

	bool ParsePacket();

	void Disconnect();

	SOCKADDR_IN getAddr() const { return mAddr; }

private:
	//only GPServerManager can manage ClientSession.
	friend class GPServerManager;
	
	ClientSession(SOCKET asock);

	bool OnConnect();
	void OnChat();
	void OnLogin();
	void OnLogout();

	bool			mConnected;
	SOCKET			mSocket; //Accepted Socket.
	SOCKADDR_IN		mAddr;
	char 			mID[MAX_DAT_SIZ]; //Client's name

	char 			mBuf[MAX_PKT_SIZ]; // Recv/Send buf. // 
};
