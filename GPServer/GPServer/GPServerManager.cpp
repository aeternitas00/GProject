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

//
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
		cout << "WSAStartup failed, error : " << iResult << endl;
		return false;
	}

	//���� ���� ����.
	mListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (mListenSocket == INVALID_SOCKET)
	{
		cout << "Invalid socket, error : " << WSAGetLastError() << endl;		
		return false;
	}

	//���� �ּ� ����.
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(GP_PORT);
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
	closesocket(mListenSocket);
	cout << "Server closed." << endl;

	WSACleanup();
}


//
bool GPServerManager::AcceptLoop()
{
	while (true) //ctrl?
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
	CSLock lock(mClientsCS);

	client->Disconnect();

	//create disconnection message
	static const char msg[] = ", disconnectd.";
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;
	pckt->header.type = PT_MSG;

	int i = sizeof(PacketH);
	int len = strlen(client->mID);
	memcpy(sendbuf + i, client->mID, len);
	i += len;
	memcpy(sendbuf + i, msg, sizeof(msg));

	pckt->header.size = i + sizeof(msg);

	mClients.erase(client);
	//cout << "ClientSession removed." << endl;

	SendToAll(sendbuf, pckt->header.size);
}

void GPServerManager::SendToAll(const char* buf, int buflen)
{
	for (auto iter = mClients.begin(); iter != mClients.end(); iter++)
	{
		auto client = *iter;
		client->Send(buf, buflen);
	}
}

void GPServerManager::OnChat(ClientSession* client)
{
	Packet* pckt = (Packet*)client->mBuf;

	//
	stringstream ss;
	ss << client->mID << ": " << (char*)&pckt->data << endl;
	pckt->header.size = sizeof(PacketH) + ss.str().length();//
	ss.getline((char*)&pckt->data, MAX_DAT_SIZ);
	cout << (char*)&pckt->data << endl;

	SendToAll(client->mBuf, pckt->header.size);
}

void GPServerManager::OnLogin(ClientSession* client)
{
	Packet* pckt = (Packet*)client->mBuf;

	static const char msg[] = ", logged in.";

	//set client ID //todo func, length check
	char* id = (char*)&pckt->data;
	cout << id << msg << endl;
	strcpy_s(client->mID, id);

	//char sendbuf[MAX_PKT_SIZ];
	//memcpy(sendbuf, pckt, pckt->header.size);
	//pckt = (Packet*)sendbuf;

	//add connection msg
	//char* pData = (char*)&pckt->data;
	strcat_s(id, MAX_DAT_SIZ, msg);
	//memcpy(sendbuf + pckt->header.size - 1, msg, strlen(msg) + 1);// +1 for '\0'

	pckt->header.size += sizeof(msg); //-1?
	pckt->header.type = PT_MSG;

	SendToAll(client->mBuf, pckt->header.size);
}

void GPServerManager::OnLogout(ClientSession* client)
{
	//cout << client->mID << ", disconnecting." << endl;

	DeleteClient(client); //
}

DWORD WINAPI GPServerManager::IocpSockRecvProc(PVOID pParam)
{
	GPServerManager* mngr = (GPServerManager*) pParam;

	DWORD dwErr = ERROR_SUCCESS;

	DWORD		dwTrBytes = 0;
	ULONG_PTR	upDevKey = 0;
	ClientSession* pCl = nullptr;

	while (true)
	{
		BOOL bIsOK = GetQueuedCompletionStatus
		(
			mngr->mIocp, &dwTrBytes, &upDevKey, (LPOVERLAPPED*) &pCl, INFINITE
		);
		if (bIsOK == FALSE)
		{
			dwErr = GetLastError();

			if (dwErr != ERROR_NETNAME_DELETED)
			{
				cout << "GQCS error: " << dwErr << endl;
				break;
			}
		}

		//���� �ٸ� Ű�� ����ϰ� ���� �ʾƼ� ���� ����.
		/*if (upDevKey == IOKEY_LISTEN)
		{
			continue;
		}*/

		//���� ����. IOKEY_CHILD
		if (dwTrBytes != 0) 
		{
			switch (pCl->ParsePacket())
			{
			case PT_MSG:
				mngr->OnChat(pCl);
				break;
			case PT_USER_LOGIN:
				mngr->OnLogin(pCl);
			}

			//�ٽ� ���� ����
			if (!pCl->Recv())
				break;//���� �� ����

			continue;
		}
		
		//Ŭ�� ���� ����(dwTrBytes == 0) or ����(dwErr == ERROR_NETNAME_DELETED)//
		mngr->DeleteClient(pCl);	
	}

	return 0;
}



