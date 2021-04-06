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
	virtual bool Init();
	virtual uint32 Run();
	virtual void Stop();
	// End FRunnable interface
};