#ifndef WPP5_VLSYMDEF_H
#define WPP5_VLSYMDEF_H

#include <vector>
#include <deque>
#include <map>
#include "vldatapool.hpp"
#include "vlentrynode.hpp"

namespace xlang{


enum SymbolKind : unsigned char
{
	skError			= 0,
	skNil			= 0,
	skType			= 1,
	skField			= 2,
	skNamespace		= 3,
	skMethod		= 4,
	skVariant		= 5,
	skParam			= 6,
	skTypeParam		= 7,
	skCompType		= 8,
	skOleTerm		= 9,
	skNSOleTerm		= 10,
	skOleField		= 11,
	skOleMethod		= 12,
	skNType			= 13, //native type
	skContext		= 14,
	skItem			= 15 //comp field param variant
};

enum TypeKind : unsigned char
{
	tkUnknown = 0,
	tkArrayType = 1,
	tkClass = 2,
	tkEnum = 3,
	tkError = 4,
	tkInterface = 5,
	tkPointerType = 6,
	tkStruct = 7,
	tkTypeParam = 8,
	tkPair		= 9,
	tkProcessor	= 10
};

enum TypeCatKind : unsigned char
{
	 tcUndef = 0,
     tcPrimitive = 1,
     tcEnum = 2,
     tcType = 3,
     tcArray = 4
};

enum MethodKind : unsigned  char
{
	mkGeneralMethod	= 0,
    mkConstructor	= 1,
    mkDestructor	= 2,
	mkConversion	= 3,
	mkOperator		= 4,
    mkLambdaMethod	= 5,
	mkApply			= 6
};

enum AccessKind : unsigned char
{
	akPrivate	= 0,
	akProtected = 1,
	akPublic	= 2
};

struct StorageSpec
{
	static const unsigned int extern_		= 0x1;
	static const unsigned int static_		= 0x2;
	static const unsigned int mutable_		= 0x4;
	static const unsigned int readonly_		= 0x8;
	static const unsigned int thread_local_	= 0x10;	
	static const unsigned int volatile_		= 0x20;
};

typedef unsigned int ParamSpec_t;
struct ParamSpec
{
	static const unsigned int readonly_	= StorageSpec::readonly_;//0x8;
	static const unsigned int native	= 0x10;
	static const unsigned int safe_		= 0x20;
	static const unsigned int const_	= 0x40;
	static const unsigned int in_		= 0x80;
	static const unsigned int out_		= 0x100;
	static const unsigned int byref_	= 0x200;
	static const unsigned int byval_	= 0x400;
};

typedef ParamSpec	FieldSpec;
typedef ParamSpec	VariantSpec;

typedef unsigned int MethodSpec_t;
struct MethodSpec
{
	static const unsigned int extern_	= StorageSpec::extern_; //0x1
	static const unsigned int static_	= StorageSpec::static_;	//0x2

	static const unsigned int native_	= ParamSpec::native;	//0x10;
	static const unsigned int safe_		= ParamSpec::safe_;		//0x20;
	static const unsigned int const_	= ParamSpec::const_;	//0x40;

	static const unsigned int inline_	= 0x100;
	static const unsigned int explicit_	= 0x400;
	static const unsigned int override_	= 0x800;
	static const unsigned int stdcall_	= 0x1000;
	static const unsigned int friend_	= 0x2000;
	static const unsigned int virtual_  = 0x4000;
};


struct Handle
{
	unsigned int kind  : 4; //ThingKind
	unsigned int index : 28;

