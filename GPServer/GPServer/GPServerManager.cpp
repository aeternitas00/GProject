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
//소켓 api 실패 후 closesocket을 필요할 때 여기서 해주는게 직관적일수도 있다.
bool GPServerManager::InitServer()
{
	WSADATA wsaData;
	int iResult;	

	//서버 wsa 시작.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//2.2 
	if (iResult)
	{
		cout << "WSAStartup failed, Error : " << WSAGetLastError() <<endl;
		return false;
	}

	//서버 소켓 생성.
	mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mListenSocket == INVALID_SOCKET)
	{
		cout << "Invalid socket, Error : " << WSAGetLastError() << endl;		
		return false;
	}

	//서버 주소 정보.
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(GP_PORT); //
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //TODO ip

	//서버 소켓을 주소와 연결.
	iResult = bind(mListenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (iResult)
	{
		cout << "bind failed : " << WSAGetLastError() << endl;
		return false;
	}

	//응답대기 시작.
	iResult = listen(mListenSocket, SOMAXCONN); //TODO tweak backlog?
	if (iResult)
	{
		cout << "listen failed : " << WSAGetLastError() << endl;
		return false;
	}

	cout << "Server initialized, listening clients..." << endl;

	mIocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	//테스트 IOCP 스레드 
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
		//접속 소켓 생성.
		SOCKET acceptSock = accept(mListenSocket, NULL, NULL);
		if (acceptSock == INVALID_SOCKET)
		{
			cout << "accept error : Invalid socket, code : " << WSAGetLastError() << endl;
			continue;
		}

		//ClientSession을 생성하고 Recv 시작. IOCP 스레드에서는 정상적인 완료 통지를 받으면 다시 Recv를 해서 반복. 
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
		else //수신 받음. CHILD
		{
			if (dwTrBytes == 0)//클라 소켓 닫힘
				mgr->DeleteClient(pcl);

			// 수신 메시지 에코 송신 테스트
			cout << "Bytes : " << dwTrBytes << endl;
			//pcl->mBuf[dwTrBytes] = 0;
			mgr->SendToAll(pcl->mBuf, dwTrBytes);
		}

		//수신 시작
		if (!pcl->Recv())
			break;
	
	}

	return 0;
}



