#include "vltrap.hpp"
#include "vlutils.hpp"
#include <assert.h>
#include "vlplatform.hpp"
#include "vlole.hpp"
#include "vlcontext.hpp"
#include "xstring.hpp"
#include "vlparser.hpp"
#include "vlapply.hpp"

using namespace xlang;

static vlAstAxis * Unify_apply(vlContext * context,vlParser * parser,vlApply * apply ,vlAstAxis * anode)
{
	return parser->unify_apply(context,apply,anode);
}

static void * JitMethod(vlContext * context,HMethod hMethod)
{
	vlParser parser;
	vlAstAxis * applyRhs = 0;
	std::map<int,vlAstAxis*>  paramBind;
	vlApply* apply = context->applysvr.getApply(util::hashString(L"wpp_complier"));
	if(!apply)
	{
		throw("\n未找到处理器 wpp_complier!");
	}

	std::multimap<int,int /*index*/ >::iterator lIter;
	int ide = util::hashString(L"JitMethod");
	
	if(apply->getMap().find(ide) != apply->getMap().end())
	{
		lIter = apply->getMap().lower_bound(ide);
		applyRhs = apply->getItem(lIter->second).code;
	}
	if(!applyRhs)
	{
		throw("\n未指定apply JitMethod( hmethod )!");
	}
	paramBind.insert(std::pair<int,vlAstAxis*>(util::hashString(L"hmethod"),apply->makeIntegerToken(context->getAstFactory(),hMethod)));
	
	context->contexts.push_back(util::hashString(L"context-method-codegenex"));
	context->enterUnifyApply(util::getHashString(ide),lIter->second,
		apply->getItem(lIter->second).varCount);
	apply->eval(applyRhs,&parser,context,0,paramBind);
	context->leaveUnifyApply();
	return context->trap->getMethod(hMethod)->addr;
}

static int WarnToCall(int errCode, vlContext * context,HMethod hMethod, HMethod hCall,HMethod hType)
{
	std::wstring str;
	if(errCode==3)
		str = L"\r\nwarn! 空的对象调用函数";
	else if(errCode==4)
		str = L"\r\nwarn! 本地函数返回空指针";
	else 
		str = L"\r\nwarn! 未识别的错误";
	if(hType)
		str += (std::wstring)context->trap->getTermName(hType)+L" ";
	str += (std::wstring)context->trap->getTermName(hCall);
	str += (std::wstring)L" at " +  context->trap->getTermName(context->trap->getParent(hMethod)) +
		L"::" + context->trap->getTermName(hMethod);
	
	if(context->wppcontext->get_error())
		context->wppcontext->get_error()->output(0,str.c_str());
	else
		os_t::print(L"%s",str.c_str());
	return 0;
}

static int OutputText(vlContext * context,wchar_t* msg)
{
	if(context->wppcontext->get_error())
		context->wppcontext->get_error()->output(0,msg);
	else
		os_t::print(L"%s",msg);
	return 0;
}

void * __stdcall GetMethodAddr(SymTable* table, HMethod hMethod)
{
	assert(table);
	assert(hMethod.index);
	return table->methods[hMethod.index -1].addr;
}

wchar_t * __stdcall LocalStringAdd(wchar_t * lhs, wchar_t * rhs)
{
	if(!lhs && !rhs) return _wcsdup(L"");
	if(!lhs) return _wcsdup(rhs);
	if(!rhs) return _wcsdup(lhs);

	int len = (int)wcslen(lhs) + (int)wcslen(rhs) + 1;
	wchar_t * str = (wchar_t *)malloc(len * sizeof(wchar_t));
	str[0] = 0;
	wcscat(str,lhs);
	wcscat(str,rhs);
	return str;
}

void* FindMatchVirtualMethodAddr(void* pContext,Handle wc, Handle wm)
{
	vlContext * context = (vlContext *)pContext;
	HMethod h = context->trap->findMatchAsMethod(wc,wm);
	return context->trap->getMethodAddr(h); 
}


vlTrap::vlTrap() : coder(new vlCoder)
{
	initial();

	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"LoadProcAddress"),
	os_t::load_module));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"GetMethodAddr"),
	GetMethodAddr));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"StringAdd"),
	LocalStringAdd));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"alloc"),
	malloc));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"calloc"),
	calloc));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"free"), 
	free));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"memcpy"), 
	memcpy));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"memset"), 
	memset));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"memmove"), 
	memmove));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"wcscmp"),    
		xstring::stringCmp));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"SysAllocString"), 
	SysAllocString));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"SysFreeString"), 
	SysFreeString));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"AutoTransOleObject"), 
		vlOle::objectTransEx));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"FindMatchVirtualMethodAddr"), 
		FindMatchVirtualMethodAddr));
	
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"OleCreateInstanceEx"),
		vlOle::OleCreateInstanceEx));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"OleCreateInstance"),
		vlOle::OleCreateInstance));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"MessageBox"),
	os_t::load_module(L"user32.dll",L"MessageBoxW"))); 
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"JitMethod"),
		JitMethod));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"WarnToCall"),
		WarnToCall));
	this->mFunctions.insert(std::pair<int,void*>(util::hashString(L"OutputText"),
		OutputText));
}

vlTrap::~vlTrap()
{
	if(coder) delete coder;
}

void vlTrap::initial()
{
	SymNamespace ns;
	ns.name = util::hashString(L"__global__");
	ns.parent = 0;
	symTable.namespaces.push_back(ns);

	struct primaryType
	{
		int	name; 
		int	size;
		TypeKind	kind;
		TypeCatKind	catKind;	
	} ptypes[] = { 
		{util::hashString(L"unknown"),	 0,		 		tkUnknown,tcPrimitive},
		{util::hashString(L"undef"),	 0,				tkUnknown,tcPrimitive},
		{util::hashString(L"nilltyp"),	 0,				tkUnknown,tcPrimitive},
		{util::hashString(L"void"),		 0,				tkUnknown,tcPrimitive},
		{util::hashString(L"int8"),	 sizeof(int),		tkUnknown,tcPrimitive},
		{util::hashString(L"int"),	 sizeof(int),		tkUnknown,tcPrimitive},
		{util::hashString(L"double"),sizeof(double),	tkUnknown,tcPrimitive},
		{util::hashString(L"float"), sizeof(float),		tkUnknown,tcPrimitive},
		{util::hashString(L"short"), sizeof(short),		tkUnknown,tcPrimitive},
		{util::hashString(L"byte"),  sizeof(char),		tkUnknown,tcPrimitive},
		{util::hashString(L"char"),	 sizeof(wchar_t),	tkUnknown,tcPrimitive},
		{util::hashString(L"string"),sizeof(wchar_t*),	tkUnknown,tcPrimitive},
		{util::hashString(L"bool"),	 sizeof(char),		tkUnknown,tcPrimitive},
		{util::hashString(L"uint8"), sizeof(int),		tkUnknown,tcPrimitive},
		{util::hashString(L"uint"),	 sizeof(int),		tkUnknown,tcPrimitive},
		{util::hashString(L"ushort"),sizeof(short),		tkUnknown,tcPrimitive},
		{util::hashString(L"ubyte"), sizeof(char),		tkUnknown,tcPrimitive},
		{util::hashString(L"uchar"), sizeof(wchar_t),	tkUnknown,tcPrimitive},
		{util::hashString(L"oleobject"),sizeof(void*),	tkUnknown,tcPrimitive},
		{util::hashString(L"variant"),sizeof(VARIANT),	tkUnknown,tcPrimitive},
		{util::hashString(L"ptr"),	 sizeof(void*),		tkPointerType,tcPrimitive},
		{util::hashString(L"array"), -4,				tkArrayType,  tcArray},
		{util::hashString(L"enum"),	 -4,				tkEnum,		  tcEnum},
		{util::hashString(L"struct"),-4,				tkStruct,	  tcType},
		{util::hashString(L"class"), -4,				tkClass,	  tcType},
		{util::hashString(L"vector"),-4,				tkClass,	  tcType},
		{util::hashString(L"map"),	 -4,				tkClass,	  tcType},
		{util::hashString(L"pair"),	 -4,				tkClass,	  tcType}
	};

	SymType typ;
	for(int i=0;i<sizeof(ptypes)/sizeof(ptypes[0]);i++)
	{
		memset(&typ,0,sizeof(SymType));
		typ.kind = ptypes[i].kind;
		typ.name = ptypes[i].name;
		typ.nspace = (int)symTable.namespaces.size();
		typ.size = ptypes[i].size + 4;

		symTable.types.push_back(typ); 
		mTypes.insert(std::pair<int,HType>(typ.name,Handle(skType,(int)symTable.types.size())));
	}

	/*
	Handle hClass	= getTypeHandle(vlTrap::Ident(L"class"));
	Handle hStruct	= getTypeHandle(vlTrap::Ident(L"struct"));
	Handle hPointer	= getTypeHandle(vlTrap::Ident(L"ptr"));

	getType(hClass)->hTyperef = Handle(skCompType,(unsigned int)symTable.comptypes.size() + 1);
	symTable.comptypes.push_back(hPointer.index);
	symTable.comptypes.push_back( hStruct.index);
	*/
}

void vlTrap::encodeIndex(std::vector<unsigned char> & dt,unsigned int index)
{
	//assert(index > 0);
	if(index < 0xc0) 
	{
		dt.push_back((unsigned char)index);
	}else if(index <= 0x0fff)
	{
		dt.push_back(0xc0 + (index>>8));
		dt.push_back(index & 0xff);
	}else if(index <= 0xfffff)
	{
		dt.push_back(0xd0 + (index>>16));
		dt.push_back((index >> 8) &0xff);
		dt.push_back(index & 0xff);
	}else 
	{
		dt.push_back(0xe0 + (index>>24));
		dt.push_back((index >> 16) &0xff);
		dt.push_back((index >> 8) &0xff);
		dt.push_back(index & 0xff);
	}
}

unsigned int vlTrap::decodeIndex(std::vector<unsigned char> & dt, int & cindex)
{
	assert(cindex > 0 && cindex <= (int)dt.size());
	int index = 0;
	if(dt[cindex - 1] <  0xc0)
	{
		cindex++;
		return dt[cindex - 2];
	}
	if((dt[cindex - 1] & 0xf0) == 0xc0) 
	{
		cindex += 2;
		return ((dt[cindex - 3] & 0x0f)<<8) + dt[cindex-2];
	}
	if((dt[cindex - 1] & 0xf0) == 0xd0) 
	{
		cindex += 3;
		return ((dt[cindex - 4] & 0x0f)<<16) + (dt[cindex - 3]<<8)+ dt[cindex - 2];
	}
	if((dt[cindex - 1] & 0xf0) == 0xe0) 
	{
		cindex += 4;
		return ((dt[cindex - 5] & 0x0f)<<24) + (dt[cindex - 4]<<16)+ (dt[cindex - 3]<<8) + dt[cindex - 2];
	}
	assert(false);
	return 0;
}

