#include "TimeZone.h"
#include <vector>
#include <stdio.h>
#include "Endian.h"

#ifdef _WIN32
#include <basetsd.h>
#define ssize_t SSIZE_T


// Temporarily disable warning 4996 (deprecated function).
# pragma warning(push)
# pragma warning(disable:4996)

#endif

#include <algorithm>
#include <assert.h>
#include "Types.h"
#include <sys/timeb.h>
#include "Date.h"
namespace muduo
{
	namespace detail
	{
		struct Transition
		{
			time_t gmttime;
			time_t localtime;
			int locatltimeIdx;

			Transition(time_t t, time_t l, int localIdx)
				:gmttime(t), localtime(l), locatltimeIdx(localIdx)
			{}
		};

		struct Comp 
		{
			bool compareGmt;

			Comp(bool gmt)
				:compareGmt(gmt)
			{}

			bool operator()(const Transition& lhs, const Transition& rhs) const
			{
				if (compareGmt)
				{
					return lhs.gmttime < rhs.gmttime;
				}
				else
				{
					return lhs.localtime < rhs.localtime;
				}
			}

			bool equal(const Transition& lhs, const Transition& rhs) const
			{
				if (compareGmt)
				{
					return lhs.gmttime == rhs.gmttime;
				}
				else
				{
					return lhs.localtime == rhs.localtime;
				}
			}
		};

		struct Localtime
		{
			time_t gmtOffset;
			bool isDst;
			int arrbIdx;

			Localtime(time_t offset, bool dst, int arrb)
				:gmtOffset(offset), isDst(dst), arrbIdx(arrb)
			{}
		};

		inline void fillHMS(unsigned seconds, struct tm* utc)
		{
			utc->tm_sec = seconds % 60;
			unsigned minutes = seconds / 60;
			utc->tm_min = minutes % 60;
			utc->tm_hour = minutes / 60;
		}
	}

	const int kSecondsPerDay = 24 * 60 * 60;
}
using namespace muduo;
using namespace std;

struct TimeZone::Data
{
	vector<detail::Transition> transitions;
	vector<detail::Localtime> localtimes;
	vector<string> names;
	string abbreviation;
};

namespace muduo
{
	namespace detail
	{
		class File
		{
		public:
			File(const char* file)
				:fp_(::fopen(file, "rb"))
			{}

			~File()
			{
				if (fp_)
				{
					::fclose(fp_);
				}
			}

			bool valid() const
			{
				return fp_;
			}

			std::string readBytes(int n)
			{
				char * buf = new char(n);

				ssize_t nr = ::fread(buf, 1, n, fp_);

				if (nr != n)
				{
					throw logic_error("no enough data");
				}
				std::string strRet(buf, n);

				delete[] buf;
				buf = NULL;

				return strRet;
			}

			int32_t readInt32()
			{
				int32_t x = 0;
				ssize_t nr = ::fread(&x, 1, sizeof(int32_t), fp_);
				if (nr != sizeof(int32_t))
				{
					throw logic_error("bad int32_t data");
				}

				return be32toh(x);
			}

			uint8_t readUint8()
			{
				uint8_t x = 0;
				ssize_t nr = ::fread(&x, 1, sizeof(uint8_t), fp_);
				if (nr != sizeof(uint8_t))
				{
					throw logic_error("bad uint_8 data");
				}

				return x;
			}

		private:
			FILE* fp_;
		};

		//貌似没有用到这个文件不知道什么格式
		bool readTimeZoneFile(const char* zonefile, struct TimeZone::Data* data)
		{
			File f(zonefile);
			if (f.valid())
			{
				try
				{
					std::string head = f.readBytes(4);
					if (head != "TZif")
					{
						throw logic_error("bad head");
					}

					std::string version = f.readBytes(1);
					f.readBytes(15);

					int32_t isgmtcnt = f.readInt32();
					int32_t isstdcnt = f.readInt32();
					int32_t  leapcnt = f.readInt32();
					int32_t  timecnt = f.readInt32();
					int32_t  typecnt = f.readInt32();
					int32_t  charcnt = f.readInt32();

					vector<int32_t> trans;
					vector<int> localtimes;
					trans.reserve(timecnt);

					for (int i = 0; i < timecnt; i++)
					{
						trans.push_back(f.readInt32());
					}

					for (int i = 0; i < timecnt;i++)
					{
						uint8_t local = f.readUint8();
						localtimes.push_back(local);
					}


				}
				catch (logic_error& e)
				{
					
				}
			}

			return true;
		}

