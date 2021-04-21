#pragma once

#include "GProject.h"
#include "HAL/Runnable.h"
#include "../GPServer/common/GPPacket.h"

#include <WinSock2.h>
#pragma comment(lib,"ws2_32")



class GPROJECT_API FGPClient : public FRunnable
{
	/** Singleton instance, can access the thread any time via static accessor, if it is active! */
	static  FGPClient* Runnable;

	/** Thread to run the worker FRunnable on */
	FRunnableThread* Thread;

protected:
	SOCKET Socket;
	HANDLE ConnEvent; //���� ��� �̺�Ʈ.

	/** Makes sure this thread has stopped properly */
	//void EnsureCompletion(); //FRunnableThreadWin�� �Ҹ� �� WaitForSingleObject�� ȣ��Ǳ� ������ �ʿ� ����. ������ ���� ���� Stop�� �ߺ� ȣ�� ��.

public:
	FGPClient();
	virtual ~FGPClient() override;

	// Begin FRunnable interface. 

	// �� �Լ����� FRunnableThreadWin::Run�� ���� ȣ�� ��. �� ������ ���ƿ� ����.

	virtual bool Init() override; //true�� �����ϸ� Run.
	virtual uint32 Run() override; //recv loop
	virtual void Exit() override; //Run�� ���� �ڿ� ���ҽ� Ŭ������ ���� ȣ��.

	////

	//recv loop�� Ż���ϰ� send shutdown.
	virtual void Stop() override; // FRunnableThreadWin�� �Ҹ��� �� �����带 �����ϱ� ���� ȣ��.

	// End FRunnable interface

	// Runnable �ν��Ͻ� �Ҹ�.
	void Shutdown();
	

	// Winsock api 

	bool Connect(u_short port = GP_PORT, char* ip = "127.0.0.1");
	bool Send(char* buf, int len);
	bool SendChat(FString str);

	////

	bool Login(); // todo //����� ������ �α��� ����. //TODO DB


	/*
		Start the thread and the worker from static (easy access)!
		This code ensures only thread will be able to run at a time.
		This function returns a handle to the newly started instance.
	*/
	static FGPClient* InitClient();

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	//static void Shutdown();
};