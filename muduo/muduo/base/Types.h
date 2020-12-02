#pragma once
#include <string>

namespace muduo
{
	inline void memZero(void* p, size_t n)
	{
		memset(p, 0, n);
	}

	//隐式转换
	template<typename To,typename From>
	inline To implicit_cast(From const& f)
	{
		return f;
	}

	//父类向子类转换，需要明确它本来就指向子类
	template<typename To,typename From>
	inline down_cast(From* f)
	{
#ifdef _DEBUG
		assert(f == NULL || dynamic_cast<To>(f) != NULL)
#endif
		return static_cast<To>(f);
	}

}