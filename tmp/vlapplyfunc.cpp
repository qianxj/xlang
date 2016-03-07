
#include "vlcontext.hpp"
#include "vlutils.hpp"
#include "vltoken.hpp"
#include "vlastToken.hpp"
#include "vlastfactory.hpp"
#include "vlparser.hpp"
#include "vlapply.hpp"
#include "vlapplyfunc.hpp"

using namespace xlang;

VLANG_API vlContext * fnSymGetContext(vlContext* context)
{
	return context;
}

VLANG_API wchar_t* fnSymGetNamespaceName(vlContext* context,Handle hNamespace)
{
	return util::getHashString(context->trap->getNamespace(hNamespace)->name);
}

//ole process
VLANG_API int fnSymGetOleMethod(vlContext* context,Handle hMethod)
{
	return ((SymOleMethod *)context->trap->getMethod(hMethod))->nMethod;
}

VLANG_API int fnSymGetOlePropGet(vlContext* context,Handle hField)
{
	return ((SymOleField *)context->trap->getField(hField))->propGet;
}

VLANG_API int fnSymGetOlePropPut(vlContext* context,Handle hField)
{
	return ((SymOleField *)context->trap->getField(hField))->propPut;
}

VLANG_API bool fnSymIsOleItem(vlContext* context,Handle  hTerm)
{
	return context->trap->isOleItem(hTerm);
}

VLANG_API wchar_t* fnSymGetTypeOleName(vlContext* context,Handle  hType)
{
	return util::getHashString(context->trap->getTypeNSOleTerm(hType)->oleName);
}

VLANG_API void * fnSymGetOleTypeInfo(vlContext* context,Handle  hType)
{
	return context->trap->getTypeOleTerm(hType)->typeInfo;
}

//symtable
VLANG_API void *  fnSymGetSymTable(vlContext* context)
{
	return context->trap;
}
//code prt
VLANG_API unsigned char* fnSymGetCodePc(vlContext* context)
{
	return context->trap->getCoder()->getCodePc();
}

//add symbol term
VLANG_API Handle fnSymAddNamespace(vlContext* context,Handle hAxis, int id)
{
	return context->trap->addNamespace(hAxis,id);
}

VLANG_API Handle fnSymAddType(vlContext* context,Handle hAxis,int kind, int id,bool todef)
{
	return context->trap->addType(hAxis,(xlang::TypeKind)kind,id,todef);
}

VLANG_API Handle fnSymAddField(vlContext* context,Handle hAxis,int  id,int spec, Handle typeref)
{
	return context->trap->addField(hAxis,id,spec,typeref);
}

VLANG_API Handle fnSymAddMethod(vlContext* context,Handle hAxis, int id, int spec, Handle hType)
{
	return context->trap->addMethod(hAxis,id,spec,hType);
}

VLANG_API Handle fnSymAddParam(vlContext* context,Handle hMethod,int id, int spec,Handle hType)
{
	return context->trap->addParam(hMethod,id,spec,hType);
}

VLANG_API Handle fnSymAddVariant(vlContext* context,Handle hMethod,int id,int spec,Handle  hType)
{
	return context->trap->addVariant(hMethod,id,spec,hType);
}


//comp type
VLANG_API Handle fnSymMakeTypeRef(vlContext* context)
{
	return Handle(skCompType,(int)context->trap->symTable.comptypes.size()+ 1) ;
}

VLANG_API void fnSymAddCompTypeItem(vlContext* context,Handle hType)
{
	context->trap->addCompTypeItem(hType);
}

VLANG_API void fnSymAddCompTypeRank(vlContext* context,int rank)
{
	context->trap->addCompTypeRank(rank);
}

VLANG_API void fnSymInsertCompTypeItem(vlContext* context,Handle hCompType,Handle hType)
{
	context->trap->insertCompTypeItem(hCompType,hType);
}

VLANG_API void fnSymInsertCompTypeRank(vlContext* context,Handle hCompType,int rank)
{
	context->trap->insertCompTypeItem(hCompType,rank);
}

