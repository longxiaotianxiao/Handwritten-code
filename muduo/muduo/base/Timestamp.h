#pragma once

#include "Types.h"
#include <stdint.h>

namespace muduo
{
	class Timestamp
	{
	public:
		Timestamp()
			:microSecondsSinceEpich_(0)
		{}

		explicit Timestamp(int64_t microSecondsSinceEpich)
			: microSecondsSinceEpich_(microSecondsSinceEpich)
		{}

		void swap(Timestamp& that)
		{
			std::swap(microSecondsSinceEpich_, that.microSecondsSinceEpich_);
		}

		std::string toString() const;
		std::string toFormattedString(bool showMicroseconds = true) const;

		bool vaild() const
		{
			return microSecondsSinceEpich_ > 0;
		}

		int64_t microSecondsSinceEpich() const
		{
			return microSecondsSinceEpich_;
		}

		time_t secondsSinceEpoch() const
		{
			return static_cast<time_t>(microSecondsSinceEpich_ / kMicroSecondsPerSecond);
		}

		static Timestamp now();
		static Timestamp invalid()
		{
			return Timestamp();
		}

		static Timestamp fromUnixTime(time_t t)
		{
			return fromUnixTime(t, 0);
		}

		static Timestamp fromUnixTime(time_t t, int microseconds)
		{
			return Timestamp(static_cast<int64_t>(t)*kMicroSecondsPerSecond + microseconds);
		}

		static const int kMicroSecondsPerSecond = 1000 * 1000;
	private:
		int64_t microSecondsSinceEpich_;
	};

	inline bool operator<(Timestamp lhs, Timestamp rhs)
	{
		return lhs.microSecondsSinceEpich() < rhs.microSecondsSinceEpich();
	}

	inline bool operator==(Timestamp lhs, Timestamp rhs)
	{
		return lhs.microSecondsSinceEpich() == rhs.microSecondsSinceEpich();
	}

	inline bool operator > (Timestamp lhs, Timestamp rhs)
	{
		return lhs.microSecondsSinceEpich() > rhs.microSecondsSinceEpich();
	}

	inline double timeDifference(Timestamp high, Timestamp low)
	{
		int64_t diff = high.microSecondsSinceEpich() - low.microSecondsSinceEpich();
		return static_cast<double>(diff / Timestamp::kMicroSecondsPerSecond);
	}

	inline Timestamp addTime(Timestamp timestamp, double seconds)
	{
		int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
		return Timestamp(timestamp.microSecondsSinceEpich() + delta);
	}
}