#include "vlwpp_runtime.hpp"
#include "vlplatform.hpp"
#include "vldebuger.hpp"
#include "vlTrap.hpp"
#include "vlparser.hpp"
#include "vlutils.hpp"
#include "vlContext.hpp"

using namespace xlang;
using namespace xlang;

#define __meta ((vlTrap*)((vlContext *)context->get_metatable())->trap) 
//#define __masm ((mill::vill_asm*)context->get_vill_asm())

wchar_t* (* wpp_runtime::GetSource)(wchar_t* purl) = 0 ;
void (* wpp_runtime::FreeSource)(wchar_t* purl) = 0  ;
FILETIME  (* wpp_runtime::GetSourceDT)(wchar_t* purl) = 0  ;
FILETIME  (* wpp_runtime::GetSourceVER)(wchar_t* purl) = 0  ;

std::vector<void *> * wpp_runtime::get_vruntime()
{
	static std::vector<void *> vruntimes;
	return &vruntimes;
}

int wpp_runtime::find_method(wpp_context* context, wchar_t* class_name, wchar_t* method_name)
{
	HType hType = __meta->lookupType(__meta->getGlobalNamespace(),util::hashString(class_name));
	if(hType)
		return __meta->findMethod(hType,util::hashString(method_name)).index;
	return 0;
}

int wpp_runtime::find_method(wpp_context* context, int cls, wchar_t* method_name)
{
	return __meta->findMethod(Handle(cls),util::hashString(method_name)).index;
}

int wpp_runtime::push_arg(void* ctx,int param)
{
	//((mill::vill_context*)ctx)->ptr_runtime->stack_sp--;
	//*(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = param;
	return 1;
}

int wpp_runtime::push_arg(void* ctx,__int64 param)
{
	//((mill::vill_context*)ctx)->ptr_runtime->stack_sp -= 2;
	//*(__int64 *)(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = param;
	return 1;

}
int wpp_runtime::push_arg(void* ctx,unsigned int param)
{
	//((mill::vill_context*)ctx)->ptr_runtime->stack_sp--;
	//*(unsigned int *)(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = param;
	return 1;

}
int wpp_runtime::push_arg(void* ctx,unsigned __int64 param)
{
	//((mill::vill_context*)ctx)->ptr_runtime->stack_sp -= 2;
	//*(unsigned __int64 *)(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = param;
	return 1;
}

int wpp_runtime::push_arg(void* ctx,float param)
{
	//((mill::vill_context*)ctx)->ptr_runtime->stack_sp --;
	//*(float *)(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = param;
	return 1;
}

int wpp_runtime::push_arg(void* ctx,double param)
{
	//((mill::vill_context*)ctx)->ptr_runtime->stack_sp -= 2;
	//*(double *)(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = param;
	return 1;
}

int wpp_runtime::push_arg(void* ctx,void* param)
{
	//((mill::vill_context*)ctx)->ptr_runtime->stack_sp--;
	//*(void **)(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = param;
	return 1;
}


int wpp_runtime::call_method(wpp_context* context,int fn,int pthis)
{
	int rt = 0;
	void * faddr =  __meta->getMethodAddr(Handle(skMethod,fn));
	if(faddr)
	{
		_asm mov ecx,pthis;
		_asm call faddr;
		_asm mov rt,eax;
	}

	/*
	debuger::get_debuger()->enter_method(fn);

	intptr_t v = wpMethod(__meta,wpMethod::getItem(fn)).getAddr(__meta);
	unsigned char * addr = __masm->code + v; 

	mill::vill_context ctx;
	mill::vill_inter inter;
	ctx.ptr_runtime->set_err_ptr(context->get_rrror());
	inter.code = __masm->code;
	inter.pc = __masm->pc;
	
	//this
	ctx.ptr_runtime->stack_sp--;
	*(ctx.ptr_runtime->stack_sp) = pthis; 
	
	//设置返回地址
	ctx.ptr_runtime->stack_sp--;
	*(ctx.ptr_runtime->stack_sp) = 0; //this should exit

	inter.call(&ctx,addr,__masm->pc);
	
	int ret = (*ctx.ptr_runtime->ptr_registeres)[0].val_i32;
	
	return ret;
	*/
	return rt;
}

int wpp_runtime::call_method(wpp_context* context,void* ctx,int fn)
{
	int rt = 0;
	void * faddr =  __meta->getMethodAddr(Handle(skMethod,fn));
	if(faddr)
	{
		_asm mov esi ,esp;
		_asm call faddr;
		_asm mov rt,eax;
		_asm mov esp,esi;
	}

	/*
	debuger::get_debuger()->enter_method(fn);

	int v = wpMethod(__meta,wpMethod::getItem(fn));
	unsigned char * addr = __masm->code + v;

	mill::vill_inter inter;
	((mill::vill_context*)ctx)->ptr_runtime->set_err_ptr(context->get_rrror());
	inter.code = __masm->code;
	inter.pc = __masm->pc;
	
	//设置返回地址
	((mill::vill_context*)ctx)->ptr_runtime->stack_sp--;
	*(((mill::vill_context*)ctx)->ptr_runtime->stack_sp) = 0; //this should exit
	
	inter.call((mill::vill_context*)ctx,addr,__masm->pc);
	
	int ret = (*((mill::vill_context *)ctx)->ptr_runtime->ptr_registeres)[0].val_i32;
	//delete (mill::vill_context*)ctx;
	((mill::vill_context *)ctx)->ptr_runtime->reset();
	get_vruntime()->push_back(ctx);

	return ret;
	*/
	return 1;
}