VLANG_API Handle fnSymGetCompTypeItem(vlContext* context,Handle hCompType)
{
	return context->trap->getCompTypeItem(hCompType);
}
VLANG_API int fnSymGetCompTypeRank(vlContext* context,Handle hCompType)
{
	return context->trap->getCompTypeRank(hCompType);
}
VLANG_API Handle fnSymStepCompType(vlContext* context,Handle hCompType)
{
	return context->trap->stepCompType(hCompType);
}

//method
VLANG_API Handle fnSymGetMethodHandle(vlContext* context,Handle hType, int id)
{
	return context->trap->getMethodHandle(hType,id);
}

VLANG_API Handle fnSymGetMethodVariantHandle(vlContext* context,Handle hMethod,int id)
{
	return context->trap->getMethodVariantHandle(hMethod,id);
}

//method paramm
VLANG_API Handle fnSymGetMethodParamHandle(vlContext* context,Handle hMethod,int id)
{
	return context->trap->getMethodParamHandle(hMethod,id);
}

VLANG_API Handle fnSymGetMethodParamHandleByIndex(vlContext* context,Handle hMethod,int idx)
{
	return context->trap->getMethodParamHandleByIndex(hMethod,idx);
}

VLANG_API int fnSymGetParamSpec(vlContext* context,Handle hParam)
{
	return context->trap->getParam(hParam)->spec;
}

VLANG_API int fnSymGetParamInitValue(vlContext* context,Handle hParam)
{
	return context->trap->getParam(hParam)->initValue;
}

VLANG_API int fnSymGetMethodParamCount(vlContext* context,Handle hMethod)
{
	return context->trap->getMethod(hMethod)->paramCount;
}

//method pcode
VLANG_API void fnSymSetMethodPCode(vlContext* context,Handle hMethod,vlAstAxis* pcode)
{
	context->trap->getMethod(hMethod)->pcode = pcode;
}

VLANG_API vlAstAxis* fnSymGetMethodPCode(vlContext* context,Handle hMethod)
{
	return (vlAstAxis*)context->trap->getMethod(hMethod)->pcode;
}


VLANG_API Handle fnSymGetTypeHandle(vlContext* context,int id)
{
	return context->trap->getTypeHandle(id);
}
VLANG_API Handle fnSymGetGlobalNamespace(vlContext* context)
{
	return context->trap->getGlobalNamespace();
}
VLANG_API void fnSymSetTypeKind(vlContext* context,Handle hType,int kind)
{
	return context->trap->setTypeKind(hType,(TypeKind)kind);
}

VLANG_API int fnSymGetTypeKind(vlContext* context,Handle hType)
{
	return context->trap->getTypeKind(hType);
}

VLANG_API Handle fnSymGetFieldHandle(vlContext* context, Handle hType,int id)
{
	return context->trap->getFieldHandle(hType,id);
}
VLANG_API Handle fnSymGetTypeMethodHandle(vlContext* context,Handle hType,int id)
{
	return context->trap->getMethodHandle(hType,id);
}

VLANG_API int fnSymGetTypeAllocSize(vlContext* context,Handle hType)
{
	int msize = 0;
	return context->trap->getAllocSize(hType,msize);
}

VLANG_API int fnSymGetFieldAllocSize(vlContext* context,Handle hField)
{
	return context->trap->getFieldAllocSize(hField);
}
VLANG_API int fnSymGetOffsetOfField(vlContext* context,Handle hType, Handle hField)
{
	 return context->trap->getFieldOffset(hType,hField);
}

VLANG_API int fnSymGetMethodVariantSize(vlContext* context,Handle hMethod)
{
	return context->trap->getMethodVariantSize(hMethod);
}

//term
VLANG_API void fnSymSetNSOleTerm(vlContext* context,Handle hNamespace, Handle hNsOleTerm)
{
	return context->trap->setNSOleTerm(hNamespace,hNsOleTerm);
}

VLANG_API Handle fnSymAddNSOleTerm(vlContext* context,int name, void * ptLib, void * typeInfo )
{
	return context->trap->addNSOleTerm(name,ptLib,typeInfo);
}