//is a primary type
bool vlTrap::isIntType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"int"));
}
bool vlTrap::isUIntType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"uint"));
}
bool vlTrap::isShortType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"short"));
}
bool vlTrap::isUShortType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"ushort"));
}
bool vlTrap::isFloatType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"float"));
}
bool vlTrap::isDoubleType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"double"));
}

bool vlTrap::isCharType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"char"));
}
bool vlTrap::isBoolType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"bool"));
}
bool vlTrap::isStringType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"string"));
}
bool vlTrap::isNillType(HType hType)
{
	return (int)hType == (int)getTypeHandle(vlTrap::Ident(L"nilltyp"));
}

bool vlTrap::isNumberType(HType hType)
{
	return isIntNumberType(hType)||isFloatNumberType(hType);
}

bool vlTrap::isIntNumberType(HType hType)
{
	return isIntType(hType)||isUIntType(hType)||isShortType(hType)||isUShortType(hType)||isCharType(hType)
		||isUInt8Type(hType)||isInt8Type(hType);
}
bool vlTrap::isFloatNumberType(HType hType)
{
	return isFloatType(hType)||isDoubleType(hType);//||isDouble80(hType)
}
bool vlTrap::isUInt8Type(HType hType)
{
	return  (int)hType == (int)GetTypeUInt8();
}
bool vlTrap::isInt8Type(HType hType)
{
	return (int)hType == (int)GetTypeInt8();
}

HType vlTrap::GetItemType(Handle hItem)
{
	if(!hItem) return 0;
	switch(hItem.kind)
	{
	case skType:
		return hItem;
	case skField:
		return getField(hItem)->hType;
	case skNamespace:
		return hItem;
	case skMethod:
		//return getMethod(hItem)->hType;
		return hItem;
	case skOleMethod:
		//return getMethod(hItem)->hType;
		return hItem;
	case skVariant:
		return getVariant(hItem)->hType;
	case skParam:
		return getParam(hItem)->hType;
	default:
		//assert(false);
		return 0;
	}
}


//get primary type
HType vlTrap::GetTypeNill()
{
	return getTypeHandle(util::hashString(L"nilltyp"));
}
HType vlTrap::GetTypeInt()
{
	return getTypeHandle(util::hashString(L"int"));
}
HType vlTrap::GetTypeUInt()
{
	return getTypeHandle(util::hashString(L"uint"));
}
HType vlTrap::GetTypeShort()
{
	return getTypeHandle(util::hashString(L"short"));
}
HType vlTrap::GetTypeUShort()
{
	return getTypeHandle(util::hashString(L"ushort"));
}
HType vlTrap::GetTypeUnknown()
{
	return getTypeHandle(util::hashString(L"unknown"));
}
HType vlTrap::GetTypeDouble()
{
	return getTypeHandle(util::hashString(L"double"));
}
HType vlTrap::GetTypeFloat()
{
	return getTypeHandle(util::hashString(L"float"));
}
HType vlTrap::GetTypeBool()
{
	return getTypeHandle(util::hashString(L"bool"));
}
HType vlTrap::GetTypeInt8()
{
	return getTypeHandle(util::hashString(L"int8"));
}
HType vlTrap::GetTypeUInt8()
{
	return getTypeHandle(util::hashString(L"uint8"));
}

HType vlTrap::GetTypeChar()
{
	return getTypeHandle(util::hashString(L"char"));
}
HType vlTrap::GetTypeString()
{
	return getTypeHandle(util::hashString(L"string"));
}
HType vlTrap::GetTypeVoid()
{
	return getTypeHandle(util::hashString(L"void"));
}
HType vlTrap::GetTypeClass()
{
	return getTypeHandle(util::hashString(L"class"));
}
HType vlTrap::GetTypeStruct()
{
	return getTypeHandle(util::hashString(L"struct"));
}
HType vlTrap::GetTypeEnum()
{
	return getTypeHandle(util::hashString(L"enum"));
}
HType vlTrap::GetTypeArray()
{
	return getTypeHandle(util::hashString(L"array"));
}

int vlTrap::getTypeTermFrameSize(Handle hTerm)
{
	if(hTerm.kind==skVariant)
	{
		if(getVariant(hTerm)->size)
		{
			int sz = getVariant(hTerm)->size - 4;
			if(sz % 4) sz += 4 - (sz%4);
			return sz;
		}
	}else if(hTerm.kind==skParam)
	{
		if(getParam(hTerm)->size)
		{
			int sz = getParam(hTerm)->size - 4;
			if(sz % 4) sz += 4 - (sz%4);
			return sz;
		}
	}
	else if(hTerm.kind==skType || hTerm.kind==skNType)
	{
		if(getType(hTerm)->size)
		{
			int sz = getType(hTerm)->size - 4;
			if(sz % 4) sz += 4 - (sz%4);
			return sz;
		}
	}
	Handle hType = getTermType(hTerm);
	if(hType.kind==skMethod || hType.kind==skOleMethod) return sizeof(void *);
	if(getType(hType)->kind==tkClass||getType(hType)->kind==tkInterface)
		return sizeof(void*);
	else if(getType(hType)->size)
	{
		int sz= getType(hType)->size - 4;
		if(sz % 4) sz += 4 - (sz%4);
		return sz;
	}
	else
	{
		int msize = 0;
		int sz = getAllocSize(hType,msize);
		if(sz % 4) sz += 4 - (sz%4);
		return sz;
	}
}


int vlTrap::getTypeTermAllocSize(SymTypeTerm* term)
{
	if(term->size)return term->size - 4;
	
	if(term->spec & ParamSpec::byref_ ) 
	{
		term->size = sizeof(void *) + 4;
		return term->size - 4;
	}else
	{
		if((term->hType.kind==skType || term->hType.kind==skNType) && symTable.types[term->hType.index - 1].kind == tkClass)
			term->size = sizeof(void *) + 4;
		else
		{
			int msize = 0;
			term->size = getAllocSize(term->hType,msize) + 4;
		}
	}
	return term->size - 4;
}

int vlTrap::getFieldAllocSize(Handle & hField)
{
	return getTypeTermAllocSize(getField(hField));
}

int vlTrap::getAllocSize(Handle & hType,int &msize, int align)
{
	if(hType.kind==skMethod||hType.kind==skOleMethod) return 4;

	assert(hType.kind== skType || hType.kind== skNType || hType.kind==skCompType);
	if(hType.kind!= skType && hType.kind!=skCompType && hType.kind!= skNType) return 0;
	int size = 0;

	if((hType.kind== skType || hType.kind== skNType)&& symTable.types[hType.index - 1].hTyperef)
		size = getAllocSize(symTable.types[hType.index - 1].hTyperef,msize,align);
	else if(hType.kind==skCompType)
	{
		int cindex = hType.index ;
		int index = decodeIndex(symTable.comptypes,cindex);
		if(symTable.types[index - 1].kind==xlang::tkPointerType)
			size = sizeof(void *);
		else if(symTable.types[index - 1].kind==xlang::tkArrayType)
		{
			int rank = decodeIndex(symTable.comptypes,cindex);
			if(rank==0) 
			{//et int[], alloc use heap
				size = sizeof(void *);
			}else
			{//et int[8],alloc use stack
				size = rank * getAllocSize(Handle(skCompType,cindex),msize);
			}
		}else if(symTable.types[index - 1].kind==xlang::tkClass)
		{
			size += sizeof(void *);
		}
		else
		{
			int ms = 0;
			size += getAllocSize(Handle(skType,index),ms);
		}
	}else
	{
		SymType & ty = symTable.types[hType.index - 1];
		if(ty.size ) return ty.size - 4;

		if(ty.baseType)
			size += getAllocSize(ty.baseType,msize);
		
		Handle hField(ty.hField);
		int count = ty.fieldCount;
		while(count > 0)
		{
			int sz = 0;
			int ms = 0;
			
			sz = getFieldAllocSize(hField);
			ms = sz;

			if(ms > msize) msize = ms;
			if(ms > align) ms = align;
			if(ms && size%ms) size += ms - (size%ms);
			size +=  sz;
			
			hField.index++;
			count--;
		}

		if(msize > align) msize = align;
		if(msize && size%msize) size += msize - (size%msize);
	}
	return size;
}

int vlTrap::getAllocSize(int typeName)
{
	assert(getTypeHandle(typeName));

	int msize = 0;
	return getAllocSize(getTypeHandle(typeName),msize);
}


int vlTrap::getFieldOffset(Handle  hType, Handle hField,int & msize, bool & bfound, int align)
{
	int size = 0;
	SymType & ty = symTable.types[hType.index - 1];
	if(ty.baseType)
	{
		size += getFieldOffset(ty.baseType,hField,msize, bfound,align);
		if(bfound) return size;
	}
	
	Handle hCurField(ty.hField);
	int count = ty.fieldCount;
	while(count > 0)
	{
		int sz = 0;
		int ms = 0;
		
		sz = getFieldAllocSize(hCurField);
		ms = sz;

		if(ms > msize) msize = ms;
		if(ms > align) ms = align;
		if(ms && size%ms) size += ms - (size%ms);
		if(hCurField.index==hField.index)
		{
			bfound = true;
			break;
		}
		size +=  sz;
		
		hCurField.index++;
		count--;
	}

	if(msize > align) msize = align;
	if(msize && size%msize) size += msize - (size%msize);
	return size;
}

int	vlTrap::getFieldOffset(Handle  hType, Handle  hField,int align)
{
	bool bfound = false;
	int	 msize = 0;

	if(getField(hField)->offset) return getField(hField)->offset - 4;
	int sz = getFieldOffset(hType,hField,msize,bfound,align);
	getField(hField)->offset = sz + 4;
	return sz;
}

int vlTrap::getFrameOffset(Handle hMethod, Handle hTerm,int align)
{
	bool bfound = false;

	if(hTerm.kind == skVariant)
	{
		if(symTable.variants[hTerm.index - 1].offset) return symTable.variants[hTerm.index - 1].offset -4;
		int sz = getFrameOffset(hMethod,hTerm,bfound,align);
		symTable.variants[hTerm.index - 1].offset = sz + 4;
		return sz;
	}else if(hTerm.kind == skParam)
	{
		if(symTable.params[hTerm.index - 1].offset) return symTable.params[hTerm.index - 1].offset -  4;
		int sz = getFrameOffset(hMethod,hTerm,bfound,align);
		symTable.params[hTerm.index - 1].offset = sz + 4;
		return sz;
	}else
	{
		//should not reach here
		assert(false);
		return 0;
	}
}

int vlTrap::getFrameOffset(Handle hMethod, Handle hTerm,bool & bfound, int align)
{
	int size = 0;
	SymMethod & tm = *getMethod(hMethod);
	
	if(hTerm.kind == skVariant)
	{
		Handle hCurVariant(tm.hVariant);
		int count = tm.variantCount;
		while(count > 0)
		{
			int sz = getTypeTermAllocSize(&symTable.variants[hCurVariant.index - 1]);
			sz = util::round_up(sz,align);
			size += sz;
			if(hCurVariant.index==hTerm.index)
			{
				bfound = true; 
				break;
			}
			hCurVariant.index++;
			count--;
		}
		return size;
	}else if(hTerm.kind == skParam)
	{
		Handle hCurParam(tm.hParam);
		int count = tm.paramCount;
		while(count > 0)
		{
			int sz = getTypeTermAllocSize(&symTable.params[hCurParam.index - 1]);
			sz = util::round_up(sz,align);
			if(hCurParam.index==hTerm.index)
			{
				bfound = true;
				break;
			}
			size += sz;
			hCurParam.index++;
			count--;
		}
		return size;
	}
	else
	{
		//should not reach here
		assert(false);
		return 0;
	}
}

