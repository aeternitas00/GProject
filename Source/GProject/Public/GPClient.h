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

	// �� �Լ����� FRunnableThreadWin::Run�� ���� ȣ�� ��.

	virtual bool Init() override; //true�� �����ϸ� Run.
	virtual uint32 Run() override; //recv loop
	virtual void Exit() override; //Run�� ���� �ڿ� ���ҽ� Ŭ������ ���� ȣ��.

	////

	//Run�� ����(�� �ٶ�)�� �޼ҵ�.
	virtual void Stop() override; // FRunnableThreadWin�� �Ҹ��� �� ������(Run)�� �����ϱ� ���� ȣ��.

	// End FRunnable interface

	// Runnable �ν��Ͻ� �Ҹ�.
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
	//HANDLE ConnEvent; //���� ��� �̺�Ʈ.

	char RecvBuf[MAX_PKT_SIZ]; //
	
public:
	// Winsock api 

	bool Connect(u_short port = GP_PORT, char* ip = "127.0.0.1"); //todo async //���� �� ������ ������ �и��ϱ� ���� GPEntry������ Connect���ְ� ����.
	bool Send(char* buf, int len);
	bool SendStream(std::stringstream& ss, GPPacketType pt);

	////

	// GI features.

	bool Login(); // todo //����� ������ �α��� ����. //TODO DB
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