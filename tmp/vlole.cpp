#include "vlole.hpp"
#include "vltrap.hpp"
#include "vllog.hpp"
#include "vllogmessage.hpp"
#include "vldebuger.hpp"
#include "vlutils.hpp"
#include "vltrap.hpp"
#include "vlcontext.hpp"

using namespace xlang;

wVARIANT nilVariant;

ITypeLib* vlOle::load_ITypeLib(int nsole)
{
	ITypeLib* ptl = 0;
	wchar_t * szFile = NULL;
	GUID clsid;
	GUID tyid;
	wchar_t buf[255];
	wchar_t val[255];
	wchar_t lpsz[80];
	wchar_t ver[20];
	HKEY keyType; 
	long size = 0;
	
	/*
	GUID guid;
	HRESULT hr = CLSIDFromString((LPOLESTR)nsole,&guid);
	if( hr!=S_OK)
	{
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"无效的ole object %s.\r\n",nsole);
		msg.write();
		return 0;
	};
	
	LPUNKNOWN pUnknown = 0;
	void * pv= 0;
	hr = CoCreateInstance(guid,0,CLSCTX_ALL,IID_IUnknown,(LPVOID *)&pUnknown);
	if( hr!=S_OK)
	{
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"error call CoCreateInstance .\r\n");
		msg.write();
		return 0;
	}
	if(pUnknown)OleRun(pUnknown);
	hr = pUnknown->QueryInterface(IID_ITypeLib,(LPVOID *) &pv);
	pUnknown->Release();
	if( hr==S_OK)
	{
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"query typelib success.\r\n");
		msg.write();
	}else
		return (ITypeLib *)pv;
	if(wcsstr(nsole,L"file:"))
	{
		HRESULT hr = S_FALSE;
		ITypeLib*  ptlib = null;
		hr = LoadTypeLib(nsole + 5,&ptlib);
		if(!FAILED(hr))ptl = ptlib ;
		return ptl;
	}
	*/

	bool regFound = false;
	if(!wcschr(util::getHashString(nsole),','))
	{
		::CLSIDFromProgID(util::getHashString(nsole),&clsid);
		StringFromGUID2(clsid,lpsz,80);
		swprintf_s(buf,255,L"%s%s%s", L"CLSID\\",lpsz, L"\\TypeLib");
		size = 255;
		if(RegQueryValue(HKEY_CLASSES_ROOT,buf,val,&size)==ERROR_SUCCESS)
		{
			regFound = true;
			::CLSIDFromString(val,&tyid);
		}
	}else
	{
		regFound = true;
		::wcscpy_s(val,80,wcschr(util::getHashString(nsole),',') + 1);
	}

	if(regFound)
	{
		swprintf_s(buf,255,L"%s%s", L"TypeLib\\",val);
		::CLSIDFromString(val,&tyid);

		/*
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"typelib key %s.\r\n",buf);
		msg.write();
		*/

		::RegOpenKey(HKEY_CLASSES_ROOT,buf,&keyType);
		for(int i=0; RegEnumKey(keyType,i,ver,20)==ERROR_SUCCESS;i++); 
		::RegCloseKey(keyType);
		wchar_t* pMajorVer,* pMinusVer;
		pMajorVer = ver;
		pMinusVer = wcschr(ver,'.');
		if(pMinusVer)
		{
			pMinusVer[0] = 0;
			pMinusVer++;
		}
		short wMajorVer, wMinusVer= 0;
		if(pMajorVer)wMajorVer = (short)_wtol(pMajorVer);
		if(pMinusVer)wMinusVer  = (short)_wtol(pMinusVer);

		HRESULT hr = S_FALSE;
		ITypeLib*  ptlib = 0;
		hr =  ::LoadRegTypeLib(tyid,wMajorVer,wMinusVer,GetUserDefaultLCID(),&ptlib);
		if(!FAILED(hr))
		{
			ptl = ptlib ;
		}else
		{
			/*vlLogMessage msg(vlLog::get_Log());
			msg.append(L"open typelib fault %s.\r\n",buf);
			msg.write();
			*/
		}
	}else
	{
		size = 255;
		swprintf_s(buf,255,L"%s%s%s", L"CLSID\\",lpsz, L"\\InprocServer32");
		if(RegQueryValue(HKEY_CLASSES_ROOT,buf,val,&size)==ERROR_SUCCESS)
		{
			HRESULT hr = S_FALSE;
			ITypeLib*  ptlib = 0;
			hr = LoadTypeLib(val,&ptlib);
			if(!FAILED(hr))ptl = ptlib ;
		}
	}
	if(!ptl)
	{
		/*vlLogMessage msg(vlLog::get_Log());
		msg.append(L"open typelib fault %s.\r\n",buf);
		msg.write();
		*/
	}
	return ptl;
}