VLANG_API int fnSymGetOffsetOfTerm(vlContext* context,Handle hMethod, Handle hTerm)
{
	if(hMethod.kind==skType||hMethod.kind==skNType) return context->trap->getFieldOffset(hMethod,hTerm);
	return context->trap->getFrameOffset(hMethod,hTerm);
}

VLANG_API void fnSymAddBase(vlContext* context,Handle hType,Handle hTypeBase)
{
	return context->trap->addTypeBase(hType,hTypeBase);
}
VLANG_API int fnSymGetTermNameID(vlContext* context,Handle hTerm)
{
	return context->trap->getTermNameID(hTerm);
}
VLANG_API const wchar_t* fnSymGetTermName(vlContext* context,Handle hTerm)
{
	return context->trap->getTermName(hTerm);
}
VLANG_API Handle fnSymGetTermType(vlContext* context,Handle hTerm)
{
	return context->trap->getTermType(hTerm);
}
VLANG_API int fnSymGetTermSpec(vlContext* context,Handle hTerm)
{
	return context->trap->getTermSpec(hTerm);
}
VLANG_API int fnSymGetTermInitValue(vlContext* context,Handle hTerm)
{
	return context->trap->getTermInitValue(hTerm);
}
VLANG_API void fnSymSetTermInitValue(vlContext* context,Handle hTerm, int val)
{
	return context->trap->setTermInitValue(hTerm,val);
}
VLANG_API int fnSymGetTypeTermFrameSize(vlContext* context,Handle hTerm)
{
	return context->trap->getTypeTermFrameSize(hTerm);
}

VLANG_API Handle fnSymLookupTypeItem(vlContext* context,Handle axis,int id)
{
	return context->trap->lookupItem(axis,id);
}
VLANG_API Handle fnSymLookupType(vlContext* context,Handle axis,int id)
{
	return context->trap->lookupType(axis,id);
}
VLANG_API Handle fnSymLookupMethod(vlContext* context,Handle axis,int id)
{
	return context->trap->lookupMethod(axis,id);
}
VLANG_API Handle fnSymLookupItem(vlContext* context,Handle axis,int id)
{
	return context->trap->lookupItem(axis,id);
}

VLANG_API Handle fnSymGetParent(vlContext* context,Handle hTerm)
{
	return context->trap->getParent(hTerm);
}
VLANG_API bool fnSymIsSuperClass(vlContext* context,Handle hSuper, Handle hType)
{
	return context->trap->isSuperClass(hSuper,hType);
}

//real const ptr
VLANG_API double * fnSymAddReal(vlContext* context,double val)
{
	return context->trap->addReal(val);
}
VLANG_API double * fnSymGetRealPtr(vlContext* context,double val)
{
	return context->trap->getRealPtr(val);
}
//dump symbol term
VLANG_API const wchar_t* fnSymDump(vlContext* context)
{
	return context->trap->dump().c_str();
}
VLANG_API const wchar_t* fnSymDumpType(vlContext* context,Handle hType)
{
	return context->trap->dumpType(hType).c_str();
}
VLANG_API const wchar_t* fnSymDumpMethod(vlContext* context,Handle hMethod)
{
	return context->trap->dumpMethod(hMethod).c_str();
}
VLANG_API const wchar_t* fnSymDumpField(vlContext* context,Handle hTerm)
{
	return context->trap->dumpField(hTerm).c_str();
}
VLANG_API const wchar_t* fnSymDumpParam(vlContext* context,Handle hMethod)
{
	return context->trap->dumpParam(hMethod).c_str();
}
VLANG_API const wchar_t* fnSymDumpVariant(vlContext* context,Handle hMethod)
{
	return context->trap->dumpVariant(hMethod).c_str();
}

VLANG_API const wchar_t* fnSymDumpTypeParam(vlContext* context,Handle hTerm)
{
	return context->trap->dumpTypeParam(hTerm).c_str();
}

VLANG_API int fnSymGetMethodSpec(vlContext* context,Handle hMethod)
{
	return context->trap->getTermSpec(hMethod);
}
VLANG_API unsigned char * fnSymGetMethodAddr(vlContext* context,Handle hMethod)
{
	return (unsigned char *)context->trap->getMethod(hMethod)->addr;
}
VLANG_API void fnSymSetMethodAddr(vlContext* context,Handle hMethod,void* addr)
{
	context->trap->getMethod(hMethod)->addr = addr;
}

