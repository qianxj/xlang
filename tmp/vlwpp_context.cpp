#include "vlwpp_context.hpp"
#include "vldebuger.hpp"
#include "vlwpp_runtime.hpp"
#include "vlplatform.hpp"
#include "vlutils.hpp"
#include "vlparser.hpp"
#include "vltrap.hpp"

using namespace xlang;
using namespace xlang;

//#define __meta ((wpSymtable_t*)get_metatable())
//#define __masm ((mill::vill_asm*)get_vill_asm())

map<wstring,wpp_context::sourcetime_t>* wpp_context::sources =
		new map<wstring,wpp_context::sourcetime_t>;
wpp_context::wpp_context(wpp_runtime* runtime):/*ptr_vill_asm_(new mill::vill_asm),
	ptr_metatable_(new wpSymtable_t),*/files(new map<wstring,FILETIME>),
	ptr_runtime_(runtime),errPtr(0),ptr_metatable_(new vlContext)
{
	((vlContext *)ptr_metatable_)->wppcontext = this;
	//__masm->exit(); 

}

wpp_context::~wpp_context()
{
	delete (vlContext*)ptr_metatable_;
	//delete ptr_vill_asm_;
	delete  files;
}

void * wpp_context::get_vill_asm()
{
	return this->ptr_vill_asm_;
}

void * wpp_context::get_metatable()
{
	return this->ptr_metatable_;
}
void wpp_context::set_vill_asm(void * ptr_code)
{
	this->ptr_vill_asm_ = ptr_code;
}
void wpp_context::set_metatable(void * ptr_metatable)
{
	this->ptr_metatable_ = ptr_metatable;
}

Wpp5* wpp_context::get_wpp_ptr()
{
	return ptr_wpp_;
}

void wpp_context::set_wpp_ptr(Wpp5* ptr_wpp)
{ 
	ptr_wpp_ = ptr_wpp;
}

wpp_runtime* wpp_context::get_runtime()
{
	return ptr_runtime_;
}

void wpp_context::set_runtime(wpp_runtime* ptr_runtime)
{
	ptr_runtime_ = ptr_runtime;
}

int wpp_context::process_file(const wchar_t* file)
{
	wchar_t buf[255];
	int pos = 0;
	while(file[pos] && file[pos]!=':')pos++;
	if(file[pos]!=':')
	{
		swprintf_s(buf,255,L"dev:vl[%s]",file);
		file = buf;
	}

	wchar_t path[255];
	::GetModuleFileName(NULL,path,MAX_PATH);
	::PathRemoveFileSpec(path);
	wchar_t lfile[MAX_PATH];
	::swprintf_s(lfile,MAX_PATH,L"%s\\cache\\code\\%s",path,file+4);

	int nstep = 0;
	if(get_runtime()->GetSource)
	{
		bool toload = false;
		FILETIME ft = get_runtime()->GetSourceDT((wchar_t*)file);
		map<wstring,FILETIME>::iterator tr = files->find(file);
		if(tr==files->end())
		{
			pair<wstring,FILETIME> pr;
			pr.first = file;
			pr.second = ft;
			files->insert(pr);

			toload = true;
			//nstep = 1;
			nstep = 3;
		}else
		{
			FILETIME oft = tr->second;
			LONG  ret = CompareFileTime(&oft, &ft);
			if(ret==-1)
			{						
				toload = true;
				tr->second = ft;
				//this->setFileDirty(f);
				nstep = 2;
			}
		}

		if(toload)
		{
			if(nstep==1)
			{
				HANDLE hFile = ::CreateFile(lfile,GENERIC_READ,FILE_SHARE_WRITE|FILE_SHARE_READ|FILE_SHARE_DELETE,
					NULL,OPEN_EXISTING,FILE_ATTRIBUTE_READONLY,NULL);
				if(hFile!=INVALID_HANDLE_VALUE)
				{
					FILETIME lpCreationTime;	// 文件夹的创建时间
					FILETIME lpLastAccessTime; // 对文件夹的最近访问时间
					FILETIME lpLastWriteTime; // 文件夹的最近修改时间
					// 获取文件夹时间属性信息
					if (GetFileTime(hFile, &lpCreationTime, &lpLastAccessTime, &lpLastWriteTime))
					{
						LONG  ret = 1;//CompareFileTime(&lpLastWriteTime, &ft);
						unsigned int  nlen = ::GetFileSize(hFile,NULL);
						wchar_t * pStr = new wchar_t[nlen/sizeof(wchar_t)];
						DWORD dw = 0;
						pStr[ nlen / sizeof(wchar_t) - 1] = 0; 
						if(ret != -1)
						{
							LONG dl = 2;
							SetFilePointer(hFile,dl,NULL,FILE_BEGIN);
							::ReadFile(hFile,pStr,nlen, &dw,NULL);
							::CloseHandle(hFile);
							parse(pStr);
							delete pStr;
							return 1;
						}
					}
					::CloseHandle(hFile);
				}
			}

			wchar_t * pStr = 0;
			if(sources->find(file) != sources->end())
			{
				FILETIME oft = (*sources)[file].dt;
				LONG  ret = CompareFileTime(&oft, &ft);
				if(ret < 0)
				{
					pStr = get_runtime()->GetSource((wchar_t*)file);
					(*sources)[file].text = pStr;
					(*sources)[file].dt = ft;
				}
				pStr = (wchar_t *)(*sources)[file].text.c_str();
			}else
			{
				pStr = get_runtime()->GetSource((wchar_t*)file);
				if(pStr)
				{
					sourcetime_t sourcetime ={ft,pStr};
					sources->insert(pair<wstring,sourcetime_t>(file,sourcetime));
					get_runtime()->FreeSource(pStr);
					pStr = (wchar_t *)(*sources)[file].text.c_str();
				}
			}
			if(pStr)
			{
				parse(pStr);
			}
		}
	}
	return 1;
}

