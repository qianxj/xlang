#include "xint.hpp"
#include <tchar.h>
#include "vlplatform.hpp"
using namespace xlang;

wchar_t* xint::toString(int v)
{
	wchar_t * p = new wchar_t[64];
	_ltow(v, p,10);
	return p;
}