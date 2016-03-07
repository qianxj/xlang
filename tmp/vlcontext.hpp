#ifndef WPP5_CONTEXT_H
#define WPP5_CONTEXT_H

#include "vlAstAxis.hpp"
#include "vlwpp_context.hpp"
#include "vlhashcontext.hpp"
#include <map>
#include <vector>
#include <set>
#include <stack>
#include "vltrap.hpp"
#include "vlhashcontext.hpp"
#include "vlAstFactory.hpp"

using namespace xlang;

namespace xlang
{

class vlApply;
class vlApplySvr
{
public:
	vlApply * getApply(int ide);
public:
	void addApply(int ide,vlApply* app);
private:
	std::vector<vlApply*> applys;
	std::map<int, int /*index*/> mapplys;
};

class vlModelSvr
{
public:
	vlAstNode * getItem(int ide);
	void addItem(int ide,vlAstNode* node);
private:
	std::vector<vlAstNode*> items;
	std::map<int /*ident*/, int /*index*/> mitems;
};


class vlContext
{
public:
	vlContext();
	~vlContext();
public:
	int langterm;
public:
	std::map<int /*ide*/,vlAstAxis*> rulers;
	std::map<int /*ide*/,vlAstAxis*> surerulers;
	//std::set<int /*ide*/> keywords;
	std::map<int /*ide*/, std::set<int>/*ide*/> scopeKeywords;
	std::map<int /*ide*/, std::set<int>/*ide*/> scopeKeywordsRef;

	vlAstAxis * primaryRuler;
	xlang::wpp_context * wppcontext;
public:
	bool isScopeKeyword(int ide,int kw);
	bool isScopeKeyword(int kw);
	void addKeyword(int kw);
public:
	std::map<int /*ide*/,vlAstAxis*> fires;
	std::map<int /*ide*/,vlAstAxis*> contructs;
	std::map<int /*ide*/,vlAstAxis*> patterns;
	std::map<int /*ide*/,int /*processor*/> firesProcess;
public:
	vlApplySvr applysvr;
	vlModelSvr modelsvr;
public:
	vlHashContext hashContext;
public:
	vlTrap*	   trap;
public:
	std::set<int> traceAstNodes;
	std::set<int> traceConstructs;
	std::set<int> traceFires;
public:
	int string_literal;
	int integer_literal;
	int double_literal;
	int char_literal;
	int bool_literal;
	int literal;
	int identifier;
	int declare;
	int constructor;
	int gener;
	int output_statement;
	int trace_ast;
	int trace_constructor;
	int trace_fire;
	int apply_element;
	int apply_ellipsisparam;
	int apply_param ;
	int apply_listparam ;
	int apply_match_any;
	int apply_rhs;
	int apply_compparam ;
	int apply_optparam ;
	int apply_lhs ;

public:
	bool bTraceAst;
	bool bTraceConstructor;
	bool bTraceFire;
	bool bTraceApply;
public:
	std::vector<int> contexts;
public:
	static int getOperatorPriortyLevel(int lang,int op);
public:
	vlHashContext * getHashContext(){return &hashContext;}
public:
	struct NameItem
	{
		int name;
		int item;
	};

	struct ValueItem
	{
		union
		{
			int			iValue;
			__int64		i8Value;
			double		dValue;
			void *		ptrValue;
			wchar_t*	strValue;
			char *		astrValue;
			vlAstAxis*	astAxis;
		};
		ValueItem(): dValue(0){};
	};
	struct ContextItem
	{
		int label;
		int variant;
		int tdata;
		wchar_t* ide;
		int item;
		int varcount;
		ContextItem(wchar_t* ide,int item):label(0),varcount(0),item(item),variant(0),tdata(0),ide(ide){}
	};

	std::vector<NameItem> labels;
	std::vector<ValueItem> variants;
	std::vector<ValueItem> tdatas; //ÁÙÊ±Êý¾Ý
	std::vector<ContextItem> stackItems;
public:
	void enterUnifyApply(wchar_t * ide,int item,int varCount = 0);
	void leaveUnifyApply();
public:
	int sureLabel(int ident); 
public:
	vlAstFactory* getAstFactory(int what = 0);
private:
	vlAstFactory* astFactory;
};

}//namespace xlang


#endif //WPP5_CONTEXT_H