wchar_t* wpp_context::fetch_file(const wchar_t* file)
{
	wchar_t buf[255];
	int pos = 0;
	if(!this->get_runtime()->localSource)
	{
		while(file[pos] && file[pos]!=':')pos++;
		if(file[pos]!=':')
		{
			swprintf_s(buf,255,L"dev:vl[%s]",file);
			file = buf; 
		}
	}

	wchar_t path[255];
	::GetModuleFileName(NULL,path,MAX_PATH);
	::PathRemoveFileSpec(path);
	wchar_t lfile[MAX_PATH];
	::swprintf_s(lfile,MAX_PATH,L"%s\\cache\\code\\%s",path,file+4);

	int nstep = 0;
	if(get_runtime()->GetSource)
	{
		bool toload = false;
		FILETIME ft =get_runtime()->GetSourceDT((wchar_t*)file);
		map<wstring,FILETIME>::iterator tr = files->find(file);
		if(tr==files->end())
		{
			pair<wstring,FILETIME> pr;
			pr.first = file;
			pr.second = ft;
			files->insert(pr);

			toload = true;
			//nstep = 1;
			nstep = 3;
		}else
		{
			FILETIME oft = tr->second;
			LONG  ret = CompareFileTime(&oft, &ft);
			if(ret==-1)
			{						
				toload = true;
				tr->second = ft;
				//this->setFileDirty(f);
				nstep = 2;
			}
		}
		if(toload)
		{
			wchar_t * pStr = 0;
			if(sources->find(file) != sources->end())
			{
				FILETIME oft = (*sources)[file].dt;
				LONG  ret = CompareFileTime(&oft, &ft);
				if(ret == 0)
				{
					pStr = get_runtime()->GetSource((wchar_t*)file);
					(*sources)[file].text = pStr;
					(*sources)[file].dt = ft;
				}else
				{
					pStr = get_runtime()->GetSource((wchar_t*)file);
					if(pStr)
					{
						sourcetime_t sourcetime ={ft,pStr};
						(*sources)[file] = sourcetime;
						get_runtime()->FreeSource(pStr);
					}
				}
				pStr = (wchar_t *)(*sources)[file].text.c_str();
			}else
			{
				pStr = get_runtime()->GetSource((wchar_t*)file);
				if(pStr)
				{
					sourcetime_t sourcetime ={ft,pStr};
					sources->insert(pair<wstring,sourcetime_t>(file,sourcetime));
					get_runtime()->FreeSource(pStr);
					pStr = (wchar_t *)(*sources)[file].text.c_str();
				}
			}
			return pStr;
		}
	}
	return 0;
}


