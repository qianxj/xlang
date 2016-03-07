#include "vlStringPool.hpp"
using namespace xlang;

wchar_t* vlStringPool::append_string(const wchar_t* str)
{
	return (wchar_t*)this->append_data((void *) str, (int)(wcslen(str) + 1)*sizeof(wchar_t));
}

wchar_t* vlStringPool::append_string(const wchar_t* str,int size)
{
	wchar_t * r = (wchar_t*)this->append_data((void *) str, (int)(size + 1)*sizeof(wchar_t));
	r[size] = 0;
	return r;
}