int vlTrap::getMethodParamSize(Handle hMethod,int align)
{
	int size = 0;
	SymMethod & tm = *getMethod(hMethod);
	Handle hCurParam(tm.hParam);
	int count = tm.paramCount;
	while(count > 0)
	{
		int sz = getTypeTermAllocSize(&symTable.params[hCurParam.index - 1]);
		sz = util::round_up(sz,align);
		size += sz;
		hCurParam.index++;
		count--;
	}
	return size;
}

int vlTrap::getMethodVariantSize(Handle hMethod,int align)
{
	int size = 0;
	SymMethod & tm = *getMethod(hMethod);
	Handle hCurVariant(tm.hVariant);
	int count = tm.variantCount;
	while(count > 0)
	{
		int sz = getTypeTermAllocSize(&symTable.variants[hCurVariant.index - 1]);
		if(sz%align) sz += align - (sz%align);
		size += sz;
		hCurVariant.index ++;
		count--;
	}
	return size;
}

Handle vlTrap::getTypeHandle(int name) 
{
	multimap<int,HType>::iterator lIter = mTypes.lower_bound(name);
	multimap<int,HType>::iterator uIter = mTypes.upper_bound(name);
	lIter--;
	uIter--;
	if(lIter!=uIter) 
		return uIter->second;
	return Handle();
}

Handle vlTrap::getGlobalNamespace()
{
	return Handle(skNamespace,1);
}

Handle vlTrap::getFieldHandle(int name)
{
	multimap<int,std::pair<HField,HType>>::iterator lIter = mFields.lower_bound(name);
	multimap<int,std::pair<HField,HType>>::iterator uIter = mFields.upper_bound(name);
	lIter--;
	uIter--;
	if(lIter!=uIter) 
		return uIter->second.first;
	return Handle();
}
Handle vlTrap::getFieldHandle(Handle hType,int name)
{
	return this->lookupItem(hType,name);
}

bool	vlTrap::isOleItem(Handle  hItem)
{
	int sCount = 2;
	while(sCount > 0)
	{
		if(hItem.kind==skNamespace)
		{
			if(getNamespace(hItem) && getNamespace(hItem)->hNsOleTerm) return true;
		}
		else if(hItem.kind==skType)
		{
			if(getType(hItem) && getType(hItem)->hOleTerm) return true;
		}
		else if(hItem.kind==skOleField) return true;
		else if(hItem.kind==skOleMethod) return true;

		hItem = getParent(hItem);
		sCount--;
	}
	return false;
}

Handle	vlTrap::addOleTerm(void* typeInfo)
{
	SymOleTerm term;
	memset(&term,0,sizeof(SymOleTerm));
	term.hLastField = 0;
	term.hLastMethod = 0;
	term.typeInfo = typeInfo;
	symTable.oleterms.push_back(term);
	Handle hOleTerm(skOleTerm,(int)symTable.oleterms.size());
	return hOleTerm;
}

Handle  vlTrap::addNSOleTerm(int name,void * ptLib, void* typeInfo)
{
	SymNSOleTerm term;
	memset(&term,0,sizeof(SymNSOleTerm));
	term.ptLib = ptLib;
	term.oleName = name;
	term.typeInfo = typeInfo;
	symTable.nsoleterms.push_back(term);
	Handle hNSOleTerm(skNSOleTerm,(int)symTable.nsoleterms.size()); 
	return hNSOleTerm; 
}

void vlTrap::setNSOleTerm(Handle hNamespace, Handle hNsOleTerm)
{
	assert(hNamespace.kind ==skNamespace);
	getNamespace(hNamespace)->hNsOleTerm = hNsOleTerm;
}

Handle	vlTrap::addProcessor(Handle hAxis, int name,bool todef)
{
	HType hType;
	hType = lookupType(hAxis,name);
	if(hType && !todef) return hType;

	if( hType && !getType(hType)->undef)
	{
		getType(hType/*lookupType(hAxis,name)*/)->dirty = true; 
		hType = HType();  
	}

	if(!hType)
	{
		SymType tk;
		::memset(&tk,0,sizeof(SymType));
		tk.name = name;
		tk.kind = tkProcessor;
		tk.undef = !todef;
		symTable.types.push_back(tk);
		hType =  Handle(skType,(int)symTable.types.size());
		if(hAxis)
		{
			symTable.parentitems.insert(std::pair<Handle,Handle>(hType,hAxis));
			if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hType));
		}
		mTypes.insert(std::pair<int,HType>(name,hType));
	}else
	{
		if(todef)getType(hType)->undef = false;
	}
	return hType; 
}

Handle	vlTrap::addType(Handle hAxis,TypeKind kind,int name,bool todef)
{
	HType hType;
	if(!isOleItem(hAxis))
	{
		hType = lookupType(hAxis,name);
		if(hType && !todef) return hType;

		if( hType && !getType(hType)->undef)
		{
			getType(hType/*lookupType(hAxis,name)*/)->dirty = true; 
			hType = HType();  
		}
	}
	if(!hType)
	{
		SymType tk;
		::memset(&tk,0,sizeof(SymType));
		tk.name = name;
		tk.kind = kind;
		tk.undef = !todef;
		symTable.types.push_back(tk);
		hType =  Handle(skType,(int)symTable.types.size());
		if(hAxis)
		{
			symTable.parentitems.insert(std::pair<Handle,Handle>(hType,hAxis));
			if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hType));
		}
		mTypes.insert(std::pair<int,HType>(name,hType));
	}else
	{
		if(todef)getType(hType)->undef = false;
	}
	return hType; 
}

Handle	vlTrap::addContext(Handle hAxis,int name)
{
	multimap<Handle,Handle>::iterator lIter = symTable.ctxitems.lower_bound(hAxis);
	multimap<Handle,Handle>::iterator uIter = symTable.ctxitems.upper_bound(hAxis);
	for(;lIter!=uIter; lIter++)
	{
		if(lIter->second.kind==skContext)
		{
			if(getNamespace(lIter->second)->name==name) return lIter->second.kind;
		}
	}	
	SymContext symContext;
	memset(&symContext,0,sizeof(SymContext));
	symContext.name = name;
	symContext.parent = hAxis;
	symTable.contexts.push_back(symContext);

	HContext hContext(skContext,(int)symTable.contexts.size());
	if(hAxis)
	{
		symTable.parentitems.insert(std::pair<Handle,Handle>(hContext,hAxis));
		if(hAxis.kind==skContext)symTable.ctxitems.insert(std::pair<Handle,Handle>(hAxis,hContext));
	}
	return hContext;
}

Handle vlTrap::addNamespace(Handle hAxis,int name)
{
	multimap<Handle,Handle>::iterator lIter = symTable.nsitems.lower_bound(hAxis);
	multimap<Handle,Handle>::iterator uIter = symTable.nsitems.upper_bound(hAxis);
	for(;lIter!=uIter; lIter++)
	{
		if(lIter->second.kind==skNamespace)
		{
			if(getNamespace(lIter->second)->name==name) return lIter->second;
		}
	}	
	SymNamespace symNamespace;
	memset(&symNamespace,0,sizeof(SymNamespace));
	symNamespace.name = name;
	symNamespace.parent = hAxis;
	symTable.namespaces.push_back(symNamespace);

	HNamespace hNamespace(skNamespace,(int)symTable.namespaces.size());
	if(hAxis)
	{
		symTable.parentitems.insert(std::pair<Handle,Handle>(hNamespace,hAxis));
		if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hNamespace));
	}
	return hNamespace;
}

Handle	vlTrap::addField(Handle hAxis,int name)
{
	return addField(hAxis,name,0,Handle());
}

Handle	vlTrap::addField(Handle hAxis,int name, unsigned spec ,Handle hType)
{
	if(hAxis.kind!=skType && hAxis.kind!=skNamespace) return Handle();

	if(hAxis.kind==skNamespace)
	{
		if(!isOleItem(hAxis))
		{
			SymField field;
			::memset(&field,0,sizeof(SymField));
			field.name = name;

			field.spec = spec;
			field.hType = hType;
			symTable.fields.push_back(field);

			HField hField(skField,(int)symTable.fields.size());
			if(hAxis)
			{
				symTable.parentitems.insert(std::pair<Handle,Handle>(hField,hAxis));
				if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hField));
			}
			mFields.insert(std::pair<int,std::pair<HField,HNamespace>>(
				name,std::pair<HField,HType>(hField,hAxis)));
			return hField;
		}else
		{
			return Handle();
		}
	}else if(hAxis.kind==skType || hAxis.kind==skNType)
	{
		if(!isOleItem(hAxis))
		{
			SymField field;
			::memset(&field,0,sizeof(SymField));
			field.name = name;

			field.spec = spec;
			field.hType = hType;
			symTable.fields.push_back(field);

			HField hField(skField,(int)symTable.fields.size());
			if(!symTable.types[hAxis.index - 1].hField)symTable.types[hAxis.index - 1].hField = hField;
			symTable.types[hAxis.index - 1].fieldCount++;

			if(hAxis)
			{
				symTable.parentitems.insert(std::pair<Handle,Handle>(hField,hAxis));
				if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hField));
			}
			mFields.insert(std::pair<int,std::pair<HField,HType>>(
				name,std::pair<HField,HType>(hField,hAxis)));
			return hField;
		}else
		{
			assert(symTable.types[hAxis.index - 1].hOleTerm);
			SymOleField field;
			::memset(&field,0,sizeof(SymOleField));
			field.name = name;

			field.spec = spec;
			field.hType = hType;
			symTable.olefields.push_back(field);

			HField hField(skOleField,(int)symTable.olefields.size());
			if(!symTable.types[hAxis.index - 1].hField)
			{
				symTable.types[hAxis.index - 1].hField = hField;
			}
			symTable.types[hAxis.index - 1].fieldCount++;

			if(!getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastField)
				getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastField = hField;
			else
			{
				symTable.olefields[getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastField.index - 1].next = hField;
				symTable.olefields.back().piror = getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastField;
				getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastField = hField;
			}
				
			if(hAxis)
			{
				symTable.parentitems.insert(std::pair<Handle,Handle>(hField,hAxis));
				if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hField));
			}
			mFields.insert(std::pair<int,std::pair<HField,HType>>(
				name,std::pair<HField,HType>(hField,hAxis)));
			return hField;
		}
	}
	return Handle();
}

