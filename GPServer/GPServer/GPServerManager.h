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

	//Ŭ���̾�Ʈ ���� �� �߰� �� ����ȭ�� ���� �Ӱ迵��
	CriticalSection mClientsCS;

	//IOCP �׽�Ʈ ������
	static DWORD WINAPI IocpSockRecvProc(PVOID pParam);

#define NUM_THREAD 2
	SOCKET mListenSocket;
	HANDLE mIocp;
	HANDLE mThreads[NUM_THREAD];
};