	Handle(SymbolKind kind,unsigned int index):kind(kind),index(index){}
	Handle():kind(skNil),index(0){}
	Handle(int h) { *(unsigned int *)this = h; }
	operator int()  { return *(int *)this;}
	operator unsigned int () { return *(unsigned int *)this;}
	operator bool() { return (unsigned int)*this ? true :false;}
	bool operator ==(Handle & rhs) {return (unsigned int)*this == (unsigned int)rhs;}
	bool operator !=(Handle & rhs) {return (unsigned int)*this != (unsigned int)rhs;}
	bool operator <(const Handle & rhs) const {
		return (this->kind < rhs.kind ||  (this->kind == rhs.kind  && this->index < rhs.index));
	}
};

typedef Handle HProcessor;
typedef Handle HContext;
typedef Handle HType;
typedef Handle HField;
typedef Handle HFunction;
typedef Handle HParam;
typedef Handle HVariant;
typedef Handle HTypeParam;
typedef Handle HNamespace;
typedef Handle HMethod;
typedef Handle HOleTerm;
typedef Handle HNsOleTerm;

struct SymContext
{
	int name;
	HContext parent;
	//entrypoint
	void*	entryPointApplys;
};

struct SymNamespace 
{
	int name;
	HNamespace parent;
	HNsOleTerm	hNsOleTerm;
};

struct SymType  
{
	int name;
	HNamespace nspace;

	TypeKind kind;
	TypeCatKind catKind;

	unsigned int flag;
	//value = real value + 4 when 0 need recal
	int		size;
	HField	hField; 
	int		fieldCount; 

	HMethod	hMethod;
	int		methodCount;


	//typeparam
	HTypeParam	hTypeField;
	int			typeFieldCount;
	//base
	HType		baseType; 
	//typedef
	HType		hTyperef;
	//native
	int			nativeFrom;
	int			nativeAlias;
	//dirty
	bool		dirty;
	//bol
	bool		undef;
	//ole
	HOleTerm	hOleTerm;

	//entrypoint
	void*		entryPointApplys;
};

struct SymNSOleTerm
{
	void*		ptLib;
	int			oleName;
	void*		typeInfo;
};

struct SymOleTerm
{
	void* typeInfo;
	HField hLastField;
	HMethod hLastMethod;
};

struct SymTypeTerm
{
	ParamSpec_t spec;
	int			name;
	HType		hType; 
	//value = real value + 4 when 0 need recal
	int			offset;	
	//value = real value + 4 when 0 need recal
	int			 size;

	bool hasInitValue;
	int  initValue;
};

struct SymField : public SymTypeTerm
{
	AccessKind accessKind;
};

struct SymParam :  public SymTypeTerm
{
	bool ellipsis;
	void * terms; //child terms
	void * value; //match value
	bool any;	 //match any
};

struct SymVariant :  public SymTypeTerm
{
};

struct SymTypeParam :  public SymTypeTerm
{
};

/*
struct SymArray
{
	HType eleType; //pointer typeitem
	int rank;
};
*/

struct SymMethod
{
	int				name;
	MethodKind		kind;
	MethodSpec_t	spec;

	HContext		hContext;
	HType			hType;
	HParam			hParam;
	int				paramCount;
	HTypeParam		hTypeParam;
	int				typeParamCount;
	HVariant		hVariant;
	int				variantCount;

	int				nativeFrom;
	int				nativeAlias;
	
	void*			precode;
	void*			pcode;
	void*			vcode;

	void*			addr;
	//dirty
	bool			dirty;
};

//ole support
struct SymOleField : public SymField
{
	int propGet;
	int propPut;
	HField  piror;
	HField  next;
};

struct SymOleMethod : public SymMethod
{
	int nMethod;	
	HMethod piror;
	HMethod next;
};

struct SymTable
{
	std::vector<SymNamespace>			namespaces;
	std::vector<SymType>				types;
	std::vector<SymField>				fields;
	std::vector<SymTypeParam>			typeparams;
	//std::vector<SymArray>				arrays;
	std::vector<SymMethod>				methods;
	std::vector<SymParam>				params;
	std::vector<SymVariant>				variants;
	std::vector<SymNSOleTerm>			nsoleterms;
	std::vector<SymOleTerm>				oleterms;
	std::vector<SymOleField>			olefields;
	std::vector<SymOleMethod>			olemethods;
	std::vector<SymContext>				contexts;
	std::multimap<Handle /*namespace*/,Handle /*item*/>	nsitems;
	std::multimap<Handle /*namespace*/,Handle /*item*/>	ctxitems;
	std::map<Handle /*child*/,Handle /*parent*/> parentitems;
	std::map<double ,double * > realitems; 
public:
	std::vector<unsigned char>	comptypes;
public:
	xlang::vlDataPool	datapool;
};


} //namespace xlang;

#endif //WPP5_SYMDEF_H