VLANG_API void fnSymSetMethodNativeFrom(vlContext* context,Handle hMethod,wchar_t* from)
{
	context->trap->setMethodNativeFrom(hMethod,util::hashString(from));
}
VLANG_API void fnSymSetMethodNativeAlias(vlContext* context,Handle hMethod,wchar_t* alias)
{
	context->trap->setMethodNativeAlias(hMethod,util::hashString(alias));
}
VLANG_API wchar_t* fnSymGetMethodNativeFrom(vlContext* context,Handle hMethod)
{
	return context->trap->getMethodNativeFrom(hMethod);
}
VLANG_API wchar_t* fnSymGetMethodNativeAlias(vlContext* context,Handle hMethod)
{
	return context->trap->getMethodNativeAlias(hMethod);
}
VLANG_API bool fnSymIsMethodNative(vlContext* context,Handle hMethod)
{
	return context->trap->isMethodNative(hMethod);
}
VLANG_API void fnSymSetMethodNative(vlContext* context,Handle hMethod,int nativeFrom,int nativeAlias)
{
	context->trap->setMethodNative(hMethod,nativeFrom,nativeAlias);
}

VLANG_API void fnSymSetTypeNativeFrom(vlContext* context,Handle hType,wchar_t* from)
{
	context->trap->setTypeNativeFrom(hType,util::hashString(from));
}
VLANG_API void fnSymSetTypeNativeAlias(vlContext* context,Handle hType,wchar_t* alias)
{
	context->trap->setTypeNativeAlias(hType,util::hashString(alias));
}
VLANG_API wchar_t* fnSymGetTypeNativeFrom(vlContext* context,Handle hType)
{
	return context->trap->getTypeNativeFrom(hType);
}
VLANG_API wchar_t* fnSymGetTypeNativeAlias(vlContext* context,Handle hType)
{
	return context->trap->getTypeNativeAlias(hType);
}


VLANG_API Handle fnSymFindOleChild(vlContext* context,Handle hAxis,int id)
{
	return context->trap->findOleChild(hAxis,id);
}
VLANG_API Handle fnSymFindMatchedMethod(vlContext* context,Handle scope, Handle nest, int ident, int nargs,std::vector<HType> & args, int argstart)
{
	return	context->trap->findMatchedMethod(scope,nest,ident,nargs,args,argstart);
}

//logic operate
bool fnGt(vlContext* context,int lhs,int rhs)
{
	return lhs>rhs;
}
bool fnGte(vlContext* context,int lhs,int rhs)
{
	return lhs>=rhs;
}
bool fnLt(vlContext* context,int lhs,int rhs)
{
	return lhs<rhs;
}
bool fnLte(vlContext* context,int lhs,int rhs)
{
	return lhs<=rhs;
}
bool fnEq(vlContext* context,int lhs,int rhs) 
{
	return lhs==rhs;
}
bool fnNeq(vlContext* context,int lhs,int rhs)
{
	return lhs!=rhs; 
}
bool fnNil(vlContext* context,int lhs)
{
	return !lhs;
}
bool fnNot(vlContext* context,int lhs)
{
	return !lhs;
}

bool fnGt(vlContext* context,wchar_t* lhs,wchar_t* rhs)
{
	return wcscmp(lhs,rhs) > 0;
}
bool fnGte(vlContext* context,wchar_t* lhs,wchar_t* rhs)
{
	return wcscmp(lhs,rhs) >= 0;
}
bool fnLt(vlContext* context,wchar_t* lhs,wchar_t* rhs) 
{
	return wcscmp(lhs,rhs) < 0;
}
bool fnLte(vlContext* context,wchar_t* lhs,wchar_t* rhs)
{
	return wcscmp(lhs,rhs) <= 0;
}
bool fnEq(vlContext* context,wchar_t* lhs,wchar_t* rhs) 
{
	return wcscmp(lhs,rhs) == 0;
}
bool fnNeq(vlContext* context,wchar_t* lhs,wchar_t* rhs)
{
	return wcscmp(lhs,rhs) != 0;
}
bool fnNil(vlContext* context,wchar_t* lhs)
{
	return !lhs; 
}
bool fnNot(vlContext* context,wchar_t* lhs) 
{
	return !lhs;
}

