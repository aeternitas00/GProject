#pragma once

#include "GProject.h"
#include "HAL/Runnable.h"
#include "../GPServer/common/GPPacket.h"

#include <WinSock2.h>
#pragma comment(lib,"ws2_32")



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
	static FGPClient* InitClient();

public:
	// Begin FRunnable interface. 

	// �� �Լ����� FRunnableThreadWin::Run�� ���� ȣ�� ��. �� ������ ���ƿ� ����.

	virtual bool Init() override; //true�� �����ϸ� Run.
	virtual uint32 Run() override; //recv loop
	virtual void Exit() override; //Run�� ���� �ڿ� ���ҽ� Ŭ������ ���� ȣ��.

	////

	//Run�� ����(�� �ٶ�)�� �޼ҵ�.
	virtual void Stop() override; // FRunnableThreadWin�� �Ҹ��� �� ������(Run)�� �����ϱ� ���� ȣ��.

	// End FRunnable interface

	// Runnable �ν��Ͻ� �Ҹ�.
	void Shutdown();

private:
	class AGProjectPlayerController* PlayerCon;
	//class AGProjectGameMode* GameMode;

public:
	void SetPlayerController(AGProjectPlayerController* PC) {
		PlayerCon = PC;
		//GP_LOG(Warning, TEXT("%x"), PlayerCon);
	}
	/*void SetGameMode(AGProjectGameMode* GM) {
		GameMode = GM;
	}*/


protected:
	SOCKET Socket;
	HANDLE ConnEvent; //���� ��� �̺�Ʈ.

	char RecvBuf[MAX_PKT_SIZ]; //
	
public:
	// Winsock api 

	bool Connect(u_short port = GP_PORT, char* ip = "127.0.0.1"); //todo async //���� �� ������ ������ �и��ϱ� ���� GPEntry������ Connect���ְ� ����.
	bool Send(char* buf, int len);
	bool SendChat(FString Chat);

	////

	bool Login(); // todo //����� ������ �α��� ����. //TODO DB

};