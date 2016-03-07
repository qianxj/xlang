#ifndef WPP_PREPARSE_H
#define WPP_PREPARSE_H

#include <stdlib.h>
#include <assert.h>
#include <windows.h>
#include <vector>
#include <set>
#include <map>
#include <list>

#include "vllexer.hpp"

#include "vlerrors.hpp"
#include "vlwerr.hpp"
#include "vlcontext.hpp"

#include "tppsymnode.hpp"
#include "vlparser.hpp"
#include "vlerrorsvr.hpp"
#include "tppgensvr.hpp"
#include "tpppcode.hpp"

using namespace xlang;

namespace trap {
namespace complier {

typedef vlContext Context;
typedef vlContext TContext;
typedef SNode TNode;
typedef OpList TNodeList;
typedef vlToken TToken;
typedef OpToken TTokenNode;

#define Ensure(r,kind,val) if(!r){parser->error(ctx,parser->makeToken(kind,val),tk); parser->skipLine(tk); return $result;}

struct VToken
{
	enum Kind
	{
		tkUnknown		= 0x00,
		tkString		= 0x01,
		tkChar			= 0x02,
		tkBool			= 0x03,
		tkInteger		= 0x04,
		tkDouble		= 0x05,
		tkIdent			= 0x06,
		tkHString		= 0x07,
		tkSym			= 0x08,
		tkKeyword		= 0x09,
		tkEof			= 0x0A,
		tkCustom		= 0x0B,
		tkLiteral		= 0x0C,

		//math operator
		opAdd			= 0x21,
		opSub			= 0x22,
		opMul			= 0x23,
		opDiv				= 0x24,
		opMod			= 0x25,
		
		//unary
		opAsn			= 0x26,
		opInc				= 0x27,
		opDec			= 0x28,
		
		//bit operator
		opShl				= 0x2A,
		opShr			= 0x2B,
		opBand			= 0x2C,
		opBor				= 0x2D,
		opXor			= 0x2E,
		opBnot			= 0x2F,
		
		//logic operator
		opOr				= 0x30,
		opAnd			= 0x31,
		opGt				= 0x32,
		opGte			= 0x33,
		opLt				= 0x34,
		opLte				= 0x35,
		opEq				= 0x36,
		opNeq			= 0x37,
		opNot			= 0x38,
		
		opThis			= 0x3A,
		opSuper			= 0x3B,

		//group operator
		opRparen		= 0x40,
		opLparen		= 0x41,
		opRbrace		= 0x42,
		opLbrace		= 0x43,
		opRbrackets	= 0x44,
		opLbrackets	= 0x45,
		opComma		= 0x46,
		opSemicolon	= 0x47,
		opSign		 	= 0x48,
		opDollar		 	= 0x49,
		
		opApos			= 0x4A,
		opPath2			= 0x4B,
		opSizeof			= 0x4C,
		
		opNest			= 0x50,
		opPointer  		= 0x51,
		opDot3	    	= 0x52,
		opDot2	    	= 0x53,
		opIn	    		= 0x54,
		opMref			= 0x55,
		opMpointer 	= 0x56,
		opVarrow   		= 0x57,
		opBecomes 	= 0x58,
		opQuery			= 0x59,
		opColon			= 0x5A,
		opHash			= 0x5B,
		opRef				= 0x5C,
		opBackslash	= 0x5D,
		
		//math asn operator
		opAddAsn		= 0x71,
		opSubAsn		= 0x72,
		opMulAsn		= 0x73,
		opDivAsn		= 0x74,
		opModAsn		= 0x75,
		
		//bit asn operator
		opShlAsn		= 0x7A,
		opShrAsn		= 0x7B,
		opBandAsn		= 0x7C,
		opBorAsn		= 0x7D,
		opXorAsn		= 0x7E,
		opBnotAsn		= 0x7F,
		
		opExclam   		= opNot,
		opPercent  		= opMod,
		opDot			= opRef,
		opPeriod			= opRef,
		opArrow	    	= opPointer,
		opPath			= opDiv
	};
	static Kind getOperator(wstring_t op);
	static const wchar_t* getOperatorString(Kind kind);
	const wchar_t* getOperatorString();