wErr*  wpp_context::get_error()
{
	return errPtr ;
}

void wpp_context::set_error(wErr* errPtr_)
{
	errPtr = errPtr_;
}



bool wpp_context::parse(wchar_t *src)
{
	struct __
	{
		static void ErrorMsg(wpp_context* pThis)
		{
			std::wstring msg;
			//msg += (std::wstring)L"apply nill node ";
			vlContext * context = (vlContext *)pThis->get_metatable();
			if(context->stackItems.size())
				msg += (wstring)L"  apply '" + context->stackItems.back().ide +L"'.";
			msg = L"\r\nException raised:\r\n" + msg;
			msg += (wstring)L"\r\ncall stack:";
			for(int i = (int)context->stackItems.size()- 1;i>=0;i--)
			{
				msg += (std::wstring)L"\r\napply " + context->stackItems[i].ide;
			}
			if(pThis->get_error())
				pThis->get_error()->output(0,msg.c_str());
			else
				wprintf(L"%s",msg.c_str());  
		}

		static int Filter(unsigned int code, struct _EXCEPTION_POINTERS *ep)
		{
			if (code == EXCEPTION_ACCESS_VIOLATION)
			{
				//wprintf(L"Caught access violation expected.\r\n");
				MessageBox(NULL, L"程序出现空指针调用.\r\n",L"错误",MB_OK|MB_ICONERROR);
				
				//LogMessage msg(Log::get_Log());
				//msg.appendString(L"检查一下，程序中有空指针调用.\r\n");
				//msg.appendString(debuger::get_debuger()->get_method_stack_string().c_str());
				//msg.write();

				return EXCEPTION_EXECUTE_HANDLER;
			}
			else
			{
				MessageBox(NULL, L"出现一个意外了...\r\n",L"错误",MB_OK|MB_ICONERROR);

				//LogMessage msg(Log::get_Log());
				//msg.appendString(L"出现一个意外了...\r\n");
				//msg.appendString(debuger::get_debuger()->get_method_stack_string().c_str());
				//msg.write();

				return EXCEPTION_EXECUTE_HANDLER;
				//return EXCEPTION_CONTINUE_SEARCH;
			}
		}
	};

	bool ret = false;
	__try
	{
		ret = this->get_wpp_ptr()->parse(this,src);
	}
	__except(__::Filter(GetExceptionCode(), GetExceptionInformation()))
	{
		__::ErrorMsg(this);
	}
	//ret = this->get_wpp_ptr()->parse(this,src); 
	return ret; 
}

int	wpp_context::get_current_class()  
{
	/*
	wp::hitem_t t = __meta->getContext()->get_scope_top();
	if(__meta->isVariant(t)) return t.index;

	if(__meta->isMethod(t))
	{
		wp::hitem_t p = __meta->getParent(t);
		if( p && __meta->isClass(p)) return p.index;
	}
	*/

	return 0;
}

int wpp_context::get_current_method()
{
	//wp::hitem_t t = __meta->getContext()->get_scope_top();
	//if(__meta->isMethod(t)) return t.index;
	return 0;
}

