#ifndef WPP5_DEBUGER_H
#define WPP5_DEBUGER_H
#include <assert.h>
#include <vector>
#include <string>
#include "vlang5.h"
#include "vlplatform.hpp"

#define ASSERT(b) \
{					\
	if(!(b))			\
	{				\
		fwprintf(stderr,L"Asser failed %S,  file %S, line %d\r\n",#b,__FILE__,__LINE__);\
		assert(b);	\
	}	\
}		

#define CHECK(b,...) if(!(b))		\
	{							\
		fwprintf(stderr,L"Checked failed %S,  file %S, line %d\r\n",#b,__FILE__,__LINE__); \
		fwprintf(stderr, __VA_ARGS__);	\
		/*assert(b);*/	DEBUGBREAK		\
	}						  
#define DEBUGBREAK DebugBreak();

class VLANG_API vlDebuger
{
public:
	 vlDebuger();
	~vlDebuger();
public:
	std::vector<int> * method_stack_;
public:
	void enter_method(int fn);
	void leave_method(int fn);
public:
	static  vlDebuger * get_debuger();
public:
	std::wstring get_method_stack_string();
};

#endif //WPP5_DEBUGER_H