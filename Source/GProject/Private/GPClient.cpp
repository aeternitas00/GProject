#include "GPClient.h"
#include "GProjectPlayerController.h"
//#include "GProjectGameMode.h"

//Thread Worker Starts as NULL, prior to being instanced
FGPClient* FGPClient::Runnable = nullptr;

FGPClient::FGPClient()
{
	PlayerCon = nullptr;
	//GameMode = nullptr;

	//?�����Ϳ����� �ѹ� �����Ǹ� ���� ��ȭ�� �ֱ� ������ �Ҹ����� ����. static �ν��Ͻ��� ����?
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

	//Ŭ�� wsa ����.
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

	//�������� ������ �˸��� �ڵ� ���� �̺�Ʈ ����.
	ConnEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return true;
}

uint32 FGPClient::Run()
{
	GP_LOG_C(Warning);

	//�������� ������ ��ٸ�.
	WaitForSingleObject(ConnEvent, INFINITE);

	int iResult; //recv ��� ����Ʈ.

	do
	{
		iResult = recv(Socket, RecvBuf, sizeof(RecvBuf), 0);

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
		while (size < iResult) //���۰� ���� ���� ������ �ݺ� ó�� �õ�.
		{
			GP_LOG(Warning, TEXT("Bytes: %d"), iResult);
			Packet* pckt = (Packet*)(RecvBuf + size);
			GPPacketType pt = pckt->header.type;

			switch (pt)
			{
			case PT_MSG:
			case PT_USER_LOGIN:
			case PT_USER_LOGOUT:
				//ANSI_TO_TCHAR((char*)&pckt->data);
				//FText::AsCultureInvariant(msg);
				FString msg((char*)&pckt->data);
				GP_LOG(Warning, TEXT("Message: %s"), *msg);
				if (PlayerCon)
				{
					//GP_LOG(Warning, TEXT("%x"), PlayerCon);
					PlayerCon->AddChat(msg);
				}
				break;

			/*default:
				GP_LOG(Warning, TEXT("Error Packet type: %d"), pckt->header.type);*/
			}

			size += pckt->header.size;
		}
	} while (iResult > 0); // recv�� ���������� ó���Ǹ� �ݺ�.
	
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

	if (shutdown(Socket, SD_SEND) == SOCKET_ERROR) {
		GP_LOG(Warning, TEXT("shutdown failed: %d"), WSAGetLastError());
	}
	//shutdown�Ǿ� ���������� �������� 0����Ʈ�� �����ϸ� �ٽ� 0����Ʈ�� ���� ���̹Ƿ� recv������ Ż�� �� ��.

	SetEvent(ConnEvent); //���� ������ ���� ���� ������ �� Run�� ��⸦ Ǯ����. 
}

void FGPClient::Shutdown()
{
	if (Runnable)
	{
		delete Runnable; 
		Runnable = nullptr;
	}
}

bool FGPClient::Connect(u_short port, char* ip)
{
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(ip);

	if (connect(Socket, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		int err = WSAGetLastError(); 
		if (err == WSAEISCONN) return true; //already connected.

		GP_LOG(Warning, TEXT("connect failed, code : %d"), err);
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

bool FGPClient::SendChat(FString Chat)
{
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;

	pckt->header.size = sizeof(PacketH) + Chat.Len() + 1;
	pckt->header.type = PT_MSG;
	memcpy(&pckt->data, TCHAR_TO_ANSI(*Chat), Chat.Len() + 1);

	return Send(sendbuf, pckt->header.size);
}

bool FGPClient::Login()
{
	//test Log in
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;

	pckt->header.type = PT_USER_LOGIN;

	sendbuf[sizeof(PacketH)] = rand() % 9 + '1';
	sendbuf[sizeof(PacketH) + 1] = 0;

	pckt->header.size = sizeof(PacketH) + 2;

	GP_LOG(Warning, TEXT("Test ID: %s, Bytes: %d"), ANSI_TO_TCHAR((char*)&pckt->data), pckt->header.size);

	return Send(sendbuf, pckt->header.size);
}