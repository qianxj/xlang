#include "wpcode.hpp"

using namespace xl::wpp::coder;

PContext * coder::Context(){
	PContext * context = allocNode<PContext>();
	context->kind = kContext;
	return context;
}

PFunction * coder::Function(PType * type,const wchar_t* name){
	PFunction * func = allocNode<PFunction>();
	func->kind = kFunction;
	func->type = type;
	func->name = name;
	return func;
}

PParam * coder::Param(PType * type,const wchar_t* name, PNode * init){
	PParam * param = allocNode<PParam>();
	param->kind = kParam;
	param->type = type;
	param->name = name;
	param->init = init;
	return param;
}

PVariant * coder::Variant(PType * type,const wchar_t* name, PNode * init){
	PVariant * term = allocNode<PVariant>();
	term->kind = kVariant;
	term->type = type;
	term->name = name;
	term->init = init;
	return term;
}


PCall * coder::Call(PNode * func, PNode * args){
	PCall* call = allocNode<PCall>();
	call->kind = kCall;
	call->func = func;
	call->args = args;
	call->name = 0;
	return call;
}

PCall * coder::Call(const wchar_t* name, PNode * args){
	PCall* call = allocNode<PCall>();
	call->kind = kCall;
	call->func = 0;
	call->name = name;
	call->args = args;
	return call;
}

PReturn * coder::Return(PExpr * term){
	PReturn* ret = allocNode<PReturn>();
	ret->kind= kReturn;
	ret->expr = term;
	return ret;
}

PClass * coder::Class(const wchar_t* name){
	PClass * cls = allocNode<PClass>();
	cls->kind = kClass;
	cls->classKind = ckClass;
	cls->fields = 0;
	cls->methods = 0;
	cls->name = name;
	return cls;
}

PList * coder::List(PList* piror, PNode * term){
	PList * list = allocNode<PList>();
	list->kind = kList;
	list->next = 0;
	list->term = term;
	PList*  t = piror;
	while(t->next)t = t->next;
	t->next = list;
	return piror;
}

PList * coder::List(PNode * term){
	PList * list = allocNode<PList>();
	list->kind = kList;
	list->next = 0;
	list->term = term;
	return list;
}

PList * coder::List(){
	PList * list = allocNode<PList>();
	list->kind = kList;
	list->next = 0;
	list->term = 0;
	return list;
}

PExpr * coder::Expr(PNode * term){
	PExpr * expr = allocNode<PExpr>();
	expr->kind = kExpr;
	expr->node = term;
	return expr;
}

PLoop * coder::Loop(PNode * cond, PNode *term){
	PLoop * loop = allocNode<PLoop>();
	loop->kind = kLoop;
	loop->cond = cond;
	loop->stmt = term;
	return loop;
}
PBreak * coder::Break(){
	PBreak* break_ = allocNode<PBreak>();
	break_->kind = kBreak;
	return break_;
}
PContinue * coder::Continue(){
	PContinue* continue_ = allocNode<PContinue>();
	continue_->kind = kContinue;
	return continue_;

}
PUntil * coder::Until(PNode *term, PNode * cond){
	PUntil * until_ = allocNode<PUntil>();
	until_->kind = kUntil;
	until_->cond = cond;
	until_->stmt = term;
	return until_;
}

PBranch * coder::Branch(PExpr * expr, PList * items, PNode* defautStmt ){
	PBranch * branch = allocNode<PBranch>();
	branch->kind = kBranch;
	branch->expr = expr;
	branch->branchItemList = items;
	branch->defaultStmt = defautStmt;
	return branch;
}

PIf * coder::If(PExpr * cond, PNode * trueStmt, PNode * falseStmt )
{
	PIf * if_ = allocNode<PIf>();
	if_->kind = kIf;
	if_->expr = cond;
	if_->trueStmt = trueStmt;
	if_->falseStmt = falseStmt;
	return if_;
}

