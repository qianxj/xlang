#ifndef WPP5_PARSER_H
#define WPP5_PARSER_H

#include "vllexer.hpp"

#include "vlerrors.hpp"
#include "vlwerr.hpp"
#include "vlcontext.hpp"
#include "vlastaxis.hpp"
#include <stack>
#include <map>

using namespace std;
using namespace xlang;

namespace xlang
{

class vlParser
{
public:
	vlParser();
	~vlParser();
//base interface
public:
	virtual void* parse(vlContext* context,wstring_t src,wstring_t url);
public:
	//lexer
	virtual vlLexer* getLexer();
	virtual vlParser& setLexer(vlLexer* lexer_);
//util
public:
	virtual vlToken& getToken();
	virtual vlToken& nextToken(lexer_flag_t flag=0);
public:
	virtual bool skipLine();
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
	bool matchKeyword(wchar_t* kw);
	bool matchiKeyword(wchar_t* kw);
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
	static bool matchKeyword(vlToken &tk,wchar_t* kw);
	static bool matchiKeyword(vlToken &tk,wchar_t* kw);
	static bool matchBool(vlToken &tk);
public:
	int parse_bnf(vlContext * context,map<int ,vlAstAxis*> & rulers);
	vlAstAxis * parse_bnf_expression(vlContext* context);
	int parse_xlang_translation_unit(vlContext * context);
	int parse_lang_define(vlContext * context);
public:
	bool keywordCanIdentifier(vlContext * context, vlAstAxis * anode,vlToken & tk);
	int printAstAxis(vlAstAxis * anode);
	int parser_unify_term(vlContext * context);
	vlAstAxis * unify_term(vlContext * context, vlAstAxis * anode, bool option = false,bool trydo = true);
	bool ensure_unify_term(vlContext * context, vlAstAxis * anode,bool option);
private:
	int decodeApplyDefine(vlContext * context,vlAstAxis * node,
			vlAstAxis*	&lhs , 
			vlAstAxis*	&rhs , 
			vlAstAxis*	&action,
			int	&aliasid,
			int	&contextid,
			int	&elementid);
public:
	int fireGener(vlContext * context,vlAstAxis * curNode);
	int fireConstructor(vlContext * context,vlAstAxis * curNode);
	int fireNode(vlContext * context,vlAstAxis * curNode);
	int firePrint(vlContext * context,vlAstAxis * curNode);
	int fireTrace(vlContext * context,vlAstAxis * curNode);
public:
	int firePrioritySort(vlContext * context,vlAstAxis * curNode);
public:
	int doProcess(vlContext * context,vlAstAxis * curNode);
public:
	vlAstAxis * unify_apply(vlContext * context,vlApply*  apply, vlAstAxis * curNode);
	vlAstAxis * unify_apply_call(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyRhs,std::map<int,vlAstAxis*> & paramBind,bool child = true);
	vlAstAxis * unify_apply_action(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyRhs,std::map<int,vlAstAxis*> & paramBind,bool child = true);
public:
	struct MatchState
	{
		bool skipItem;
		bool optionItem;
		bool optionUnify;
		bool optionProcessing;
		vlAstAxis * nextNodeAfterProcess;
		void reset(){ memset(this,0,sizeof(MatchState));}
	};
	bool unify_apply_match_token(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyLhs,std::map<int,vlAstAxis*> & paramBind);
	bool unify_apply_match(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * applyLhs,std::map<int,vlAstAxis*> & paramBind,bool option = false);
	bool unify_apply_matchSpecialItem(vlContext * context,vlAstAxis * curNode,int ident,std::map<int,vlAstAxis*> & paramBind);
	bool unify_apply_matchTokenItem(vlContext * context,vlAstAxis * curNode,vlToken &token,std::map<int,vlAstAxis*> & paramBind);
	bool unify_apply_matchApplyParam(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * tNode,MatchState & state,std::map<int,vlAstAxis*> & paramBind);
	bool unify_apply_ensureMatchApplyParam(vlContext * context,vlApply* apply,vlAstAxis * curNode,vlAstAxis * tNode,std::map<int,vlAstAxis*> & paramBind);

public:
	VLANG_API static  void ReleaseParser(vlParser*  parser);
	VLANG_API static  vlParser * makeParser(vlContext * context);
	VLANG_API static  vlAstAxis * Parser(vlContext * context,vlParser* parser,wchar_t* ruler, wchar_t * source);
	VLANG_API static  vlAstAxis * Process(vlContext * context,vlParser* parser,wchar_t* processor, vlAstAxis * data);
	VLANG_API static  wchar_t * NodeToString(vlAstAxis * axis);
	VLANG_API static  vlAstAxis * ParserUrl(vlContext * context,vlParser* parser,wchar_t* ruler,wchar_t * source);
	VLANG_API static  intptr_t MakeClass(vlContext * context,const wchar_t* urlClass);
	VLANG_API static  void Output(vlContext * context,const wchar_t* str);
	VLANG_API static  void Print(vlContext * context,const wchar_t* format,const wchar_t* str);
public:
	std::stack<vlLexer *>* ptr_lexer_pool;
	vlLexer* lexer;
	xlang::wErr * werr;
	std::wstring soureUrl;
	std::wstring pirorSoureUrl;
};

} //namespace xlang

#endif //WPP5_PARSER_H
