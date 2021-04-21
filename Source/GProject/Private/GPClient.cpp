#include "GPClient.h"

//Thread Worker Starts as NULL, prior to being instanced
FGPClient* FGPClient::Runnable = nullptr;

FGPClient::FGPClient()
{
	//?에디터에서는 한번 생성되면 다음 변화가 있기 전까지 소멸하지 않음. static 인스턴스로 만들어서?
	Thread = FRunnableThread::Create(this, TEXT("FGPClient"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FGPClient::~FGPClient()
{
	GP_LOG_C(Warning);
	delete Thread; //FRunnableThreadWin::Kill(true)
	Thread = nullptr;
}

FGPClient* FGPClient::InitClient()
{
	if (!Runnable && FPlatformProcess::SupportsMultithreading())
	{
		Runnable = new FGPClient();
	}

	return Runnable;
}

bool FGPClient::Init()
{
	WSADATA wsaData;
	int iResult;

	//클라 wsa 시작.
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);//2.2 
	if (iResult)
	{
		UE_LOG(LogGProject, Warning, TEXT("WSAStartup failed, code: %d"), iResult);
		return false;
	}

	Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (Socket == INVALID_SOCKET)
	{
		UE_LOG(LogGProject, Warning, TEXT("Invalid socket, code : %d"), WSAGetLastError());
		return false;
	}

	ConnEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return true;
}

uint32 FGPClient::Run()
{
	GP_LOG_C(Warning);

	WaitForSingleObject(ConnEvent, INFINITE);

	int iResult;
	char recvbuf[MAX_PKT_SIZ];
	int recvbuflen = MAX_PKT_SIZ;

	do
	{
		iResult = recv(Socket, recvbuf, recvbuflen, 0);
		if (iResult == SOCKET_ERROR)
		{
			GP_LOG(Warning, TEXT("recv faild, code : %d"), WSAGetLastError());
			return 2;
		}
		else if (iResult == 0) //the connection has been gracefully closed.
		{
			GP_LOG(Warning, TEXT("recv is done. Session disconnected."));
			return 0;
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
				GP_LOG(Warning, TEXT("Message: %s"), *FString((char*)&pckt->data));
				break;

			default:
				GP_LOG(Warning, TEXT("Error Packet type: %d"), pckt->header.type);
			}

			size += pckt->header.size;
		}
	} while (iResult > 0); //
	
	return 0;
}

void FGPClient::Exit()
{
	closesocket(Socket);
	WSACleanup();
}

void FGPClient::Stop()
{
	GP_LOG_C(Warning);

	int iResult = shutdown(Socket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		GP_LOG(Warning, TEXT("shutdown failed: %d"), WSAGetLastError());
	}
	//shutdown되어 정상적으로 서버에서 0바이트를 수신하면 다시 0바이트를 보낼 것이므로 recv루프가 탈출 될 것.
}

//void FGPClient::EnsureCompletion()
//{
//	GP_LOG_C(Warning);
//	Stop();
//	Thread->WaitForCompletion();
//}

void FGPClient::Shutdown()
{
	if (Runnable)
	{
		//Runnable->EnsureCompletion();
		delete Runnable; 
		Runnable = nullptr;
	}
}

bool FGPClient::Login()
{
	//test LogIn
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;

	pckt->header.type = PT_USER_LOGIN;

	FString id = FString::FromInt(FMath::Rand());
	pckt->header.size = sizeof(PacketH) + id.Len() + 1;
	memcpy(&pckt->data, TCHAR_TO_ANSI(*id), id.Len() + 1);

	GP_LOG(Warning, TEXT("%s, %d"), *id, pckt->header.size);

	return Send(sendbuf, pckt->header.size);
}

bool FGPClient::Connect(u_short port, char* ip)
{
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(ip);

	if (connect(Socket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		GP_LOG(Warning, TEXT("connect failed, code : %d"), WSAGetLastError());
		return false;
	}

	GP_LOG(Display, TEXT("Connected to server."));

	SetEvent(ConnEvent);

	return true;
}

bool FGPClient::Send(char* buf, int len)
{
	int iResult = send(Socket, buf, len, 0);
	if (iResult == SOCKET_ERROR) {
		GP_LOG(Warning, TEXT("send failed, code : %d"), WSAGetLastError());
		return false;
	}
	return true;
}

bool FGPClient::SendChat(FString str)
{
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;

	pckt->header.size = sizeof(PacketH) + str.Len() + 1;
	pckt->header.type = PT_MSG;
	memcpy(&pckt->data, TCHAR_TO_ANSI(*str), str.Len() + 1);

	return Send(sendbuf, pckt->header.size);
}

