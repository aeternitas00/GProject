#pragma once

#include "CriticalSection.h"
#include <set>

class ClientSession;

class GPServerManager
{
public:
	~GPServerManager();

	bool InitServer();
	void CloseServer();

	bool AcceptLoop();

	ClientSession* CreateClient(SOCKET sock);
	void DeleteClient(ClientSession* client);

	void SendToAll(const char* buf, int buflen);

	static GPServerManager* getSingleton();


private:
	GPServerManager();

	typedef set<ClientSession*> Clients;
	Clients mClients;

	//클라이언트 삭제 및 추가 시 동기화를 위한 임계영역
	CriticalSection mClientsCS;


	//클라이언트가 보낸 패킷에 따른 반응 함수들 

	void OnChat(ClientSession* client);
	void OnLogin(ClientSession* client);
	void OnLogout(ClientSession* client);

	////

	//IOCP 테스트 스레드
	static DWORD WINAPI IocpSockRecvProc(PVOID pParam);

#define NUM_THREAD 2
	SOCKET mListenSocket;
	HANDLE mIocp;
	HANDLE mThreads[NUM_THREAD];
};