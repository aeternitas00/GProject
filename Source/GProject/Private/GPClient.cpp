#include "GPClient.h"

//Thread Worker Starts as NULL, prior to being instanced
FGPClient* FGPClient::Runnable = NULL;

FGPClient::FGPClient()
{

	Thread = FRunnableThread::Create(this, TEXT("FGPClient"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FGPClient::~FGPClient()
{
	delete Thread;
	Thread = NULL;
}

bool FGPClient::Init()
{
	WSADATA wsaData;
	int iResult;

	//클라 wsa 시작.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//2.2 
	if (iResult)
	{
		UE_LOG(LogGProject, Warning, TEXT("WSAStartup failed, code: %d"), WSAGetLastError());
		return false;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		UE_LOG(LogGProject, Warning, TEXT("Invalid socket, code : %d"), WSAGetLastError());
		return false;
	}

	//UE_LOG(LogGProject, Warning, TEXT("WSAStartup!"));

	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(9000);
	sa.sin_addr.s_addr = inet_addr("127.0.0.1");

	//connect to server
	if (connect(Socket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		UE_LOG(LogGProject, Warning, TEXT("connect failed, code : %d"), WSAGetLastError());
		closesocket(Socket);
		return false;
	}

	UE_LOG(LogGProject, Warning, TEXT("Connected to server."));

	return true;
}

uint32 FGPClient::Run()
{
	//GetQueuedCompletionStatus()

	return 0;
}

void FGPClient::Stop()
{
	closesocket(Socket);
	WSACleanup();
}