Handle	vlTrap::addApply(Handle hContext, Handle hAxis,int name, unsigned int spec ,Handle hType)
{
	if(!hAxis)hAxis = Handle(skContext,1);

	SymMethod mt;
	::memset(&mt,0,sizeof(SymMethod));
	mt.name = name;
	mt.spec = spec;
	mt.hType = hType;
	
	symTable.methods.push_back(mt);
	HMethod hMethod(skMethod,(int)symTable.methods.size());
	if(hAxis.kind==skType ||hAxis.kind==skNType )
	{
		if(!symTable.types[hAxis.index - 1].hMethod)symTable.types[hAxis.index - 1].hMethod = hMethod;
		symTable.types[hAxis.index - 1].methodCount++;
	}

	if(hAxis)
	{
		symTable.parentitems.insert(std::pair<Handle,Handle>(hMethod,hAxis));
		if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hMethod));
	}
	getMethod(hMethod)->kind = mkApply;

	return hMethod;
}

Handle	vlTrap::addMethod(Handle hAxis,int name, unsigned int spec ,Handle hType)
{
	if(hAxis.kind!=skType && hAxis.kind!=skNType  && hAxis.kind!=skNamespace) return Handle();

	if(hAxis.kind==skNamespace)
	{
		if(!isOleItem(hAxis))
		{
			SymMethod mt;
			::memset(&mt,0,sizeof(SymMethod));
			mt.name = name;
			mt.spec = spec;
			mt.hType = hType;
			
			symTable.methods.push_back(mt);
			HMethod hMethod(skMethod,(int)symTable.methods.size());

			if(hAxis)
			{
				symTable.parentitems.insert(std::pair<Handle,Handle>(hMethod,hAxis));
				if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hMethod));
			}
			getMethod(hMethod)->kind = mkGeneralMethod;
			return hMethod;
		}
	}else if(hAxis.kind==skType ||hAxis.kind==skNType )
	{
		if(!isOleItem(hAxis))
		{
			SymMethod mt;
			::memset(&mt,0,sizeof(SymMethod));
			mt.name = name;
			mt.spec = spec;
			mt.hType = hType;
			
			symTable.methods.push_back(mt);
			HMethod hMethod(skMethod,(int)symTable.methods.size());
			if(!symTable.types[hAxis.index - 1].hMethod)symTable.types[hAxis.index - 1].hMethod = hMethod;
			symTable.types[hAxis.index - 1].methodCount++;

			if(hAxis)
			{
				symTable.parentitems.insert(std::pair<Handle,Handle>(hMethod,hAxis));
				if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hMethod));
			}
			getMethod(hMethod)->kind = mkGeneralMethod;

			return hMethod;
		}else
		{
			SymOleMethod mt;
			::memset(&mt,0,sizeof(SymMethod));
			mt.name = name;
			mt.spec = spec;
			mt.hType = hType;
			
			symTable.olemethods.push_back(mt);
			HMethod hMethod(skOleMethod,(int)symTable.olemethods.size());
			if(!symTable.types[hAxis.index - 1].hMethod)
				symTable.types[hAxis.index - 1].hMethod = hMethod;
			symTable.types[hAxis.index - 1].methodCount++;

			if(!getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastMethod)
				getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastMethod = hMethod;
			else
			{
				symTable.olemethods[getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastMethod.index - 1].next = hMethod;
				symTable.olemethods.back().piror = getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastMethod;
				getOleTerm(symTable.types[hAxis.index - 1].hOleTerm)->hLastMethod = hMethod;
			}

			if(hAxis)
			{
				symTable.parentitems.insert(std::pair<Handle,Handle>(hMethod,hAxis));
				if(hAxis.kind==skNamespace)symTable.nsitems.insert(std::pair<Handle,Handle>(hAxis,hMethod));
			}
			getMethod(hMethod)->kind = mkGeneralMethod;

			return hMethod;
		}
	}
	return Handle();
}

Handle	vlTrap::addVariant(Handle hMethod, int name, unsigned int spec ,Handle hType)
{
	Handle& h = hMethod;

	SymVariant var;
	::memset(&var,0,sizeof(SymVariant));
	var.name = name;
	var.spec = spec;
	var.hType = hType;
	
	symTable.variants.push_back(var);
	HVariant hVariant(skVariant,(int)symTable.variants.size());
	if(!getMethod(hMethod)->hVariant)getMethod(hMethod)->hVariant = hVariant;
	getMethod(hMethod)->variantCount++;

	return hVariant;
}

Handle	vlTrap::addParam(Handle hMethod,int name, unsigned int spec ,Handle hType)
{
	Handle& h = hMethod;

	SymParam var;
	::memset(&var,0,sizeof(SymParam));
	var.name = name;
	var.spec = spec;
	var.hType = hType;
	
	symTable.params.push_back(var);
	HParam hParam(skParam,(int)symTable.params.size());
	if(!getMethod(hMethod)->hParam)getMethod(hMethod)->hParam = hParam;
	getMethod(hMethod)->paramCount++;

	return hParam;
}


void vlTrap::addTypeBase(Handle hType, Handle hTypeBase)
{
	assert(getType(hType));
	getType(hType)->baseType = hTypeBase;

}

Handle vlTrap::getMethodHandle(Handle hType,int id)
{
	return this->lookupMethod(hType,id);
}

Handle vlTrap::getMethodVariantHandle(Handle hMethod,int id)
{
	Handle hVariant = getMethod(hMethod)->hVariant;
	int count = getMethod(hMethod)->variantCount;
	int i = 0;
	while(i < count)
	{
		if(getVariant(hVariant)->name==id)break;
		hVariant.index++;
		i++;
	}
	if(i==count) return Handle();
	return hVariant;
}

Handle vlTrap::getMethodParamHandleByIndex(Handle hMethod,int index)
{
	Handle hParam = getMethod(hMethod)->hParam;
	int count = getMethod(hMethod)->paramCount;
	assert(count > index );
	hParam.index += index;
	return hParam;
}

Handle vlTrap::getMethodParamHandle(Handle hMethod,int id)
{
	Handle hParam = getMethod(hMethod)->hParam;
	int count = getMethod(hMethod)->paramCount;
	int i = 0;
	while(i < count)
	{
		if(getParam(hParam)->name==id)break;
		hParam.index++;
		i++;
	}
	if(i==count) return Handle();
	return hParam;
}

void vlTrap::addCompTypeItem(Handle hType)
{
	this->encodeIndex(symTable.comptypes,hType.index);
}

void	vlTrap::addCompTypeRank(int rank)
{
	this->encodeIndex(symTable.comptypes,rank);
}

void vlTrap::insertCompTypeItem(Handle hCompType,Handle hType)
{
	std::vector<unsigned char> dt;
	this->encodeIndex(dt,hType.index);
	if(this->symTable.comptypes.size())
	{
		if(hCompType.index - 1==this->symTable.comptypes.size())
			this->symTable.comptypes.insert(this->symTable.comptypes.end(),dt.begin(),dt.end());
		else
			this->symTable.comptypes.insert(this->symTable.comptypes.begin()+ hCompType.index - 1,dt.begin(),dt.end());
	}
	else
	{
		this->symTable.comptypes =  dt;
	}
}
void vlTrap::insertCompTypeRank(Handle hCompType,int rank)
{
	std::vector<unsigned char> dt;
	this->encodeIndex(dt,rank);
	if(hCompType.index - 1==this->symTable.comptypes.size())
		this->symTable.comptypes.insert(this->symTable.comptypes.end(),dt.begin(),dt.end());
	else
		this->symTable.comptypes.insert(this->symTable.comptypes.begin()+ hCompType.index - 1,dt.begin(),dt.end());
}

HType	vlTrap::getCompTypeItem(Handle hCompType)
{
	if(hCompType.kind!=skCompType) return Handle();

	int index =  hCompType.index;
	return Handle(skType,decodeIndex(symTable.comptypes,index));
}
int		vlTrap::getCompTypeRank(Handle hCompType)
{
	if(hCompType.kind!=skCompType) return 0;

	int index =  hCompType.index;
	decodeIndex(symTable.comptypes,index);
	return decodeIndex(symTable.comptypes,index);
}

HType	vlTrap::stepCompType(Handle hCompType)
{
	if(hCompType.kind!=skCompType) return Handle();

	int index =  hCompType.index;
	decodeIndex(symTable.comptypes,index); //primary
	decodeIndex(symTable.comptypes,index); //rank
	
	Handle hVector  = getTypeHandle(Ident(L"vector"));
	Handle hMap	    = getTypeHandle(Ident(L"map"));
	Handle hPair    = getTypeHandle(Ident(L"pair"));
	Handle hPointer = getTypeHandle(Ident(L"ptr"));
	Handle hArray	= getTypeHandle(Ident(L"array"));

	int ctype = this->symTable.comptypes[index - 1];
	if(ctype!=hVector.index && ctype!=hMap.index && ctype!=hPair.index &&
		ctype!=hPair.index && ctype!=hPointer.index &&
		ctype!=hArray.index)
		return Handle(skType,this->symTable.comptypes[index - 1]);

	return Handle(skCompType,index);
}

void*	vlTrap::getMethodAddr(Handle hMethod)
{
	if(!hMethod) return 0;
	if(!getMethod(hMethod)->addr && isMethodNative(hMethod))
	{
		getMethod(hMethod)->addr = os_t::load_module(
			getMethodNativeFrom(hMethod),
			getMethodNativeAlias(hMethod));
	}
	return getMethod(hMethod)->addr;
}

void	vlTrap::setMethodAddr(Handle hMethod,void * addr)
{
	getMethod(hMethod)->addr = addr;
}


void	vlTrap::setTypeNativeFrom(Handle hType, int nativeFrom)
{
	getType(hType)->nativeFrom = nativeFrom;;
}
void	vlTrap::setTypeNativeAlias(Handle hType,int nativeAlias)
{
	getType(hType)->nativeAlias = nativeAlias;;
}
wchar_t* vlTrap::getTypeNativeFrom(Handle hType)
{
	return util::getHashString(getType(hType)->nativeFrom);
}
wchar_t* vlTrap::getTypeNativeAlias(Handle hType)
{
	return util::getHashString(getType(hType)->nativeAlias); 
}

void	vlTrap::setMethodNativeFrom(Handle hMethod,int nativeFrom)
{
	getMethod(hMethod)->spec |= MethodSpec::native_;
	getMethod(hMethod)->nativeFrom = nativeFrom;;
}

void	vlTrap::setMethodNativeAlias(Handle hMethod,int nativeAlias)
{
	getMethod(hMethod)->spec |= MethodSpec::native_;
	getMethod(hMethod)->nativeAlias = nativeAlias;
}

wchar_t* vlTrap::getMethodNativeFrom(Handle hMethod)
{
	if(!getMethod(hMethod)->nativeFrom)
	{
		if(getParent(hMethod) && getParent(hMethod).kind==skType)
			if(getType(getParent(hMethod))->nativeFrom)
				return util::getHashString(getType(getParent(hMethod))->nativeFrom);
	}
	return util::getHashString(getMethod(hMethod)->nativeFrom);
}

wchar_t* vlTrap::getMethodNativeAlias(Handle hMethod)
{
	return util::getHashString(getMethod(hMethod)->nativeAlias);
}

