#ifndef WPP5_VLTRAP_H
#define WPP5_VLTRAP_H

#include "vlsym.hpp"
#include "vlcoder.hpp"
#include <vector>
#include <stack>
#include <map>
#include <string>

namespace xlang{

class vlTrap
{
public:
	vlTrap();
	~vlTrap();
private:
	void initial();
private:
	void encodeIndex(std::vector<unsigned char> & dt,unsigned int index);
	unsigned int decodeIndex(std::vector<unsigned char> &dt, int & cindex);
public:
	int getFieldAllocSize(Handle & hField);
	int getTypeTermAllocSize(SymTypeTerm * term);
	int getAllocSize(Handle & hType,int & msize, int align = 4);
	int getAllocSize(int typeName);
	int getTypeTermFrameSize(Handle term);
public:
	Handle getTypeHandle(int name);
	Handle getNamespaceHandle(int name);
	Handle getGlobalNamespace();
	Handle getFieldHandle(int name);
	Handle getFieldHandle(Handle hType,int name);

	Handle getMethodHandle(Handle hType,int name);
	Handle getMethodVariantHandle(Handle hMethod,int name);
	Handle getMethodParamHandle(Handle hMethod,int name);
	Handle getMethodParamHandleByIndex(Handle hMethod,int index);

public:
	int	getFieldOffset(Handle  hType, Handle hField, int align = 4);
	int getFieldOffset(Handle  hType, Handle hField, int & msize,bool & bfound, int align = 4);
public:
	int getFrameOffset(Handle hMethod, Handle hTerm,int align=4);
	int getFrameOffset(Handle hMethod, Handle hTerm,bool & bfound, int align = 4);

	int getMethodParamSize(Handle hMethod,int align=4);
	int getMethodVariantSize(Handle hMethod,int align=4);

public:
	Handle	addItem(Handle hAxis,SymbolKind sk,int name);
	Handle	addType(Handle hAxis,TypeKind tkind, int name,bool todef = true);
	Handle	addNamespace(Handle hAxis,int name);
	Handle	addField(Handle hAxis,int name);
	Handle	addField(Handle hAxis,int name, unsigned int spec ,Handle hType);
	Handle	addMethod(Handle hAxis,int name, unsigned int spec ,Handle hType);
	Handle	addVariant(Handle hMethod,int name, unsigned int spec ,Handle hType);
	Handle	addParam(Handle hMethod,int name, unsigned int spec ,Handle hType);
	Handle	addApply(Handle hContext, Handle hAxis,int name, unsigned int spec ,Handle hType);
	Handle	addContext(Handle hAxis,int name);
	Handle	addProcessor(Handle hAxis, int name,bool todef = true);
	//class native
	void	setTypeNativeFrom(Handle hType, int nativeFrom);
	void	setTypeNativeAlias(Handle hType,int nativeAlias);
	wchar_t* getTypeNativeFrom(Handle hType);
	wchar_t* getTypeNativeAlias(Handle hType);

	//support ole
	bool	isOleItem(Handle  hItem);
	Handle	addOleTerm(void* typeInfo);
	Handle  addNSOleTerm(int name,void * ptLib, void* typeInfo);
	void	setNSOleTerm(Handle hNamespace, Handle hNsOleTerm);

	void	addTypeBase(Handle hType, Handle hBase);
	void	addCompTypeItem(Handle hType);
	void	addCompTypeRank(int rank);
	void	insertCompTypeItem(Handle hCompType,Handle hType);
	void	insertCompTypeRank(Handle hCompType,int rank);
	HType	getCompTypeItem(Handle hCompType);
	int		getCompTypeRank(Handle hCompType);
	HType	stepCompType(Handle hCompType);

	void*	getMethodAddr(Handle hMethod);
	void	setMethodAddr(Handle hMethod,void * addr);
	void	setMethodNativeFrom(Handle hMethod, int nativeFrom);
	void	setMethodNativeAlias(Handle hMethod,int nativeAlias);
	wchar_t* getMethodNativeFrom(Handle hMethod);
	wchar_t* getMethodNativeAlias(Handle hMethod);
	bool	isMethodNative(Handle hMethod);
	void	setMethodNative(Handle hMethod,int nativeFrom = 0,int nativeAlias = 0);
	unsigned int getMethodSpec(Handle hMethod);
	unsigned int getMethodKind(Handle hMethod);

public:
	//apply entry point
	void * getProcessApplyEntryPoint(HType htype);
	void * getContextApplyEntryPoint(HContext hContext);
	void * sureProcessApplyEntryPoint(HType htype);
	void * sureContextApplyEntryPoint(HContext hContext);

