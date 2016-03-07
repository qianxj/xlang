#ifndef WPSYMNODE_H
#define WPSYMNODE_H

#include <string>
#include "../common/util/xlvalue.hpp"

namespace xl {namespace wp {

enum SymKind : char
{
	kSymUnknown 	= 0,
	kSymAxis 			= 1,
	kSymOneof 		= 2,
	kSymOption 		= 3,
	kSymList 			= 4,
	kSymVar				= 5,
	kSymAssign			= 6,
	kSymAction			= 7,
	kSymKeyword 		= 8,
	kSymLiteral   		= 9,
	kSymOperate		= 10,
	kSymSyn			= 11,
	kSymSynTerm		= 12,
	kSymGuard			= 13,
	kSymSatisfy  		= 14,
	kSymStmt			= 15,
	kLiteral				= 16
};

template<class TKind>	
struct SymNode
{
	TKind kind;
};

struct SymList : public SymNode<SymKind>
{
	SymList* next;
	SymNode<SymKind>* term;
};

struct SymOneof : public SymNode<SymKind>
{
	SymNode<SymKind>* terms;
};

struct SymUntil : public SymNode<SymKind>
{
	SymNode<SymKind>* cond;
	SymNode<SymKind>* term;
};

struct SymOption : public SymNode<SymKind>
{
	SymNode<SymKind>* term;
};

struct SymDeclVar : public SymNode<SymKind>
{
	SymNode<SymKind>* type;
	int ident;
	SymNode<SymKind>* init;
};

struct SymAction : public SymNode<SymKind>
{
	int ident;
	SymNode<SymKind> param;
};

struct SymSym : public SymNode<SymKind>
{
	int name;
	SymNode<SymKind> * node;
};

struct SymSymTerm : public SymNode<SymKind>
{
	int name;
};

struct SymStmt : public SymNode<SymKind>
{
	SymNode<SymKind> * node;
};

struct SymLiteral : public SymNode<SymKind>
{
	util::misc::TValue* val;
	inline std::wstring toString() { return !val ? L"Literal()" : L"Literal(" + val->toStringEx() +L")" ;}
	inline ~SymLiteral() { if(val) delete val; }
};

struct SymKeyword : public SymNode<SymKind>
{
	int name;
};

template<class T> T * allocNode(){ return new T;}

//literal
template<class T>
SymLiteral*  makeLiteralEx(T &val)
{
	SymLiteral * term = allocNode<SymLiteral>();
	term->kind = kLiteral;
	term->val = new util::misc::TValue(val);
	return term;
}
inline SymLiteral*  makeLiteral(long long int val)						{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(unsigned long long int val) 		{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(int val)									{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(unsigned int val)						{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(short val)								{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(unsigned short val)					{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(char val) 								{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(unsigned char val)					{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(wchar_t val)							{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(double val)								{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(float val)									{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(wchar_t* val)							{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(char* val)								{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(bool val)									{ return makeLiteralEx(val); } 
inline SymLiteral*  makeLiteral(void* val)								{ return makeLiteralEx(val); } 

//list
inline SymList* makeSymList(SymNode<SymKind> * data)
{
	SymList * term = allocNode<SymList>();
	term->kind = kSymList;
	term->term = data;
	term->next = 0;
	return term;
}
//list expand term
inline SymNode<SymKind>* makeSymList(SymList * termList, SymNode<SymKind> * data)
{
	SymList * term = termList;
	while(term->next) term = term->next;
	term->next = makeSymList(data);
	return termList;
}

}} //namespace xl::wp

#endif //WPSYMNODE_H