bool	vlTrap::isMethodNative(Handle hMethod)
{
	return getMethod(hMethod)->spec & MethodSpec::native_ ? true:false;
}

void	vlTrap::setMethodNative(Handle hMethod,int nativeFrom,int nativeAlias)
{
	if(nativeFrom)getMethod(hMethod)->nativeFrom = nativeFrom;
	if(nativeAlias)getMethod(hMethod)->nativeAlias = nativeAlias;
	getMethod(hMethod)->spec |= MethodSpec::native_;
}

unsigned int vlTrap::getMethodSpec(Handle hMethod)
{
	return getMethod(hMethod)->spec;
}

unsigned int vlTrap::getMethodKind(Handle hMethod)
{
	return getMethod(hMethod)->kind;
}

void * vlTrap::getProcessApplyEntryPoint(HType htype)
{
	return this->getType(htype)->entryPointApplys;
}

void * vlTrap::getContextApplyEntryPoint(HContext hContext)
{
	return this->getContext(hContext)->entryPointApplys;
}

void * vlTrap::sureProcessApplyEntryPoint(HType htype)
{
	if(!this->getType(htype)->entryPointApplys)
		this->getType(htype)->entryPointApplys = new EntryNode;
	return this->getType(htype)->entryPointApplys;
}

void * vlTrap::sureContextApplyEntryPoint(HContext hContext)
{
	if(!this->getContext(hContext)->entryPointApplys)
		this->getContext(hContext)->entryPointApplys = new EntryNode;
	return this->getContext(hContext)->entryPointApplys;
}

void * vlTrap::addMatchCount(void * entryPoint,int count)
{
	EntryNode * ep = (EntryNode *)entryPoint;
	ep->test_kind = counttest;
	if(!ep->items) ep->items = new OneOfCountList();
	EntryNode * result = ep->items->findItem(TValue(count));
	if(!result)
	{
		result = new EntryNode();
		result->test_kind = matchtest;
		ep->items->addItem(TValue(count),result);
	}
	return result;
}

void * vlTrap::addMatchLiteral(void * entryPoint, int pos, class vlAstAxis * lt)
{
	EntryNode * ep = (EntryNode *)entryPoint;
	if(!ep->items) ep->items = new OneOfMatchList();

	TValue value(lt->getInteger(),(literaltest<<16)|pos);
	EntryNode * result = ep->items->findItem(value);
	if(!result)
	{
		result = new EntryNode();
		result->test_kind = matchtest;
		ep->items->addItem(value,result);
	}
	return result;
}

void * vlTrap::addMatchName(void * entryPoint, int pos,int id)
{
	EntryNode * ep = (EntryNode *)entryPoint;
	if(!ep->items) ep->items = new OneOfMatchList();

	TValue value(id,(nametest<<16)|pos);
	EntryNode * result = ep->items->findItem(value);
	if(!result)
	{
		result = new EntryNode();
		result->test_kind = matchtest;
		ep->items->addItem(value,result);
	}
	return result;
}

void * vlTrap::addMatchApplyName(void * entryPoint,int id)
{
	EntryNode * ep = (EntryNode *)entryPoint;
	if(!ep->items) ep->items = new OneOfNameList();

	EntryNode * result = ep->items->findItem(TValue(id));
	if(!result)
	{
		result = new EntryNode();
		result->test_kind = counttest;
		ep->items->addItem(TValue(id),result);
	}
	return result;
}

void * vlTrap::addMatchAny(void * entryPoint, int pos)
{
	EntryNode * ep = (EntryNode *)entryPoint;
	if(!ep->items) ep->items = new OneOfMatchList();

	TValue value(0,(literaltest<<16)|pos);
	EntryNode * result = ep->items->findItem(value);
	if(!result)
	{
		result = new EntryNode();
		result->test_kind = matchtest;
		ep->items->addItem(value,result);
	}
	return result;
}
void * vlTrap::addApplyParam(void * entryPoint, int pos,int id)
{
	return 0;
}

void vlTrap::setTyperef(Handle hType, Handle hTypeRef)
{
	assert(hType.kind==skType || hType.kind==skNType);
	symTable.types[hType.index - 1].hTyperef = hTypeRef;
}

void vlTrap::setTypeSize(Handle hType, int size)
{
	symTable.types[hType.index - 1].size = size;
}

void vlTrap::setTypeKind(Handle hType, TypeKind kind)
{
	symTable.types[hType.index - 1].kind = kind;
}

TypeKind vlTrap::getTypeKind(Handle hType)
{
	if(hType.index< 1)
		throw(L"error argument for getTypeKind");
	return symTable.types[hType.index - 1].kind;
}

void vlTrap::setTypeCatKind(Handle hType, TypeCatKind catKind)
{
	if(hType.index< 1)
		throw(L"error argument for setTypeCatKind");
	symTable.types[hType.index - 1].catKind = catKind;
}

int vlTrap::Ident(wchar_t* name)
{
	return util::hashString(name);
}

SymType * vlTrap::getType(Handle  hType)
{
	assert(hType.kind==skType || hType.kind==skNType);
	if(hType.index< 1)
		throw(L"error argument for getType");
	return &symTable.types[hType.index - 1];
}

SymContext * vlTrap::getContext(Handle  hContext)
{
	assert(hContext.kind==skContext);
	if(hContext.index< 1)
		throw(L"error argument for getContext");
	return &symTable.contexts[hContext.index - 1];
}

SymMethod * vlTrap::getMethod(Handle  hMethod)
{
	assert(hMethod.kind==skMethod || hMethod.kind==skOleMethod);
	if(hMethod.index< 1)
		throw(L"error argument for getMethod");
	if(hMethod.kind==skOleMethod)
		return &symTable.olemethods[hMethod.index - 1];
	return &symTable.methods[hMethod.index - 1];
}

SymParam * vlTrap::getParam(Handle  hParam)
{
	assert(hParam.kind==skParam);
	return &symTable.params[hParam.index - 1];
}

SymNamespace *  vlTrap::getNamespace(Handle hNamespace)
{
	assert(hNamespace.kind==skNamespace);
	return &symTable.namespaces[hNamespace.index - 1];
}

SymOleTerm   *  vlTrap::getOleTerm(Handle hOleTerm)
{
	assert(hOleTerm.kind==skOleTerm);
	return &symTable.oleterms [hOleTerm.index - 1];
}

SymNSOleTerm *	vlTrap::getNSOleTerm(Handle hNSOleTerm)
{
	assert(hNSOleTerm.kind==skNSOleTerm);
	return &symTable.nsoleterms [hNSOleTerm.index - 1];
}

SymOleTerm   *  vlTrap::getTypeOleTerm(Handle hType)
{
	if(!getType(hType)->hOleTerm) return 0;
	return getOleTerm(getType(hType)->hOleTerm);
}

SymNSOleTerm *  vlTrap::getNamespaceOleTerm(Handle hNamespace)
{
	if(!getNamespace(hNamespace)->hNsOleTerm) return 0;
	return getNSOleTerm(getNamespace(hNamespace)->hNsOleTerm);
}

SymNSOleTerm *  vlTrap::getTypeNSOleTerm(Handle hType)
{
	Handle hItem = getParent(hType);
	while(hItem)
	{
		if(hItem.kind==skNamespace) return getNamespaceOleTerm(hItem);
		hItem = getParent(hItem);
	}
	return 0;
}

SymVariant * vlTrap::getVariant(Handle  hVariant)
{
	assert(hVariant.kind==skVariant);
	return &symTable.variants[hVariant.index - 1];
}
SymTypeParam * vlTrap::getTypeParam(Handle  hTypeParam)
{
	assert(false);
	//assert(hType.kind==skTypeParam);
	//return &symTable.fields[hTypeParam.index - 1];
	return 0;
}

SymField * vlTrap::getField(Handle  hField)
{
	assert(hField.kind==skField || hField.kind==skOleField);
	if(hField.kind==skOleField)
		return &symTable.olefields[hField.index - 1];
	return &symTable.fields[hField.index - 1];
}

Handle vlTrap::addItem(Handle hAxis,SymbolKind sk,int name)
{
	switch(sk)
	{
	case skNType:
	case skType:
		return addType(hAxis,tkClass, name);
	case skNamespace:
		return addNamespace(hAxis,name);
	case skField:
	case skOleField:
		return addField(hAxis,name);
	default:
		assert(false);
		return Handle();
	}
}

void * vlTrap::makeType(int typeName)
{
	int sz = getAllocSize(typeName);
	void * p = ::malloc(sz);
	::memset(p,0,sz);
	return p;
}

void vlTrap::freeType(void *p)
{
	if(p)::free(p);
}

int	vlTrap::getTermNameID(Handle hterm)
{
	switch(hterm.kind)
	{
	case skNamespace :
		return symTable.namespaces[hterm.index - 1].name;
	case skNType :
	case skType :
		return symTable.types[hterm.index - 1].name;
	case skField :
		return symTable.fields[hterm.index - 1].name;
	case skMethod :
		return symTable.methods[hterm.index - 1].name;
	case skOleField :
		return symTable.olefields[hterm.index - 1].name;
	case skOleMethod :
		return symTable.olemethods[hterm.index - 1].name;
	case skVariant :
		return symTable.variants[hterm.index - 1].name;
	case skParam :
		return symTable.params[hterm.index - 1].name;
	case skTypeParam :
		return symTable.typeparams[hterm.index - 1].name;
	case skCompType :
		return 0;
	}
	return 0;
}

const wchar_t * vlTrap::getTermName(Handle hterm)
{
	int ide = getTermNameID(hterm);
	if(ide) return util::getHashString(ide);
	
	if(hterm.kind==skCompType)
		return _wcsdup(dumpCompType(hterm).c_str());
	return 0;
}

HType vlTrap::getTermType(Handle hterm)
{
	switch(hterm.kind)
	{
	case skNamespace :
		return 0;
	case skType :
	case skNType :
		return (HType)hterm;
	case skField :
		return symTable.fields[hterm.index - 1].hType;
	case skMethod :
		return symTable.methods[hterm.index - 1].hType;
	case skOleField :
		return symTable.olefields[hterm.index - 1].hType;
	case skOleMethod :
		return symTable.olemethods[hterm.index - 1].hType;
	case skVariant :
		return symTable.variants[hterm.index - 1].hType;
	case skParam :
		return symTable.params[hterm.index - 1].hType;
	case skTypeParam :
		return symTable.typeparams[hterm.index - 1].hType;
	case skCompType : return (HType)hterm;
	}
	return 0;
}

int vlTrap::getTermSpec(Handle hterm)
{
	switch(hterm.kind)
	{
	case skNamespace :
		return 0;
	case skType :
	case skNType :
		return 0;
	case skField :
		return symTable.fields[hterm.index - 1].spec;
	case skMethod :
		return symTable.methods[hterm.index - 1].spec;
	case skOleField :
		return symTable.olefields[hterm.index - 1].spec;
	case skOleMethod :
		return symTable.olemethods[hterm.index - 1].spec;
	case skVariant :
		return symTable.variants[hterm.index - 1].spec;
	case skParam :
		return symTable.params[hterm.index - 1].spec;
	case skTypeParam :
		return symTable.typeparams[hterm.index - 1].spec;
	case skCompType : return 0;
	}
	return 0;
}

