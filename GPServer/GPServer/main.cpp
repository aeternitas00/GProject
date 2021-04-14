#include "stdafx.h"
#include "GPServerManager.h"
#include "ClientSession.h"

int _tmain(int argc, _TCHAR* argv[])
{
	GPServerManager* mtsMgr = GPServerManager::getSingleton();

	if (!mtsMgr->InitServer())
	{
		return 1;
	}

	if (!mtsMgr->AcceptLoop())
	{
		return 2;
	}

	return 0;
}