bool fnAnd(vlContext* context,bool lhs,bool rhs)
{
	return lhs && rhs;
}
bool fnOr(vlContext* context,bool lhs,bool rhs)
{
	return lhs || rhs;
}

wchar_t* fntoString(vlContext* context,int lhs)
{
	wchar_t buf[80];
	swprintf_s(buf,80,L"%d",lhs);
	return _wcsdup(buf);
}

wchar_t* fnAdd(vlContext* context,wchar_t* lhs,wchar_t* rhs)
{
	std::wstring str = lhs;
	str += rhs;
	return _wcsdup(str.c_str());
}
int fnAdd(vlContext* context,int lhs,int rhs)
{
	return lhs + rhs;
}
int fnSub(vlContext* context,int lhs,int rhs)
{
	return lhs - rhs;
}
int fnMul(vlContext* context,int lhs,int rhs)
{
	return lhs * rhs;
}
int fnDiv(vlContext* context,int lhs,int rhs)
{
	return lhs / rhs;
}
int fnMod(vlContext* context,int lhs,int rhs)
{
	return lhs % rhs;
}
int fnShr(vlContext* context,int lhs,int rhs)
{
	return lhs >> rhs;
}
int fnShl(vlContext* context,int lhs,int rhs)
{
	return lhs<<rhs;
}
int fnBand(vlContext* context,int lhs,int rhs)
{
	return lhs & rhs;
}
int fnBor(vlContext* context,int lhs,int rhs)
{
	return lhs | rhs;
}
int fnBnot(vlContext* context,int lhs)
{
	return ~lhs;
}
int fnXor(vlContext* context,int lhs,int rhs)
{
	return lhs ^ rhs;
}
int fnInc(vlContext* context,int lhs)
{
	return lhs + 1;
}
int fnDec(vlContext* context,int lhs)
{
	return lhs - 1;
}
int fnNeg(vlContext* context,int lhs)
{
	return - lhs; 
}

VLANG_API bool fnIsIdentifier(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akIdentifierNode)unify = true;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tk.ty == tokenType::tIdent)unify = true;
	}
	return unify ;
}

VLANG_API bool fnIsLiteral(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tk.ty == tokenType::tBool || tk.ty == tokenType::tDouble ||
			tk.ty == tokenType::tInt || tokenType::tString || tk.ty==tokenType::tChar)
			unify = true;
	}

	return unify ;
}
VLANG_API bool fnIsNumber(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tk.ty == tokenType::tDouble ||
			tk.ty == tokenType::tInt)
			unify = true;
	}
	return unify ;
}
VLANG_API bool fnIsDouble(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tk.ty == tokenType::tDouble )
			unify = true;
	}

	return unify ;
}
VLANG_API bool fnIsInteger(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tk.ty == tokenType::tInt )
			unify = true;
	}

	return unify ;
}
VLANG_API bool fnIsChar(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tk.ty==tokenType::tChar)
			unify = true;
	}
	return unify ;
}
VLANG_API bool fnIsBool(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tk.ty == tokenType::tBool)
			unify = true;
	}

	return unify ;
}
VLANG_API bool fnIsString(vlContext* context,vlAstAxis* anode)
{
	bool unify = false;
	if(anode->LastChildNode()->kind==akTokenNode)
	{
		vlToken & tk = *anode->Node()->getToken();
		if(tokenType::tString)
			unify = true;
	}
	return unify ;
}
bool fnIsCall(vlContext* context,vlAstAxis* anode)
{
	if(anode && anode->Node()->kind == akProcCall) return true;
	return false ;
}
bool fnIsTerm(vlContext* context,vlAstAxis* anode)
{
	if(anode && (anode->Node()->kind==akProcItem||anode->Node()->kind==akProcVarItem)) return true;
	return false ;
}


