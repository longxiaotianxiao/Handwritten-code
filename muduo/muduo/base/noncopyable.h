#pragma once

//��ֹ��������
namespace muduo
{
	class noncopyable
	{
	public:
		noncopyable(noncopyable&) = delete;
		noncopyable& operator=(noncopyable&) = delete;
	protected:
		noncopyable() = default;
		~noncopyable() = default;
	};
}