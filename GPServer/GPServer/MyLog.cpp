#include "stdafx.h"
#include "MyLog.h"


MyLog::MyLog()
{
	mFstream.open("�����α�.txt");
}


MyLog::~MyLog()
{
	/*if (mFstream.is_open())
		mFstream.close();*/
}

MyLog * MyLog::GetSingletonPtr()
{
	static MyLog* log = new MyLog();

	return log;
}