VLANG_API unsigned char * fnListTop(vlContext* context,vlDynVector * tm,int index)
{
	int nLen = tm->getLength();
	assert(nLen - 1 - index >= 0);
	if(nLen - 1 - index < 0) return  0;
	return tm->getElement(nLen - 1 - index);
}
VLANG_API unsigned char * fnListItem(vlContext* context,vlDynVector * tm,int index)
{
	int nLen = tm->getLength();
	assert(index >=0 && nLen > index);
	return tm->getElement(index);
}
VLANG_API int fnListSize(vlContext* context,vlDynVector * tm)
{
	return tm->getLength();
}
VLANG_API void fnListSetSize(vlContext* context,vlDynVector * tm,int size)
{
	return tm->setLength(size);
}
VLANG_API void fnListSetTop(vlContext* context,vlDynVector * tm,unsigned char *  addr)
{
	int nLen = tm->getLength();
	if(nLen < 1) return ;
	int eleLen = tm->getEleSize();
	switch(eleLen)
	{
	case 1:
		*tm->getElement(nLen -1) = *addr;
		break;
	case 2:
		*(short *)tm->getElement(nLen -1) = *(short *)addr;
	case 4:
		*(int *)tm->getElement(nLen -1) = *(int *)addr;
	case 8:
		*(__int64 *)tm->getElement(nLen -1) = *(__int64 *)addr;
	default:
		memcpy(tm->getElement(nLen -1),addr,eleLen);
	}
}

VLANG_API void fnListPush(vlContext* context,vlDynVector * tm,unsigned char * addr)
{
	int eleLen = tm->getEleSize();
	unsigned char* pEle = tm->newElemnt();

	switch(eleLen)
	{
	case 1:
		*pEle = *addr;
		break;
	case 2:
		*(short *)pEle = *(short *)addr;
	case 4:
		*(int *)pEle = *(int *)addr;
	case 8:
		*(__int64 *)pEle = *(__int64 *)addr;
	default:
		memcpy(pEle,addr,eleLen);
	}
}

VLANG_API void fnListPop(vlContext* context,vlDynVector * tm)
{
	tm->pop();
}

VLANG_API vlAstAxis *	fnListTop(vlContext* context,vlAstAxis * anode,int index)
{
	int count = 0;
	vlAstAxis * node = anode->getChild();
	while(node)
	{
		count++;
		node = node->getSibling();
	}
	index = count - 1 - index;
	int i = 0;
	node = anode->getChild();
	while(node)
	{
		if(i==index) return node;
		node = node->getSibling();
		i++;
	}
	return 0;
}

VLANG_API vlAstAxis *	fnListItem(vlContext* context,vlAstAxis * anode,int index)
{
	if(index==0) return anode->getChild();
	vlAstAxis * node = anode->getChild();
	while(index>0 && node)
	{
		node = node->getSibling();
		index--;
	}
	return node;
}

VLANG_API int	 fnListSize(vlContext* context,vlAstAxis * anode)
{
	vlAstAxis * node = anode->getChild();
	int count = 0;
	while(node)
	{
		count++;
		node = node->getSibling();
	}
	return count;

}


VLANG_API int fnHash(vlContext* context,wchar_t* pStr)
{
	return util::hashString(pStr);
}

VLANG_API wchar_t* fnGetHashString(vlContext* context,int ide)
{
	return util::getHashString(ide);
}

	
VLANG_API void fnSavePoint(vlContext* context)
{
	context->getAstFactory()->checkpoint();
}

VLANG_API void fnRestorePoint(vlContext* context)
{
	context->getAstFactory()->restorepoint();
}

VLANG_API int fnAllocPages(vlContext* context)
{
	return (int)context->getAstFactory()->blocks_->size();
}

//node operate
vlAstAxis* fnMakeTypeDataNode(vlContext* context)
{
	return context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstTypeData());
}

vlAstAxis* fnMakeDataBufferNode(vlContext* context)
{
	return context->getAstFactory()->CreateAxis(context->getAstFactory()->CreateAstDataBuffer());
}

VLANG_API vlAstAxis* fnRemoveChildNode(vlContext* context,vlAstAxis* anode, vlAstAxis * node )
{
	anode->removeChildNode(node);
	return anode;
}

