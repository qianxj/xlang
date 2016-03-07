#ifndef WPP5_WPP5_CONTEXT_H
#define WPP5_WPP5_CONTEXT_H
#include "vlplatform.hpp"
#include "vlwerr.hpp"
#include "vlwpp.hpp"
#include <set>
#include <vector>
using namespace xlang;

namespace xlang{

class wpp_runtime; 
typedef class VLANG_API wpp_context
{
public:
	wpp_context(wpp_runtime* runtime);
	~wpp_context();
public:
	/*mill::vill_asm*/void * get_vill_asm();
	
	/*wpSymtable_t*/void * get_metatable();
	void set_vill_asm(/*mill::vill_asm*/void * ptr_code);
	void set_metatable(/*wpSymtable_t*/void * ptr_metatable);
	Wpp5* get_wpp_ptr();
	void set_wpp_ptr(Wpp5* ptr_wpp);
	wpp_runtime* get_runtime();
	void set_runtime(wpp_runtime* ptr_runtime);
public:
	struct sourcetime_t
	{
		FILETIME dt;
		wstring  text;
	};
	static map<wstring,sourcetime_t>* sources;
	map<wstring,FILETIME>* files;
public:
	void set_error(wErr* errPtr_); 
	wErr* get_error(); 
	wErr* get_rrror(){return get_error();}
public:
	bool parse(wchar_t *src); 
public:
	int process_file(const wchar_t* file);
	wchar_t* fetch_file(const wchar_t* file);

private:
	Wpp5 * ptr_wpp_;
	/*mill::vill_asm*/ void * ptr_vill_asm_;
	/*wpSymtable_t */  void * ptr_metatable_;
	wpp_runtime* ptr_runtime_;
	wErr*			errPtr;
public:
	int	get_current_class();
	int get_current_method();
public:
	void GetClassMethodString(int pclass, wchar_t* fct, std::set<std::wstring> * & pSet);
	void wpp_context::GetClassAutoString(int pclass, std::set<std::wstring> * & pSet);
	void wpp_context::GetNamespaceAutoString(int item, std::set<std::wstring> * & pSet);
	wchar_t* wpp_context::getVarAutoString(wchar_t* v);
	wchar_t * wpp_context::getFctTipString(wchar_t* v);
	void wpp_context::freeAutoString(wchar_t * p);

} wpp_context_t;

}//namespace vpp

#endif //WPP5_WPP5_CONTEXT_H