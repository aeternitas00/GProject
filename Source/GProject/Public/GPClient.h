#pragma once

#include "GProject.h"
#include "HAL/Runnable.h"
#include "../GPServer/common/GPPacket.h"

#include "Windows/AllowWindowsPlatformAtomics.h"
#include <WinSock2.h>
#include "Windows/HideWindowsPlatformAtomics.h"
#pragma comment(lib,"ws2_32")
#include <sstream>

class GPROJECT_API FGPClient : public FRunnable
{

public:
	FGPClient();
	virtual ~FGPClient() override;

private:
	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  FGPClient* Runnable;

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

public:
	/*
		Start the thread and the worker from static (easy access)!
		This code ensures only thread will be able to run at a time.
		This function returns a handle to the newly started instance.
	*/
	static FGPClient* GetGPClient();

public:
	// Begin FRunnable interface. 

	// 이 함수들은 FRunnableThreadWin::Run에 의해 호출 됨.

	virtual bool Init() override; //true를 리턴하면 Run.
	virtual uint32 Run() override; //recv loop
	virtual void Exit() override; //Run이 끝난 뒤에 리소스 클린업을 위해 호출.

	////

	//Run을 끝내(길 바라)는 메소드.
	virtual void Stop() override; // FRunnableThreadWin이 소멸할 때 스레드(Run)를 중지하기 위해 호출.

	// End FRunnable interface

	// Runnable 인스턴스 소멸.
	void Shutdown();

	void CreateAsyncSendTask(std::stringstream& ss, GPPacketType pt);

private:
	class AGProjectPlayerController* PlayerCon;
	class AGProjectGameMode* GameMode;
	class UGPGameInstanceBase* Game;

public:
	//convenient setters

	void SetPlayerController(AGProjectPlayerController* PC) { PlayerCon = PC; }
	void SetGameMode(AGProjectGameMode* GM) { GameMode = GM; }
	void SetGameInstance(UGPGameInstanceBase* GI) { Game = GI; }

	////

protected:
	SOCKET Socket;
	//HANDLE ConnEvent; //연결 대기 이벤트.

	char RecvBuf[MAX_PKT_SIZ]; //
	
public:
	// Winsock api 

	bool Connect(u_short port = GP_PORT, char* ip = "127.0.0.1"); //todo async //현재 이 서버와 게임을 분리하기 위해 GPEntry에서만 Connect해주고 있음.
	bool Send(char* buf, int len);
	bool SendStream(std::stringstream& ss, GPPacketType pt);

	////

	// GI features.

	bool Login(); // todo //현재는 가상의 로그인 과정. //TODO DB
	bool SendChat(FString Chat);

	////

	// Player features.

	bool SendPlayerData();
};


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