ITypeInfo * vlOle::lookup_ITypeInfo(ITypeLib* ptl, int tk)
{
	ITypeLib * ptlib = ptl;
	const wchar_t * pstr =  util::getHashString(tk);
	ITypeInfo * pti[1];
	MEMBERID  pid[1];
	unsigned short pcFound = 1;
	int nc = 0;
	HRESULT hr = ptlib ->FindName(  (LPOLESTR)pstr,0, pti,pid,&pcFound);
	if(!FAILED(hr) && pcFound > 0 && pid[0]==MEMBERID_NIL)
	{
		TYPEATTR* pTypeAttr;
		hr = pti[0]->GetTypeAttr(&pTypeAttr);
		if(!FAILED(hr))
		{
			if(pTypeAttr->typekind==TKIND_INTERFACE|| pTypeAttr->typekind==TKIND_DISPATCH)
			{
				pti[0]->ReleaseTypeAttr(pTypeAttr);
				return pti[0];
			}
		}
		pti[0]->ReleaseTypeAttr(pTypeAttr);
	}
	return 0;
}

IDispatch * vlOle::OleCreateInstance(const wchar_t * olename)
{
	GUID guid;
	wchar_t prog[255];
	if(wcschr(olename,','))
	{
		wcsncpy_s(prog,255,olename,wcschr(olename,',') - olename);
	}else
		wcscpy_s(prog,255,olename);
		
	HRESULT hr = CLSIDFromString(prog,&guid);
	if( hr!=S_OK)
	{
		wchar_t buf[255];
		swprintf_s(buf,255,L"无效的ole object %s.\r\n",olename);
		//ptr_context->ptr_runtime->get_err_ptr()->output(0,L"error call CoCreateInstance  .\r\n");
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"无效的ole object %s.\r\n",olename);
		msg.write();
		return 0;
	};
	
	LPUNKNOWN pUnknown = 0;
	hr = CoCreateInstance(guid,0,CLSCTX_ALL,IID_IUnknown,(LPVOID *)&pUnknown);
	if( hr!=S_OK)
	{
		//ptr_context->ptr_runtime->get_err_ptr()->output(0,L"error call CoCreateInstance  .\r\n");
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"error call CoCreateInstance .\r\n");
		msg.write();
		return 0;
	}
	if(pUnknown)OleRun(pUnknown);
	IDispatch * pv = 0;

	hr = pUnknown->QueryInterface(IID_IDispatch,(LPVOID *) &pv);
	if( hr!=S_OK)
	{
		//ptr_context->ptr_runtime->get_err_ptr()->output(0,L"error query oleobject interface  .\r\n");
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"error query oleobject dispatch interface  .\r\n");
		msg.write();
		return 0;
	}
	return pv;
}

IDispatch * vlOle::OleCreateInstanceEx(const wchar_t * olename,ITypeInfo * ptInfo)
{
	GUID guid;
	wchar_t prog[255];
	if(wcschr(olename,','))
	{
		wcsncpy_s(prog,255,olename,wcschr(olename,',') - olename);
	}else
		wcscpy_s(prog,255,olename);
		
	HRESULT hr = CLSIDFromString(prog,&guid);
	if( hr!=S_OK)
	{
		wchar_t buf[255];
		swprintf_s(buf,255,L"无效的ole object %s.\r\n",olename);
		//ptr_context->ptr_runtime->get_err_ptr()->output(0,L"error call CoCreateInstance  .\r\n");
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"无效的ole object %s.\r\n",olename);
		msg.write();
		return 0;
	};
	
	LPUNKNOWN pUnknown = 0;
	hr = CoCreateInstance(guid,0,CLSCTX_ALL,IID_IUnknown,(LPVOID *)&pUnknown);
	if( hr!=S_OK)
	{
		//ptr_context->ptr_runtime->get_err_ptr()->output(0,L"error call CoCreateInstance  .\r\n");
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"error call CoCreateInstance .\r\n");
		msg.write();
		return 0;
	}
	if(pUnknown)OleRun(pUnknown);
	IDispatch * pv = 0;

	TYPEATTR* pAttr = 0;
	ptInfo->GetTypeAttr(&pAttr);

	hr = pUnknown->QueryInterface(pAttr->guid,(LPVOID *) &pv);
	ptInfo->ReleaseTypeAttr(pAttr);
	if( hr!=S_OK)
	{
		//ptr_context->ptr_runtime->get_err_ptr()->output(0,L"error query oleobject interface  .\r\n");
		vlLogMessage msg(vlLog::get_Log());
		msg.append(L"error query oleobject interface  .\r\n");
		msg.write();
		return 0;
	}
	if(!pv)
	{
		hr = pUnknown->QueryInterface(IID_IDispatch,(LPVOID *) &pv);
		if( hr!=S_OK)
		{
			//ptr_context->ptr_runtime->get_err_ptr()->output(0,L"error query oleobject interface  .\r\n");
			vlLogMessage msg(vlLog::get_Log());
			msg.append(L"error query oleobject dispatch interface  .\r\n");
			msg.write();
			return 0;
		}
	}
	return pv;
}


