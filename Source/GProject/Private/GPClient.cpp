#include "GPClient.h"
#include "GPGameInstanceBase.h"
#include "GProjectPlayerController.h"
#include "GProjectGameMode.h"
#include "GPCharacterBase.h"
#include <sstream>

//todo shared pointer for FGPClient pointers! 

namespace GPClient {

	using namespace std;

	bool Send(SOCKET sock, char* buf, int len)
	{
		int iResult = send(sock, buf, len, 0);
		if (iResult == SOCKET_ERROR) {
			GP_LOG(Warning, TEXT("send failed, code : %d"), WSAGetLastError());
			return false;
		}
		return true;
	}

	bool SendStream(SOCKET sock, std::stringstream& ss, GPPacketType pt)
	{
		char sendbuf[MAX_PKT_SIZ]{};
		Packet* pckt = (Packet*)sendbuf;
		int len = ss.fail() ? ss.str().length() : ss.tellp();
		pckt->header.size = sizeof(PacketH) + len;//todo check size overflow
		pckt->header.type = pt;
		ss.read((char*)&pckt->data, len);
		//if (ss.good())
		GP_LOG(Warning, TEXT("stream data: %s tellg:%d tellp: %d"), ANSI_TO_TCHAR((char*)&pckt->data), (int)ss.tellg(), (int)ss.tellp());
		return Send(sock, sendbuf, pckt->header.size);
	}

} // namespace

//used to send data 'fire-and-forget'
class FGPSendTask : public FNonAbandonableTask
{
	friend class FAutoDeleteAsyncTask<FGPSendTask>;

	std::stringstream StringStream;
	GPPacketType PacketType;
	SOCKET Socket;

	FGPSendTask(SOCKET sock, std::stringstream& InStream, GPPacketType InPacketType)
	: Socket(sock), StringStream(std::move(InStream)), PacketType(InPacketType)
	{
	}

