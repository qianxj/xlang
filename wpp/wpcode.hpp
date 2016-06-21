#ifndef WPCODE_H
#define WPCODE_H

#include "wpsymnode.hpp"
#include "../common/util/xlvalue.hpp"
#include "stringpool.hpp"

namespace xl {namespace wp { namespace coder {
	
	enum CodeKind
	{
		kNode = 1,
		kContext = 2,
		kFunction = 3,
		kParam = 4,
		kVariant = 5,
		kBranch = 6,
		kType = 7,
		kClass = 8,
		kReturn = 9,
		kList = 10,
		kExpr = 11,
		kStmt = 12,
		kBranchItem = 13,
		kCall = 14,
		kLiteral = 15,
		kLoop = 16,
		kUntil = 17,
		kBreak = 18,
		kContinue = 19,
		kIf = 20,
		kMatch = 21,
		kOneof = 22,

		kRef   = 23,
		kDec   = 24,
		kInc   = 25,

		/*
		kAdd   = 26,
		kSub   = 27,
		kMul   = 28,
		kDiv   = 29,

		kOr	   = 30,
		kAnd   = 31,
		kNot   = 32,
		*/

		kTerm  = 33,
		kBinary = 34,
		kUnary  = 35,
		kToken  = 36,
		kOption = 37,

		kGuard	= 38,
		kCompStmt = 39
	};

	struct PNode {
		CodeKind kind;
	};

	struct PContext : public PNode{
		
	};

	struct PFunction : public PNode{
		const wchar_t * name;
		PNode* type;
		PNode* params;
	};
	
	struct PMatch : public PNode
	{
		enum MatchKind
		{
			mkIdentifier,
			mkKeyword,
			mkOperate,
			mkLiteral,
			mkStringLiteral,
			mkIntLiteral,
			mkCharLiteral,
			mkBoolLiteral,
			mkDoubleLiteral,
			mkSymbolTerm,
			mkValue
		} matckKind;
		union
		{
			HSymbol kword;
			HSymbol ident
			Operate op;
			TValue val;
		};
	};

	struct PParam : public PNode{
		PNode* type;
		const wchar_t* name;
		PNode* init;
	};

	struct PVariant : public PNode {
		bool islist
		//PNode* type;
		const wchar_t* name;
		//PNode* init;
	};


	struct PTokenNode : public PNode
	{
		Token tk;
	};

	struct PExpr : public PNode {
		PNode* node;
	};

	struct PBranch : public PNode {
		PNode* expr;
		PNode* branchItemList;
		PNode* defaultStmt;
	};

	struct PIf : public PNode {
		PExpr* expr;
		PNode* trueStmt;
		PNode* falseStmt;
	};

	struct PType	: public PNode {
		PNode* node;
	};

	struct PClass : public PNode {
		ClassKind classKind;
		PNode* fields;
		PNode* methods;
		const wchar_t * name;
	};

	struct PReturn : public PNode {
		PNode * expr;
	};

	struct PList : public PNode {
		PList* next;
		PNode* term;
	};

	struct PStmt : public PNode {
		PNode * node;
	};

	struct PBranchItem : public PNode {
		PNode * cond;
		PNode * stmt;
	};

	struct PCall : public PExpr {
		const wchar_t * name;
		PNode * func;
		PNode * args;
	};

	struct PBinary : public PNode
	{
		tokenKind_t opKind;
		PNode * rhs;
		PNode * lhs;
	};

	struct PUnary : public PNode
	{
		tokenKind_t opKind;
		PNode * node;
	};

	struct PCompStmt : public PNode
	{
		PNode * stmt;
	};

	struct PLiteral : public PExpr{
		TValue val;
	};

	struct PLoop : public PNode{
		PNode * cond;
		PNode * stmt;
	};
	struct PBreak : public PNode{};
	struct PContinue : public PNode{};
	struct PUntil : public PLoop {};

	struct POneof : public PNode{
		PNode * nodes;
	};

	struct POption : public PNode{
		PNode * node;
	};

	/*
	struct PMatch : public PExpr{
		PNode * term;
		PExpr * temp;
	};*/

	struct PRef : public PExpr
	{
	};

	struct PTerm : public PNode
	{
		HSymbol name;
	};

	struct PGuard : public PNode
	{
	};

	template<class T> T * allocNode(){ return new T();}
	//lookup
	template<class T> T * LookupContextObject(const wchar_t* name){
		return (T*) 0;
	}

	PContext * Context();
	PFunction * Function(PType * type,const wchar_t* name);
	PParam * Param(PType * type,const wchar_t* name,PNode * init = 0);
	PVariant * Variant(PType * type,const wchar_t* name,PNode * init = 0);
	PCall * Call(PNode * func, PNode * args);
	PCall * Call(const wchar_t* name, PNode * args);
	PReturn * Return(PExpr * term);
	PClass * Class(const wchar_t* name);
	PList * List(PList* piror, PNode * term);
	PList * List(PNode * term);
	PList * List();
	PExpr * Expr(PNode * term);
	PLoop * Loop(PNode * cond, PNode *term);
	PBreak * Break();
	PContinue * Continue();
	PUntil * Until(PNode *term, PNode * cond);
	PBranch * Branch(PExpr * expr, PList * items, PNode* defautStmt );
	PIf * If(PExpr * cond, PNode * trueStmt, PNode * falseStmt );
	PBranchItem* BranchItem(PExpr * match , PNode * stmt);
	PBinary* Binary(tokenKind_t opKind, PNode * rhs, PNode * lhs);
	PUnary* Unary(tokenKind_t opKind, PNode * node);
	PTerm* Term(HSymbol name);
	
	PMatch * MatchKeyword(HSymbol kword);
	PMatch * MatchLiteral(TValue &val);
	PMatch * MatchOperate(Operate op);
	PMatch * MatchIdentifier();
	PMatch * MatchLiteral();
	PMatch * MatchIntergerLiteral();
	PMatch * MatchBoolLiteral();
	PMatch * MatchCharLiteral();
	PMatch * MatchDoubleLiteral();
	PMatch * MatchStringLiteral();
	PMatch * MatchSymbolTerm();


	//POneof * OneOf(PList * term);
	//POption * Option(PNode * term);
	//PGuard * Guard();

	PType * ContextType(const wchar_t* name);
	PClass * ContextClass(const wchar_t* name);
	PParam * ContextParam(const wchar_t* name);
	PVariant * ContextVariant(const wchar_t* name);
	PFunction * ContextFunction(const wchar_t* name);
	PNode * ContextTerm(const wchar_t* name);
	PCompStmt * CompStmt(PNode* stmt);
	//context
	PNode * Append(PNode * parent, PNode * term);
	//literal
	PLiteral * Literal(int val);
	PLiteral * Literal(double val);
	PLiteral * Literal(const wchar_t* val);
	PLiteral * Literal(bool val);
	PLiteral * Literal(wchar_t val);
	PLiteral * Literal(char val);
	PExpr *  Ref(PNode* expr);

	PTokenNode * TokenNode(Token &tk);
};


}}} //namespace xl::wp::coder


#endif //WPCODE_H