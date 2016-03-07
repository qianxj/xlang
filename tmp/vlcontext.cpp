#include "vlcontext.hpp"
#include "vlutils.hpp"
#include "vltoken.hpp"
#include "vlastToken.hpp"
#include "vlastfactory.hpp"
#include "vlparser.hpp"

using namespace xlang;

vlApply * vlApplySvr::getApply(int ide)
{
	if(mapplys.find(ide) == mapplys.end()) return 0;
	int index = mapplys[ide];
	return applys[ index - 1];
}

void vlApplySvr::addApply(int ide,vlApply* app)
{
	this->applys.push_back(app);
	if(mapplys.find(ide) != mapplys.end())
	{
		mapplys[ide] = (int)applys.size();
	}else
	{
		this->mapplys.insert(std::pair<int,int>(ide,(int)applys.size())); 
	}
}


vlAstNode * vlModelSvr::getItem(int ide)
{
	if(mitems.find(ide)==mitems.end()) return 0;
	return items[mitems[ide] - 1];
}
void vlModelSvr::addItem(int ide,vlAstNode* node)
{
	items.push_back(node);
	int index = (int)items.size();
	mitems.insert(std::pair<int,int>(ide,index)); 
}


vlContext::vlContext(): trap(new vlTrap),primaryRuler(0),langterm(0),wppcontext(0),bTraceAst(false),bTraceConstructor(false),bTraceFire(false),bTraceApply(false),
	astFactory(new vlAstFactory)
{
	string_literal	= util::hashString(L"string-literal"); 
	integer_literal = util::hashString(L"integer-literal");
	double_literal	= util::hashString(L"double-literal");
	char_literal	= util::hashString(L"char-literal");
	bool_literal	= util::hashString(L"bool-literal"); 
	literal	= util::hashString(L"literal");
	identifier	= util::hashString(L"identifier");

	declare = util::hashString(L"declare");
	constructor = util::hashString(L"constructor-define");
	output_statement = util::hashString(L"output-statement");

	trace_ast = util::hashString(L"trace-ast");
	trace_constructor = util::hashString(L"trace-constructor");
	trace_fire = util::hashString(L"trace-fire");

	gener = util::hashString(L"gener-define");

	apply_ellipsisparam = util::hashString(L"apply-ellipsisparam");
	apply_element = util::hashString(L"apply-element");
	apply_param = util::hashString(L"apply-param");
	apply_match_any = util::hashString(L"apply-match-any");
	apply_rhs = util::hashString(L"apply-rhs");
	apply_listparam = util::hashString(L"apply-listparam");

	apply_compparam = util::hashString(L"apply-compparam");
	apply_optparam  = util::hashString(L"apply-optparam");
	apply_lhs  = util::hashString(L"apply-lhs");

	hashContext.enter_scope();
}

vlContext::~vlContext()
{
	hashContext.leave_scope();
	if(astFactory)delete astFactory;
}

bool vlContext::isScopeKeyword(int ide,int kw)
{
	if(scopeKeywords.find(ide)==scopeKeywords.end()) return false;
	if(scopeKeywords[ide].find(kw)==scopeKeywords[ide].end()) return false;
	return true;
}

bool vlContext::isScopeKeyword(int kw)
{
	//return isScopeKeyword(langterm,kw);
	map<int,set<int>>::iterator iter = scopeKeywords.begin();
	while(iter!=scopeKeywords.end())
	{
		if(iter->second.find(kw)!=iter->second.end()) return true;
		iter++;
	}
	return false;
}

void vlContext::addKeyword(int kw)
{
	if(scopeKeywords.find(langterm)==scopeKeywords.end())
	{
		set<int> v;
		v.insert(kw);
		scopeKeywords.insert(pair<int,set<int>>(langterm,v));
	}
	else
	{
		if(scopeKeywords[langterm].find(kw)==scopeKeywords[langterm].end())
			scopeKeywords[langterm].insert(kw);
	}	
}

int vlContext::getOperatorPriortyLevel(int lang,int op)
{
	const int Assignment = 1;
	const int Conditional= 2;
	const int Logical	 = 3;
	const int Bit		 = 4;
	const int Equality	 = 5;
	const int Relational = 6;
	const int Shift		 = 7;
	const int Additive	 = 8;
	const int Multiplicative = 9;
	const int Pm	    = 10;
	const int Cast	    = 11;
	const int Unary     = 12;
	const int Postfix   = 13;
	const int Primary   = 14;
	const int Unvalid   = 15;
	switch(op)
	{
	case opValue::add:
	case opValue::sub:
		return Additive;
	case opValue::mul:
	case opValue::div:
	case opValue::mod:
		return Multiplicative;
	case opValue::shl:
	case opValue::shr:
		return Shift;
	case opValue::asn:
		return Assignment;
	case opValue::inc:
	case opValue::dec:
		return Unary;
	case opValue::band:
	case opValue::bnot:
	case opValue::xor:
	case opValue::bor:
		return Bit;
	case opValue::or:
	case opValue::and:
	case opValue::not:
		return Logical;
	case opValue::eq:
	case opValue::neq:
		return Equality;
	case opValue::gt:
	case opValue::gte:
	case opValue::lt:
	case opValue::lte:
		return Relational;
	}
	return Unvalid;
}

void vlContext::enterUnifyApply(wchar_t* ide,int item ,int varCount)
{
	stackItems.push_back(ContextItem(ide,item - 1));
	if(varCount > 0)
	{
		stackItems.back().variant = (int)variants.size();
		stackItems.back().varcount = varCount;
		variants.resize((int)variants.size() + varCount);
	}
	
}
void vlContext::leaveUnifyApply()
{
	if(stackItems.back().label)
	{
		for(int i= stackItems.back().label - 1;i<(int)labels.size();i++)
			trap->getCoder()->freeLabel(labels[i].item);
		labels.resize(stackItems.back().label - 1);
	}
	if(stackItems.back().varcount)
		variants.resize(stackItems.back().variant);
	if(stackItems.back().tdata)
		tdatas.resize(stackItems.back().tdata - 1);

	stackItems.pop_back();
}

vlAstFactory* vlContext::getAstFactory(int what)
{
	return this->astFactory;
}

int vlContext::sureLabel(int ident)
{
	int parent = (int)stackItems.size() - 2;
	if(stackItems[parent].label)
	{
		for(int i= stackItems[parent].label - 1;i<(int)labels.size();i++)
		{
			if(labels[i].name == ident) return labels[i].item;
		}
	}

	NameItem nItem;
	nItem.name = ident;
	nItem.item = trap->getCoder()->askLabel();
	labels.push_back(nItem);
	if(!stackItems[parent].label)stackItems[parent].label = (int)labels.size();
	return nItem.item;
}


