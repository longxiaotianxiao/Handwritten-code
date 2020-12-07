#include "Timestamp.h"
#include <inttypes.h>
#include <time.h>
#include <stdio.h>

#ifdef _WIN32 
#include <WinSock2.h>

inline int gettimeofday(struct timeval* tp, void* tzp)
{
	uint64_t intervals;
	FILETIME ft;

	GetSystemTimeAsFileTime(&ft);

	intervals = ((uint64_t)ft.dwHighDateTime << 32) | ft.dwLowDateTime;

	intervals -= 116444736000000000;

	tp->tv_sec = (long)(intervals / 10000000);
	tp->tv_usec = (long)((intervals % 10000000) / 10);

	return 0;
}

#endif

using namespace muduo;

std::string Timestamp::toString() const
{
	char buf[32] = { 0 };
	int64_t seconds = microSecondsSinceEpich_ / kMicroSecondsPerSecond;
	int64_t microseconds = microSecondsSinceEpich_ % kMicroSecondsPerSecond;
#ifdef _WIN32
#define  snprintf sprintf_s
#endif

	snprintf(buf, sizeof(buf), "%" PRId64 ".%0d" PRId64 "", seconds, microseconds);
	return buf;
}

std::string Timestamp::toFormattedString(bool showMicroseconds /* = true */) const
{
	char buf[64] = { 0 };

	time_t seconds = static_cast<time_t>(microSecondsSinceEpich_ / kMicroSecondsPerSecond);
	struct tm tm_time;
#ifdef _WIN32
	::gmtime_s(&tm_time, &seconds);
#else
	::gmtime_r(&seconds, &tm_time);
#endif

	if (showMicroseconds)
	{
		int microSeconds = static_cast<int>(microSecondsSinceEpich_ % kMicroSecondsPerSecond);
#ifdef _WIN32
#define  snprintf sprintf_s
#endif
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1990,
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec,
			microSeconds);

	}
	else
	{
		snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
			tm_time.tm_year + 1990,
			tm_time.tm_mon + 1,
			tm_time.tm_mday,
			tm_time.tm_hour,
			tm_time.tm_min,
			tm_time.tm_sec);
	}
	return buf;
}

Timestamp Timestamp::now()
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	int64_t seconds = tv.tv_sec;
	return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}