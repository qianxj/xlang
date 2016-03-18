#include "wpast.hpp"

using namespace trap::complier;
using namespace coder;

astContext * coder::Context(){
	astContext * context = allocNode<astContext>();
	context->kind = kContext;
	return context;
}

astFunction * coder::Function(astType * type,const wchar_t* name){
	astFunction * func = allocNode<astFunction>();
	func->kind = kFunction;
	func->type = type;
	func->name = name;
	return func;
}

astParam * coder::Param(astType * type,const wchar_t* name, astNode * init){
	astParam * param = allocNode<astParam>();
	param->kind = kParam;
	param->type = type;
	param->name = name;
	param->init = init;
	return param;
}

astVariant * coder::Variant(astType * type,const wchar_t* name, astNode * init){
	astVariant * term = allocNode<astVariant>();
	term->kind = kVariant;
	term->type = type;
	term->name = name;
	term->init = init;
	return term;
}


astCall * coder::Call(astNode * func, astNode * args){
	astCall* call = allocNode<astCall>();
	call->kind = kCall;
	call->func = func;
	call->args = args;
	call->name = 0;
	return call;
}

astCall * coder::Call(const wchar_t* name, astNode * args){
	astCall* call = allocNode<astCall>();
	call->kind = kCall;
	call->func = 0;
	call->name = name;
	call->args = args;
	return call;
}

astReturn * coder::Return(astExpr * term){
	astReturn* ret = allocNode<astReturn>();
	ret->kind= kReturn;
	ret->expr = term;
	return ret;
}

astClass * coder::Class(const wchar_t* name){
	astClass * cls = allocNode<astClass>();
	cls->kind = kClass;
	cls->classKind = ckClass;
	cls->fields = 0;
	cls->methods = 0;
	cls->name = name;
	return cls;
}

astList * coder::List(astList* piror, astNode * term){
	astList * list = allocNode<astList>();
	list->kind = kList;
	list->next = 0;
	list->term = term;
	astList*  t = piror;
	while(t->next)t = t->next;
	t->next = list;
	return piror;
}

astList * coder::List(astNode * term){
	astList * list = allocNode<astList>();
	list->kind = kList;
	list->next = 0;
	list->term = term;
	return list;
}

astList * coder::List(){
	astList * list = allocNode<astList>();
	list->kind = kList;
	list->next = 0;
	list->term = 0;
	return list;
}

astExpr * coder::Expr(astNode * term){
	astExpr * expr = allocNode<astExpr>();
	expr->kind = kExpr;
	expr->node = term;
	return expr;
}

astLoop * coder::Loop(astNode * cond, astNode *term){
	astLoop * loop = allocNode<astLoop>();
	loop->kind = kLoop;
	loop->cond = cond;
	loop->stmt = term;
	return loop;
}
astBreak * coder::Break(){
	astBreak* break_ = allocNode<astBreak>();
	break_->kind = kBreak;
	return break_;
}
astContinue * coder::Continue(){
	astContinue* continue_ = allocNode<astContinue>();
	continue_->kind = kContinue;
	return continue_;

}
astUntil * coder::Until(astNode *term, astNode * cond){
	astUntil * until_ = allocNode<astUntil>();
	until_->kind = kUntil;
	until_->cond = cond;
	until_->stmt = term;
	return until_;
}

astBranch * coder::Branch(astExpr * expr, astList * items, astNode* defautStmt ){
	astBranch * branch = allocNode<astBranch>();
	branch->kind = kBranch;
	branch->expr = expr;
	branch->branchItemList = items;
	branch->defaultStmt = defautStmt;
	return branch;
}

astIf * coder::If(astExpr * cond, astNode * trueStmt, astNode * falseStmt )
{
	astIf * if_ = allocNode<astIf>();
	if_->kind = kIf;
	if_->expr = cond;
	if_->trueStmt = trueStmt;
	if_->falseStmt = falseStmt;
	return if_;
}