void vlTrap::setTermInitValue(Handle hterm,int val)
{
	switch(hterm.kind)
	{
	case skNamespace :
		return ;
	case skType :
	case skNType :
		return ;
	case skField :
		{
			symTable.fields[hterm.index - 1].hasInitValue = true;
			symTable.fields[hterm.index - 1].initValue= val;
		}
	case skMethod :
		return ;
	case skOleField :
		{
			symTable.olefields[hterm.index - 1].hasInitValue = true;
			symTable.olefields[hterm.index - 1].initValue = val;
		}
	case skOleMethod :
		return ;
	case skVariant :
		{
			symTable.variants[hterm.index - 1].hasInitValue = true;
			symTable.variants[hterm.index - 1].initValue = val;
		}
	case skParam :
		{
			symTable.params[hterm.index - 1].hasInitValue = true;
			symTable.params[hterm.index - 1].initValue = val;
		}
	case skTypeParam :
		return ;
	case skCompType : return ;
	}
}

int vlTrap::getTermInitValue(Handle hterm)
{
	switch(hterm.kind)
	{
	case skNamespace :
		return 0;
	case skType :
	case skNType :
		return 0;
	case skField :
		return symTable.fields[hterm.index - 1].initValue;
	case skMethod :
		return 0;
	case skOleField :
		return symTable.olefields[hterm.index - 1].initValue;
	case skOleMethod :
		return 0;
	case skVariant :
		return symTable.variants[hterm.index - 1].initValue;
	case skParam :
		return symTable.params[hterm.index - 1].initValue;
	case skTypeParam :
		return 0;
	case skCompType : return 0;
	}
	return 0;
}

std::wstring vlTrap::dumpType(HType hType)
{
	wstring str;
	int i = hType.index - 1;
	if(!hType) return L"nil type";
	if(symTable.types[i].hTyperef)
	{
		if(symTable.types[i].hTyperef.kind==skType)
		{
			str += (std::wstring)L"typedef " + (std::wstring)util::getHashString(symTable.types[symTable.types[i].hTyperef.index - 1].name) + L" ";
		}else
		{
			str += (std::wstring)L"typedef " + dumpCompType(Handle(skCompType,symTable.types[i].hTyperef.index)) + L" ";
		}
	}
	str += util::getHashString(symTable.types[i].name) ;
	if(symTable.types[i].baseType) str +=  (std::wstring)L"\t" + L":" + util::getHashString(symTable.types[symTable.types[i].baseType.index - 1].name);
	if(symTable.types[i].hField || symTable.types[i].hMethod)
		str += (std::wstring)L"{";
	if(symTable.types[i].hField)
		str +=  dumpField(hType);
	if(symTable.types[i].hMethod)
	{
		HMethod hMethod = symTable.types[i].hMethod;
		if(isOleItem(hType))
		{
			while(hMethod)
			{
				str += (std::wstring)L"\n\t";
				str += dumpMethod(hMethod);
				hMethod = ((SymOleMethod *)getMethod(hMethod))->next;
			}
		}else
		{
			for(int j=0;j<symTable.types[i].methodCount;j++)
			{
				str += (std::wstring)L"\n\t";
				str += dumpMethod(hMethod);
				hMethod.index ++;
			}
		}
	}
	if(symTable.types[i].hField || symTable.types[i].hMethod)
		str += (std::wstring)L"\n}";
	else
		str += (std::wstring)L";";

	return str;
}

std::wstring vlTrap::dumpCompType(HType hCompType)
{
	wstring str;
	Handle hVector  = getTypeHandle(Ident(L"vector"));
	Handle hMap	    = getTypeHandle(Ident(L"map"));
	Handle hPair    = getTypeHandle(Ident(L"pair"));
	Handle hPointer = getTypeHandle(Ident(L"ptr"));
	Handle hArray	= getTypeHandle(Ident(L"array"));

	int cindex = hCompType.index;
	int index = decodeIndex(symTable.comptypes,cindex);

	if(index == hArray.index)
	{
		wchar_t buf[32];
		int rand = decodeIndex(symTable.comptypes,cindex);
		buf[0] = 0;
		if(rand>0) swprintf_s(buf,32,L"%d",rand);

		str += dumpCompType(Handle(skCompType,cindex)) +
				L"[" + buf + L"]";
	}
	else if(index == hPointer.index)
	{
		str += dumpCompType(Handle(skCompType,cindex)) + L"*";
	}			else if(index == hVector.index)
		str += (std::wstring)util::getHashString(symTable.types[index - 1].name) +
			L"<" + dumpCompType(Handle(skCompType,cindex)) + L">";
	else if(index == hMap.index || index == hPair.index)
	{
		str += (std::wstring)util::getHashString(symTable.types[index - 1].name) +
			L"<" +dumpCompType(Handle(skCompType,cindex)) ;
		str += (std::wstring)L"," + dumpCompType(Handle(skCompType,cindex)) + L">";
	}else
	{
		str += (std::wstring)util::getHashString(symTable.types[index - 1].name);
	}
	return str;
}

std::wstring vlTrap::dumpMethod(HMethod hMethod)
{
	wstring str ;
	HType hType = getMethod(hMethod)->hType;
	if(hType.index < 1)
	{
		str += (std::wstring)L"miss-type\t";
	}
	else if(hType.kind == skCompType)
	{
		str += dumpCompType(hType);
		str += (std::wstring)L"\t";
	}else
	{
		str += (std::wstring)util::getHashString(symTable.types[hType.index - 1].name) +L"\t";
	}
	str += (std::wstring)util::getHashString(getMethod(hMethod)->name) ;

	if(getMethod(hMethod)->hParam)
		str += (std::wstring)L"(" + dumpParam(hMethod) + L")";
	if(getMethod(hMethod)->hVariant)
		str += (std::wstring)L"\n\t{" + dumpVariant(hMethod) + L"\n\t}";
	else
		str += (std::wstring)L";";

	return str;
}

std::wstring vlTrap::dumpField(HType hType) 
{
	assert(hType.index > 0 && hType.index <= (int)symTable.types.size());
	std::wstring str;

	int i = hType.index - 1;
	if(isOleItem(hType))
	{
		HField hField = symTable.types[i].hField;
		while(hField)
		{
			str += (std::wstring)L"\n\t"; 

			HType hType = getField(hField)->hType;
			if(hType.index < 1)
			{
				str += (std::wstring)L"miss-type\t";
			}
			else if(hType.kind == skCompType)
			{
				str += dumpCompType(hType);
				str += (std::wstring)L"\t";
			}else if(hType.kind == skType)
			{
				str += (std::wstring)util::getHashString(getType(hType)->name) +L"\t";
			}else  if(hType.kind == skMethod)
			{
				str += (std::wstring)util::getHashString(symTable.methods[hType.index - 1].name) +L"\t";
			}else 
			{
				str += L" unknown kind";
			}
			str += (std::wstring)util::getHashString(getField(hField)->name) + L";" ;
			hField = ((SymOleField *)getField(hField))->next;
		}
	}else
	{
		HField hField = symTable.types[i].hField;
		for(int j=0;j<symTable.types[i].fieldCount;j++)
		{
			str += (std::wstring)L"\n\t"; 

			HType hType = symTable.fields[symTable.types[i].hField.index -1  + j].hType;
			if(hType.index < 1)
			{
				str += (std::wstring)L"miss-type\t";
			}
			else if(hType.kind == skCompType)
			{
				str += dumpCompType(hType);
				str += (std::wstring)L"\t";
			}else  if(hType.kind == skType)
			{
				str += (std::wstring)util::getHashString(symTable.types[hType.index - 1].name) +L"\t";
			}else  if(hType.kind == skMethod)
			{
				str += (std::wstring)util::getHashString(symTable.methods[hType.index - 1].name) +L"\t";
			}else 
			{
				str += L" unknown kind";
			}
			str += (std::wstring)util::getHashString(symTable.fields[symTable.types[i].hField.index -1 + j ].name) + L";" ;
		}
	}
	return str;
}

std::wstring vlTrap::dumpParam(HMethod hMethod)
{
	assert(hMethod.index > 0 && hMethod.index <= (int)symTable.methods.size());

	std::wstring str;
	HParam hParam = getMethod(hMethod)->hParam;
	for(int j=0;j< getMethod(hMethod)->paramCount;j++)
	{
		if(j)str += (std::wstring)L","; 

		HType hType = getParam(hParam)->hType;
		if(hType.index < 1)
		{
			str += (std::wstring)L"miss-type ";
		}
		else if(hType.kind == skCompType)
		{
			str += dumpCompType(hType);
			str += (std::wstring)L" ";
		}else if(hType.kind == skType)
		{
			str += (std::wstring)util::getHashString(getType(hType)->name) +L" ";
		}else  if(hType.kind == skMethod)
		{
			str += (std::wstring)util::getHashString(symTable.methods[hType.index - 1].name) +L" ";
		}else 
		{
			str += L" unknown kind";
		}
		str += (std::wstring)util::getHashString(symTable.params[hParam.index - 1].name);
		hParam.index++;
	}
	return str;
}
std::wstring vlTrap::dumpVariant(HMethod hMethod)
{
	assert(hMethod.index > 0 && hMethod.index <= (int)symTable.methods.size());

	std::wstring str;
	HVariant hVariant = getMethod(hMethod)->hVariant;
	for(int j=0;j< getMethod(hMethod)->variantCount;j++)
	{
		str += (std::wstring)L"\n\t\t"; 

		HType hType = getParam(hVariant)->hType;
		if(hType.index < 1)
		{
			str += (std::wstring)L"miss-type\t";
		}
		else if(hType.kind == skCompType)
		{
			str += dumpCompType(hType);
			str += (std::wstring)L"\t"; 
		}else if(hType.kind == skType)
		{
			str += (std::wstring)util::getHashString(getType(hType)->name) +L" ";
		}else  if(hType.kind == skMethod)
		{
			str += (std::wstring)util::getHashString(symTable.methods[hType.index - 1].name) +L" ";
		}else 
		{
			str += L" unknown kind";
		}
		str += (std::wstring)util::getHashString(symTable.variants[hVariant.index - 1].name) + L";" ;
		hVariant.index ++;
	}
	return str;
}

std::wstring vlTrap::dumpTypeParam(Handle hType)
{
	/*HTypeParam hTypeParam;
	int count = 0;
	if(hType.kind==skType)
	{
		hTypeParam = getType(hType)->hTypeField);
		count = getType(hType)->typeFieldCount;
	}else if(hType.kind==skMethod)
	{
		hTypeParam = getType(hType)->hTypeField);
		count = getType(hType)->typeFieldCount;

	}*/

	return L"";
}

std::wstring vlTrap::dump()
{
	wstring str;
	//dump types
	for(int i=0;i<(int)symTable.types.size();i++)
	{
		str += (std::wstring)L"\n" ;
		str += dumpType(Handle(skType,i+1));
	}
	return str;
}