void wpp_context::GetClassMethodString(int pclass, wchar_t* fct, std::set<std::wstring> * & pSet)
{
#if 0
	wpClass wc(__meta,wpClass::getItem(pclass));
	void* typeInfo = wc.getOleTypeInfo(__meta);
	bool bole = typeInfo?true:false;

	if(bole)
	{
		ITypeInfo * pti = (ITypeInfo *)typeInfo ;
		if(pti != 0)
		{
			TYPEATTR* pTypeAttr;
			TYPEDESC* pVar;
			VARTYPE VarType;
			pti->GetTypeAttr(&pTypeAttr);
			std::wstring  arg;

			unsigned short pcFound = 1;
			MEMBERID  pid[1];
			HRESULT hr = pti ->GetIDsOfNames(  &fct,1,pid);
			if(!FAILED(hr))
			{
				FUNCDESC* pFuncDesc = null;
				int nIndexGet = 0;
				int nIndexPut = 0;
				for (int i=0;i<pTypeAttr->cFuncs;i++)
				{
					hr = pti->GetFuncDesc(i, &pFuncDesc);
					if (FAILED(hr)) break;
					if (pFuncDesc->memid == pid[0])
					{
						nIndexGet = i + 1;
						if(pFuncDesc->invkind==INVOKE_PROPERTYGET && i + 1< pTypeAttr->cFuncs)
						{
							FUNCDESC* pFuncDesc1 = null;
							pti->GetFuncDesc(i, &pFuncDesc1);
							if (pFuncDesc1->memid == pid[0])  nIndexPut = i+ 2;
						}
						break;
					}
					pti->ReleaseFuncDesc(pFuncDesc);
					pFuncDesc = NULL;
				}
				
				if(pFuncDesc)
				{
					arg = L"";
					BSTR  bstrName[32] ;
					UINT      nNames[32];
					pti->GetNames( pFuncDesc->memid, bstrName, 32, nNames );
					for (int i = -1; i < pFuncDesc->cParams; i++)
					{
						
						if(i>=0)
						{
							pVar =  &pFuncDesc->lprgelemdescParam[i].tdesc;
							VarType = pVar->vt;
						}else
						{
							pVar = &pFuncDesc->elemdescFunc.tdesc;
         							VarType = pVar->vt;
						}
						if(i>0) arg+=L", ";
						if(VarType==VT_PTR)
						{
							VarType = pVar->lptdesc->vt;
							pVar 	    = pVar->lptdesc;
							arg +=L"ref ";
						}
						switch(VarType)
						{
						case VT_EMPTY:
							arg +=L"empty ";
							break;
						case VT_NULL:
							arg +=L"null ";
							break;
						case VT_I2:
							arg +=L"int16 ";
							break;
						case VT_I4:
							arg +=L"int ";
							break;
						case VT_R4:
							arg +=L"float ";
							break;
						case VT_R8:
							arg +=L"double ";
							break;
						case VT_CY:
							arg +=L"currency ";
							break;
						case VT_DATE:
							arg +=L"date ";
							break;
						case VT_BSTR:
							arg +=L"bstr ";
							break;
						case VT_DISPATCH:
							arg +=L"IDispatch ";
							break;
						case VT_ERROR:
							arg +=L"int ";
							break;
						case VT_BOOL:
							arg +=L"bool ";
							break;
						case VT_VARIANT:
							arg +=L"variant ";
							break;
						case VT_UNKNOWN:
							arg +=L"IUnknown ";
							break;
						case VT_DECIMAL:
							arg +=L"decimal ";
							break;
						case VT_I1:
							arg +=L"int8 ";
							break;
						case VT_UI1:
							arg +=L"uint8 ";
							break;
						case VT_UI2:
							arg +=L"uint16 ";
							break;
						case VT_UI4:
							arg +=L"uint ";
							break;
						case VT_I8:
							arg +=L"int64 ";
							break;
						case VT_INT:
							arg +=L"int ";
							break;
						case VT_UINT:
							arg +=L"uint ";
							break;
						case VT_HRESULT:
							arg +=L"hresult ";
							break;
						case VT_PTR:
							arg +=L"ptr ";
							break;
						case VT_SAFEARRAY:
							arg +=L"safearray ";
							break;
						case VT_CARRAY:
							arg +=L"carray ";
							break;
						case VT_USERDEFINED:
						{
							ITypeInfo* pTypeInfo;
							HREFTYPE hreftype = pVar->hreftype;
							pti->GetRefTypeInfo(hreftype,&pTypeInfo);
							
							TYPEATTR* pTypeAttr;
							HRESULT hr = pti->GetTypeAttr(&pTypeAttr);
							if(!FAILED(hr))
							{
								if(pTypeAttr->typekind==TKIND_INTERFACE|| pTypeAttr->typekind==TKIND_DISPATCH)
								{
									BSTR tname= null;
									pTypeInfo->GetDocumentation(MEMBERID_NIL,&tname,null,null,null);
									arg +=tname;
									::SysFreeString(tname);
								}
							}
							pTypeInfo->ReleaseTypeAttr(pTypeAttr);
							break;
						}
						case VT_VOID:
							arg +=L"void ";
							break;
						case VT_LPSTR:
							arg +=L"astring ";
							break;
						case VT_LPWSTR:
							arg +=L"wstring ";
							break;
						case VT_RECORD:
							arg +=L"record ";
							break;
						case VT_INT_PTR:
							arg +=L"intPtr ";
							break;
						case VT_UINT_PTR:
							arg +=L"uintPtr ";
							break;
						case VT_FILETIME:
							arg +=L"filetime ";
							break;
						case VT_BLOB:
							arg +=L"blob ";
							break;
						case VT_STREAM:
							arg +=L"IStrem ";
							break;
						case VT_STORAGE:
							arg +=L"IStorage ";
							break;
						case VT_STREAMED_OBJECT:
							arg +=L"IStremObject ";
							break;
						case VT_STORED_OBJECT:
							arg +=L"IStoredObject ";
							break;
						case VT_BLOB_OBJECT:
							arg +=L"IBlobObject ";
							break;
						case VT_CF:
							arg +=L"CF ";
							break;
						case VT_CLSID:
							arg +=L"guid ";
							break;
						case VT_VERSIONED_STREAM:
							arg +=L"IVersionStream ";
							break;
						case VT_BSTR_BLOB:
							arg +=L"bstrblob ";
							break;
						case VT_VECTOR:
							arg +=L"vector ";
							break;
						case VT_ARRAY:
							arg +=L"array ";
							break;
						case VT_BYREF:
							arg +=L"& ";
							break;
						}
						if(i<0)
						{
							arg +=L" " ;
							arg += (wstring)fct +L"(";
						}else
						{
							arg += bstrName[i + 1];
							::SysFreeString(bstrName[i + 1]);
						}
					}
					pSet->insert(arg+L" )");
					pti->ReleaseFuncDesc(pFuncDesc);
				}
			}
		}
	}
	else
	{
		wpClass::extendIterator iter = wc.extend_begin(__meta);
		while(iter)
		{
			GetClassMethodString(iter->typelist,fct, pSet);
			iter++;
		}

		wpClass::methodIterator it = wc.method_begin(__meta);
		while(it)
		{
			int ide = util::hashString(fct);
			if(it->name == ide)
			{
				type_t typ = wpMethod(&*it).getType(__meta);
				const wchar_t * ptype = (typ> 0 && typ>>8) ? wpMethod(__meta,wpMethod::getItem(typ>>8)).getName() :
					type::get_type_string(typ);
				wchar_t buf[255];
				_stprintf_s(buf,255,L"%s %s(",ptype,wpMethod(&*it).getName());
				int args = 0;
				bool bfirst = true;
				wpMethod::paramIterator iter_arg = wpMethod(&*it).param_begin(__meta);
				while(iter_arg)
				{   
					wpParam wa(&*iter_arg);
					const wchar_t * paname = wa.getName();
					type_t typ = wa.getType(__meta);
					const wchar_t *  ptype = (typ> 0 && typ>>8) ?  wpClass(__meta,wpClass::getItem(typ>>8)).getName():
											type::get_type_string(typ);
					wchar_t buf1[255];
					if(bfirst)
					{
						if(paname)
							_stprintf_s(buf1,255,L"%s %s",ptype,paname);
						else
							_stprintf_s(buf1,255,L"%s",ptype);
						bfirst = false;
					}else
					{
						if(paname)
							_stprintf_s(buf1,255,L", %s %s",ptype,paname);
						else
							_stprintf_s(buf1,255,L", %s",ptype);
					}
					_tcscat_s(buf,255,buf1);
					iter_arg++;
				}
				_tcscat_s(buf,255,L")");
				pSet->insert(buf);
			}
			it++;
		}
	}
#endif
}

