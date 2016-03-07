#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string>

#include "util/xlaxisnode.hpp"
#include "util/xlvalue.cpp"

using namespace xl::util::misc;

template <class T> 
AxisNode<T> & makeAxisNode(T val)
{
	return *(new AxisNode<T>(val));
}

int main()
{
	#define A(x) makeAxisNode(TValue(x))
	printf("\ntest axisnode");
	
	auto t = A("for") + A(3) * (A(4) + A(6));
	printf("/naxis tree:%S\n", t.dump(L"\n ").c_str()) ;
	
	return 1;
	#undef A
}