bool vlTrap::isSuperClass(HType super, HType child)
{
	if(!child) return false;
	SymType * parent = getType(child);
	if(parent)
	{   
		if(parent->baseType == super) return true;
		if(parent->baseType) return isSuperClass(super,parent->baseType);
	}
	return false;
}

HType vlTrap::findOleChild(Handle hAxis,int tk)
{
	const wchar_t* str = util::getHashString(tk);
	//ole namespace 
	if(hAxis.kind == skNamespace)
	{
		SymNSOleTerm * nsOleTerm = 0;
		if(!getNamespace(hAxis)->hNsOleTerm) return Handle();
		nsOleTerm = getNSOleTerm(getNamespace(hAxis)->hNsOleTerm);
		const wchar_t* atole = util::getHashString(nsOleTerm->oleName);
		if(atole || (ITypeLib*)nsOleTerm->ptLib)    //load ole class
		{
			ITypeLib* ptl = (ITypeLib*)nsOleTerm->ptLib;
			if(!ptl)
			{
				ptl = vlOle::load_ITypeLib(util::hashString((const wstring_t)atole));
				//set typelib to back
				nsOleTerm->ptLib = ptl;
			}
			if(ptl)
			{
				ITypeInfo * pti = vlOle::lookup_ITypeInfo(ptl,tk);
				if(pti)
				{

					//make a new ole object
					HType h = this->addType(hAxis,tkClass,tk);
					this->getType(h)->hOleTerm = this->addOleTerm(pti);
					return h;
				}
			}
		}
		return Handle();
	}
	else if(hAxis.kind==skType || hAxis.kind==skNType)
	{//ole class
		if(!getType(hAxis)->hOleTerm) return Handle();
		SymOleTerm* oleTerm = getOleTerm(getType(hAxis)->hOleTerm);
		ITypeInfo * pti = (ITypeInfo *)oleTerm->typeInfo;
		if(!pti) return Handle();

		MEMBERID  pid[1];
		TYPEATTR* pTypeAttr;
		HRESULT hr = pti->GetTypeAttr(&pTypeAttr);
		unsigned short pcFound = 1;
		hr = pti ->GetIDsOfNames(  (LPOLESTR *)&str,1,pid);
		if(FAILED(hr))
		{
			pti->ReleaseTypeAttr(pTypeAttr);
			return Handle();
		}

		FUNCDESC* pFuncDesc = 0;
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
					FUNCDESC* pFuncDesc1 = 0;
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
			for (int i = 0; i < pFuncDesc->cParams; i++)
			{
				/*info.pVarTypes[i] = pFuncDesc->lprgelemdescParam[i].tdesc.vt;
				if (info.pVarTypes[i] == VT_PTR)
				info.pVarTypes[i] = (VARTYPE)(pFuncDesc->lprgelemdescParam[i].tdesc.lptdesc->vt | VT_BYREF);
				if (info.pVarTypes[i] == VT_USERDEFINED)
				info.pVarTypes[i] = AtlGetUserDefinedType(pTypeInfo, pFuncDesc->lprgelemdescParam[i].tdesc.hreftype);  */
			}
			HType ty =  getTypeHandle(util::hashString(L"undef"));
			VARTYPE vtReturn = pFuncDesc->elemdescFunc.tdesc.vt;
			TYPEDESC*  lptdesc = &pFuncDesc->elemdescFunc.tdesc;
			if(vtReturn==VT_PTR)
			{
				vtReturn = pFuncDesc->elemdescFunc.tdesc.lptdesc->vt;
				lptdesc = pFuncDesc->elemdescFunc.tdesc.lptdesc;
			}
			if(vtReturn == VT_USERDEFINED)
			{
				ITypeInfo* pTypeInfo;
				HREFTYPE hreftype = lptdesc->hreftype;
				pti->GetRefTypeInfo(hreftype,&pTypeInfo);

				TYPEATTR* pTypeAttr;
				hr = pti->GetTypeAttr(&pTypeAttr);
				ty =   getTypeHandle(util::hashString(L"int"));
				if(!FAILED(hr))
				{
					if(pTypeAttr->typekind==TKIND_INTERFACE|| pTypeAttr->typekind==TKIND_DISPATCH)
					{
						BSTR tname= 0;
						pTypeInfo->GetDocumentation(MEMBERID_NIL,&tname,0,0,0);

						if(getType(hAxis)->name==util::hashString(tname))
						{
							ty = hAxis;
						}else
						{
							HType wc = lookupType(
									getParent(hAxis),util::hashString(tname));
							if(wc)
							{
								ty = wc;
							}
							else
							{
								ty = findOleChild(getParent(hAxis),util::hashString(tname));
							}
						}
						::SysFreeString(tname);
					}
				}
				pTypeInfo->ReleaseTypeAttr(pTypeAttr);
			}else
			{
				ty =  vlOle::transOleType(this,vtReturn);
			}
			pti->ReleaseTypeAttr(pTypeAttr); 

			if(!FAILED(hr) && pFuncDesc )
			{
				
				HField field ;
				HMethod method ;
				if(!(pFuncDesc->invkind== INVOKE_FUNC || 
					(pFuncDesc->invkind== INVOKE_PROPERTYGET && pFuncDesc->cParams >= 1) ||
					(pFuncDesc->invkind== INVOKE_PROPERTYPUT && pFuncDesc->cParams >= 2) ))
				{
					//add class field
					field = addField(hAxis,tk,0,ty);
					((SymOleField*)getField(field))->propGet = nIndexGet;
					((SymOleField*)getField(field))->propPut = nIndexPut;
					pti->ReleaseFuncDesc(pFuncDesc);
					return  field;
				}else
				{
					//add class method
					method = addMethod(hAxis,tk,0,ty);
					((SymOleMethod*)getMethod(method))->nMethod = nIndexGet;

					BSTR  bstrName[32] ;
					UINT      nNames;
					pti->GetNames( pFuncDesc->memid, bstrName, 32, & nNames );
					for (int i = 0; i < pFuncDesc->cParams; i++)
					{
						HType typ =  getTypeHandle(util::hashString(L"undef"));
						VARTYPE typeParam = 0;
						TYPEDESC*  lptdesc = 0;
						if(i>=0)
						{
							typeParam = pFuncDesc->lprgelemdescParam[i].tdesc.vt;
							lptdesc = &pFuncDesc->lprgelemdescParam[i].tdesc;
						}else
						{
							typeParam = pFuncDesc->elemdescFunc.tdesc.vt;
							lptdesc = &pFuncDesc->elemdescFunc.tdesc;
						}
						if(typeParam==VT_PTR)
						{
							typeParam = pFuncDesc->lprgelemdescParam[i].tdesc.lptdesc->vt;
							lptdesc = pFuncDesc->lprgelemdescParam[i].tdesc.lptdesc;
						}
						if(typeParam == VT_USERDEFINED)
						{
							ITypeInfo* pTypeInfo;
							HREFTYPE hreftype = lptdesc->hreftype;
							pti->GetRefTypeInfo(hreftype,&pTypeInfo);

							TYPEATTR* pTypeAttr;
							hr = pti->GetTypeAttr(&pTypeAttr);
							typ =   getTypeHandle(util::hashString(L"int"));
							if(!FAILED(hr))
							{
								if(pTypeAttr->typekind==TKIND_INTERFACE|| pTypeAttr->typekind==TKIND_DISPATCH)
								{
									BSTR tname= 0;
									pTypeInfo->GetDocumentation(MEMBERID_NIL,&tname,0,0,0);

									if(getType(hAxis)->name==util::hashString(tname))
									{
										typ = hAxis;
									}else
									{
										HType wc = lookupType(
											getParent(hAxis),util::hashString(tname));
										if(wc)
											typ = wc;
										else
											typ = findOleChild(getParent(hAxis),util::hashString(tname));
									}
									::SysFreeString(tname);
								}
							}
							pTypeInfo->ReleaseTypeAttr(pTypeAttr);
						}else
						{
							typ =  vlOle::transOleType(this,typeParam);
						}
						if(i>=0)
						{
							addParam(method,util::hashString(bstrName[i + 1]),0,typ);
							::SysFreeString(bstrName[i + 1]);
						}
					}
					pti->ReleaseFuncDesc(pFuncDesc);
					return  method;
				}
			}
		}
	}
	return Handle();
}

Handle  vlTrap::findMatchedMethod( Handle scope, Handle nest, int ident, int nargs,const std::vector<HType> &typs,int argstart)
{
	struct
	{
		bool bShouldClass;
		bool bNotMatch;
	public:
		Handle found_in_child(vlTrap* trap ,Handle  parent , int ident, int nargs,const std::vector<HType> &typs,int argstart = 0)
		{
			if(parent.kind==skNamespace)
			{
				multimap<Handle,Handle>::iterator lIter = trap->symTable.nsitems.lower_bound(parent);
				multimap<Handle,Handle>::iterator uIter = trap->symTable.nsitems.upper_bound(parent);
				for(;lIter!=uIter; lIter++)
				{
					if(lIter->second.kind!=skMethod && lIter->second.kind!=skOleMethod) continue;
					if(!trap->getMethod(lIter->second)->dirty && trap->getMethod(lIter->second)->name==ident)
					{
						if(trap->isMethodArgMatched(lIter->second,nargs,typs,argstart)) return lIter->second;
						bNotMatch = true;
					}
				}
			}else if(parent.kind==skType)
			{
				if(trap->isOleItem(parent))
				{
					HMethod hMethod = trap->getType(parent)->hMethod;
					while(hMethod)
					{
						if(!trap->getMethod(hMethod)->dirty && trap->getMethod(hMethod)->name==ident)
						{
							if(trap->isMethodArgMatched(hMethod,nargs,typs,argstart)) return hMethod;
							bNotMatch = true;
						}
						hMethod = ((SymOleMethod*)trap->getMethod(hMethod))->next;
					}
				}
				else
				{
					HMethod hMethod = trap->getType(parent)->hMethod;
					int methodCount = trap->getType(parent)->methodCount;
					while(methodCount)
					{
						if(!trap->getMethod(hMethod)->dirty && trap->getMethod(hMethod)->name==ident)
						{
							if(trap->isMethodArgMatched(hMethod,nargs,typs,argstart)) return hMethod;
							bNotMatch = true;
						}
						hMethod.index++;
						methodCount--;
					}
				}
			}
			return Handle(skError,1);
		}

		Handle found_in_super(vlTrap* trap ,HType hType , int ident, int nargs,const std::vector<HType> &typs,int argstart = 0)
		{
			Handle ret = found_in_child(trap,hType,ident,nargs,typs,argstart);
			if(ret.kind) return ret;
			if(trap->getType(hType)->baseType)
				return found_in_super(trap,trap->getType(hType)->baseType,ident,nargs,typs,argstart);
			return Handle(skError,1);
		}
	}__;


	__.bShouldClass =  false; 
	__.bNotMatch = false;

	Handle  wclass;

	if(scope != nest) nest = scope;
	//find nest
	if(nest)
	{
		if(nest.kind==skType)
		{
			wclass = nest;
			__.bShouldClass = wclass.index ? true : false;
		}else
		{
			//found in namespace
			Handle ret = __.found_in_child(this,nest,ident,nargs,typs,argstart);
			if(ret.kind) return ret;
			if(__.bNotMatch) return Handle(skError,2);
			return Handle(skError,1);
		}
	}

	//found in class
	if(!wclass.index && scope) 
	{
		//get scope wclass
		wclass = scope;
		if(wclass.kind==skMethod || wclass.kind==skOleMethod)wclass = getParent(scope);
	}
	if(wclass)
	{
		//found owner
		Handle ret = __.found_in_child(this,wclass,ident,nargs,typs,argstart);
		if(ret.kind)  return ret;
		//found base class
		ret = __.found_in_super(this,wclass,ident,nargs,typs,argstart);
		if(ret.kind)  return ret;
		if(__.bShouldClass)
		{
			if(__.bNotMatch) return Handle(skError,2);
			return Handle(skError,1);
		}
	}

	//find in current namespace and forword namespace
	while(scope.index)
	{
		scope = getParent(scope);
		if(!scope)break;
		Handle ret = __.found_in_child(this,scope,ident,nargs,typs,argstart);
		if(ret.kind)  return ret;
	}
	if(__.bNotMatch) return Handle(skError,2);
	return Handle(skError,1);
}