POneof * coder::OneOf(PList * term){
	POneof * oneof_ = allocNode<POneof>();
	oneof_->kind = kOneof;
	oneof_->nodes = term;
	return oneof_;
}

POption * coder::Option(PNode * term)
{
	POption * option_ = allocNode<POption>();
	option_->kind = kOption;
	option_->node = term;
	return option_;
}

PGuard * coder::Guard()
{
	PGuard * guard_ = allocNode<PGuard>();
	guard_->kind = kGuard;
	return guard_;
}

PMatch * coder::Match(PNode * term, PExpr * temp){
	PMatch * match_ = allocNode<PMatch>();
	match_->kind = kMatch;
	match_->term = term;
	match_->temp = temp;
	return match_;
}

PBranchItem* coder::BranchItem(PExpr * match , PNode * stmt){
	PBranchItem* branchItem = allocNode<PBranchItem>();
	branchItem->kind = kBranchItem;
	branchItem->cond = match;
	branchItem->stmt = stmt;
	return branchItem;
}

PBinary* coder::Binary(tokenKind_t opKind, PNode * lhs, PNode * rhs )
{
	PBinary * binary = allocNode<PBinary>();
	binary->kind = kBinary;
	binary->opKind = opKind;
	binary->rhs = rhs;
	binary->lhs = lhs;
	return binary;
}

PUnary* coder::Unary(tokenKind_t opKind, PNode * node)
{
	PUnary * unary = allocNode<PUnary>();
	unary->kind = kUnary;
	unary->opKind = opKind;
	unary->node = node;
	return unary;
}


PType * coder::ContextType(const wchar_t* name){
	return LookupContextObject<PType>(name);
}

PClass * coder::ContextClass(const wchar_t* name){
	return LookupContextObject<PClass>(name);
}

PParam * coder::ContextParam(const wchar_t* name){
	return LookupContextObject<PParam>(name);
}

PVariant * coder::ContextVariant(const wchar_t* name){
	return LookupContextObject<PVariant>(name);
}

PFunction * coder::ContextFunction(const wchar_t* name){
	return LookupContextObject<PFunction>(name);
}

PNode * coder::ContextTerm(const wchar_t* name){
	PNode * term =  ContextVariant(name);
	if(!term) term = ContextParam(name);
	term = allocNode<PTerm>();
	term->kind = kTerm;
	((PTerm *)term)->name = name;
	return term;
}

PCompStmt * coder::CompStmt(PNode* stmt)
{
	PCompStmt * term =  allocNode<PCompStmt>();
	term->kind = kCompStmt;
	term->stmt = stmt;
	return term;
}

//context
PNode * coder::Append(PNode * parent, PNode * term){return 0;}

//literal
PLiteral * coder::Literal(int val){
	PLiteral* term = allocNode<PLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkInt;
	term->val.i32 = val;
	return term;
}

PLiteral * coder::Literal(double val){
	PLiteral* term = allocNode<PLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkDouble;
	term->val.d64 = val;
	return term;
}

PLiteral * coder::Literal(const wchar_t* val){
	PLiteral* term = allocNode<PLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkString;
	term->val.str = val;
	return term;
}
PLiteral * coder::Literal(bool val){
	PLiteral* term = allocNode<PLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkBool;
	term->val.b = val;
	return term;
}
PLiteral * coder::Literal(wchar_t val){
	PLiteral* term = allocNode<PLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkChar;
	term->val.c16 = val;
	return term;
}
PLiteral * coder::Literal(char val){
	PLiteral* term = allocNode<PLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkByte;
	term->val.c8 = val;
	return term;
}

PExpr * coder::Ref(PNode* expr)
{
	PExpr* term = allocNode<PExpr>();
	term->kind = kRef;
	term->node = expr;
	return term;
};

PTokenNode * coder::TokenNode(Token &tk)
{
	PTokenNode * term = allocNode<PTokenNode>();
	term->kind = kToken;
	term->tk = tk;
	return term;
}
