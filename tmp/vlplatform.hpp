#ifndef WPP5_PLATFORM_H
#define WPP5_PLATFORM_H

#include "vlang5.h"
#include "vlos_win32.hpp"

namespace xlang
{

typedef class VLANG_API vlOS
{
public:
	static int allocateAlignment();
	static int pageSize();
	static int numberOfProcessers();

	//virtual manage
	static void virtual_alloc(void *p, size_t size);
	static void virtual_free(void *p);

	//heap manage
	static void* heap_alloc(void *p, size_t size);
	static void heap_free(void *p);
	
	//
	static void mem_copy(void * dest,const void * src, int size);

	//print out
	static void print(const wchar_t* format,...);
	
	//print out
	static wchar_t * trace(wchar_t * format,...);

public:
	static FARPROC load_module(const wchar_t* pdll, const wchar_t * pfn);
} os_t;

} //namespace xlang

#endif //WPP5_PLATFORM_H