	Kind kind;
	union 
	{
		int ival;
		double dval;
		wstring_t str;
	};
	int len;
};

class SParser: public vlParser
{
public:
	vlLexer * scan;
	std::list<TToken> tokens;
public:
	inline VToken makeToken(int kind, int v)
	{
		VToken tk;
		tk.kind = (VToken::Kind)kind;
		tk.ival = v;
		return tk;
	}
	inline VToken makeToken(int kind, const wchar_t* val)
	{
		VToken tk;
		tk.kind=(VToken::Kind)kind;
		tk.str = (wstring_t)val;
		tk.len = (int)wcslen(val);
		return tk;
	}

	inline TToken pickToken(int index = 1)
	{
		TToken tk;
		int i = 1;
		while((int)tokens.size() < index)
		{
			scan->insymbol();
			tk = scan->token;
			tokens.push_back(tk);
		}
		std::list<TToken>::iterator iter = tokens.begin();
		while(i < index){i++; iter++;}
		tk = * iter;
		return tk;
	}

//lexer
public:
	virtual vlLexer* getLexer();
	virtual SParser& setLexer(vlLexer* lexer_);
//util
public:
	virtual vlToken& getToken();
	virtual vlToken& nextTokenEx(lexer_flag_t flag=0);
	virtual int nextToken(vlToken&, lexer_flag_t flag=0);
public:
	virtual bool skipLine();
	bool skipLine(vlToken & tk);
public:
	xlang::wErr * get_err_ptr();
	void set_err_ptr(xlang::wErr * err);
public:
	wchar_t token_str[256];
	static int	getTokenIdent(vlToken& tk);
	static const wstring_t	getTokenString(vlToken& tk);
	int	getTokenIdent();
	const wstring_t	getTokenString();
public:
	int  output(wstring_t str);
	int  format_output(wstring_t format,...);
	void error(error::err nError, const wstring_t str, const vlToken& tk);
public:
	bool ensurseToken(wstring_t kw,error::err  err,wchar_t* tk,bool skip_line = false);
	bool ensurseToken(wstring_t kw,error::err  err,bool skip_line = false);
	bool ensurseOperator(int op,error::err  err,bool skip_line = false);
	bool ensurseOperator(int op,error::err  err,wchar_t* tk, bool skip_line = false);
	bool ensurseIdent(error::err  err,bool skip_line = false);
	bool ensurseString(error::err  err,bool skip_line = false);
	bool ensurseNumber(error::err  err,bool skip_line = false);
public:
	bool match(int ty);
	bool matchEof();
	bool matchIdent();
	bool matchString();
	bool matchChar();
	bool matchBool();
	bool matchNumber();
	bool matchInteger();
	bool matchNumber(int v);
	bool matchNumber(double v);
	bool matchOperate();
	bool matchOperate(int op);
	bool matchKeyword();
	bool matchKeyword(const wchar_t* kw);
	bool matchiKeyword(const wchar_t* kw);
	bool matchChar(wchar_t ch);

	static bool matchChar(vlToken &tk,wchar_t ch);
	static bool match(vlToken &tk, int ty);
	static bool matchEof(vlToken &tk);
	static bool matchIdent(vlToken &tk);
	static bool matchString(vlToken &tk);
	static bool matchChar(vlToken &tk);
	static bool matchInteger(vlToken &tk);
	static bool matchNumber(vlToken &tk);
	static bool matchOperate(vlToken &tk);
	static bool matchOperate(vlToken &tk,int op);
	static bool matchKeyword(vlToken &tk);
	static bool matchKeyword(vlToken &tk,const wchar_t* kw);
	static bool matchiKeyword(vlToken &tk,const wchar_t* kw);
	static bool matchBool(vlToken &tk);

	inline bool isKeyword(TToken &tk,const wchar_t* name)
	{
		return matchKeyword(tk,name);
	}
	inline bool isIdent(TToken &tk)
	{
		return matchIdent(tk);
	}
	inline bool isLiteral(TToken &tk)
	{
		return matchNumber(tk)||matchBool(tk)|| matchChar(tk)|| matchString(tk);
	}
	inline bool isInteger(TToken &tk)
	{
		return matchInteger(tk);
	}
	inline bool isBool(TToken &tk)
	{
		return matchBool(tk);
	}
	inline bool isChar(TToken &tk)
	{
		return matchChar(tk);
	}

	inline bool isString(TToken &tk)
	{
		return matchString(tk);
	}
	inline bool isOperator(TToken &tk, wchar_t* op)
	{
		return matchOperate(tk,opValue::getOperateByString(op));
	}

	inline bool isOperator(TToken &tk, wchar_t op)
	{
		return matchOperate(tk,opValue::getOperateByToken(op));
	}

