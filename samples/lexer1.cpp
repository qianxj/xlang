#include <assert.h>
#include <stdio.h>
#include <wchar.h>
#include <string>

#include "xllexer.hpp"
#include "xltokenutil.cpp"

using namespace xl;
using namespace xl::util::lexer;
using namespace xl::util::token;

int test_eatOperate()
{
	
	const char* test1 = "++ok operate!";
	const char* test2 = "<<=ok operate!";
	const char* str = 0;
	Operate op;
	str = test1 + util::lexer::eatCppOperate(test1, op);
	printf("\n%s",str);
	str = test2 + util::lexer::eatCppOperate(test2, op);
	printf("\n%s",str);	
	
	return 1;
}

int test_eatXQueryComment()
{
	const char* test1 = "(:xxxxxxx:)ok comment!";
	const char* test2 = "(:xx(:xx:)xxx:)ok block comment!";
	const char* str = 0;
	int row = 1;
	int col  = 1;
	str = test1 + util::lexer::eatXQueryComment(test1, row, col);
	printf("\n%s",str);
	str = test2 + util::lexer::eatXQueryComment(test2, row, col);
	printf("\n%s",str);	
	
	return 1;
}

int test_eatNumber()
{
	util::lexer::Number number;
	const char* test1 = "0x1000ok int!";
	const char* test2 = "123.004589e2ok double!";
	const char* str = 0;

	str = test1 + util::lexer::eatNumber(test1,number);
	printf("\n%s",str);
	
	printf("\tresult: %d",number.intPart);
	if(number.kind==util::lexer::Number::kDouble)
	{
		if(number.scale)
			printf(" %d:%d e%d",number.mantissa,number.mantissaScale,number.scale);
		else
			printf(" %d:%d",number.mantissa,number.mantissaScale);
	}

	str = test2 + util::lexer::eatNumber(test2,number);
	printf("\n%s",str);
	printf("\tresult: %d",number.intPart);
	if(number.kind==util::lexer::Number::kDouble)
	{
		if(number.scale)
			printf(" %d:%d e%d",number.mantissa,number.mantissaScale,number.scale);
		else
			printf(" %d:%d",number.mantissa,number.mantissaScale);
	}
	
	return 1;
}

int test_eatCppComment()
{
	const char* test1 = "//xxxxxxx\nok comment!";
	const char* test2 = "/*xxxxxxx\n*/ok block comment!";
	const char* str = 0;
	
	int row = 1;
	int col  = 1;
	str = test1 + util::lexer::eatCppComment(test1, row, col);
	printf("\n%s",str);
	str = test2 + util::lexer::eatCppComment(test2, row, col);
	printf("\n%s",str);	
	
	return 1;
}

std::wstring dumpOperateTerm(OperateTerm<wchar_t> & term, std::wstring prestr)
{
	std::wstring str;
	OperateTerm<wchar_t> * t = &term;
	while(t)
	{
		str += prestr + t->ch;
		if(t->op != util::token::opUnknown) 
		{
			wchar_t buf[255];
			_snwprintf(buf,255,L"0x%x",t->op);
			str +=(std::wstring)L"  ->  " + buf + L" " + util::token::getOperateText(t->op) +L"\n";
		}
		if(t->child) str += dumpOperateTerm(*t->child,prestr + t->ch);
		t = t->next;
	}
	return str;
}

#define R(x,y)  OpTerm(L##x,y)
int test_OperateTerm()
{
	OperateTerm<wchar_t> & tm = R('+',opAdd) * (R('+',opInc) + R('=',opAddAsn));
	tm += R('-',opSub) * (R('-',opDec) + R('=',opSubAsn));
	
	std::wstring str = dumpOperateTerm(tm,L"");
	printf("\nOperate ruler:\n%S",str.c_str());
	return 1;
}
#undef R

int main()
{
	printf("\ntest eat comment");
	test_eatCppComment();
	test_eatXQueryComment();
	printf("\ntest eat number");
	test_eatNumber();
	test_eatOperate();
	test_OperateTerm();
	
	
	Lexer<wchar_t> lexer;
	lexer.SetSource(L"a + b + 12 + \"test\"+ 'A' + 'C'+ 10",0);
	while(true)
	{
		Token<wchar_t> tk = lexer.insymbol();
		if(tk.IsEof())break;
		printf("%S",tk.toString().c_str());
	}
	
	
	return 1;
}