VLANG_API vlAstAxis* fnAppendSiblingNode(vlContext* context,vlAstAxis* anode, vlAstAxis * node)
{
	anode->appendSibling(node);
	return anode;
}

VLANG_API vlAstAxis* fnAppendNextNode(vlContext* context,vlAstAxis* anode, vlAstAxis * node )
{
	anode->appendNext(node);
	return anode;
}

VLANG_API vlAstAxis* fnAppendChildNode(vlContext* context,vlAstAxis* anode, vlAstAxis * node )
{
	anode->appendChild(node);
	return anode;
}

VLANG_API vlAstAxis* fnMakeAxis(vlContext* context,vlAstNode* node)
{
	return context->getAstFactory()->CreateAxis(node);
}

VLANG_API vlAstAxis* fnSibling(vlContext* context,vlAstAxis* anode)
{
	return anode->sibling;
}
VLANG_API vlAstAxis* fnNext(vlContext* context,vlAstAxis* anode )
{
	return anode->next;
}
VLANG_API vlAstAxis* fnChild(vlContext* context,vlAstAxis* anode )
{
	return anode->child;
}
vlAstNode* fnGetNode(vlContext* context,vlAstAxis* anode )
{
	return anode->node;
}
VLANG_API vlAstAxis* fnParent (vlContext* context,vlAstAxis* anode )
{
	return anode->parent;
}
vlAstAxis* fnLastChild(vlContext* context,vlAstAxis* anode)
{
	return anode->LastChild();
}
VLANG_API vlAstAxis* fnChildNext(vlContext* context,vlAstAxis* anode ,int level )
{
	return anode->ChildNext(level);
}
VLANG_API vlAstAxis* fnChild(vlContext* context,vlAstAxis* anode,int level)
{
	return anode->Child(level);
}
VLANG_API vlAstAxis* ChildSibling(vlContext* context,vlAstAxis* anode ,int level )
{
	return anode->Sibling(level);
}

//replace node value
VLANG_API int  fnNodeNameID(vlContext* context,vlAstAxis* anode)
{
	return anode->getIdent();
}

VLANG_API void fnSetValue(vlContext* context,vlAstAxis* anode, vlAstAxis* newnode)
{
	anode->node = newnode->node;
}

//Ô´´úÂëÎ»ÖÃ
VLANG_API wchar_t* fnTextUrl(vlContext* context)
{
	return 0;
}

VLANG_API int fnTextRow(vlContext* context,vlAstAxis* anode)
{
	if(anode->LastFirstChild()->getToken())
		return anode->LastFirstChild()->getToken()->row + 1;
	return -1;
}

VLANG_API int fnTextCol(vlContext* context,vlAstAxis* anode)
{
	if(anode->LastFirstChild()->getToken())
		return anode->LastFirstChild()->getToken()->col;
	return -1;
}

//register alloc not used
int fnCurRegister(vlContext* context)
{
	return context->trap->getCoder()->curRegister();
}
int fnAskRegister(vlContext* context)
{
	return context->trap->getCoder()->askRegister();
}
void fnFreeRegister(vlContext* context)
{
	return context->trap->getCoder()->freeRegister();
}
void fnClearRegister(vlContext* context)
{
	return context->trap->getCoder()->clearRegister();
}
int fnGetRegisterCount(vlContext* context)
{
	return context->trap->getCoder()->getRegisterCount();
}

//vlContext * lookup
VLANG_API void fnContextEnterScope(vlContext* context)
{
	context->getHashContext()->enter_scope();
}
VLANG_API void fnContextLeaveScope(vlContext* context)
{
	context->getHashContext()->leave_scope();
}
VLANG_API int  fnContextPushWord(vlContext* context,int id,int val)
{
	return context->getHashContext()->push_word(val,id);
}
VLANG_API int  fnContextGetWord(vlContext* context,int idx)
{
	return context->getHashContext()->get_word(idx);
}
VLANG_API int  fnContextFindToken(vlContext* context,int id)
{
	return context->getHashContext()->find_token(id);
}
VLANG_API int  fnContextFindNextToken(vlContext* context,int idx,int id)
{
	return context->getHashContext()->find_next_token(idx,id);
}

