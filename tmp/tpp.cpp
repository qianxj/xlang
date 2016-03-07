#include <assert.h>
#include "tpp.hpp"

namespace trap {
namespace complier {


int makeTpp(LangContext &context)
{
	//register keyword
	RegisterKeyword(context,L"context");
	RegisterKeyword(context,L"vord");
	RegisterKeyword(context,L"lang");
	RegisterKeyword(context,L"with");
	RegisterKeyword(context,L"syntax");
	RegisterKeyword(context,L"constructor");
	RegisterKeyword(context,L"namespace");
	RegisterKeyword(context,L"param");

	RegisterKeyword(context,L"voc");
	RegisterKeyword(context,L"vordx");

	RegisterKeyword(context,L"list");
	RegisterKeyword(context,L"pair");
	RegisterKeyword(context,L"or");
	RegisterKeyword(context,L"unit");
	RegisterKeyword(context,L"one");
	RegisterKeyword(context,L"of");

	RegisterKeyword(context,L"as");
	RegisterKeyword(context,L"option");
	RegisterKeyword(context,L"end");
	RegisterKeyword(context,L"var");
	RegisterKeyword(context,L"sure");
	
	//register term
	RegisterTerm(context,L"unit");
	RegisterTerm(context,L"context");
	RegisterTerm(context,L"vord");
	RegisterTerm(context,L"lang");
	RegisterTerm(context,L"with");
	RegisterTerm(context,L"syntax");
	RegisterTerm(context,L"constructor");
	RegisterTerm(context,L"namespace");
	RegisterTerm(context,L"sure");

	//define unit
	int unit_synTerms[] = { 
		synContainer,
		synCompose,
		synSyntax,
		synUntil,
		synError,
		synNill
	};
	makeTerm(context, L"unit", unit_synTerms); 
	
	//set unit container
	int unit_containers[] = {
		(int)context.mTerm[L"context"], 
		(int)context.mTerm[L"vord"],
		(int)context.mTerm[L"constructor"],
		(int)context.mTerm[L"lang"],
		(int)context.mTerm[L"with"],
		synNill
	};
	setSynTerm(context, L"unit", synContainer, unit_containers);
	
	//define  context and so on
	int context_synTerms[] = {
		synCompose,
		synSyntax,
		synUntil,
		synParam,
		synVariant,
		synConstructor,
		synNill
	};
	makeTerm(context, L"context", context_synTerms); 
	makeTerm(context, L"vord", context_synTerms); 
	makeTerm(context, L"lang", context_synTerms); 
	makeTerm(context, L"constructor", context_synTerms); 
	makeTerm(context, L"with", context_synTerms); 
	
	//define context syntax
	int param_idents[] = {
		synParam,
		0,
		tkIdent,
		0,
		synAssign,
		(int)L"$name",
		opSemicolon,
		synConstructor,
		(int)L"makeLangTerm",
		(int)L"$kind",
		(int)L"$name",
		synNill,
		synAssign,
		(int)L"$result",
		synNill
	};
	makeSyntaxTerm(context, L"param-ident", param_idents, sizeof(param_idents)/sizeof(param_idents[0]));
	context.data[getSynTermHole(context, L"context", synSyntax)] = getTermPc(context,L"param-ident");
	context.data[getSynTermHole(context, L"lang", synSyntax)] = getTermPc(context,L"param-ident");
	context.data[getSynTermHole(context, L"constructor", synSyntax)] = getTermPc(context,L"param-ident");
	context.data[getSynTermHole(context, L"vord", synSyntax)] = getTermPc(context,L"param-ident");
	//context.data[getSynTermHole(context, L"with", synSyntax)] = getTermPc(context,L"param-ident");

	//define context until
	int context_until[] = {
		opSemicolon,
		synNill
	};
	setSynTerm(context, L"context", synUntil, context_until);
	setSynTerm(context, L"vord", synUntil, context_until);
	setSynTerm(context, L"lang", synUntil, context_until);
	setSynTerm(context, L"constructor", synUntil, context_until);
	setSynTerm(context, L"with", synUntil, context_until);
	
	std::vector<int> params;
	params.push_back(context.pc);
	context.data[context.pc++] = tkKeyword;
	context.data[context.pc++] = (int)context.mKeyword[L"context"];
	context.data[context.pc++] = synAssign;
	context.data[context.pc++] = (int)L"$kind";
	context.data[context.pc++] = synNill;

	params.push_back(context.pc);
	context.data[context.pc++] = synVariant;
	context.data[context.pc++] = (int)L"$kind";
	context.data[context.pc++] = (int)L"TTokenNode*";
	context.data[context.pc++] = 0;
	context.data[context.pc++] = synOption;
	context.data[context.pc++] = tkKeyword;
	context.data[context.pc++] = (int)context.mKeyword[L"vordx"];
	context.data[context.pc++] = synNill;

	context.data[context.pc++] = synOneOf;
	context.data[context.pc++] = tkKeyword;
	context.data[context.pc++] = (int)context.mKeyword[L"vord"];
	context.data[context.pc++] = synAssign;
	context.data[context.pc++] = (int)L"$kind";
	context.data[context.pc++] = synNill;

	context.data[context.pc++] = tkKeyword;
	context.data[context.pc++] = (int)context.mKeyword[L"voc"];
	context.data[context.pc++] = synAssign;
	context.data[context.pc++] = (int)L"$kind";
	context.data[context.pc++] = synNill;

	context.data[context.pc++] = synNill;
	context.data[context.pc++] = synNill;

	params.push_back(context.pc);
	context.data[context.pc++] = tkKeyword;
	context.data[context.pc++] = (int)context.mKeyword[L"lang"];
	context.data[context.pc++] = synAssign;
	context.data[context.pc++] = (int)L"$kind";
	context.data[context.pc++] = synNill;

	params.push_back(context.pc);
	context.data[context.pc++] = tkKeyword;
	context.data[context.pc++] = (int)context.mKeyword[L"constructor"];
	context.data[context.pc++] = synAssign;
	context.data[context.pc++] = (int)L"$kind";
	context.data[context.pc++] = synNill;

	params.push_back(context.pc);
	context.data[context.pc++] = tkKeyword;
	context.data[context.pc++] = (int)context.mKeyword[L"with"];
	context.data[context.pc++] = synAssign;
	context.data[context.pc++] = (int)L"$kind";
	context.data[context.pc++] = synNill;

	int context_params[] = {
		params[0],
		synNill
	};
	context_params[0] = params[0];
	setSynTerm(context, L"context", synParam, context_params);
	context_params[0] = params[1];
	setSynTerm(context, L"vord", synParam, context_params);
	context_params[0] = params[2];
	setSynTerm(context, L"lang", synParam, context_params);
	context_params[0] = params[3];
	setSynTerm(context, L"constructor", synParam, context_params);
	//context_params[0] = params[4];
	//setSynTerm(context, L"with", synParam, context_params);

	//define context syntax
	//unit:= {[list] ident,','};
	int unit_syntaxs[] = {
		tkKeyword,(int)context.mKeyword[L"unit"],
		opColon,
		synList,
		synOption,
		tkKeyword,(int)context.mKeyword[L"list"],synNill,
		tkIdent,0,synNill,
		opComma,
		synNill,
		opSemicolon,
		synNill,
		synNill
	};
	makeSyntaxTerm(context, L"with_unit_syntax", unit_syntaxs, sizeof(unit_syntaxs)/sizeof(unit_syntaxs[0]));
	
	int term_syntax[] = {
		tkIdent,0,
		synAssign, (int)L"$id",
		tkKeyword,(int)context.mKeyword[L"syntax"],
		synAssign, (int)L"$kind",
		tkIdent,0,
		synAssign, (int)L"$name",
		synConstructor, (int)L"makeSyntax"/*,(int)L"$kind"*/, (int)L"$id", (int)L"$name",synNill,
		synAssign, (int)L"$result",
		opSemicolon,
		synNill};
	makeSyntaxTerm(context, L"with_term_syntax", term_syntax, sizeof(term_syntax)/sizeof(term_syntax[0]));

	makeSyntaxTerm(context, L"bnf_rhs");
	makeSyntaxTerm(context, L"bnf_term");

	int term_syntax_define[] = {
		tkKeyword,
		(int)context.mKeyword[L"syntax"],
		tkIdent,0,
		synAssign, (int)L"$name",
		opColon,
		synSyntax,
		(int)context.mTerm[L"bnf_rhs"],
		synAssign, (int)L"$snode",
		opSemicolon,
		synConstructor, (int)L"makeBnfRuler",(int)L"$name",(int)L"$snode",synNill,
		synAssign, (int)L"$result",
		synNill};
	makeSyntaxTerm(context, L"term_syntax_define", term_syntax_define, sizeof(term_syntax_define)/sizeof(term_syntax_define[0]));

	int term_sure_define[] = {
		tkKeyword,
		(int)context.mKeyword[L"sure"],
		tkIdent,0,
		synAssign, (int)L"$name",
		opColon,
		synSyntax,
		(int)context.mTerm[L"bnf_rhs"],
		synAssign, (int)L"$snode",
		opSemicolon,
		synConstructor, (int)L"makeSureBnfRuler",(int)L"$name",(int)L"$snode",synNill,
		synAssign, (int)L"$result",
		synNill};
	makeSyntaxTerm(context, L"term_sure_define", term_sure_define, sizeof(term_sure_define)/sizeof(term_sure_define[0]));

	int bnf_syntax_term[] = {
		synOneOf, 
		tkKeyword,(int)context.mKeyword[L"one"], 
		tkKeyword,(int)context.mKeyword[L"of"],
		synVariant,(int)L"$items",(int)L"TNodeList*",synNill,
		opLparen,
		synList,
		synSyntax,(int)context.mTerm[L"bnf_rhs"],
		synAssign,(int)L"$items",
		synNill,
		opComma,synNill, 
		opRparen,
		synConstructor,(int)L"makeBnfOneof",(int)L"$items",synNill,
		synAssign,(int)L"$result",
		synNill,

		tkKeyword,(int)context.mKeyword[L"option"],
		//opLparen,
		synSyntax,(int)context.mTerm[L"bnf_term"],
		synAssign,(int)L"$options",
		//opRparen,
		synConstructor,(int)L"makeBnfOption",(int)L"$options",synNill,
		synAssign,(int)L"$result",
		synNill,

		tkKeyword,(int)context.mKeyword[L"list"],
		opLparen,
		synSyntax,(int)context.mTerm[L"bnf_rhs"],
		synAssign,(int)L"$lists",
		synVariant,(int)L"$listcond",(int)L"TNodeList*",synNill,
		synOption,
		opComma,synSyntax,(int)context.mTerm[L"bnf_rhs"],synAssign,(int)L"$listcond",synNill,
		opRparen,
		synConstructor,(int)L"makeBnfList",(int)L"$lists",(int)L"$listcond",synNill,
		synAssign,(int)L"$result",
		synNill,
		tkIdent,0,synAssign,(int)L"$result", synNill, 

		opNot,
		tkIdent,0,
		synAssign, (int)L"$_id",
		synVariant,(int)L"$_params",(int)L"TNodeList*",synNill,
		opLparen,
		synList,

		synOneOf,
		tkIdent,0,synAssign,(int)L"$_params",synNill,
		tkLiteral,0,synAssign,(int)L"$_params",synNill,
		synNill,
		synNill,

		opComma, synNill,
		opRparen,
		synConstructor, (int)L"makeType",(int)L"$_id",(int)L"$_params",0,
		synAssign, (int)L"$result",
		synNill,

		opSub,
		synConstructor,(int)L"makeGuard",0,
		synAssign,(int)L"$result",
		synNill,

		synOneOf, 
		tkString,0,synAssign,(int)L"$result",synNill,
		tkChar,0,synAssign,(int)L"$result",synNill,
		synNill, 
		synNill, 

		opLparen,
		synSyntax,
		(int)context.mTerm[L"bnf_rhs"],
		synAssign,(int)L"$result",
		opRparen,
		synNill, 

		synNill, 
		synNill 
	};
	
	int bnf_syntax_rhs[] = {
		synVariant, (int)L"$result",(int)L"TNodeList*",synNill,
		synOption,
		tkKeyword,(int)context.mKeyword[L"var"],
		synList,
		synVariant, (int)L"$isList",(int)L"TNode*",synNill,
		synOption,tkKeyword,(int)context.mKeyword[L"list"],synAssign,(int)L"$isList",synNill,
		tkIdent,0, synAssign,(int)L"$id",
		synConstructor, (int)L"makeVar",(int)L"$id",(int)L"$isList",synNill,
		synAssign, (int)L"$result", 
		synNill,
		opComma,synNill,
		synNill,

		synList,
		synSyntax,(int)context.mTerm[L"bnf_term"],synAssign,(int)L"$result",synNill,
		synNill,
		synOption,
		opBor,
		synSyntax,(int)context.mTerm[L"bnf_rhs"],synAssign,(int)L"$result",
		synNill, 
		synNill
	};

	setSyntaxTerm(context, L"bnf_rhs", bnf_syntax_rhs, sizeof(bnf_syntax_rhs)/sizeof(bnf_syntax_rhs[0]));
	setSyntaxTerm(context, L"bnf_term", bnf_syntax_term, sizeof(bnf_syntax_term)/sizeof(bnf_syntax_term[0]));
	
	int term_param_syntaxs[] =
	{
		synVariant,(int)L"$paramItems",(int)L"TNodeList*",synNill,
		tkIdent,0,
		synAssign, (int)L"$paramID",
		tkKeyword,(int)context.mKeyword[L"param"],
		synList,
		tkIdent,0,
		synAssign,
		(int)L"$paramItemID",
		synOption,
		tkKeyword,(int)context.mKeyword[L"option"],synNill,
		tkKeyword,(int)context.mKeyword[L"as"],
		synOption,
		tkKeyword,(int)context.mKeyword[L"list"],synNill,
		synOneOf,
		tkKeyword,(int)context.mKeyword[L"pair"],
		tkIdent,0,
		tkKeyword,(int)context.mKeyword[L"or"],
		tkIdent,0,
		synConstructor,	(int)L"makeItem",	(int)L"$paramItemID",synNill,
		synAssign, (int)L"$paramItems",
		synNill,
		tkIdent,0,
		synConstructor,	(int)L"makeItem",	(int)L"$paramItemID",synNill,
		synAssign, (int)L"$paramItems",
		synNill,
		synNill,
		synNill,
		opComma,
		synNill,
		opSemicolon,
		synConstructor,(int)L"makeParam",(int)L"$paramID",(int)L"$paramItems",synNill,
		synAssign, (int)L"$result",
		synNill
	};
	makeSyntaxTerm(context, L"term_param_syntax", term_param_syntaxs, sizeof(term_param_syntaxs)/sizeof(term_param_syntaxs[0]));

	int with_body_syntaxs[] =
	{
		synVariant, (int)L"$result",(int)L"TNodeList*",synNill,
		synList,
		synOneOf,
		synSyntax,(int)context.mTerm[L"term_param_syntax"],synAssign, (int)L"$result",synNill,
		synSyntax,(int)context.mTerm[L"term_syntax_define"],synAssign, (int)L"$result",synNill,
		synSyntax,(int)context.mTerm[L"with_unit_syntax"],synAssign, (int)L"$result",synNill,
		synSyntax,(int)context.mTerm[L"with_term_syntax"],synAssign, (int)L"$result",synNill,
		synSyntax,(int)context.mTerm[L"term_sure_define"],synAssign, (int)L"$result",synNill,
		synNill,
		synNill, //list cond
		synNill, //end list
		synNill  //end syntax
	};
	makeSyntaxTerm(context, L"with_body_syntax",with_body_syntaxs, 
		sizeof(with_body_syntaxs)/sizeof(with_body_syntaxs[0]));
	int with_syntaxs[] =
	{
		tkKeyword,(int)context.mKeyword[L"with"],
		tkIdent,0,
		synSyntax,
		(int)context.mTerm[L"with_body_syntax"],
		synAssign, (int)L"$result",
		tkKeyword,(int)context.mKeyword[L"end"],
		tkKeyword,(int)context.mKeyword[L"with"],
		synNill
	};
	makeSyntaxTerm(context, L"with_syntax",with_syntaxs, 
		sizeof(with_syntaxs)/sizeof(with_syntaxs[0]));
	context.data[getSynTermHole(context, L"with", synSyntax)] = getTermPc(context,L"with_syntax");
	
	/*
	//define context construct
	int context_constructor_pc = pos;
	data[pos++] = synConstructor;
	data[pos++] = 1;
	data[pos++] = 1;
	data[pos++] = synNill;
	*/
	return 1;
}


}//complier
}//trap
