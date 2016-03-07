#include "xdouble.hpp"
#include "vlplatform.hpp"
using namespace xlang;

wchar_t* xdouble::toString(double v)
{
	wchar_t * p = new wchar_t[64];
	swprintf_s(p,64,L"%f",v);
	return p;
}

wchar_t* xdouble::toStringex(float v)
{
	return toString(v);
}

int xdouble::toInt(double v)
{
	return (int)v;
}