	inline bool getSourcePosition(TContext &ctx, int offset,int & row,int & col)
	{
		row = 1;
		col = 0;

		if(scan->sourceLen < offset) return false;
		if(offset < 0) return false;
		int pos = 0;
		while(pos <= offset)
		{
			if((scan->source[pos]=='\r' && scan->source[pos + 1]!='\n') ||scan->source[pos]=='\n')
			{
				row++;
				col = 1;
				pos++;
			}else
			{
				pos++;
				col++;
			}
		}
		return true;
	}

	inline TTokenNode *  TokenKeyword(TToken &tk)
	{
		return new OpToken(tk);
	}

	inline TTokenNode *  TokenNode(TToken &tk)
	{

		return new OpToken(tk);
	}

	inline bool error(TContext &context,VToken &hope, TToken &tk)
	{
		int row = 0;
		int col = 0;

		if(hope.kind >= 0x20)
		{
			error(error::err_needToken,(const wstring_t)hope.getOperatorString(),tk);
		}else if(hope.kind==VToken::tkKeyword)
		{
			error(error::err_needkeyword,hope.str,tk);
		}else if(hope.kind==VToken::tkIdent)
		{
			error(error::err_needident,0,tk);
		}else if(hope.kind==VToken::tkString)
		{
			error(error::err_needstring,0,tk);
		}else if(hope.kind==VToken::tkIdent)
		{
			error(error::err_unknownstmt,0,tk);
		}
		//this->error(error::m
//		getSourcePosition(context,tk.pos.offset,row,col);
//		printf("\nerror at %d:%d mis token %S before %S ",row,col, traceMisToken(hope).c_str(),
//			traceToken(tk).c_str());
		return true;
	}

	inline OpList*  makeList()
	{
		return new OpList();
	}
	
