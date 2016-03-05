#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include "wpsymnode.hpp"
#include <string>


using namespace xl::wp;

int main()
{
	printf("\ntest axisnode");
	
	#define A (*(new AxisNode<SymNode*>()))	
	
	AxisNode<SymNode*>  term;
	//操作符定义缺少优先级
	printf("/naxis tree:%S\n", (A + (A*(A + A)*A)).dump(L"\n  ").c_str());
	
	#undef A
	
	return 1;
}