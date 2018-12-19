#include "stdafx.h"
#include "TimeDate.h"


TimeDate::TimeDate(void)
: year_(0), month_(0), day_(0), hour_(0), min_(0), sec_(0), millisec_(0), timeCurrent_(0)
{
}

bool TimeDate::SetCurrentTime(void)
{
	bool result = false;
	struct tm currentTimeInfo;
	struct __timeb64 currentTimeInfoEx;
	
	if ((_ftime64_s(&currentTimeInfoEx) == 0) && (_localtime64_s(&currentTimeInfo, &currentTimeInfoEx.time) == 0))
	{	
		timeCurrent_ = currentTimeInfoEx.time; 

		year_ = currentTimeInfo.tm_year + 1900;
		month_ = currentTimeInfo.tm_mon + 1;
		day_ = currentTimeInfo.tm_mday;
		hour_ = currentTimeInfo.tm_hour;
		min_ = currentTimeInfo.tm_min;
		sec_ = currentTimeInfo.tm_sec;
		millisec_ = currentTimeInfoEx.millitm;

		result = true;
	}
	return result;
}