void wpp_context::GetNamespaceAutoString(int item, std::set<std::wstring> * & pSet)
{
#if 0
	wpNameSpace ns(__meta,wpClass::getItem(item));
	if(ns.isOleItem())
	{
		const wchar_t* atole = ns.getOleName(get_metatable());
		if(atole)    //load ole class
		{
			ITypeLib* ptl = (ITypeLib*)ns.getOleTypeLib(__meta);
			if(!ptl)
			{
				ptl = wole_t::load_ITypeLib(atole);
				//set typelib to back
				ns.setOleTypeLib(__meta,ptl);
			}
			if(ptl)
			{
				for (int i = 0/*-1*/; i < (int)ptl->GetTypeInfoCount(); i++)
				{
					BSTR bstrName = null;
					ptl->GetDocumentation(i, &bstrName, NULL, NULL, NULL);
					pSet->insert(bstrName);
					SysFreeString(bstrName);
				}
			}
		}
	}else
	{
		wpNameSpace::childIterator iter = ns.child_begin(__meta);
		while(iter)
		{
			if(__meta->isClass(iter->node))
			{
				pSet->insert(wpClass(__meta,iter->node).getName());
			}else if(__meta->isNamespace(iter->node))
			{
				pSet->insert(wpNameSpace(__meta,iter->node).getName());
			}else if(__meta->isMethod(iter->node))
			{
				pSet->insert(wpMethod(__meta,iter->node).getName());
			}else
			{
			}

			iter++;
		}
	}
#endif
}

