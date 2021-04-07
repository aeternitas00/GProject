#pragma once
#include "GPPacket.h"

class GPServerManager;

//accepted client session
class ClientSession : OVERLAPPED
{
public:
	
	~ClientSession() {}

	bool IsConnected() const { return mConnected; }

	bool Send(const char* buf, int len); // �����. @ WSASend �񵿱�� ���� IOCP�����忡�� �Ϸ� ������ ��� �޾� �ݺ��� ���̱� ������ �ϴ� ���� ����.
	//bool Send(); //TODO ���� ����(mBuf)�� Send �Ϸ��� ����ȭ ���� ������ Recv/Send ������ buf �� bytes �������� �ʿ��� ��.

	bool Recv(); //�񵿱��. TODO ���� ó��

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