	void * addMatchCount(void * entryPoint,int count);
	void * addMatchLiteral(void * entryPoint, int pos, class vlAstAxis * lt);
	void * addMatchName(void * entryPoint, int pos,int id);
	void * addMatchApplyName(void * entryPoint,int id);
	void * addMatchAny(void * entryPoint, int pos);
	void * addApplyParam(void * entryPoint, int pos,int id);
public:
	//typedef
	void	setTyperef(Handle hType, Handle hTypeRef);
	void	setTypeSize(Handle hType, int size);
	void	setTypeKind(Handle hType, TypeKind kind);
	void	setTypeCatKind(Handle hType, TypeCatKind catKind);
public:
	TypeKind getTypeKind(Handle hType);
public:
	void* makeType(int typeName);
	void  freeType(void *p);

public:
	SymContext * getContext(Handle  hContext);
	SymType * getType(Handle  hType);
	SymField * getField(Handle  hField);
	
	SymMethod *		getMethod(Handle  hMethod);
	SymParam *		getParam(Handle  hParam);
	SymVariant *	getVariant(Handle  hVariant);
	SymTypeParam *	getTypeParam(Handle  hTypeParam);
	SymNamespace *  getNamespace(Handle hNamespace);
	SymOleTerm   *  getOleTerm(Handle hOleTerm);
	SymNSOleTerm *	getNSOleTerm(Handle hNSOleTerm);
	SymOleTerm   *  getTypeOleTerm(Handle hType);
	SymNSOleTerm *  getNamespaceOleTerm(Handle hNamespace);
	SymNSOleTerm *  getTypeNSOleTerm(Handle hType);

public: 
	int	getTermNameID(Handle hterm);
	const wchar_t * getTermName(Handle hterm);
	HType getTermType(Handle hterm);
	int getTermSpec(Handle hterm);
	int getTermInitValue(Handle hterm);
	void setTermInitValue(Handle hterm,int val);
public:
	static int Ident(wchar_t*); 
public:
	std::wstring dump();
	std::wstring dumpType(HType hType);
	std::wstring dumpCompType(HType hCompType);
	std::wstring dumpMethod(HMethod hMethod);
	std::wstring dumpField(HType hType);
	std::wstring dumpParam(HMethod hMethod);
	std::wstring dumpVariant(HMethod hMethod);
	std::wstring dumpTypeParam(Handle hType);
public:
	bool isSuperClass(HType super, HType child);
public:
	Handle findMatchedMethod( Handle scope, Handle axis, int ident, int nargs,const std::vector<HType> &typs,int argstart = 0);
	bool isMethodArgMatched(Handle fn, int nargs,const std::vector<HType> &typs,int argstart = 0);
public:
	HMethod findMethod(Handle hAxis, int methodID);
	Handle findOleChild(Handle hAxis,int id); 
public:
	HMethod findMatchAsMethod(Handle wc, Handle wm);
public:
	Handle getParent(Handle hItem);
public:	
	Handle lookupIn(Handle axis,int id);
	Handle lookup(Handle axis,int id);
	Handle lookup(Handle axis,int id,int kind);
	Handle lookupItem(Handle axis,int id);
	Handle lookupMethod(Handle axis,int id);
	Handle lookupType(Handle axis,int id);
public:
	//is a primary type
	bool isIntType(HType hType);
	bool isUIntType(HType hType);
	bool isShortType(HType hType);
	bool isUShortType(HType hType);
	bool isFloatType(HType hType);
	bool isDoubleType(HType hType);
	bool isNumberType(HType hType);
	bool isCharType(HType hType);
	bool isBoolType(HType hType);
	bool isStringType(HType hType);
	bool isNillType(HType hType);
	bool isUInt8Type(HType hType);
	bool isInt8Type(HType hType);
	bool isIntNumberType(HType hType); //(unsign) int short char 
	bool isFloatNumberType(HType hType); //float double
	//get primary type
	HType GetTypeInt();
	HType GetTypeUInt();
	HType GetTypeShort();
	HType GetTypeUShort();
	HType GetTypeUnknown();
	HType GetTypeDouble();
	HType GetTypeFloat();
	HType GetTypeBool();
	HType GetTypeChar();
	HType GetTypeString();
	HType GetTypeVoid();
	HType GetTypeClass();
	HType GetTypeStruct();
	HType GetTypeEnum();
	HType GetTypeArray();
	HType GetTypeNill();
	HType GetTypeInt8();
	HType GetTypeUInt8();

	HType GetItemType(Handle hItem);
public:
	inline vlCoder * getCoder() {return coder;}
public:
	double * addReal(double d);
	double * getRealPtr(double d);
private:
	vlCoder * coder;
public:
	SymTable  symTable;
public:
	std::map<int /*id*/,void *> mFunctions;
private:
	std::multimap<int /*name*/,HType> mTypes;
	std::multimap<int /*name*/,std::pair<HField ,HType>> mFields;
	std::multimap<int /*name*/,HNamespace> mNamespaces;
};

}

#endif //WPP5_VLTRAP_H