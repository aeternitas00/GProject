#include "stdafx.h"
#include "GPServerManager.h"
#include "ClientSession.h"

GPServerManager::GPServerManager() : mListenSocket(NULL), mIocp(NULL)
{
}

GPServerManager::~GPServerManager()
{
	mClients.clear();//
}

GPServerManager* GPServerManager::getSingleton()
{
	static GPServerManager* mtsm = new GPServerManager();

	return mtsm;
}

#define IOKEY_LISTEN	1
#define IOKEY_CHILD		2

//WSAStartup ~ listen
//���� api ���� �� closesocket�� �ʿ��� �� ���⼭ ���ִ°� �������ϼ��� �ִ�.
bool GPServerManager::InitServer()
{
	WSADATA wsaData;
	int iResult;	

	//���� wsa ����.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//2.2 
	if (iResult)
	{
		cout << "WSAStartup failed, Error : " << WSAGetLastError() <<endl;
		return false;
	}

	//���� ���� ����.
	mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mListenSocket == INVALID_SOCKET)
	{
		cout << "Invalid socket, Error : " << WSAGetLastError() << endl;		
		return false;
	}

	//���� �ּ� ����.
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(GP_PORT); //
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //TODO ip

	//���� ������ �ּҿ� ����.
	iResult = bind(mListenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (iResult)
	{
		cout << "bind failed : " << WSAGetLastError() << endl;
		return false;
	}

	//������ ����.
	iResult = listen(mListenSocket, SOMAXCONN); //TODO tweak backlog?
	if (iResult)
	{
		cout << "listen failed : " << WSAGetLastError() << endl;
		return false;
	}

	cout << "Server initialized, listening clients..." << endl;

	mIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//�׽�Ʈ IOCP ������ 
	for (int i = 0; i < NUM_THREAD; i++)
	{
		DWORD dwThreadId;
		mThreads[i] = CreateThread(NULL, 0, IocpSockRecvProc, this, 0, &dwThreadId);
	}


	return true;
}


void GPServerManager::CloseServer()
{
	for (Clients::iterator it = mClients.begin(); it != mClients.end(); it++)
	{
		ClientSession* cl = *it;
		closesocket(cl->mSocket);
		delete cl;
	}
	closesocket(mListenSocket);//
	cout << "Server closed." << endl;

	WSACleanup();
}


//
bool GPServerManager::AcceptLoop()
{
	while (true)
	{
		//���� ���� ����.
		SOCKET acceptSock = accept(mListenSocket, NULL, NULL);
		if (acceptSock == INVALID_SOCKET)
		{
			cout << "accept error : Invalid socket, code : " << WSAGetLastError() << endl;
			continue;
		}

		//ClientSession�� �����ϰ� Recv ����. IOCP �����忡���� �������� �Ϸ� ������ ������ �ٽ� Recv�� �ؼ� �ݺ�. 
		ClientSession * client = CreateClient(acceptSock);
		client->Recv();
	}
	//ctrl?
	
	//Cleanup

	CloseHandle(mIocp);
	WaitForMultipleObjects(NUM_THREAD, mThreads, TRUE, INFINITE);
	for (int i = 0; i < NUM_THREAD; i++)
	{
		CloseHandle(mThreads[i]);
	}

	CloseServer();

	return true;
}


ClientSession* GPServerManager::CreateClient(SOCKET sock)
{
	ClientSession* client = new ClientSession(sock);

	if (client != nullptr)
	{
		CreateIoCompletionPort((HANDLE)sock, mIocp, IOKEY_CHILD, 0);//
		CSLock lock(mClientsCS);
		mClients.insert(client);
		cout << "ClientSession created." << endl;
	}

	return client;
}

void GPServerManager::DeleteClient(ClientSession* client)
{
	CSLock lock(mClientsCS);//
	for (auto iter = mClients.begin(); iter != mClients.end(); ) {
		if (*iter == client) {
			//
			client->Disconnect();
			
			iter = mClients.erase(iter);
			cout << "ClientSession removed." << endl;
		}
		else {
			iter++;
		}
	}
}

void GPServerManager::SendToAll(const char* buf, int buflen)
{
	for (auto iter = mClients.begin(); iter != mClients.end(); iter++)
	{
		auto client = *iter;
		client->Send(buf, buflen);
	}
}

DWORD WINAPI GPServerManager::IocpSockRecvProc(PVOID pParam)
{
	GPServerManager* mgr = (GPServerManager*) pParam;//

	DWORD		dwTrBytes = 0;
	ULONG_PTR	upDevKey = 0;
	ClientSession* pcl = nullptr;

	while (true)
	{

		BOOL bIsOK = GetQueuedCompletionStatus
		(
			mgr->mIocp, &dwTrBytes, &upDevKey, (LPOVERLAPPED*) &pcl, INFINITE
		);
		if (bIsOK == FALSE)
		{
			//
			cout << "GQCS error: " << WSAGetLastError() << endl;
			break;
		}

		//
		if (upDevKey == IOKEY_LISTEN)
		{
			continue;
		}
		else //���� ����. CHILD
		{
			if (dwTrBytes == 0)//Ŭ�� ���� ����
				mgr->DeleteClient(pcl);

			// ���� �޽��� ���� �۽� �׽�Ʈ
			cout << "Bytes : " << dwTrBytes << endl;
			//pcl->mBuf[dwTrBytes] = 0;
			mgr->SendToAll(pcl->mBuf, dwTrBytes);
		}

		//���� ����
		if (!pcl->Recv())
			break;
	
	}

	return 0;
}



