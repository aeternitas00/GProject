#include "GPClient.h"
#include "../GPServer/common/GPPacket.h"//

//Thread Worker Starts as NULL, prior to being instanced
FGPClient* FGPClient::Runnable = nullptr;

FGPClient::FGPClient()
{
	//생성되면 Init()을 호출함 //?에디터에서는 한번 생성되면 다음 변화가 있기 전까지 소멸하지 않음.
	Thread = FRunnableThread::Create(this, TEXT("FGPClient"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FGPClient::~FGPClient()
{
	GP_LOG_C(Warning);
	delete Thread; //이 때 FRunnable::Stop을 호출하는 것으로 보임.
	GP_LOG_C(Warning);
	Thread = nullptr;
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
		return false;
	}

	UE_LOG(LogGProject, Warning, TEXT("Connected to server."));

	//test
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;
	pckt->header.type = PT_MSG;

	char test[] = "Hello, world!";
	pckt->header.size = sizeof(PacketH) + sizeof(test);
	memcpy(&pckt->data, test, sizeof(test));

	iResult = send(Socket, sendbuf, pckt->header.size, 0);
	if (iResult == SOCKET_ERROR) {
		GP_LOG(Warning, TEXT("send failed, code : %d"), WSAGetLastError());
		return false;
	}
	return true; //FRunnable::Create 호출로 Init이 호출 된 경우 true를 리턴하면 스레드에서 Run이 실행되는 것으로 보임.
}

uint32 FGPClient::Run()
{
	//GetQueuedCompletionStatus()
	GP_LOG_C(Warning);
	

	int iResult;
	char recvbuf[MAX_PKT_SIZ];
	int recvbuflen = MAX_PKT_SIZ;

	do
	{
		iResult = recv(Socket, recvbuf, recvbuflen, 0);
		if (iResult == SOCKET_ERROR)
		{
			GP_LOG(Warning, TEXT("recv faild, code : %d"), WSAGetLastError());
			return false;
		}
		else if (iResult == 0) //the connection has been gracefully closed.
		{
			GP_LOG(Warning, TEXT("recv is done. Session disconnected."));
			return true;
		}
		int size = 0;
		while (size < iResult) //버퍼가 많을 수도 있으니 반복 처리 시도.
		{
			GP_LOG(Warning, TEXT("Bytes: %d"), iResult);
			Packet* pckt = (Packet*)(recvbuf + size);
			GPPacketType pt = pckt->header.type;

			switch (pt)
			{
			case PT_MSG:
			case PT_USER_LOGIN:
			case PT_USER_LOGOUT:
				GP_LOG(Warning, TEXT("Message: %s"), *FString((char*)&pckt->data)/**FString(ANSI_TO_TCHAR(recvbuf+sizeof(PacketH))*//**/);
				break;

			default:
				GP_LOG(Warning, TEXT("Error Packet type: %d"), pckt->header.type);
			}

			size += pckt->header.size;
		}
	} while (iResult > 0);
	
	return 0;
}

void FGPClient::Stop()
{
	GP_LOG_C(Warning);
}

void FGPClient::EnsureCompletion()
{
	GP_LOG_C(Warning);
	Stop();
	Thread->WaitForCompletion();
}

void FGPClient::Shutdown()
{
	closesocket(Socket);
	WSACleanup();

	if (Runnable)
	{
		Runnable->EnsureCompletion();
		delete Runnable; 
		Runnable = nullptr;
	}
}

FGPClient* FGPClient::InitClient()
{
/*	if (Runnable)
	{
		Runnable->Init();
	}
	else */if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FGPClient(); 
	}

	return Runnable;
}
