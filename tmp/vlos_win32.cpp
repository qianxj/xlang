#include "vlplatform.hpp"
#include "vlos_win32.hpp"
#include <memory.h>
#include <locale.h>

using namespace xlang;

HMODULE hCrt = GetModuleHandle(L"msvcr80.dll"); 
wchar_t * vlOS::trace(wchar_t * format,...)
{
	wchar_t* str = new wchar_t[1024*512]; 
	va_list args;
	va_start(args, format);
	int len = vswprintf_s(str,1024*512,format,args);
	va_end(args);
	return str;	
}

int vlOS::allocateAlignment()
{
	static int allocateAlignment_ = 0;
	if(!allocateAlignment_)
	{
		SYSTEM_INFO s_info;
		GetSystemInfo(&s_info);
		allocateAlignment_ = s_info.dwAllocationGranularity;
	}
	return allocateAlignment_;
}

int vlOS::pageSize()
{
	static int pageSize_ = 0;
	if(!pageSize_)
	{
		SYSTEM_INFO s_info;
		GetSystemInfo(&s_info);
		pageSize_ = s_info.dwPageSize;
	}
	return pageSize_;
}

int vlOS::numberOfProcessers()
{
	static int numberOfProcessers_ = 0;
	if(!numberOfProcessers_)
	{
		SYSTEM_INFO s_info;
		GetSystemInfo(&s_info);
		numberOfProcessers_ = s_info.dwNumberOfProcessors;
	}
	return numberOfProcessers_;
}

void vlOS::virtual_alloc(void *p, size_t size)
{
	
}

void vlOS::virtual_free(void *p)
{
	VirtualFree(p, 0, MEM_RELEASE);
}

void* vlOS::heap_alloc(void *p, size_t size)
{
	return malloc(size);
}
void vlOS::heap_free(void *p)
{
	return free(p);
}

void vlOS::mem_copy(void * dest,const void * src, int size)
{
	memcpy_s(dest,size,src,size);
}

FARPROC vlOS::load_module(const wchar_t* pdll, const wchar_t * pfn)
{
	HMODULE hDll = ::GetModuleHandle(pdll);
	if(!hDll)
	{
		if(wcsstr(pdll,L"msvcr80"))
			hDll = hCrt;
		else
			hDll = ::LoadLibraryW(pdll);
	}
	
	char sfn[128];
	size_t sz;
	wcstombs_s(&sz,sfn,128,pfn,128); 
	return GetProcAddress(hDll,sfn);
}

void vlOS::print(const wchar_t* format,...)
{
	va_list args;
	va_start(args, format);
	setlocale( LC_ALL, "" );
	vwprintf_s(format,args);
	va_end(args);
}

