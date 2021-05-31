#include "GPClient.h"
#include "GPGameInstanceBase.h"
#include "GProjectPlayerController.h"
#include "GProjectGameMode.h"
#include "GPCharacterBase.h"
#include <sstream>

class FGPSendTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FGPSendTask>;

	std::stringstream StringStream;
	GPPacketType PacketType;

	FGPSendTask(std::stringstream& InStream, GPPacketType InPacketType)
	: StringStream(std::move(InStream)), PacketType(InPacketType)
	{
	}

	void DoWork()
	{
		FGPClient::GetGPClient()->SendStream(StringStream, PacketType);
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGPSendTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

//Thread Worker Starts as NULL, prior to being instanced
FGPClient* FGPClient::Runnable = nullptr;

FGPClient::FGPClient()
{
	PlayerCon = nullptr;
	GameMode = nullptr;
	Game = nullptr;

	Thread = FRunnableThread::Create(this, TEXT("FGPClient"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FGPClient::~FGPClient()
{
	GP_LOG_C(Warning);
	delete Thread; //FRunnableThreadWin::Kill(true)
	Thread = nullptr;
	closesocket(Socket);
	WSACleanup();
}

FGPClient* FGPClient::GetGPClient()
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

	//서버와의 연결을 알리는 자동 리셋 이벤트 생성.
	//ConnEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return Connect(); //Init 자체가 Connect가 없다면 필요없다고 판단하여 불필요한 Connect대기를 없애기 위해 여기서 Connect.
}

uint32 FGPClient::Run()
{
	GP_LOG_C(Warning);

	//서버와의 연결을 기다림.
	//WaitForSingleObject(ConnEvent, INFINITE);

	int iResult; //recv 결과 바이트.

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
		while (size < iResult) //버퍼가 많을 수도 있으니 반복 처리 시도.
		{
			GP_LOG(Warning, TEXT("Bytes: %d"), iResult);
			Packet* pckt = (Packet*)(RecvBuf + size);
			GPPacketType pt = pckt->header.type;

			switch (pt)
			{
			case PT_MSG:
			case PT_USER_LOGIN:
			case PT_USER_LOGOUT:
				//FText::AsCultureInvariant(msg);
			{
				FString msg(RecvBuf + size + sizeof(PacketH));
				GP_LOG(Display, TEXT("Message: %s"), *msg);
				if (PlayerCon)
				{
					AsyncTask(ENamedThreads::GameThread, [this, msg]()
						{
							PlayerCon->AddChat(msg);
						});
				}
			}
				break;
			case PT_BE_HOST:
				if (Game)
				{
					Game->BeGPHost();
				}
				break;
			case PT_PLAYER_START:
				if (GameMode && PlayerCon)
				{
					AsyncTask(ENamedThreads::GameThread, [this]()
						{
							Game->bGPStartPlayer = true;
							GameMode->RestartPlayer(PlayerCon);//test
							//Game->GPGameObjects.Add(PlayerCon->GetPawn());
						});
				}
				break;
			case PT_PLAYER_UPDATE: //todo more useful data
				if (Game)
				{
					std::stringstream ss(RecvBuf + size + sizeof(PacketH));
					int idx = 0; 
					ss >> idx;
					float x, y, z, yaw, pitch, roll;
					ss >> x >> y >> z >> yaw >> pitch >> roll;
					if (!Game->GPGameObjects.IsValidIndex(idx))//
					{
						AsyncTask(ENamedThreads::GameThread, [this, idx, x, y, z, yaw, pitch, roll]()
							{
								AGPCharacterBase* GPChar = Game->GetWorld()->SpawnActor<AGPCharacterBase>(FVector(x, y, z), FRotator(yaw, pitch, roll), FActorSpawnParameters());
								Game->GPGameObjects.Insert(GPChar, idx);
							});
					}
				}
				break;

			/*default:
				GP_LOG(Warning, TEXT("Error Packet type: %d"), pckt->header.type);*/
			}

			size += pckt->header.size;
		}
	} while (iResult > 0); // recv가 정상적으로 처리되면 반복.
	
	return 0;
}

void FGPClient::Exit()
{
	GP_LOG_C(Warning);

	/*closesocket(Socket);
	WSACleanup();*/
}

void FGPClient::Stop()
{
	GP_LOG_C(Warning);

	if (shutdown(Socket, SD_SEND) == SOCKET_ERROR) {
		GP_LOG(Warning, TEXT("shutdown failed: %d"), WSAGetLastError());
	}
	//shutdown되어 정상적으로 서버에서 0바이트를 수신하면 다시 0바이트를 보낼 것이므로 recv루프가 탈출 될 것.

	//SetEvent(ConnEvent); //아직 연결이 되지 않은 상태일 때 Run의 대기를 풀어줌. 
}

void FGPClient::Shutdown()
{
	if (Runnable)
	{
		delete Runnable; 
		Runnable = nullptr;
	}
}

void FGPClient::CreateAsyncSendTask(std::stringstream& ss, GPPacketType pt)
{
	(new FAutoDeleteAsyncTask<FGPSendTask>(ss, pt))->StartBackgroundTask();
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

	//SetEvent(ConnEvent);

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

bool FGPClient::SendStream(std::stringstream& ss, GPPacketType pt)
{
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;
	pckt->header.size = sizeof(PacketH) + ss.fail() ? ss.str().length() : ss.tellp();//todo check size overflow
	pckt->header.type = pt;
	ss >> (char*)&pckt->data;
	/*if (ss.good())
	GP_LOG(Warning, TEXT("stream tellg:%d tellp: %d"), (int)ss.tellg(), (int)ss.tellp());*/
	return Send(sendbuf, pckt->header.size);
}

bool FGPClient::SendChat(FString Chat)
{
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;

	pckt->header.size = sizeof(PacketH) + Chat.Len() + 1;
	pckt->header.type = PT_MSG;
	memcpy(sendbuf + sizeof(PacketH), TCHAR_TO_ANSI(*Chat), Chat.Len() + 1);
	//GP_LOG(Warning, TEXT("%s, buf: %x %d, data: %x %d"), ANSI_TO_TCHAR((char*)&pckt->data), sendbuf + sizeof(PacketH), sizeof(PacketH), &pckt->data, sizeof(Packet))

	return Send(sendbuf, pckt->header.size);
}

bool FGPClient::SendHeader(GPPacketType pt)
{
	PacketH header{ sizeof(PacketH), pt };
	return Send((char*)&header, header.size);
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

	//GP_LOG(Warning, TEXT("Test ID: %s, Bytes: %d"), ANSI_TO_TCHAR((char*)&pckt->data), pckt->header.size)

	return Send(sendbuf, pckt->header.size);
}
