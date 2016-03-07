#ifndef WPP5_WPP5_H
#define WPP5_WPP5_H

#include "vlang5.h"
#include "vlwerr.hpp"
#include <map>

using namespace std;
namespace xlang
{

	class VLANG_API Wpp5
	{
	public:
		Wpp5();
		~Wpp5();
	public:
		bool parse(/*wpp_context*/ void * context,wchar_t* pSrc,bool btrace=false);
	};
}

//

#endif //WPP5_WPP5_H