#ifndef WPP5_WPP5_RUNTIME_H
#define WPP5_WPP5_RUNTIME_H

#include "vlwpp_context.hpp"

namespace xlang{

typedef class VLANG_API wpp_runtime
{
public:
	static wchar_t* (* GetSource)(wchar_t* purl) ;
	static void (* FreeSource)(wchar_t* purl) ;
	static FILETIME  (* GetSourceDT)(wchar_t* purl) ;
	static FILETIME  (* GetSourceVER)(wchar_t* purl) ;

public:
	wpp_runtime():localSource(false){}
	~wpp_runtime(){}
public:
	int find_method(wpp_context* context,wchar_t * class_name, wchar_t* method_name);
	int find_method(wpp_context* context,int cls, wchar_t* method_name);
	int call_method(wpp_context* context,int  fn);
	int load_class(wpp_context* ,const wchar_t * urlClass);
	intptr_t create_class(wpp_context* ,int cf);
	void set_nativePointer(wpp_context* ,intptr_t cls,void * p);
	int call_method(wpp_context* context,int  fn,int pthis);
	int call_method(wpp_context* context,void* ctx,int  fn);
public:
	void * prepare_call();
	int push_arg(void* ctx,int param);
	int push_arg(void* ctx,__int64 param);
	int push_arg(void* ctx,unsigned int param);
	int push_arg(void* ctx,unsigned __int64 param);
	int push_arg(void* ctx,float param);
	int push_arg(void* ctx,double param);
	int push_arg(void* ctx,void* param);
public:
	std::vector<void *> * get_vruntime();

public:
	std::vector<wchar_t*> directories;
	bool localSource;
public:
	void UseLocalDirectory(wchar_t * path);
	void SetLocalSource(bool bLocalSource = true);
public:
	static wchar_t* GetFileData(wchar_t* purl) ;
	static void FreeData(wchar_t* purl) ;
	static FILETIME  GetDataDT(wchar_t* purl) ;
	static FILETIME  GetDataVER(wchar_t* purl);

} wpp_runtime_t;

}//namespace wpp


#endif //WPP5_WPP5_RUNTIME_H