#pragma once

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

	
	private:
		int julianDayNumber_;
	};
}