#ifndef WPP5_VLAPPLY_H
#define WPP5_VLAPPLY_H

#include "vlAstAxis.hpp"
#include "vlwpp_context.hpp"
#include "vlhashcontext.hpp"
#include <map>
#include <vector>
#include <set>
#include <deque>
#include "vltrap.hpp"

namespace xlang
{

class vlContext;
class vlParser;
class vlApply
{
public:
	vlApply(int ide);
	~vlApply();
public:
	bool Initial();
public:
	int ident;
public:


	struct Context
	{
		int	id;
		vlAstAxis * lhs;
		vlAstAxis * rhs;
		vlAstAxis * code;
		int		  varIndex;
		int		  varCount;
	};

	void addItem(int ide, int contextid, vlAstAxis * lhs,vlAstAxis * rhs,vlAstAxis * code = 0,int varStart =0);
	Context & getItem(int index);
	std::multimap<int, int > & getMap();
public:
	void addAlias(int ident, vlAstAxis * lhs);
private:
	std::deque<Context> items;
	std::multimap<int /*ident*/, int /*index to items from 1*/> mItems;
public:
	std::map<int /*ident*/,vlAstAxis * /*lhs*/> mAlias;
public:
	struct UsingProcessor
	{
		int id;
		int alias;
		UsingProcessor(int ident,int alias):id(ident),alias(alias){}
	};
	std::vector<UsingProcessor> mProcessores;
	inline void addUsingProcessor(int ident, int alias){
		mProcessores.push_back(UsingProcessor(ident,alias));
	}
public:
	struct Variant
	{
		int id; /*ident*/
		bool list;
		int typ; /*index*/
		vlAstAxis * val;
		std::vector<Variant> * fields;
	public:
		Variant(int id,bool list=false):id(id),list(list),val(0),fields(0),typ(0){}
	};
	std::deque<Variant> variants;
	vlHashContext hashContext;
//public:
//	int makeVariantFrame();
public:
	std::wstring dumpVariant();
	std::wstring dumpCode(vlAstAxis * code,bool bfirst = true);
public:
	VLANG_API static vlAstAxis * makeLiteralNode(vlAstFactory* astFactory,vlAstNode * node);
	VLANG_API static vlAstAxis * makeItemNode(vlAstFactory* astFactory,int ide);
	VLANG_API static vlAstAxis * makeVarItemNode(vlAstFactory* astFactory,int ide);
	VLANG_API static vlAstAxis * makeCallNode(vlAstFactory* astFactory,int ide);
	VLANG_API static vlAstAxis * makeCompBlock(vlAstFactory* astFactory);
	VLANG_API static vlAstAxis * makeNillNode(vlAstFactory* astFactory);
	VLANG_API static vlAstAxis * makeThisNode(vlAstFactory* astFactory);
	VLANG_API static vlAstAxis * makeBaseTypeNode(vlAstFactory* astFactory);
	VLANG_API static vlAstAxis * makeNameNode(vlAstFactory* astFactory,int ide);
	VLANG_API static vlAstAxis * makeNameNode(vlAstFactory* astFactory,wchar_t* name);
	VLANG_API static vlAstAxis * makeBoolToken(vlAstFactory* astFactory,bool v);
	VLANG_API static vlAstAxis * makeIntegerToken(vlAstFactory* astFactory,int v);
	VLANG_API static vlAstAxis * makeDoubleToken(vlAstFactory* astFactory,double v);
	VLANG_API static vlAstAxis * makeStringToken(vlAstFactory* astFactory,const wchar_t* v);
	VLANG_API static vlAstAxis * makeOperateToken(vlAstFactory* astFactory,const wchar_t* v);
	VLANG_API static vlAstAxis * makeIdentNode(vlAstFactory* astFactory,int ide);
	VLANG_API static vlAstAxis * makeIdentToken(vlAstFactory* astFactory,int ide);

