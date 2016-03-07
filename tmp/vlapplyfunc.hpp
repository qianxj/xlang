#ifndef WPP5_VLAPPLYFUNC_H
#define WPP5_VLAPPLYFUNC_H

#include "vlang5.h"
#include "vlAstAxis.hpp"
#include "vlwpp_context.hpp"
#include "vlhashcontext.hpp"
#include <map>
#include <vector>
#include <set>
#include <deque>
#include "vltrap.hpp"

namespace xlang
{
	VLANG_API vlContext * fnSymGetContext(vlContext* context);
	VLANG_API wchar_t* fnSymGetNamespaceName(vlContext* context,Handle hNamespace);
	
	//ole process
	VLANG_API int fnSymGetOleMethod(vlContext* context,Handle hMethod);
	VLANG_API int fnSymGetOlePropGet(vlContext* context,Handle hField);
	VLANG_API int fnSymGetOlePropPut(vlContext* context,Handle hField);
	VLANG_API bool fnSymIsOleItem(vlContext* context,Handle  hTerm);
	VLANG_API wchar_t* fnSymGetTypeOleName(vlContext* context,Handle  hType);
	VLANG_API void * fnSymGetOleTypeInfo(vlContext* context,Handle  hType);
	
	//symtable
	VLANG_API void * fnSymGetSymTable(vlContext* context);
	//code prt
	VLANG_API unsigned char * fnSymGetCodePc(vlContext* context);
	
	//add symbol term
	VLANG_API Handle fnSymAddNamespace(vlContext* context,Handle hAxis, int id);
	VLANG_API Handle fnSymAddType(vlContext* context,Handle hAxis,int kind, int id,bool todef);
	VLANG_API Handle fnSymAddField(vlContext* context,Handle hAxis,int  id,int spec, Handle typeref);
	VLANG_API Handle fnSymAddMethod(vlContext* context,Handle hAxis, int id, int spec, Handle hType);
	VLANG_API Handle fnSymAddParam(vlContext* context,Handle hMethod,int id, int spec,Handle hType);
	VLANG_API Handle fnSymAddVariant(vlContext* context,Handle hMethod,int id,int spec,Handle  hType);
	
	//comp type
	VLANG_API Handle fnSymMakeTypeRef(vlContext* context);
	VLANG_API void fnSymAddCompTypeItem(vlContext* context,Handle hType);
	VLANG_API void fnSymAddCompTypeRank(vlContext* context,int rank);
	VLANG_API void fnSymInsertCompTypeItem(vlContext* context,Handle hCompType,Handle hType);
	VLANG_API void fnSymInsertCompTypeRank(vlContext* context,Handle hCompType,int rank);
	VLANG_API Handle fnSymGetCompTypeItem(vlContext* context,Handle hCompType);
	VLANG_API int fnSymGetCompTypeRank(vlContext* context,Handle hCompType);
	VLANG_API Handle fnSymStepCompType(vlContext* context,Handle hCompType);
	
	//method
	VLANG_API Handle fnSymGetMethodHandle(vlContext* context,Handle hType, int id);
	VLANG_API Handle fnSymGetMethodVariantHandle(vlContext* context,Handle hMethod,int id);
	//method paramm
	VLANG_API Handle fnSymGetMethodParamHandle(vlContext* context,Handle hMethod,int id);
	VLANG_API Handle fnSymGetMethodParamHandleByIndex(vlContext* context,Handle hMethod,int idx);
	VLANG_API int fnSymGetParamSpec(vlContext* context,Handle hParam);
	VLANG_API int fnSymGetParamInitValue(vlContext* context,Handle hParam);
	VLANG_API int fnSymGetMethodParamCount(vlContext* context,Handle hMethod);
	//method pcode
	VLANG_API void fnSymSetMethodPCode(vlContext* context,Handle hMethod,vlAstAxis*);
	VLANG_API vlAstAxis* fnSymGetMethodPCode(vlContext* context,Handle hMethod);
	
	VLANG_API Handle fnSymGetTypeHandle(vlContext* context,int id);
	VLANG_API Handle fnSymGetGlobalNamespace(vlContext* context);
	VLANG_API void fnSymSetTypeKind(vlContext* context,Handle hType,int kind);
	VLANG_API int fnSymGetTypeKind(vlContext* context,Handle hType);
	VLANG_API Handle fnSymGetFieldHandle(vlContext* context, Handle hType,int id);
	VLANG_API Handle fnSymGetTypeMethodHandle(vlContext* context,Handle hType,int id);
	VLANG_API int fnSymGetTypeAllocSize(vlContext* context,Handle hType);
	VLANG_API int fnSymGetFieldAllocSize(vlContext* context,Handle hField);
	VLANG_API int fnSymGetOffsetOfField(vlContext* context,Handle hType,Handle hField);
	VLANG_API int fnSymGetMethodVariantSize(vlContext* context,Handle hMethod);