void wpp_context::GetClassAutoString(int pclass, std::set<std::wstring> * & pSet)
{
#if 0
	wpClass wc(__meta,wpClass::getItem(pclass));
	void* typeInfo = wc.getOleTypeInfo(__meta);
	bool bole = typeInfo?true:false;
	if(bole)
	{
		ITypeInfo * pti = (ITypeInfo *) typeInfo;
		if(pti != 0)
		{
			TYPEATTR* pAttr;
			FUNCDESC* pFunc;
			pti->GetTypeAttr(&pAttr);
			for(int i=0;i<(int)pAttr->cFuncs;i++)
			{
				pti->GetFuncDesc(i,&pFunc);
				BSTR bstr = null;
				pti->GetDocumentation(pFunc->memid , &bstr,null,null,null);
				pSet->insert((std::wstring)bstr);
				SysFreeString(bstr);
				pti->ReleaseFuncDesc(pFunc);
			}
			/*for(int i=0;i<pAttr->cVars;i++)
			{
				pti->GetVarDesc(i,&pVar);
				BSTR bstr = null;
				pti->GetDocumentation(pVar->memid , &bstr,null,null,null);
				pSet->insert((std::wstring)bstr);
				SysFreeString(bstr);
				pti->ReleaseVarDesc(pVar);
			}*/
			pti->ReleaseTypeAttr(pAttr);
		}
	}else
	{
		wpClass::extendIterator super = wc.extend_begin(__meta);
		while(super)
		{
			GetClassAutoString(super->typelist,pSet);
			super++;
		}

		wpClass::methodIterator iter = wc.method_begin(__meta);
		while(iter )
		{
			const wchar_t* pmname = wpMethod(&*iter).getName();
			pSet->insert(pmname);
			iter++;
		}

		wpClass::fieldIterator it = wc.field_begin(__meta);
		while(it )
		{
			const wchar_t* pmname = wpField(&*it).getName();
			pSet->insert(pmname);
			it++;
		}
	}
#endif
}

