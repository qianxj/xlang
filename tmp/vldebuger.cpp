#include "vldebuger.hpp"  

vlDebuger::vlDebuger():method_stack_(new std::vector<int>) 
{
	
}

vlDebuger::~vlDebuger()
{
	if(method_stack_) delete method_stack_;
}

void vlDebuger::enter_method(int fn)
{
#if UN
	const wchar_t * fx = CWord(fn,0)[wpp_ast::id].sval;
	method_stack_->push_back(fn);
#endif
}

void vlDebuger::leave_method(int fn)
{
#if UN
	const wchar_t * fx = 0;
	if(method_stack_->size() > 0 )
		fx = CWord(method_stack_->back(),0)[wpp_ast::id].sval;

	method_stack_->pop_back();
#endif
}

vlDebuger * vlDebuger::get_debuger()
{
	static vlDebuger  debuger_;
	return & debuger_;
}

std::wstring vlDebuger::get_method_stack_string()
{
	std::wstring str;
	int size = (int)method_stack_->size();
#if UN
	for(int i=0;i < size; i++)
	{
		int fn = (*method_stack_)[i];
		if(CWord(fn,0)[wpp_ast::forward].val)
		{
			str += CWord(CWord(fn,0)[wpp_ast::forward].wi)[wpp_ast::id].sval;
			str += L"::";
		}
		str += CWord(fn,0)[wpp_ast::id].sval;
		str +=L"\r\n";
	}
#endif
	return str;
}