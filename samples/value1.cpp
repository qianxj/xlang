#include <assert.h>
#include <stdio.h>
#include <wchar.h>

#include <string>

#include "util/xlvalue.cpp"

using namespace xl::util::misc;

int main()
{
	#define S(x) TValue(x).toString()	
	
	printf("use value\n");
	printf("%S\n", (S(3)+S('+')+S(4)+S('=')+S(7)).c_str());
	printf("%S\n",S(L"hello world!").c_str());
	printf("%S\n",S("hello world!").c_str());
	
	return 1;
	
	#undef S
}