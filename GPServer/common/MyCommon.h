#pragma once
#include "stdafx.h"

void GetCTime(char * buf, int len)
{
	time_t conTime;
	time(&conTime);
	ctime_s(buf, len, &conTime);
	buf[strlen(buf) - 1] = '\0';
}