	inline OpList*  makeList(TNode* term)
	{
		return new OpList(term);
	}
	inline OpList*  makeList(OpList* head , TNode* term)
	{
		return opList(head,term);
	}

};
typedef SParser TParser;

inline void* makeNode(Context &ctx,const wchar_t* kind,...)
{
	return 0;
}

/*
inline coder::astTokenNode *  TokenKeyword(TToken &tk)
{
	coder::astTokenNode * node = coder::TokenNode(tk);
	node->tk.kind = tkKeyword;
	return node;
}
*/

TNode* addOleNamespace(TContext &context, TNode* id, TNode* olestr);
TNode* enterNamespace(TContext &context,TNode* id);
TNode* leaveNamespace(TContext &context);

TNode * ProcessApply(TContext &context,HType hType);

TNode* finishedClass(TContext &context);
TNode* finishedMethod(TContext &context);

//
TNode* makeClass(TContext &context,TNode* class_key,TNode* id,TNode* base);
TNode* forwardClass(TContext &context,TNode* class_key,TNode* id,TNode* base);
TNode* makeFunction(TContext &context,TNode* id,TNode* Type,TNode* paramList);
TNode* makeApply(TContext &context,TNode* id,TNode* typ, TNode* paramList);
TNode* makeTypeField(TContext &context,TNode* Type,TNode* id,TNode * init);
TNode* makeClassBase(TContext &context,TNode* class_key,TNode* typ);
TNode* makeThis(TContext &context);
TNode* makeQualifiedId(TContext &context,TNode * spec,TNode * terms,TNode * rank);
TNode* makeQualifiedTerm(TContext &context,TNode * nest,TNode* ident, TNode * param);
TNode* makeSuper(TContext &context);
TNode* makeItem(TContext &context,TNode* id);
TNode* makeLiteral(TContext &context,TNode* r);
TNode* makeField(TContext &context,TNode* exp,TNode* id);
TNode* makeElement(TContext &context,TNode* exp,TNode* rank);
TNode* makeInc(TContext &context,TNode* exp);
TNode* makeDec(TContext &context,TNode* exp);
TNode* makeUnryExpr(TContext &context,TNode* unary,TNode* exp);
TNode* makeBinaryExpr(TContext &context,TNode* op,TNode* lhs,TNode* rhs  );
TNode* makeTypeID(TContext &context,TNode* pid,TNodeList* id,TNodeList* params );
TNode* makeContinue(TContext &context);
TNode* makeBreak(TContext &context);
TNode* makeFor(TContext &context,TNode* init, TNode* cond,TNode* inc,TNode* body);
TNode* makeWhile(TContext &context,TNode* cond,TNode* body);
TNode* makeUntil(TContext &context,TNode* cond,TNode* body);
TNode* makeCompStmt(TContext &context, TNode* body);
TNode* makeIf(TContext &context, TNode* cond, TNode* tbody, TNode* fbody);
TNode* makeTypeID(TContext &context,TNode* axis,TNode* spec,TNode* id,TNode* param,TNode* rank);
TNode* makeReturn(TContext &context,TNode* expr);
TNode* makeCall(TContext &context, TNode* fn, TNode* args);
TNode* makeVar(TContext &context, TNode* id,TNode* typ,TNode* init);
TNode* makeParam(TContext &context, TNode* id,TNode* typ,TNode* init);
TNode* makeParam(TContext &context, TNode* id,TNode* typ,TNode* init,TNode* terms);
TNode* makeCast(TContext &context, TNode* expr, TNode* toType);
TNode* makeExprStmt(TContext &context, TNode* expr);
TNode* makeExpression(TContext &context, TNode* expr);

TNode* SetNativeFrom(TContext &context, TNode* val);
TNode* SetNativeAlias(TContext &context, TNode* val);

std::wstring TraceNode(TContext &context, TNode* stmt, std::wstring space =L"");
std::wstring TraceNode(TContext &context,PCode::Oprand* stmt,std::wstring  space =L"");
TNode*  dumpNode(TContext &context,TNode* node);

//语义检查
TNode* checkBinary(TContext & context,OpBinary* term,vlErrorSvr * errMgr);
TNode* checkItem(TContext &context,OpItem* expr,vlErrorSvr * errMgr);
TNode* checkLiteral(TContext &context,OpValue* expr,vlErrorSvr * errMgr);
TNode* checkTypeId(TContext &context,OpItem* node,vlErrorSvr * errMgr);
TNode* checkQualifiedId(TContext & context,OpItem* node,vlErrorSvr * errMgr);

TNode* checkExpression(TContext & context, TNode* node,vlErrorSvr * errMgr);
TNode* checkNode(TContext & context, TNode* node,vlErrorSvr * errMgr);
TNode* checkNode(TContext & context, TNode* node);
TNode* setFunctionBody(TContext & context, TNode* node);

TNode* normalizeConstBinary(OpBinary::Op op,OpValue* lhs, OpValue* rhs);
TNode* normalizeExpr(TNode* expr);
TNode* genExpr(TContext & context,TNode* expr, OpBlock* block,int & tVarIndex);
TNode* genExpr(TContext & context,TNode* expr, OpBlock* block);
TNode* genStmt(TContext & context,TNode* stmt);
TNode* genStmt(TContext & context,TNode* stmt,OpBlock* & block,OpBlock* continueBlock,OpBlock* breakBlock,OpBlock* exitBlock);

PCode::Oprand* genTmpLet(PCode::Oprand * opr, PCode::Block* block,int & tVarIndex);
bool OprandUsedTmp(PCode::Oprand * opr);

PCode::Oprand* genLValue(TContext & context,TNode* expr, PCode::Block* block,int & tVarIndex);
PCode::Oprand* genRValue(TContext & context,TNode* expr, PCode::Block* block,int & tVarIndex);

PCode::Oprand* genExpr(TContext & context,TNode* expr, PCode::Block* block,int & tVarIndex);
PCode::Oprand* genExpr(TContext & context,TNode* expr, PCode::Block* block);
PCode::Oprand* genStmt(TContext & context,TNode* stmt,PCode::Block* & block,PCode::Block* continueBlock,PCode::Block* breakBlock,PCode::Block* exitBlock);
PCode::Oprand* genStmtEx(TContext & context,TNode* stmt);

TNode* genCode(TContext & context, TNode* node);
TNode* genCode(TContext & context, TNode* node, GenSvr* genSvr);


PCode::Oprand* genCode(TContext & context, PCode::Oprand* node);
PCode::Oprand* genCode(TContext & context, PCode::Oprand* node, GenSvr* genSvr);


//错误及警告处理
void pushError(TNode* node,int errCode, vlErrorSvr * errMgr);
void pushError(TNode* node,int errCode, const wchar_t * errText,vlErrorSvr * errMgr);
void pushError(TNode* node,int errCode, int ident,vlErrorSvr * errMgr);
void pushWarn(TNode* node,int errCode, vlErrorSvr * errMgr);
void pushWarn(TNode* node,int errCode, const wchar_t * errText,vlErrorSvr * errMgr);
void pushWarn(TNode* node,int errCode, int ident,vlErrorSvr * errMgr);


}//namespace complier
}//namespace trap

#endif //WPP_PREPARSE_H