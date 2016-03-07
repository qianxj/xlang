#ifndef WPP5_TYPE_INT_H
#define WPP5_TYPE_INT_H

#include "vlang5.h"

namespace xlang
{

class VLANG_API xint
{
	int v;
public:
	static wchar_t* toString(int v);
};

} //namespace xlang

#endif //WPP5_TYPE_INT_H