	VLANG_API static vlAstAxis * makeLiteralNode(class vlContext* context,vlAstNode * node);
	VLANG_API static vlAstAxis * makeItemNode(class vlContext* context,int ide);
	VLANG_API static vlAstAxis * makeVarItemNode(class vlContext* context,int ide);
	VLANG_API static vlAstAxis * makeCallNode(class vlContext* context,int ide);
	VLANG_API static vlAstAxis * makeCompBlock(class vlContext* context);
	VLANG_API static vlAstAxis * makeNillNode(class vlContext* context);
	VLANG_API static vlAstAxis * makeThisNode(class vlContext* context);
	VLANG_API static vlAstAxis * makeBaseTypeNode(class vlContext* context);
	VLANG_API static vlAstAxis * makeNameNode(class vlContext* context,int ide);
	VLANG_API static vlAstAxis * makeNameNode(class vlContext* context,wchar_t* name);
	VLANG_API static vlAstAxis * makeBoolToken(class vlContext* context,bool v);
	VLANG_API static vlAstAxis * makeIntegerToken(class vlContext* context,int v);
	VLANG_API static vlAstAxis * makeDoubleToken(class vlContext* context,double v);
	VLANG_API static vlAstAxis * makeStringToken(class vlContext* context,const wchar_t* v);
	VLANG_API static vlAstAxis * makeOperateToken(class vlContext* context,const wchar_t* v);

public:
	int proc_variant_item(class vlContext* context,vlAstAxis * node,Variant &v,vlParser* parser);
	int proc_variant(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_statement(class vlContext* context, vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_action(class vlContext* context,vlAstAxis * node,vlParser* parser);
public:
	vlAstAxis * proc_stmt_if(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_stmt_while(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_stmt_break(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_stmt_continue(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_stmt_return(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_stmt_let(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_stmt_foreach(class vlContext* context,vlAstAxis * node,vlParser* parser);
public:
	vlAstAxis * proc_field(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_expression(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_primary_expression(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_expressionlist(class vlContext* context,vlAstAxis * node,vlParser* parser);
	vlAstAxis * proc_call(class vlContext* context,vlAstAxis * node,vlParser* parser);
public:
	//controlState = 1 return 2 continue 3 break
	vlAstAxis * eval(int & controlState, vlAstAxis * code,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind,bool child=false,bool bfirst = true);
	vlAstAxis * eval(vlAstAxis * code,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind,bool child=false,bool bfirst = true);
	vlAstAxis * doCall(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind) throw(...);
	vlAstAxis * doSymCall(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind) throw(...);
	vlAstAxis * doEntryPointCall(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind) throw(...);
	vlAstAxis * doApply(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind);
	//反向list
	vlAstAxis * doVApply(vlAstAxis * node,std::vector<vlAstAxis *> &args,vlParser* parser,vlContext * context,vlAstAxis * curNode,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * doMakeNode(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * doMakeListNode(vlAstAxis * node,vlContext* context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	int doBind(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	int doPrint(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind,vlParser* parser);
	int doTrace(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind,vlParser* parser);
	int doTypeText(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	int doPush(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	int doPop(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsIdentifier(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsLiteral(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsNumber(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsDouble(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsInteger(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsChar(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsBool(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsString(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsCall(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool doIsTerm(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * doTop(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * doTopEx(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	int doSetTop(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
//类型处理
public:
	//sym
	Handle doAddNamespace(vlContext * context,Handle hAxis,int Id);
	Handle doAddType(vlContext * context,Handle hAxis,int kind, int Id,bool todef);
	Handle doMakeTypeRef(vlContext * context);
	Handle doAddField(vlContext * context, Handle hAxis,int id, unsigned spec,Handle hType);
	Handle doAddMethod(vlContext * context,Handle hAxis,int id, unsigned spec,Handle hType);
	Handle doAddVariant(vlContext * context,HMethod hMethod, int id, unsigned spec,Handle hType);
	Handle doAddParam  (vlContext * context,HMethod hMethod, int id, unsigned spec,Handle hType);
	Handle doGetTypeHandle(vlContext * context,int typeId);
	Handle doGetFieldHandle(vlContext * context,Handle hType,int fieldId);
	Handle doGetTypeMethodHandle(vlContext * context,Handle hType,int id);
	int	doGetTypeAllocSize(vlContext * context,Handle hType);
	int	doGetFieldAllocSize(vlContext * context,Handle hField);
	int doGetOffsetOfField(vlContext * context,Handle hType,Handle hField);
	int doGetOffsetOfTerm(vlContext * context,Handle hMethod,Handle hTerm);
	void doAddBase(vlContext * context,Handle hType,Handle hBase);
	void doAddCompTypeItem(vlContext * context,Handle hType);
	void doAddCompTypeRank(vlContext * context,int rank);
	void doInsertCompTypeItem(vlContext * context,Handle hCompType,Handle hType);
	void doInsertCompTypeRank(vlContext * context,Handle hCompType,int rank);
	HType	doGetCompTypeItem(Handle hCompType);
	int		doGetCompTypeRank(Handle hCompType);
	HType	doStepCompType(Handle hCompType);

public:
	int doHash(const wchar_t * str);
	wchar_t * doGetHashString(int id);
public:
	bool Gt(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Gte(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Lt(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Lte(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Eq(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Neq(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Nil(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Not(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool And(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	bool Or(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	
	vlAstAxis * SqlSelect(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * toString(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Concat(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Add(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Sub(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Mul(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Div(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Mod(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Shr(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Shl(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Band(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Bor(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Bnot(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Xor(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Inc(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Dec(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * Neg(vlAstAxis * node,vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);

public:
	vlAstAxis * MakeTypeDataNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * MakeDataBufferNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	
	/*
	vlAstAxis * MakeStringNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * MakeIntegerNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * MakeShortNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * MakeCharNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * MakeByteNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * MakeDoubleNode(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis * MakeInt64Node(vlContext * context,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
	*/

	VLANG_API static vlAstAxis * AppendSiblingNode(vlAstAxis * lhs,vlAstAxis * rhs);
	VLANG_API static vlAstAxis * AppendNextNode(vlAstAxis * lhs,	vlAstAxis * rhs);
	VLANG_API static vlAstAxis * AppendChildNode(vlAstAxis * lhs,vlAstAxis * rhs);

	VLANG_API static vlAstAxis * getSibling(vlAstAxis * lhs);
	VLANG_API static vlAstAxis * getNext(vlAstAxis * lhs);
	VLANG_API static vlAstAxis * getChild(vlAstAxis * lhs);
	VLANG_API static vlAstNode * getNode(vlAstAxis * lhs);
	VLANG_API static vlAstAxis * getParent(vlAstAxis * lhs);
	VLANG_API static vlAstAxis * LastChild(vlAstAxis * lhs);
	VLANG_API static vlAstAxis * ChildNext(vlAstAxis * lhs,int step);
	VLANG_API static vlAstAxis * Child(vlAstAxis * lhs,int step);
	VLANG_API static vlAstAxis * ChildSibling(vlAstAxis * lhs,int step); 

	//param
	VLANG_API static vlAstAxis * param2Number(vlAstAxis * t,int level = 2); 
public:
	//context
	void context_enter_scope(vlContext* context);
	void context_leave_scope(vlContext* context);
	int context_push_word(vlContext* context,int v,int id);
	int context_get_word(vlContext* context,int index);
	int context_find_token(vlContext* context,int ide);
	int context_find_next_token(vlContext* context,int index, int ide);
public:
	void *  makeVariantFrame(vlContext* context,Variant & v);
	int loadItemValue(vlContext * context,vlAstAxis * & arg,std::map<int,vlAstAxis*> & paramBind);
	vlAstAxis ** ensureItemAddr(vlContext * context,vlAstAxis * arg);
};

} //namespace xlang

#endif //VLAPPLY