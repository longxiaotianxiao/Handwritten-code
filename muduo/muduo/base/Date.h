#pragma once
#include <string>

namespace muduo
{
	class Date
	{
	public:
		struct YearMonthDay
		{
			int year;
			int month;
			int day;
		};

		Date()
			:julianDayNumber_(0)
		{}

		Date(int year, int month, int day);
		explicit Date(int julianDayNumber)
			:julianDayNumber_(julianDayNumber)
		{}
		static const int kDaysPerWeek = 7;
		static const int kJulianDayof1970_01_01;

		explicit Date(const struct tm&);

		void swap(Date& that)
		{
			std::swap(julianDayNumber_, that.julianDayNumber_);
		}

		bool valid() const
		{
			return julianDayNumber_ > 0;
		}

		std::string toIsoString() const;

		struct YearMonthDay yearMonthDay() const;

		int year() const
		{
			return yearMonthDay().year;
		}

		int month() const
		{
			return yearMonthDay().month;
		}

		int day() const
		{
			return yearMonthDay().day;
		}


	
	private:
		int julianDayNumber_;
	};
}