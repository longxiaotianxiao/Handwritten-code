#pragma once
#include <string>

namespace muduo
{
	inline void memZero(void* p, size_t n)
	{
		memset(p, 0, n);
	}

	//��ʽת��
	template<typename To,typename From>
	inline To implicit_cast(From const& f)
	{
		return f;
	}

	//����������ת������Ҫ��ȷ��������ָ������
	template<typename To,typename From>
	inline down_cast(From* f)
	{
#ifdef _DEBUG
		assert(f == NULL || dynamic_cast<To>(f) != NULL)
#endif
		return static_cast<To>(f);
	}

}