	//term
	VLANG_API void fnSymSetNSOleTerm(vlContext* context,Handle hNamespace, Handle hNsOleTerm);
	VLANG_API Handle fnSymAddNSOleTerm(vlContext* context,int name, void * ptLib, void * typeInfo );
	VLANG_API int fnSymGetOffsetOfTerm(vlContext* context,Handle hMethod, Handle hTerm);
	VLANG_API void fnSymAddBase(vlContext* context,Handle hType,Handle hTypeBase);
	VLANG_API int fnSymGetTermNameID(vlContext* context,Handle hTerm);
	VLANG_API const wchar_t* fnSymGetTermName(vlContext* context,Handle hTerm);
	VLANG_API Handle fnSymGetTermType(vlContext* context,Handle hTerm);
	VLANG_API int fnSymGetTermSpec(vlContext* context,Handle hTerm);
	VLANG_API int fnSymGetTermInitValue(vlContext* context,Handle hTerm);
	VLANG_API void fnSymSetTermInitValue(vlContext* context,Handle hTerm, int val);
	VLANG_API int fnSymGetTypeTermFrameSize(vlContext* context,Handle hTerm);

	VLANG_API Handle fnSymLookupTypeItem(vlContext* context,Handle axis,int id);
	VLANG_API Handle fnSymLookupType(vlContext* context,Handle axis,int id);
	VLANG_API Handle fnSymLookupMethod(vlContext* context,Handle axis,int id);
	VLANG_API Handle fnSymLookupItem(vlContext* context,Handle axis,int id);
	
	VLANG_API Handle fnSymGetParent(vlContext* context,Handle hTerm);
	VLANG_API bool fnSymIsSuperClass(vlContext* context,Handle hSuper, Handle hType);
	
	//real const ptr
	VLANG_API double * fnSymAddReal(vlContext* context,double val);
	VLANG_API double * fnSymGetRealPtr(vlContext* context,double val);
	
	//dump symbol term
	VLANG_API const wchar_t* fnSymDump(vlContext* context);
	VLANG_API const wchar_t* fnSymDumpType(vlContext* context,Handle hType);
	VLANG_API const wchar_t* fnSymDumpMethod(vlContext* context,Handle hMethod);
	VLANG_API const wchar_t* fnSymDumpField(vlContext* context,Handle hTerm);
	VLANG_API const wchar_t* fnSymDumpParam(vlContext* context,Handle hMethod);
	VLANG_API const wchar_t* fnSymDumpVariant(vlContext* context,Handle hMethod);
	VLANG_API const wchar_t* fnSymDumpTypeParam(vlContext* context,Handle hTerm);
		
	VLANG_API int fnSymGetMethodSpec(vlContext* context,Handle hMethod);
	VLANG_API unsigned char * fnSymGetMethodAddr(vlContext* context,Handle hMethod);
	VLANG_API void fnSymSetMethodAddr(vlContext* context,Handle hMethod,void * addr);
	
	//native sppec
	VLANG_API void fnSymSetMethodNativeFrom(vlContext* context,Handle hMethod,wchar_t* from);
	VLANG_API void fnSymSetMethodNativeAlias(vlContext* context,Handle hMethod,wchar_t* alias);
	VLANG_API wchar_t* fnSymGetMethodNativeFrom(vlContext* context,Handle hMethod);
	VLANG_API wchar_t* fnSymGetMethodNativeAlias(vlContext* context,Handle hMethod);
	VLANG_API bool fnSymIsMethodNative(vlContext* context,Handle hMethod);
	VLANG_API void fnSymSetMethodNative(vlContext* context,Handle hMethod,int nativeFrom,int nativeAlias);
	VLANG_API void fnSymSetTypeNativeFrom(vlContext* context,Handle hType,wchar_t* from);
	VLANG_API void fnSymSetTypeNativeAlias(vlContext* context,Handle hType,wchar_t* alias);
	VLANG_API wchar_t* fnSymGetTypeNativeFrom(vlContext* context,Handle hType);
	VLANG_API wchar_t* fnSymGetTypeNativeAlias(vlContext* context,Handle hType);
	
	VLANG_API Handle fnSymFindOleChild(vlContext* context,Handle hAxis,int id);
	VLANG_API Handle fnSymFindMatchedMethod(vlContext* context,Handle scope, Handle nest, int ident, int nargs,std::vector<HType> & args, int argstart); 
	
