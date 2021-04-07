#pragma once
#include "CriticalSection.h"

#define LOG_TIME_LEN 32
//simple
class MyLog
{
public:
	~MyLog();
	
	static MyLog* GetSingletonPtr();

	template<typename T>
	bool  WriteLog(T str);

private:
	MyLog();

	ofstream mFstream;

	CriticalSection mCS;

};

template<typename T>
bool  MyLog::WriteLog(T str)
{
	if (!mFstream.is_open()) return false;

	CSLock lock(mCS);

	mFstream << str;

	return true;
}

