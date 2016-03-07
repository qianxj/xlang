#ifndef WPP5_WOLE_H
#define WPP5_WOLE_H

//#include "type.hpp"
#include "vlplatform.hpp"
#include "vlsym.hpp"

//ole support
namespace xlang
{

extern struct wVARIANT {
	VARIANT v;
	wVARIANT(){
		::VariantClear(&v);
		v.vt = VT_ERROR;
		v.scode = DISP_E_PARAMNOTFOUND;
	};
} nilVariant;



class vlOle
{
public:
	static HType transOleType(void* pMeta,VARTYPE vt);
	
public:
	static ITypeLib* load_ITypeLib(int nsole);
	static ITypeInfo * lookup_ITypeInfo(ITypeLib* ptl, int tk);
	static IDispatch * OleCreateInstance(const wchar_t * olename);
	static IDispatch * OleCreateInstanceEx(const wchar_t * olename,ITypeInfo * ptInfo);
public:
	static void * objectTrans(void* pMeta, void * p ,HType srcType, HType destType);
	static void * objectTransEx(void* pContext, void * p ,HType srcType, HType destType);
};

}; //namespace xlang

#endif