	//logic operate
	VLANG_API bool fnGt(vlContext* context,int lhs,int rhs);
	VLANG_API bool fnGte(vlContext* context,int lhs,int rhs);
	VLANG_API bool fnLt(vlContext* context,int lhs,int rhs);
	VLANG_API bool fnLte(vlContext* context,int lhs,int rhs);
	VLANG_API bool fnEq(vlContext* context,int lhs,int rhs);
	VLANG_API bool fnNeq(vlContext* context,int lhs,int rhs);
	VLANG_API bool fnNil(vlContext* context,int lhs);
	VLANG_API bool fnNot(vlContext* context,int lhs);
	
	VLANG_API bool fnGt(vlContext* context,wchar_t* lhs,wchar_t* rhs);
	VLANG_API bool fnGte(vlContext* context,wchar_t* lhs,wchar_t* rhs);
	VLANG_API bool fnLt(vlContext* context,wchar_t* lhs,wchar_t* rhs);
	VLANG_API bool fnLte(vlContext* context,wchar_t* lhs,wchar_t* rhs);
	VLANG_API bool fnEq(vlContext* context,wchar_t* lhs,wchar_t* rhs);
	VLANG_API bool fnNeq(vlContext* context,wchar_t* lhs,wchar_t* rhs);
	VLANG_API bool fnNil(vlContext* context,wchar_t*);
	VLANG_API bool fnNot(vlContext* context,wchar_t*);
	
	VLANG_API bool fnAnd(vlContext* context,bool lhs,bool rhs);
	VLANG_API bool fnOr(vlContext* context,bool lhs,bool rhs);
	
	VLANG_API wchar_t* fntoString(vlContext* context,int);
	VLANG_API wchar_t* fnAdd(vlContext* context,wchar_t* lhs,wchar_t* rhs);
	VLANG_API int fnAdd(vlContext* context,int lhs,int rhs);
	VLANG_API int fnSub(vlContext* context,int lhs,int rhs);
	VLANG_API int fnMul(vlContext* context,int lhs,int rhs);
	VLANG_API int fnDiv(vlContext* context,int lhs,int rhs);
	VLANG_API int fnMod(vlContext* context,int lhs,int rhs);
	VLANG_API int fnShr(vlContext* context,int lhs,int rhs);
	VLANG_API int fnShl(vlContext* context,int lhs,int rhs);
	VLANG_API int fnBand(vlContext* context,int lhs,int rhs);
	VLANG_API int fnBor(vlContext* context,int lhs,int rhs);
	VLANG_API int fnBnot(vlContext* context,int lhs);
	VLANG_API int fnXor(vlContext* context,int lhs,int rhs);
	VLANG_API int fnInc(vlContext* context,int lhs);
	VLANG_API int fnDec(vlContext* context,int lhs);
	VLANG_API int fnNeg(vlContext* context,int lhs);
	