	void DoWork()
	{
		GPClient::SendStream(Socket, StringStream, PacketType);
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGPSendTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

//test // may not useful //used to recv data //? and notify completion after EnsureCompletion()
class FGPRecvTask : public FNonAbandonableTask
{
	friend class FAsyncTask<FGPRecvTask>;

	SOCKET Socket;
	int ErrorCode;
	char RecvBuf[MAX_PKT_SIZ]; //
	FGPClient* Client;

	FGPRecvTask(SOCKET sock, FGPClient* cl)
		: Socket(sock), ErrorCode(0), Client(cl)
	{
	}
	
	/*virtual void ProcessPacket(Packet* pckt, GPPacketType pt)
	{
		GP_LOG(Warning, TEXT("Packet type: %d"), pt)
	}*/

	void DoWork()
	{
		
		int iResult; //recv 결과 바이트.

		do
		{
			iResult = recv(Socket, RecvBuf, sizeof(RecvBuf), 0);

			if (iResult == SOCKET_ERROR)
			{
				ErrorCode = WSAGetLastError();
				GP_LOG(Warning, TEXT("recv faild, code : %d"), ErrorCode);
				return;
			}
			else if (iResult == 0) //the connection has been gracefully closed.
			{
				GP_LOG(Warning, TEXT("recv is done. Session disconnected."));
				ErrorCode = 0;
				return;
			}

			int size = 0;
			while (size < iResult) //버퍼가 많을 수도 있으니 반복 처리 시도.
			{
				GP_LOG(Warning, TEXT("Bytes: %d"), iResult)
				Packet* pckt = (Packet*)(RecvBuf + size);
				GPPacketType pt = pckt->header.type;
				GP_LOG(Warning, TEXT("Packet type: %d"), pt)

				//
				switch (pt)
				{
				case PT_SERVER_LOGIN_GOOD:
					GP_LOG_C(Warning);
					Client->PostLogin();
					break;
				case PT_SERVER_LOGIN_FAIL:
					break;
				case PT_USER_SIGNUP:
					//bool bResult = ;
					Client->PostSignup(RecvBuf[size + pckt->header.size] != 0);
				case PT_TEST_ECHO:
					break;
				}

				size += pckt->header.size;
			}
		} while (iResult > 0); // recv가 정상적으로 처리되면 반복.

		return;
	}

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(FGPRecvTask, STATGROUP_ThreadPoolAsyncTasks);
	}
};

FGPClient::FGPClient() : ID("dummy")
{
	PlayerCon = nullptr;
	GameMode = nullptr;
	Game = nullptr;
	AuthRecvTask = nullptr;

	Thread = FRunnableThread::Create(this, TEXT("FGPClient"), 0, TPri_BelowNormal); //windows default = 8mb for thread, could specify more
}

FGPClient::~FGPClient()
{
	GP_LOG_C(Warning);
	delete Thread; //FRunnableThreadWin::Kill(true) ... Runnable->Stop()
	Thread = nullptr;
	closesocket(MainSocket);
	closesocket(AuthSocket);
	WSACleanup();
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

	MainSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (MainSocket == INVALID_SOCKET)
	{
		UE_LOG(LogGProject, Warning, TEXT("Invalid socket, code : %d"), WSAGetLastError());
		return false;
	}

	AuthSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (MainSocket == INVALID_SOCKET)
	{
		UE_LOG(LogGProject, Warning, TEXT("Invalid socket, code : %d"), WSAGetLastError());
		return false;
	}

	//서버와의 연결을 알리는 자동 리셋 이벤트 생성.
	//ConnEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return ConnectAll(); //Init 자체가 Connect가 없다면 필요없다고 판단하여 불필요한 Connect대기를 없애기 위해 여기서 Connect.
}

uint32 FGPClient::Run()
{
	GP_LOG_C(Warning);

	//서버와의 연결을 기다림.
	//WaitForSingleObject(ConnEvent, INFINITE);

	CreateAsyncAuthRecvTask(AuthSocket, this);

	int iResult; //recv 결과 바이트.

	do
	{
		iResult = recv(MainSocket, RecvBuf, sizeof(RecvBuf), 0);

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
				//we got string so need to null terminate 
				char temp = RecvBuf[size + pckt->header.size];
				RecvBuf[size + pckt->header.size] = 0;
				FString msg(RecvBuf + size + sizeof(PacketH));
				RecvBuf[size + pckt->header.size] = temp;
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
			case PT_USER_HOST:
				if (PlayerCon)
				{
					bool bResult = RecvBuf[size + pckt->header.size] != 0;
					AsyncTask(ENamedThreads::GameThread, [this, bResult]()
						{
							PlayerCon->RecieveHostGP(bResult);
						});
				}
				break;
			case PT_USER_JOIN:
				if (PlayerCon)
				{
					bool bResult = RecvBuf[size + pckt->header.size] != 0;
					AsyncTask(ENamedThreads::GameThread, [this, bResult]()
						{
							PlayerCon->RecieveJoinParty(bResult);
						});
				}
				break;
			case PT_BE_HOST: //for standalone test
				if (Game)
				{
					Game->BeGPHost();
				}
				break;
			case PT_PLAYER_START:
				if (GameMode && PlayerCon)
				{
					AsyncTask(ENamedThreads::GameThread, [this]()//
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

	Shutdown();//
	/*closesocket(Socket);
	WSACleanup();*/
}

void FGPClient::Stop()
{
	GP_LOG_C(Warning);

	if (shutdown(MainSocket, SD_SEND) == SOCKET_ERROR) {
		GP_LOG(Warning, TEXT("shutdown failed: %d"), WSAGetLastError());
	}
	//shutdown되어 정상적으로 서버에서 0바이트를 수신하면 다시 0바이트를 보낼 것이므로 Run의 recv루프가 탈출 될 것.

	//SetEvent(ConnEvent); //아직 연결이 되지 않은 상태일 때 Run의 대기를 풀어줌. 
}

void FGPClient::Shutdown()
{
	GP_LOG_C(Warning);

	if (AuthRecvTask)
	{
		//AuthRecvTask->EnsureCompletion(false); //stops thread
		if (!AuthRecvTask->IsDone())
		{
			if (shutdown(AuthSocket, SD_SEND) == SOCKET_ERROR) {
				GP_LOG(Warning, TEXT("shutdown failed: %d"), WSAGetLastError());
			}
		}
		AuthRecvTask->EnsureCompletion(false);

		delete AuthRecvTask;
		AuthRecvTask = nullptr;
	}
}

void FGPClient::CreateAsyncSendTask(SOCKET sock, std::stringstream& ss, GPPacketType pt)
{
	(new FAutoDeleteAsyncTask<FGPSendTask>(sock, ss, pt))->StartBackgroundTask();//
}

void FGPClient::CreateAsyncAuthRecvTask(SOCKET sock, FGPClient* cl)
{
	AuthRecvTask = new FAsyncTask<FGPRecvTask>(sock, cl);//
	AuthRecvTask->StartBackgroundTask();
}

void FGPClient::CreateAsyncMainSendTask(std::stringstream& ss, GPPacketType pt)
{
	CreateAsyncSendTask(MainSocket, ss, pt);
}

void FGPClient::CreateAsyncAuthSendTask(std::stringstream& ss, GPPacketType pt)
{
	CreateAsyncSendTask(AuthSocket, ss, pt);
}

bool FGPClient::Connect(SOCKET sock, u_short port, char* ip)
{
	SOCKADDR_IN sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = inet_addr(ip);

	if (connect(sock, (SOCKADDR*)&sa, sizeof(sa)) == SOCKET_ERROR)
	{
		int err = WSAGetLastError(); 
		if (err == WSAEISCONN) return true; //already connected.

		GP_LOG(Warning, TEXT("connect failed, code : %d"), err);
		return false;
	}

	GP_LOG(Display, TEXT("Connected to server. %s:%d"), ANSI_TO_TCHAR(ip), port);

	//SetEvent(ConnEvent);

	return true;
}

bool FGPClient::Send(SOCKET sock, char* buf, int len)
{
	int iResult = send(sock, buf, len, 0);
	if (iResult == SOCKET_ERROR) {
		GP_LOG(Warning, TEXT("send failed, code : %d"), WSAGetLastError());
		return false;
	}
	return true;
}

bool FGPClient::SendStream(SOCKET sock, std::stringstream& ss, GPPacketType pt)
{
	char sendbuf[MAX_PKT_SIZ]{};
	Packet* pckt = (Packet*)sendbuf;
	int len = ss.fail() ? ss.str().length() : ss.tellp();
	pckt->header.size = sizeof(PacketH) + len;//todo check size overflow
	pckt->header.type = pt;
	ss.read((char*)&pckt->data, len);
	//if (ss.good())
	GP_LOG(Warning, TEXT("stream data: %s tellg:%d tellp: %d"), ANSI_TO_TCHAR((char*)&pckt->data), (int)ss.tellg(), (int)ss.tellp());
	return Send(sock, sendbuf, pckt->header.size);
}

bool FGPClient::SendChat(FString Chat)
{
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;

	pckt->header.size = sizeof(PacketH) + Chat.Len() + 1;
	pckt->header.type = PT_MSG;
	memcpy(sendbuf + sizeof(PacketH), TCHAR_TO_ANSI(*Chat), Chat.Len() + 1);
	//GP_LOG(Warning, TEXT("%s, buf: %x %d, data: %x %d"), ANSI_TO_TCHAR((char*)&pckt->data), sendbuf + sizeof(PacketH), sizeof(PacketH), &pckt->data, sizeof(Packet))

	return Send(MainSocket, sendbuf, pckt->header.size);
}

bool FGPClient::SendHeader(GPPacketType pt)
{
	PacketH header{ sizeof(PacketH), pt };
	return Send(MainSocket, (char*)&header, header.size);
}

bool FGPClient::ConnectAll()
{
	if (!Connect(AuthSocket, GP_AUTH)) return false;

	if (!Connect(MainSocket, GP_PORT)) return false;

	return true;
}

void FGPClient::SignUp(FString id, FString pwd)
{
	std::stringstream ss;
	char* temp = TCHAR_TO_ANSI(*id);
	ss << temp << ' ' << TCHAR_TO_ANSI(*pwd);
	memcpy(ID, temp, id.Len() + 1);
	//GP_LOG(Warning, TEXT("%s"), ANSI_TO_TCHAR(ss.str().c_str()));
	GP_LOG(Warning, TEXT("%s"), ANSI_TO_TCHAR(ID));
	CreateAsyncAuthSendTask(ss, GPPacketType::PT_USER_SIGNUP);
}

void FGPClient::Login(FString id, FString pwd)
{
	std::stringstream ss;
	char* temp = TCHAR_TO_ANSI(*id);
	ss << temp << ' ' << TCHAR_TO_ANSI(*pwd);
	memcpy(ID, temp, id.Len() + 1);
	//GP_LOG(Warning, TEXT("%s"), ANSI_TO_TCHAR(ss.str().c_str()));
	GP_LOG(Warning, TEXT("%s"), ANSI_TO_TCHAR(ID));
	CreateAsyncAuthSendTask(ss, GPPacketType::PT_USER_LOGIN);
}

void FGPClient::Join()
{
	char sendbuf[MAX_PKT_SIZ];
	Packet* pckt = (Packet*)sendbuf;

	int len = strlen(ID);
	pckt->header.size = sizeof(PacketH) + len; //not sending terminating 0
	pckt->header.type = PT_USER_LOGIN;
	memcpy(sendbuf + sizeof(PacketH), ID, len);
	//GP_LOG(Warning, TEXT("%d %d"), len, pckt->header.size);//ANSI_TO_TCHAR(sendbuf + sizeof(PacketH))

	Send(MainSocket, sendbuf, pckt->header.size);
}

void FGPClient::PostLogin()
{
	AsyncTask(ENamedThreads::GameThread, [this]()
		{
			Game->OnLoggedIn.Broadcast();
		});
}

void FGPClient::PostSignup(bool bResult)
{
	AsyncTask(ENamedThreads::GameThread, [this, bResult]()
		{
			Game->OnSignup.Broadcast(bResult);
		});
}

void FGPClient::PostLogout()
{
}

