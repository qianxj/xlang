#ifndef WPP5_TYPE_DOUBLE_H
#define WPP5_TYPE_DOUBLE_H

#include "vlang5.h"

namespace xlang
{

class VLANG_API xdouble
{
private:
	double v;
public:
	static wchar_t* toString(double v);
	static wchar_t* toStringex(float v);
	static int toInt(double v);
};

} //namespace xlang

#endif //WPP5_TYPE_DOUBLE_H