	VLANG_API bool fnIsIdentifier(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsLiteral(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsNumber(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsDouble(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsInteger(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsChar(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsBool(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsString(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsCall(vlContext* context,vlAstAxis*);
	VLANG_API bool fnIsTerm(vlContext* context,vlAstAxis*);

	VLANG_API unsigned char *	fnListTop(vlContext* context,vlDynVector * tm,int index);
	VLANG_API unsigned char *	fnListItem(vlContext* context,vlDynVector * tm,int index);
	VLANG_API int	 fnListSize(vlContext* context,vlDynVector * tm);
	VLANG_API void fnListSetSize(vlContext* context,vlDynVector * tm,int size);
	VLANG_API void fnListSetTop(vlContext* context,vlDynVector * tm,unsigned char * addr);
	VLANG_API void fnListPush(vlContext* context,vlDynVector * tm,unsigned char * addr);
	VLANG_API void fnListPop(vlContext* context,vlDynVector * tm);

	VLANG_API vlAstAxis *	fnListTop(vlContext* context,vlAstAxis * ,int index);
	VLANG_API vlAstAxis *	fnListItem(vlContext* context,vlAstAxis *,int index);
	VLANG_API int	 fnListSize(vlContext* context,vlDynVector * tm);

	VLANG_API int fnHash(vlContext* context,wchar_t*);
	VLANG_API wchar_t* fnGetHashString(vlContext* context,int);
		
	VLANG_API void fnSavePoint(vlContext* context);
	VLANG_API void fnRestorePoint(vlContext* context);
	VLANG_API int fnAllocPages(vlContext* context);

	//node operate
	VLANG_API vlAstAxis* fnMakeTypeDataNode(vlContext* context);
	VLANG_API vlAstAxis* fnMakeDataBufferNode(vlContext* context);
	VLANG_API vlAstAxis* fnRemoveChildNode(vlContext* context,vlAstAxis* anode, vlAstAxis * cnode);
	VLANG_API vlAstAxis* fnAppendSiblingNode(vlContext* context,vlAstAxis* anode, vlAstAxis * snode);
	VLANG_API vlAstAxis* fnAppendNextNode(vlContext* context,vlAstAxis*, vlAstAxis * nnode );
	VLANG_API vlAstAxis* fnAppendChildNode(vlContext* context,vlAstAxis* anode, vlAstAxis * cnode);
	VLANG_API vlAstAxis* fnMakeAxis(vlContext* context,vlAstNode* );
	VLANG_API vlAstAxis* fnSibling(vlContext* context,vlAstAxis* );
	VLANG_API vlAstAxis* fnNext(vlContext* context,vlAstAxis* );
	VLANG_API vlAstAxis* fnChild(vlContext* context,vlAstAxis* );
	VLANG_API vlAstNode*  fnGetNode(vlContext* context,vlAstAxis* );
	VLANG_API vlAstAxis* fnParent (vlContext* context,vlAstAxis* );
	VLANG_API vlAstAxis* fnLastChild(vlContext* context,vlAstAxis*);
	VLANG_API vlAstAxis* fnChildNext(vlContext* context,vlAstAxis* ,int level =1);
	VLANG_API vlAstAxis* fnChild(vlContext* context,vlAstAxis* ,int level =1);
	VLANG_API vlAstAxis* ChildSibling(vlContext* context,vlAstAxis* ,int level =1);
	
	//replace node value
	VLANG_API int  fnNodeNameID(vlContext* context,vlAstAxis*);
	VLANG_API void fnSetValue(vlContext* context,vlAstAxis* anode , vlAstAxis* newnode);

	//Ô´´úÂëÎ»ÖÃ
	VLANG_API wchar_t* fnTextUrl(vlContext* context);
	VLANG_API wchar_t* fnTextRow(vlContext* context,vlAstAxis* anode);
	VLANG_API wchar_t* fnTextCol(vlContext* context,vlAstAxis* anode);

	//register alloc not used
	VLANG_API int fnCurRegister(vlContext* context);
	VLANG_API int fnAskRegister(vlContext* context);
	VLANG_API void fnFreeRegister(vlContext* context);
	VLANG_API void fnClearRegister(vlContext* context);
	VLANG_API int fnGetRegisterCount(vlContext* context);

	//vlContext * lookup
	VLANG_API void fnContextEnterScope(vlContext* context);
	VLANG_API void fnContextLeaveScope(vlContext* context);
	VLANG_API int  fnContextPushWord(vlContext* context,int id,int val);
	VLANG_API int  fnContextGetWord(vlContext* context,int idx);
	VLANG_API int  fnContextFindToken(vlContext* context,int id);
	VLANG_API int  fnContextFindNextToken(vlContext* context,int idx,int id);
	
	//debug
	VLANG_API void fnDebugBreak(vlContext* context);
	
	//code emit
	VLANG_API void fnEmitInteger(vlContext* context,int val);
	VLANG_API void fnEmitChar(vlContext* context,byte val);
	VLANG_API void fnEmitShort(vlContext* context,short val);
	VLANG_API void fnEmitPtr(vlContext* context,int ptr);
	//code emit util
	VLANG_API void fnLabel(vlContext* context,int ident, bool baddr = false);
	VLANG_API void fnLabelas(vlContext* context,int hlabel,int  hlabelas);
	VLANG_API void fnSureLabel(vlContext* context,int hlabel);
	VLANG_API void fnToLabel(vlContext* context,int hlabel,bool addr = false);
	VLANG_API void* fnFunction(vlContext* context,int from, int alias);
	VLANG_API void* fnFunction(vlContext* context,int ident);
	VLANG_API void fnCodeEval(vlContext* context);
	//disasssemble
	VLANG_API const wchar_t* fnDisassemble(vlContext* context,Handle hMethod);
	
	//apply vlContext *
	VLANG_API void fnEnterContextEx(vlContext* context,int id);
	VLANG_API void fnEnterContext(vlContext* context,int id);
	VLANG_API void fnLeaveContext(vlContext* context);
	
	VLANG_API void * fnEpSureProcessApply(vlContext* context,Handle htype);
	VLANG_API void * fnEpSureContextApply(vlContext* context,Handle hContext);
	VLANG_API void * fnEpAddMatchCount(vlContext* context,void * , int count);
	VLANG_API void * fnEpAddMatchApplyName(vlContext* context,void *, int id);
	VLANG_API void * fnEpAddMatchLiteral(vlContext* context,void *, int pos, vlAstAxis* lt); 
	VLANG_API void * fnEpAddMatchName(vlContext* context,void *, int pos, int id);
	VLANG_API void * fnEpAddMatchAny(vlContext* context,void *, int pos);
	
	VLANG_API void  fnTrace(vlContext* context,vlAstNode* node);
	VLANG_API void  fnPrint(vlContext* context,wchar_t * str);
}

#endif //WPP5_VLAPPLYFUNC_H