astOneof * coder::OneOf(astList * term){
	astOneof * oneof_ = allocNode<astOneof>();
	oneof_->kind = kOneof;
	oneof_->nodes = term;
	return oneof_;
}

astOption * coder::Option(astNode * term)
{
	astOption * option_ = allocNode<astOption>();
	option_->kind = kOption;
	option_->node = term;
	return option_;
}

astGuard * coder::Guard()
{
	astGuard * guard_ = allocNode<astGuard>();
	guard_->kind = kGuard;
	return guard_;
}

astMatch * coder::Match(astNode * term, astExpr * temp){
	astMatch * match_ = allocNode<astMatch>();
	match_->kind = kMatch;
	match_->term = term;
	match_->temp = temp;
	return match_;
}

astBranchItem* coder::BranchItem(astExpr * match , astNode * stmt){
	astBranchItem* branchItem = allocNode<astBranchItem>();
	branchItem->kind = kBranchItem;
	branchItem->cond = match;
	branchItem->stmt = stmt;
	return branchItem;
}

astBinary* coder::Binary(tokenKind_t opKind, astNode * lhs, astNode * rhs )
{
	astBinary * binary = allocNode<astBinary>();
	binary->kind = kBinary;
	binary->opKind = opKind;
	binary->rhs = rhs;
	binary->lhs = lhs;
	return binary;
}

astUnary* coder::Unary(tokenKind_t opKind, astNode * node)
{
	astUnary * unary = allocNode<astUnary>();
	unary->kind = kUnary;
	unary->opKind = opKind;
	unary->node = node;
	return unary;
}


astType * coder::ContextType(const wchar_t* name){
	return LookupContextObject<astType>(name);
}

astClass * coder::ContextClass(const wchar_t* name){
	return LookupContextObject<astClass>(name);
}

astParam * coder::ContextParam(const wchar_t* name){
	return LookupContextObject<astParam>(name);
}

astVariant * coder::ContextVariant(const wchar_t* name){
	return LookupContextObject<astVariant>(name);
}

astFunction * coder::ContextFunction(const wchar_t* name){
	return LookupContextObject<astFunction>(name);
}

astNode * coder::ContextTerm(const wchar_t* name){
	astNode * term =  ContextVariant(name);
	if(!term) term = ContextParam(name);
	term = allocNode<astTerm>();
	term->kind = kTerm;
	((astTerm *)term)->name = name;
	return term;
}

astCompStmt * coder::CompStmt(astNode* stmt)
{
	astCompStmt * term =  allocNode<astCompStmt>();
	term->kind = kCompStmt;
	term->stmt = stmt;
	return term;
}

//context
astNode * coder::Append(astNode * parent, astNode * term){return 0;}

//literal
astLiteral * coder::Literal(int val){
	astLiteral* term = allocNode<astLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkInt;
	term->val.i32 = val;
	return term;
}

astLiteral * coder::Literal(double val){
	astLiteral* term = allocNode<astLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkDouble;
	term->val.d64 = val;
	return term;
}

astLiteral * coder::Literal(const wchar_t* val){
	astLiteral* term = allocNode<astLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkString;
	term->val.str = val;
	return term;
}
astLiteral * coder::Literal(bool val){
	astLiteral* term = allocNode<astLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkBool;
	term->val.b = val;
	return term;
}
astLiteral * coder::Literal(wchar_t val){
	astLiteral* term = allocNode<astLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkChar;
	term->val.c16 = val;
	return term;
}
astLiteral * coder::Literal(char val){
	astLiteral* term = allocNode<astLiteral>();
	term->kind = kLiteral;
	term->literalKind = lkByte;
	term->val.c8 = val;
	return term;
}

astExpr * coder::Ref(astNode* expr)
{
	astExpr* term = allocNode<astExpr>();
	term->kind = kRef;
	term->node = expr;
	return term;
};

astTokenNode * coder::TokenNode(Token &tk)
{
	astTokenNode * term = allocNode<astTokenNode>();
	term->kind = kToken;
	term->tk = tk;
	return term;
}