wchar_t* wpp_context::getVarAutoString(wchar_t* v)
{
#if 0
	//need current method;
	int pclass = 0;
	int pnamespace = 0;
	if(_tcscmp(v,L"_global")==0)
	{

	}
	else if(_tcscmp(v,L"this")==0)
	{
		pclass = get_current_class();
	}
	else
	{
		if(v && v[_tcslen(v) -1]=='"')
			pclass = __meta->getContext()->lookup(L"xstring").index;
		else if(v && (v[0]>='0' && v[0]<='9'))
			pclass = __meta->getContext()->lookup(L"xint").index;
		else
		{
			int mt = this->get_current_method();
			wp::hitem_t t = __meta->getContext()->lookup(v);
			if(__meta->isVariant(t) ||
				__meta->isField(t) ||
				__meta->isParam(t))
			{
				int ty = 0;
				if(__meta->isVariant(t)) ty = wpVariant(__meta,t).getType(__meta);
				else if(__meta->isParam(t)) ty = wpParam(__meta,t).getType(__meta);
				else if(__meta->isField(t)) ty = wpField(__meta,t).getType(__meta);
				if(ty>>8  && (ty & 0xff) == type::class_)
					pclass = ty>>8;
			}else if(__meta->isClass(t))
				pclass = t.index;
			else if(__meta->isNamespace(t))
			{
				pnamespace = t.index;
			}
		}
	}

	if(pnamespace || pclass)
	{
		std::set<std::wstring> * pSet = new std::set<std::wstring>;
		if(pclass)
			GetClassAutoString(pclass,pSet);
		else
			GetNamespaceAutoString(pnamespace,pSet);

		wstring ws;
		std::set<std::wstring>::const_iterator tr;
		for(tr=pSet->begin();tr!=pSet->end();tr++)
		{
			if(tr==pSet->begin())
				ws=*tr;
			else
				ws += L" " + *tr;
		}
		delete pSet;
		return _tcsdup(ws.c_str());
	}
	return null;
#endif
	return 0;
}

wchar_t * wpp_context::getFctTipString(wchar_t* v)
{	
#if 0
	wchar_t fct[128];
	wchar_t sclass[128];

	int pclass = 0;
	if(get_current_method())
	{
		fct[0] = 0 ;

		const wchar_t *pos1 = _tcschr(v,'.');
		if(!pos1) pos1 = _tcschr(v,':');
		const wchar_t *pos2 = _tcschr(v,'(');
		if(pos1 > 0 && pos1 < pos2)
		{
			if(pos1[0]=='.')
				_tcsncpy_s(fct,128,pos1 + 1,pos2 - pos1 - 1);
			else
			{
				_tcsncpy_s(fct,128,pos1 + 1 + 1 ,pos2 - pos1 - 1 - 1);
			}

			_tcsncpy_s(sclass,128,v,pos1 - v); 

			if(_tcscmp(sclass,L"this")==0)
			{
				pclass = get_current_class();
			}else
			{
				wp::hitem_t t = __meta->getContext()->lookup(sclass);
				if(__meta->isVariant(t) ||
					__meta->isField(t) ||
					__meta->isParam(t))
				{
					int ty = 0;
					if(__meta->isVariant(t)) ty = wpVariant(__meta,t).getType(__meta);
					else if(__meta->isParam(t)) ty = wpParam(__meta,t).getType(__meta);
					else if(__meta->isField(t)) ty = wpField(__meta,t).getType(__meta);
					if(ty>>8  && (ty & 0xff) == type::class_)
						pclass = ty>>8;
				}else if(__meta->isClass(t))
					pclass = t.index;
			}

		} else if(pos1 <0)
		{
			_tcsncpy_s(fct,128,v ,pos2 - v);
			pclass = get_current_class();
		}

		if(pclass && fct[0])
		{
			std::set<std::wstring> * pSet = new std::set<std::wstring>;
			GetClassMethodString(pclass,fct, pSet);

			wstring ws;
			std::set<std::wstring>::const_iterator tr;
			for(tr=pSet->begin();tr!=pSet->end();tr++)
			{
				if(tr==pSet->begin())
					ws=*tr;
				else
					ws += L"\r\n" + *tr;
			}
			delete pSet;
			return _tcsdup(ws.c_str());
		}
	}

	return null;   
#endif
	return 0;
}

void wpp_context::freeAutoString(wchar_t * p)
 {
	 free(p);
}