void * wpp_runtime::prepare_call()
{

	//if(get_vruntime()->size() < 1)return new mill::vill_context();
	//void * p = (*get_vruntime()).back();
	//get_vruntime()->pop_back();
	//return p;
	return 0;
}


int wpp_runtime::call_method(wpp_context* context,int fn)
{
	int rt = 0;
	void * faddr =  __meta->getMethodAddr(Handle(skMethod,fn));
	if(faddr)
	{
		_asm call faddr;
		_asm mov rt,eax;
	}

	/*
	debuger::get_debuger()->enter_method(fn);

	mill::vill_context ctx;
	mill::vill_inter inter;
	ctx.ptr_runtime->set_err_ptr(context->get_rrror());
	inter.code = __masm->code;
	inter.pc = __masm->pc;

	int ret = (*ctx.ptr_runtime->ptr_registeres)[0].val_i32;
	return ret;
	*/
	return rt;
}

int wpp_runtime::load_class(wpp_context* context,const wchar_t * urlClass)
{
	if(!urlClass) return 0;
	int pos = (int)wcslen(urlClass);
	while( pos>=0 && urlClass[pos]!='/')pos--;
	if(pos > 0)
	{
		wchar_t buf[255];
		wcsncpy_s(buf,255,urlClass,pos);
		context->process_file(buf);
	}
	return (int)(unsigned int)__meta->lookupType(__meta->getGlobalNamespace(),util::hashString((const wstring_t)urlClass + pos + 1));
}

intptr_t wpp_runtime::create_class(wpp_context* context,int cf)
{
	HType hType(cf);
	int msize = 0;
	int sz = __meta->getAllocSize(hType,msize) + 4;
	void * p = os_t::heap_alloc(0,sz);
	::memset(p,0,sz);
	*(int *)p = cf; //first point owner
	return (intptr_t)p;
}

void wpp_runtime::set_nativePointer(wpp_context* ,intptr_t cls,void * p)
{
	*(void **)((char *)cls + 4) = p;
}

void wpp_runtime::UseLocalDirectory(wchar_t * path)
{
	directories.push_back(path);
}

void wpp_runtime::SetLocalSource(bool bLocalSource)
{
	localSource = bLocalSource;
	if(bLocalSource)
	{
		GetSource  = GetFileData;
		FreeSource = FreeData;
		GetSourceDT = GetDataDT;
		GetSourceVER = GetDataVER;
	}
}

wchar_t* wpp_runtime::GetFileData(wchar_t* purl)
{
	wchar_t* f = purl;
	HANDLE hFile = ::CreateFile(f,GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
	DWORD nLen = GetFileSize(hFile,0);

	LPBYTE pData = new BYTE[nLen + 2];
	::ReadFile(hFile,pData,nLen,&nLen,NULL);
	pData[nLen] = 0;
	pData[nLen+1] = 0;
	CloseHandle(hFile);

	wchar_t * pStr;
	int ch=0;
	bool bDelete = false;
	if((pData[0]==0xff &&  pData[1]==0xfe)||(pData[0]==0xfe &&  pData[1]==0xff))
	{
		pStr = (LPTSTR)(pData + 2);
	}else if((pData[0]==0xEF &&  pData[1]==0xBB && pData[2]==0xBF))
	{
		int _convert = MultiByteToWideChar(CP_UTF8, 0, (LPSTR)pData+3, nLen, NULL, 0);
		TCHAR * psz = new TCHAR[_convert + 1];
		int nRet = MultiByteToWideChar(CP_UTF8, 0,(LPSTR)pData+3, nLen, psz, _convert);
		psz[_convert]='\0';
		pStr = psz;
		bDelete = true;
	}
	else if(pData[0] !=0 && pData[1]!=0) //ansi should gb2312
	{
		int _convert = MultiByteToWideChar(CP_ACP, 0, (LPSTR)pData, nLen, NULL, 0);
		TCHAR * psz = new TCHAR[_convert + 1];
		int nRet = MultiByteToWideChar(CP_ACP, 0,(LPSTR)pData, nLen, psz, _convert);
		psz[_convert]='\0';
		pStr = psz;
		bDelete = true;
	}else //should utf-16
	{
		pStr = (LPTSTR)pData;
	}
	if(bDelete) delete pData;

	return pStr;	
}

void wpp_runtime::FreeData(wchar_t* purl)
{
	//if(purl) delete purl;
}

FILETIME  wpp_runtime::GetDataDT(wchar_t* purl)
{
	FILETIME ftCreate, ftAccess, ftWrite;
	::ZeroMemory(&ftWrite,sizeof(FILETIME));
	HANDLE hFile = ::CreateFile(purl,GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,
		NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
	if(hFile==INVALID_HANDLE_VALUE)return ftWrite;
		
    GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite) ;
	CloseHandle(hFile);
	return ftWrite;
 
}
FILETIME  wpp_runtime::GetDataVER(wchar_t* purl)
{
	FILETIME ft;
	::ZeroMemory(&ft,sizeof(FILETIME));
	return ft;

}

