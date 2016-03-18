//version 1.0
// writer : qian xian jie
// 2014-10-21

#ifndef WPP_AST_H
#define WPP_AST_H

#include "wptoken.hpp"

namespace trap {
namespace complier {

namespace coder
{
	enum NodeKind
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

	enum ClassKind
	{
		ckClass = 1,
		ckStruct = 2,
		ckUnoin = 3,
		ckEnum = 4,
		ckInterface = 5,
		ckEvent = 6
	};

	enum LiteralKind
	{
		lkInt		= 1,
		lkInt32		= 1,
		lkInt64		= 2,
		lkDouble	= 2,
		lkFloat		= 3,
		lkString	= 4,
		lkAString	= 5,
		lkByte		= 6,
		lkChar		= 7,
		lkBool		= 8
	};

	struct astNode {
		NodeKind kind;
	};

	struct astContext : public astNode{
		
	};

	struct astFunction : public astNode{
		const wchar_t * name;
		astNode* type;
		astNode* params;
	};

	struct astParam : public astNode{
		astNode* type;
		const wchar_t* name;
		astNode* init;
	};

	struct astVariant : public astNode {
		astNode* type;
		const wchar_t* name;
		astNode* init;
	};


	struct astTokenNode : public astNode
	{
		Token tk;
	};

	struct astExpr : public astNode {
		astNode * node;
	};

	struct astBranch : public astNode {
		astNode* expr;
		astNode* branchItemList;
		astNode* defaultStmt;
	};

	struct astIf : public astNode {
		astExpr* expr;
		astNode* trueStmt;
		astNode* falseStmt;
	};

	struct astType	: public astNode {
		astNode* node;
	};

	struct astClass : public astNode {
		ClassKind classKind;
		astNode* fields;
		astNode* methods;
		const wchar_t * name;
	};

	struct astReturn : public astNode {
		astNode * expr;
	};

	struct astList : public astNode {
		astList* next;
		astNode* term;
	};



	struct astStmt : public astNode {
		astNode * node;
	};

	struct astBranchItem : public astNode {
		astNode * cond;
		astNode * stmt;
	};

	struct astCall : public astExpr {
		const wchar_t * name;
		astNode * func;
		astNode * args;
	};

	struct astBinary : public astNode
	{
		tokenKind_t opKind;
		astNode * rhs;
		astNode * lhs;
	};

	struct astUnary : public astNode
	{
		tokenKind_t opKind;
		astNode * node;
	};

	struct astCompStmt : public astNode
	{
		astNode * stmt;
	};

	struct astLiteral : public astExpr{
		LiteralKind literalKind;
		union 
		{
			int			i32;
			__int64		i64;
			double		d64;
			float		d32;
			const wchar_t*	str;
			const char*		astr;
			wchar_t		c16;
			char		c8;
			bool		b;
		}val;
	};

	struct astLoop : public astNode{
		astNode * cond;
		astNode * stmt;
	};
	struct astBreak : public astNode{};
	struct astContinue : public astNode{};
	struct astUntil : public astLoop {};

	struct astOneof : public astNode{
		astNode * nodes;
	};

	struct astOption : public astNode{
		astNode * node;
	};

	struct astMatch : public astExpr{
		astNode * term;
		astExpr * temp;
	};

	struct astRef : public astExpr
	{
	};

	struct astTerm : public astNode
	{
		const wchar_t * name;
	};

	struct astGuard : public astNode
	{
	};

	template<class T> T * allocNode(){ return new T();}
	//lookup
	template<class T> T * LookupContextObject(const wchar_t* name){
		return (T*) 0;
	}

	astContext * Context();
	astFunction * Function(astType * type,const wchar_t* name);
	astParam * Param(astType * type,const wchar_t* name,astNode * init = 0);
	astVariant * Variant(astType * type,const wchar_t* name,astNode * init = 0);
	astCall * Call(astNode * func, astNode * args);
	astCall * Call(const wchar_t* name, astNode * args);
	astReturn * Return(astExpr * term);
	astClass * Class(const wchar_t* name);
	astList * List(astList* piror, astNode * term);
	astList * List(astNode * term);
	astList * List();
	astExpr * Expr(astNode * term);
	astLoop * Loop(astNode * cond, astNode *term);
	astBreak * Break();
	astContinue * Continue();
	astUntil * Until(astNode *term, astNode * cond);
	astBranch * Branch(astExpr * expr, astList * items, astNode* defautStmt );
	astIf * If(astExpr * cond, astNode * trueStmt, astNode * falseStmt );
	astMatch * Match(astNode * term, astExpr * temp);
	astBranchItem* BranchItem(astExpr * match , astNode * stmt);
	astBinary* Binary(tokenKind_t opKind, astNode * rhs, astNode * lhs);
	astUnary* Unary(tokenKind_t opKind, astNode * node);

	astOneof * OneOf(astList * term);
	astOption * Option(astNode * term);
	astGuard * Guard();

	astType * ContextType(const wchar_t* name);
	astClass * ContextClass(const wchar_t* name);
	astParam * ContextParam(const wchar_t* name);
	astVariant * ContextVariant(const wchar_t* name);
	astFunction * ContextFunction(const wchar_t* name);
	astNode * ContextTerm(const wchar_t* name);
	astCompStmt * CompStmt(astNode* stmt);
	//context
	astNode * Append(astNode * parent, astNode * term);
	//literal
	astLiteral * Literal(int val);
	astLiteral * Literal(double val);
	astLiteral * Literal(const wchar_t* val);
	astLiteral * Literal(bool val);
	astLiteral * Literal(wchar_t val);
	astLiteral * Literal(char val);
	astExpr * Ref(astNode* expr);

	astTokenNode * TokenNode(Token &tk);
};

} // namespace complier
} // namespace trap

#endif // WPP_AST_H