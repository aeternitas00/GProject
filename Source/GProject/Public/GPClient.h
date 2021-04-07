#pragma once

#include "GProject.h"

#include "HAL/Runnable.h"

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

public:
	FGPClient();
	virtual ~FGPClient();

	// Begin FRunnable interface. 
	/* 이 함수들은 FRunnableThread에 의해 호출되는 것으로 보이는데 
	** 에디터 dll이 살아있으면 FRunnable은 소멸하지 않기 때문에 그 호출 구조와 함께 테스트 하기가 번거로우니 게임 인스턴스 소멸 시 따로 소멸시켜 줄 필요가 있음.
	*/
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();

	// End FRunnable interface

	/** Makes sure this thread has stopped properly */
	void EnsureCompletion();

	void Shutdown();

	/*
		Start the thread and the worker from static (easy access)!
		This code ensures only thread will be able to run at a time.
		This function returns a handle to the newly started instance.
	*/
	static FGPClient* InitClient();

	/** Shuts down the thread. Static so it can easily be called from outside the thread context */
	//static void Shutdown();
};