HType vlOle::transOleType(void* pMeta, VARTYPE vt)
{
#define Type(x) meta->getTypeHandle(util::hashString(x));

	vlTrap* meta = (vlTrap*)pMeta;
	HType ty = Type(L"undef");
	switch(vt)
	{
	case VT_EMPTY:
	case VT_NULL:
		break;
	case VT_I2:
		ty = Type(L"short");
		break;
	case VT_I4:
		ty = Type(L"int");
		break;
	case VT_R4:
		ty = Type(L"float");
		break;
	case VT_R8:
		ty = Type(L"double");
		break;
	case VT_CY:
	case VT_DATE:
		break;
	case VT_BSTR:
		ty = Type(L"string");
		break;
	case VT_DISPATCH:
		ty = Type(L"oleobject");
		break;
	case VT_ERROR:
		ty = Type(L"int");
		break;
	case VT_BOOL:
		//ty = Type(L"bool");
		ty = Type(L"int");
		break;
	case VT_VARIANT:
		ty = Type(L"variant");
		break;
	case VT_UNKNOWN:
		//ty = type::oleobject_;
		ty = Type(L"ptr");
		break;
	case VT_DECIMAL:
		ty = Type(L"double"); //should error
		break;
	case VT_I1:
		ty = Type(L"byte");
		break;
	case VT_UI1:
		ty = Type(L"ubyte");
		break;
	case VT_UI2:
		ty = Type(L"ushort");
		break;
	case VT_UI4:
		ty = Type(L"uint");
		break;
	case VT_I8:
		ty = Type(L"int64");
		break;
	case VT_INT:
		ty = Type(L"int");
		break;
	case VT_UINT:
		ty = Type(L"uint");
		break;
	case VT_HRESULT:
		ty = Type(L"int");
		break;
	case VT_PTR:
		ty = Type(L"ptr");
		break;
	case VT_SAFEARRAY:
	case VT_CARRAY:
		break;
	case VT_USERDEFINED:
		break; //信息不够
	case VT_LPSTR:
		ty = Type(L"astring");
		break;
	case VT_LPWSTR:
		ty = Type(L"string");
		break;
	case VT_RECORD:
		break;
	case VT_INT_PTR:
	case VT_UINT_PTR:
		ty = Type(L"ptr");
		break;
	case VT_FILETIME:
		break;
	case VT_BLOB:
		ty = Type(L"blob");
		break;
	case VT_STREAM:
	case VT_STORAGE:
	case VT_STREAMED_OBJECT:
	case VT_STORED_OBJECT:
	case VT_BLOB_OBJECT:
		//ty = type::oleobject_;
		ty = Type(L"ptr");
		break;
	case VT_CF:
		break;
	case VT_CLSID:
		ty = Type(L"guid");
		break;
	case VT_VERSIONED_STREAM:
	case VT_BSTR_BLOB:
	case VT_VECTOR:
	case VT_ARRAY:
	case VT_BYREF:
		break;
	}
	return ty;

#undef Type
}

void * vlOle::objectTransEx(void* pContext, void * p ,HType srcType, HType destType)
{
	vlContext * context = (vlContext *)pContext;
	return objectTrans(context->trap,p,srcType,destType);
}

void *  vlOle::objectTrans(void* pMeta,void * p ,HType srcType, HType destType)
{
	vlTrap* meta = (vlTrap*)pMeta;
	bool bAutoTrans = false; 

	if(srcType != destType)
	{
		ITypeInfo * pt1 = (ITypeInfo *)meta->getOleTerm(meta->getType(srcType)->hOleTerm)->typeInfo;
		ITypeInfo * pt2 = (ITypeInfo *)meta->getOleTerm(meta->getType(destType)->hOleTerm)->typeInfo;

		GUID guid;
		TYPEATTR* pAttr1 = 0;
		TYPEATTR* pAttr2 = 0;
		pt1->GetTypeAttr(&pAttr1);
		pt2->GetTypeAttr(&pAttr2);
		if(!IsEqualGUID(pAttr1->guid, pAttr2->guid))
		{
			bAutoTrans=true;
			guid = pAttr2->guid;
		}
		pt1->ReleaseTypeAttr(pAttr1);
		pt2->ReleaseTypeAttr(pAttr2);

		if(bAutoTrans)
		{
			void * p2 = 0;
			((IUnknown*)p)->QueryInterface(guid,(void **)&p2);
			((IUnknown*)p)->Release();
			p = p2;
		}
	}
	return p;
}
