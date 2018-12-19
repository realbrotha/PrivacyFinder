#pragma once

#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>
#include <time.h>


class TimeDate
{
public:
	TimeDate(void);
	
	inline int& Year() { return year_; };
	inline int& Month() { return month_; };
	inline int& Day() { return day_; };
	inline int& Hour() { return hour_; };
	inline int& Min() { return min_; };
	inline int& Sec() { return sec_; };
	inline int& MilliSec() { return millisec_; };
	bool SetCurrentTime(void);
	inline __time64_t& GetCurrentTime() { return timeCurrent_; };

private:
	int	year_;
	int	month_;
	int	day_;
	int	hour_;
	int	min_;
	int	sec_;
	int	millisec_;
	__time64_t timeCurrent_;
};