		const Localtime* findLocaltime(const TimeZone::Data& data, Transition sentry, Comp comp)
		{
			const Localtime* local = NULL;

			if (data.transitions.empty() || comp(sentry,data.transitions.front()))
			{
				local = &data.localtimes.front();
			}
			else
			{
				vector<Transition>::const_iterator transI = std::lower_bound(data.transitions.begin(), data.transitions.end(), sentry, comp);
				if (transI != data.transitions.end())
				{
					if (comp.equal(sentry,*transI))
					{
						assert(transI != data.transitions.begin());
						--transI;
					}
					local = &data.localtimes[transI->locatltimeIdx];
					
				}
				else
				{
					local = &data.localtimes[data.transitions.back().locatltimeIdx];
				}
			}

			return local;
		}
	}
}

TimeZone::TimeZone(const char* zonefile)
:data_(new TimeZone::Data)
{
	if (!detail::readTimeZoneFile(zonefile, data_.get()))
	{
		data_.reset();
	}
}

TimeZone::TimeZone(int eastOfUtc, const char* tzname)
:data_(new TimeZone::Data)
{
	data_->localtimes.push_back(detail::Localtime(eastOfUtc, false, 0));
	data_->abbreviation = tzname;
}

struct tm TimeZone::toLocalTime(time_t seconds) const
{
	struct tm localTime;
	memZero(&localTime, sizeof(localTime));
	assert(data_ != NULL);
	const Data& data(*data_);

	detail::Transition sentry(seconds, 0, 0);
	const detail::Localtime* local = findLocaltime(data, sentry, detail::Comp(true));

	if (local)
	{
		time_t localSeconds = seconds + local->gmtOffset;
#ifdef _WIN32
		::gmtime_s(&localTime, &localSeconds);
#else
		::gmtime_r(&localSeconds,&localTime);
#endif
		localTime.tm_isdst = local->isDst;
	}

	return localTime;
}

time_t TimeZone::fromLocalTime(const struct tm& localTm) const
{
	assert(data_ != NULL);
	const Data& data(*data_);

	struct tm tmp = localTm;
	time_t seconds = 0;
#ifdef WIN32
	seconds = _mkgmtime64(&tmp);
#else
	seconds = ::timegm(&tmp);
#endif

	detail::Transition sentry(0, seconds, 0);
	const detail::Localtime* local = findLocaltime(data, sentry, detail::Comp(false));

	if (localTm.tm_isdst)
	{
		struct tm tryTm = toLocalTime(local->gmtOffset);
		if (!tryTm.tm_isdst && tryTm.tm_hour == localTm.tm_hour && tryTm.tm_min == localTm.tm_min)
		{
			seconds -= 3600;
		}
	}
	return seconds - local->gmtOffset;
}

struct tm TimeZone::toUtcTime(time_t secondsSinceEpoch, bool yday /* = false */)
{
	struct tm utc;
	memZero(&utc, sizeof(utc));
	int seconds = static_cast<int>(secondsSinceEpoch % kSecondsPerDay);
	int day = static_cast<int>(secondsSinceEpoch / kSecondsPerDay);

	if (seconds < 0)
	{
		seconds += kSecondsPerDay;
		--day;
	}

	detail::fillHMS(seconds, &utc);
	Date date(day + Date::kJulianDayof1970_01_01);
	Date::YearMonthDay ymd = date.yearMonthDay();
	utc.tm_year = ymd.year - 1900;
	utc.tm_mon = ymd.month - 1;
	utc.tm_mday = ymd.day;
	utc.tm_wday = date.weekDay();

	if (yday)
	{
		Date startOfYear(ymd.year, 1, 1);
		utc.tm_yday = date.julianDayNumber() - startOfYear.julianDayNumber();
	}

	return utc;
}

time_t TimeZone::fromUtcTime(const struct tm& utc)
{
	return fromUtcTime(utc.tm_year + 1900, utc.tm_mon + 1, utc.tm_mday, utc.tm_hour, utc.tm_min, utc.tm_sec);
}

time_t TimeZone::fromUtcTime(int year, int month, int day, int hour, int minute, int seconds)
{
	Date date(year, month, day);
	int secondsInDay = hour * 3600 + minute * 60 + seconds;
	time_t days = date.julianDayNumber() - Date::kJulianDayof1970_01_01;
	return days  * kSecondsPerDay + secondsInDay;
}