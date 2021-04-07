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
	/* �� �Լ����� FRunnableThread�� ���� ȣ��Ǵ� ������ ���̴µ� 
	** ������ dll�� ��������� FRunnable�� �Ҹ����� �ʱ� ������ �� ȣ�� ������ �Բ� �׽�Ʈ �ϱⰡ ���ŷο�� ���� �ν��Ͻ� �Ҹ� �� ���� �Ҹ���� �� �ʿ䰡 ����.
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