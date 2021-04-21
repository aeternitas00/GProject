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
	HANDLE ConnEvent; //연결 대기 이벤트.

	/** Makes sure this thread has stopped properly */
	//void EnsureCompletion(); //FRunnableThreadWin의 소멸 중 WaitForSingleObject가 호출되기 때문에 필요 없음. 오히려 쓸모 없이 Stop이 중복 호출 됨.

public:
	FGPClient();
	virtual ~FGPClient() override;

	// Begin FRunnable interface. 

	// 이 함수들은 FRunnableThreadWin::Run에 의해 호출 됨. 즉 스레드 문맥에 있음.

	virtual bool Init() override; //true를 리턴하면 Run.
	virtual uint32 Run() override; //recv loop
	virtual void Exit() override; //Run이 끝난 뒤에 리소스 클린업을 위해 호출.

	////

	//recv loop를 탈출하고 send shutdown.
	virtual void Stop() override; // FRunnableThreadWin이 소멸할 때 스레드를 중지하기 위해 호출.

	// End FRunnable interface

	// Runnable 인스턴스 소멸.
	void Shutdown();
	

	// Winsock api 

	bool Connect(u_short port = GP_PORT, char* ip = "127.0.0.1");
	bool Send(char* buf, int len);
	bool SendChat(FString str);

	////

	bool Login(); // todo //현재는 가상의 로그인 과정. //TODO DB


	/*
		Start the thread and the worker from static (easy access)!
		This code ensures only thread will be able to run at a time.
		This function returns a handle to the newly started instance.
	*/
	static FGPClient* InitClient();

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	//static void Shutdown();
};