//debug
void fnDebugBreak(vlContext* context)
{
}

//code emit
VLANG_API void fnEmitInteger(vlContext* context,int val)
{
	context->trap->getCoder()->emit(val);
}
VLANG_API void fnEmitChar(vlContext* context,unsigned char val)
{
	context->trap->getCoder()->emit(val);
}
VLANG_API void fnEmitShort(vlContext* context,short val)
{
	context->trap->getCoder()->emit(val);
}
VLANG_API void fnEmitPtr(vlContext* context,int ptr)
{
	context->trap->getCoder()->emit(ptr);
}

//code emit util
VLANG_API void fnLabel(vlContext* context,int ident, bool baddr)
{
	context->trap->getCoder()->label(context->sureLabel(ident),baddr);
}

VLANG_API void fnLabelAs(vlContext* context,int hlabel,int  hlabelas )
{
	context->trap->getCoder()->labelas(hlabel,hlabelas);
}

VLANG_API void fnSureLabel(vlContext* context,int hlabel)
{
	context->sureLabel(hlabel);
}

VLANG_API void fnToLabel(vlContext* context,int hlabel,bool addr)
{
	context->trap->getCoder()->toLabel(hlabel,addr);
}

VLANG_API void* fnFunction(vlContext* context,int from, int alias)
{
	return os_t::load_module(util::getHashString(from),util::getHashString(alias));
}


VLANG_API void* fnFunction(vlContext* context,int ide)
{
	if(context->trap->mFunctions.find(ide)!= context->trap->mFunctions.end())
		return context->trap->mFunctions[ide];
	else
		return 0;
}

void fnCodeEval(vlContext* context)
{
}
//disasssemble
VLANG_API const wchar_t* fnDisassemble(vlContext* context,Handle hMethod)
{
	std::wstring str;
	if(context->trap->getCoder()->Disassemble((unsigned char *)context->trap->getMethodAddr(hMethod),str))
	{
		return str.c_str();
	}
	return L"";
}

//apply vlContext *
void fnEnterContextEx(vlContext* context,int id)
{
	if(context->contexts.size()) context->contexts.pop_back();
	if(id!=util::hashString(L"nill"))
		context->contexts.push_back(id);
}

void fnEnterContext(vlContext* context,int id)
{
	context->contexts.push_back(id);
}

void fnLeaveContext(vlContext* context)
{
	context->contexts.pop_back();
}

void* fnEpSureProcessApply(vlContext* context,Handle htype)
{
	return context->trap->sureProcessApplyEntryPoint(htype);
}
void* fnEpSureContextApply(vlContext* context,Handle hContext)
{
	return context->trap->sureContextApplyEntryPoint(hContext);
}
void * fnEpAddMatchCount(vlContext* context,void*  enode , int count)
{
	return context->trap->addMatchCount(enode,count);
}
void * fnEpAddMatchApplyName(vlContext* context,void * enode, int id)
{
	return context->trap->addMatchApplyName(enode,id);
}
void * fnEpAddMatchLiteral(vlContext* context,void * enode, int pos, vlAstAxis* lt)
{
	return context->trap->addMatchLiteral(enode,pos,lt);
}
void * fnEpAddMatchName(vlContext* context,void * enode, int pos, int id)
{
	return context->trap->addMatchName(enode,pos,id);
}
VLANG_API void * fnEpAddMatchAny(vlContext* context,void * enode, int pos)
{
	return context->trap->addMatchAny(enode,pos);
}

VLANG_API void  fnTrace(vlContext* context,vlAstNode* node)
{
	std::wstring str = node->printOwner();
	if(context->wppcontext->get_error())
		context->wppcontext->get_error()->output(0,str.c_str());
	else
		os_t::print(L"%s",str.c_str());
}

VLANG_API void  fnPrint(vlContext* context,wchar_t * str)
{
	if(context->wppcontext->get_error())
		context->wppcontext->get_error()->output(0,str);
	else
		os_t::print(L"%s",str);
}

