#pragma once

//½ûÖ¹¿½±´»ùÀà
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