bool vlTrap::isMethodArgMatched(Handle fn, int nargs,const std::vector<HType> &typs,int argstart)
{
	//match args
	int args = 0;
	HParam hParam	= getMethod(fn)->hParam;
	int paramCount	= getMethod(fn)->paramCount;
	HType hVariant  = getTypeHandle(util::hashString(L"variant"));
	while(paramCount) 
	{
		args++;
		if(nargs - args < 0 )
		{
			if(!getParam(hParam)->hasInitValue)break;
		}else
		{
			if(getParam(hParam)->ellipsis) return true;
			HType hType = getParam(hParam)->hType;
			if(hType.index!=typs[argstart + args - 1].index)
			{
				
				if(hType.index!=hVariant.index && typs[argstart + args - 1].index!=hVariant.index)
				{
					if(typs[argstart + args - 1].kind!=skMethod)
					{
						if(!hType) break;
						if(!(isOleItem(hType)&& isOleItem(typs[argstart + args - 1])))
						{
							if(!isSuperClass(hType,typs[argstart + args - 1]))break;
						}
					}
				}
			}
			/*
			if( typ >> 8  && wp.getPrimaryType(this) == type::function_)
			{
				int ty = typs[argstart + args - 1];
				if(!(ty>>8) || (ty & 0xff) != type::function_) break;
				//判断函数指针的一致性
			}
			else if(!type_can_imp_conv(typ,typs[argstart + args - 1]))break;
			*/
		}
		hParam.index++;
		paramCount--;
	}
	if(args >= nargs && !paramCount)
		return true;
	return false;
}

HMethod vlTrap::findMethod(Handle hAxis, int methodID)
{
	if(hAxis)
		return lookupMethod(hAxis,	methodID);
	else
		return Handle();
}

Handle vlTrap::getParent(Handle hItem)
{
	if(symTable.parentitems.find(hItem)!=symTable.parentitems.end())
		return symTable.parentitems[hItem];
	return Handle();
}

HMethod vlTrap::findMatchAsMethod(Handle wc, Handle wm) //virtual found
{
	int id = getMethod(wm)->name;
	Handle hItem = getType(wc)->hMethod;
	int hsize = getType(wc)->methodCount; 
	while(hsize)
	{
		if(!getMethod(hItem)->dirty)
		{
			if(hItem==wm && getMethod(hItem)->addr) return wm; //owner;
			if(getMethod(hItem)->name==id && getMethod(hItem)->addr)
			{//判断型参是否匹配
				if(getMethod(hItem)->paramCount== getMethod(wm)->paramCount)
				{
					int paramCount = getMethod(hItem)->paramCount;
					HParam hparam1 = getMethod(hItem)->hParam;
					HParam hparam2 = getMethod(wm)->hParam;
					while(paramCount)
					{
						if(getParam(hparam1)->spec!=getParam(hparam2)->spec) break;
						if(getParam(hparam1)->hType!=getParam(hparam2)->hType) break;
						hparam1.index++;
						hparam2.index++;
						paramCount--;
					}
					if(!paramCount) return hItem;
				}
			}
		}
		hItem.index++;
		hsize--;
	}
	
	if(getParent(wm)==wc && getMethod(wm)->addr) return wm;
	if(getType(wc)->baseType)return findMatchAsMethod(getType(wc)->baseType,wm);
	return Handle();
}

Handle vlTrap::lookupIn(Handle axis,int id)
{
	if(axis.kind==skNamespace)
	{
		multimap<Handle,Handle>::iterator lIter = symTable.nsitems.lower_bound(axis);
		multimap<Handle,Handle>::iterator uIter = symTable.nsitems.upper_bound(axis);
		for(;lIter!=uIter; lIter++)
		{
			if(lIter->second.kind==skNamespace)
			{
				if(getNamespace(lIter->second)->name==id) return lIter->second;
			}
			if(lIter->second.kind==skType)
			{
				if(!getType(lIter->second)->dirty && getType(lIter->second)->name==id) return lIter->second;
			}
			if(lIter->second.kind==skField)
			{
				if(getField(lIter->second)->name==id) return lIter->second;
			}
			if(lIter->second.kind==skMethod)
			{
				if(!getMethod(lIter->second)->dirty && getMethod(lIter->second)->name==id) return lIter->second;
			}
			if(lIter->second.kind==skOleField)
			{
				if(getField(lIter->second)->name==id) return lIter->second;
			}
			if(lIter->second.kind==skOleMethod)
			{
				if(!getMethod(lIter->second)->dirty && getMethod(lIter->second)->name==id) return lIter->second;
			}
		}	
	}
	else if(axis.kind==skType)
	{
		if(isOleItem(axis))
		{
			Handle hItem = getType(axis)->hMethod;
			while(hItem)
			{
				if(!getMethod(hItem)->dirty && getMethod(hItem)->name==id) return hItem;
				hItem = ((SymOleMethod *)getMethod(hItem))->next;
			}
			hItem = getType(axis)->hField;
			while(hItem)
			{
				if(getField(hItem)->name==id) return hItem;
				hItem = ((SymOleField *)getField(hItem))->next;
			}
		}else
		{
			Handle hItem = getType(axis)->hMethod;
			int hsize = getType(axis)->methodCount; 
			while(hsize)
			{
				if(!getMethod(hItem)->dirty && getMethod(hItem)->name==id) return hItem;
				hItem.index++;
				hsize--;
			}
			hItem = getType(axis)->hField;
			hsize = getType(axis)->fieldCount;
			while(hsize)
			{
				if(getField(hItem)->name==id) return hItem;
				hItem.index++;
				hsize--;
			}
		}
	}
	else if(axis.kind==skMethod || axis.kind==skOleMethod)
	{
		Handle hItem = getMethod(axis)->hParam;
		int hsize = getMethod(axis)->paramCount;
		while(hsize)
		{
			if(getParam(hItem)->name==id) return hItem;
			hItem.index++;
			hsize--;
		}
		hItem = getMethod(axis)->hVariant;
		hsize = getMethod(axis)->variantCount;
		while(hsize)
		{
			if(getVariant(hItem)->name==id) return hItem;
			hItem.index++;
			hsize--;
		}
	}
	return Handle();
}
Handle vlTrap::lookup(Handle axis,int id)
{
	Handle w = lookupIn(axis,id);
	if(w.index) return w;
	
	if(axis.kind!=skType && axis.kind!=skNamespace) return Handle();
	if(isOleItem(axis))
	{
		Handle h = findOleChild(axis,id);
		if(h.index) return h;
	}

	if(axis.kind!=skType) return Handle();

	//find on super
	Handle  trSuper = getType(axis)->baseType;
	if(trSuper) return  lookup(trSuper,id);

	return Handle();
}

Handle vlTrap::lookup(Handle axis,int id,int kind)
{
	switch(kind)
	{
	case skType:		return lookupType(axis,id);
	case skMethod:		return lookupMethod(axis,id);
	case skOleMethod:	return lookupMethod(axis,id);
	case skItem:		return lookupItem(axis,id);
	default:
		return lookup(axis,id);
	}
}

Handle vlTrap::lookupItem(Handle axis,int id)
{
	return lookup(axis,id);
}

Handle vlTrap::lookupMethod(Handle axis,int id)
{
	if(axis.kind==skNamespace)
	{
		multimap<Handle,Handle>::iterator lIter = symTable.nsitems.lower_bound(axis);
		multimap<Handle,Handle>::iterator uIter = symTable.nsitems.upper_bound(axis);
		for(;lIter!=uIter; lIter++)
		{
			if(lIter->second.kind==skMethod)
			{
				if(!getMethod(lIter->second)->dirty && getMethod(lIter->second)->name==id) return lIter->second.kind;
			}
		}	

		//find from ole
		if(isOleItem(axis))
		{
			Handle h = findOleChild(axis,id);
			if(h.index) return h;
		}
	}
	else if(axis.kind==skType)
	{

		if(isOleItem(axis))
		{
			Handle hItem = getType(axis)->hMethod;
			while(hItem)
			{
				if(((SymOleMethod *)getMethod(hItem))->name==id) return hItem;
				hItem = ((SymOleMethod *)getMethod(hItem))->next;
			}
			Handle h = findOleChild(axis,id);
			if(h.index) return h;
		}else
		{
			Handle hItem = getType(axis)->hMethod;
			int hsize = getType(axis)->methodCount; 
			while(hsize)
			{
				if(!getMethod(hItem)->dirty && getMethod(hItem)->name==id) return hItem;
				hItem.index++;
				hsize--;
			}
		}
	}
	return Handle();
}


Handle vlTrap::lookupType(Handle axis,int id)
{
	if(axis.kind==skNamespace)
	{
		multimap<Handle,Handle>::iterator lIter = symTable.nsitems.lower_bound(axis);
		multimap<Handle,Handle>::iterator uIter = symTable.nsitems.upper_bound(axis);
		for(;lIter!=uIter; lIter++)
		{
			if(lIter->second.kind==skType)
			{
				if(!getType(lIter->second)->dirty && getType(lIter->second)->name==id) return lIter->second;
			}
		}	

		//find from ole
		if(isOleItem(axis))
		{
			Handle h = findOleChild(axis,id);
			if(h.index) return h;
		}
	}
	return Handle();
}

double * vlTrap::addReal(double d)
{
	if(symTable.realitems.find(d)==symTable.realitems.end())
	{
		double * ptr = (double *)symTable.datapool.append_data(&d,sizeof(d));
		symTable.realitems.insert(std::pair<double,double *>(d,ptr));
	}
	return symTable.realitems[d];
}
double * vlTrap::getRealPtr(double d)
{
	return addReal(d);
}

