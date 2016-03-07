#include "vlpreparse.hpp"
#include "machine_x86.hpp"
#include "vlcoder.hpp"
#include "vlole.hpp"

using namespace XM;
using namespace xlang;

namespace trap {
namespace complier {

extern  std::vector<xlang::HNamespace> namespaces;
extern  std::vector<xlang::HType> classes;
extern  std::vector<xlang::HMethod> methodes;
extern  vlErrorSvr errorMgr;


PCode::Oprand* genLValue(TContext & context,TNode* expr, PCode::Block* block,int & tVarIndex)
{
	switch(expr->kind)
	{
	case kItem:
		return PCode::tItem((OpItem*)expr);
	case kField:
		{
			PCode::Oprand * opr = genRValue(context,((OpField*)expr)->base,block,tVarIndex);
			return PCode::Field(opr,((OpField*)expr)->hField);
		}
	case kElement:
		{
			int msize = 0;
			return PCode::Element(
				genRValue(context,((OpElement*)expr)->exp,block,tVarIndex),
				genRValue(context,((OpElement*)expr)->rank,block,tVarIndex),
				context.trap->getAllocSize(expr->hType,msize));
		}
	default:
		assert(false);
		return 0;
	}
	return 0;
}

bool OprandUsedTmp(PCode::Oprand * opr)
{
	if(opr->cd==PCode::cItem && ((PCode::Item*)opr)->tempId)return true;
	else if(opr->cd==PCode::cFIELD && ((PCode::FIELD*)opr)->base->cd==PCode::cItem &&
			((PCode::Item*)((PCode::FIELD*)opr)->base)->tempId)return true;
	else if(opr->cd==PCode::cELEMENT && ((PCode::ELEMENT*)opr)->base->cd==PCode::cItem &&
			((PCode::Item*)((PCode::ELEMENT*)opr)->base)->tempId)return true;
	return false;

}
PCode::Oprand* genTmpLet(PCode::Oprand * opr, PCode::Block* block,int & tVarIndex)
{
	int tcount = OprandUsedTmp(opr)?1:0;
	tVarIndex += 1 - tcount;
	PCode::Oprand* tmp = PCode::tTmp(tVarIndex);
	*block += PCode::Let(tmp,opr);
	return tmp;	
}

PCode::Oprand* genRValue(TContext & context,TNode* expr, PCode::Block* block,int & tVarIndex)
{
	switch(expr->kind)
	{
	//primary
	case kThis:
		return PCode::tThis();
	case kSuper:
		assert(false);
		return 0;
	case kValue:
		return PCode::tImm((OpValue*)expr);
	case kNill:
		return PCode::tNill();
	case kItem:
		return PCode::tItem((OpItem*)expr);
	//postfix
	case kInc:
		{
			PCode::Oprand* opr = genLValue(context,((OpInc*)expr)->expr,block,tVarIndex);
			PCode::Oprand* tmp = genTmpLet(opr,block,tVarIndex);
			*block += PCode::Inc(opr);
			return tmp;
		}
	case kDec:
		{
			PCode::Oprand* opr = genLValue(context,((OpDec*)expr)->expr,block,tVarIndex);
			PCode::Oprand* tmp = genTmpLet(opr,block,tVarIndex);
			*block += PCode::Dec(opr);
			return tmp;
		}
	case kField:
		{
			PCode::Oprand * opr = genRValue(context,((OpField*)expr)->base,block,tVarIndex);
			return genTmpLet(PCode::Field(opr,((OpField*)expr)->hField),block,tVarIndex);
		}
	case kElement:
		{
			int msize = 0;
			return PCode::Element(
				genRValue(context,((OpElement*)expr)->exp,block,tVarIndex),
				genRValue(context,((OpElement*)expr)->rank,block,tVarIndex),
				context.trap->getAllocSize(expr->hType,msize));
		}
	case kCall:
		{
			TNode* mt = ((OpCall*)expr)->mt;
			std::vector<TNode*> terms;
			HMethod hMethod = mt->hType;

			if(((OpCall*)expr)->args)
			{
				if(((OpCall*)expr)->args->kind!=kList)
				{
					terms.push_back(((OpCall*)expr)->args);
				}else
				{
					TNodeList* args = (TNodeList*)((OpCall*)expr)->args;
					while(true)
					{
						if(args->node && args->node->kind!=kEmpty)terms.push_back(args->node);
						if(!args->next) break;
						args = args->next;
					}
				}
			}
			PCode::Block* argBlock = PCode::tBlock();
			for(int i= (int)terms.size() - 1;i>=0;i--)
			{
				HParam hParam = context.trap->getMethodParamHandleByIndex(hMethod,i);
				unsigned int spec = context.trap->getParam(hParam)->spec;
				int tIndex = tVarIndex;
				if(spec & ParamSpec::byref_)
				{
					*argBlock += PCode::Push(genLValue(context,terms[i],argBlock,tIndex),hParam);
				}else
				{
					*argBlock += PCode::Push(genRValue(context,terms[i],argBlock,tIndex),hParam);
				}
			}

			if(mt->kind==kItem)
			{
				//直接函数调用，也可能为成员调用
				xlang::Handle hTerm = ((OpItem*)mt)->hTerm;
				return genTmpLet(PCode::Call(0,0,hTerm,terms.size(),argBlock),block,tVarIndex); 
			}else if(mt->kind==kField)
			{
				//成员调用
				OpField * exp =  (OpField*)mt;
				return genTmpLet(PCode::Call(genRValue(context,exp->base,block,tVarIndex),0,exp->hField,terms.size(),argBlock),block,tVarIndex);
			}else
			{
				//函数指针
				return genTmpLet(PCode::Call(0,genRValue(context,mt,block,tVarIndex),terms.size(),argBlock),block,tVarIndex);
			}
		}
	//unary
	case kUInc:
		{
			PCode::Oprand* opr = genLValue(context,((OpUInc*)expr)->expr,block,tVarIndex);
			*block += PCode::UInc(opr);
			return block;
		}
	case kUDec:
		{
			PCode::Oprand* opr = genLValue(context,((OpUDec*)expr)->expr,block,tVarIndex);
			*block += PCode::UDec(opr);
			return block;
		}
	case kNeg:
		return genTmpLet(PCode::Neg(genRValue(context,((OpNeg*)expr)->expr,block,tVarIndex)),block,tVarIndex);
	case kNew:
		return genTmpLet( PCode::New(((OpNew*)expr)->typ->hType,
			((OpNew*)expr)->rank? genRValue(context,((OpNew*)expr)->rank,block,tVarIndex):0),block,tVarIndex);
	case kDelete:
		return PCode::Del(0,genRValue(context,((OpDelete*)expr)->exp,block,tVarIndex));
	case kNot:
		return genTmpLet(PCode::Not(0,genRValue(context,((OpNot*)expr)->expr,block,tVarIndex)),block,tVarIndex);
	case kBNot:
		return genTmpLet(PCode::Bnot(0,genRValue(context,((OpBNot*)expr)->expr,block,tVarIndex)),block,tVarIndex);
	//cast
	case kCast:
		return genTmpLet(PCode::Cast(expr->hType,((OpCast*)expr)->expr->hType,genRValue(context,((OpCast*)expr)->expr,block,tVarIndex)),block,tVarIndex);
	case kEmpty:
		return 0;
	//binary
	case kBinary:
		{
			OpBinary* term = (OpBinary*) expr;
			PCode::Oprand* cterm = 0;
			PCode::Oprand* rhs = genRValue(context,term->rhs,block,tVarIndex);
			if(term->op != OpBinary::asn)
			{
				PCode::Oprand* lhs = genRValue(context,term->lhs,block,tVarIndex);
				int tcount = 0;
				tcount += OprandUsedTmp(lhs)?1:0;
				tcount += OprandUsedTmp(rhs)?1:0;
				tVarIndex += 1 - tcount; 
				assert(tVarIndex > 0);
				cterm = PCode::Let(PCode::tTmp(tVarIndex),term->op, lhs,rhs);
				*block += cterm; 
			}else
			{
				PCode::Oprand* lhs = genLValue(context,term->lhs,block,tVarIndex);
				cterm = PCode::Let(lhs,rhs);
				if(rhs->cd==PCode::cItem && ((PCode::Item*)rhs)->tempId && block->stmts.size())
				{
					if(block->stmts.back()->cd==PCode::cLET && ((PCode::LET*)block->stmts.back())->dest->cd==PCode::cItem &&
						((PCode::Item*)((PCode::LET*)block->stmts.back())->dest)->tempId== ((PCode::Item*)rhs)->tempId)
					{
						((PCode::LET*)block->stmts.back())->dest = lhs;
					}
					else
						*block += cterm;
				}else
					*block += cterm;
			}
			return ((PCode::LET*)cterm)->dest;
		}
	default:
		assert(false);
		return 0;
	}
	return 0;
}

PCode::Oprand* genExpr(TContext & context,TNode* expr, PCode::Block* block,int & tVarIndex)
{
	return genRValue(context, expr,block,tVarIndex);
	/*
	switch(expr->kind)
	{
	//primary
	case kThis:
		return PCode::tThis();
	case kSuper:
		assert(false);
		return 0;
	case kValue:
		return PCode::tImm((OpValue*)expr);
	case kNill:
		return PCode::tNill();
	case kItem:
		return PCode::tItem((OpItem*)expr);
	//postfix
	case kInc:
		{
			int tcount = 0;
			PCode::Oprand* opr = genExpr(context,((OpInc*)expr)->expr,block,tVarIndex);
			if(opr->cd==PCode::cItem && ((PCode::Item*)opr)->tempId)tcount++;
			tVarIndex += 1 - tcount;
			PCode::Oprand* tmp = PCode::tTmp(tVarIndex);
			*block += PCode::Let(tmp,opr);
			*block += PCode::UInc(opr);
			return tmp;
		}
	case kDec:
		{
			int tcount = 0;
			PCode::Oprand* opr = genExpr(context,((OpDec*)expr)->expr,block,tVarIndex);
			if(opr->cd==PCode::cItem && ((PCode::Item*)opr)->tempId)tcount++;
			tVarIndex += 1 - tcount;
			PCode::Oprand* tmp = PCode::tTmp(tVarIndex);
			*block += PCode::Let(tmp,opr);
			*block += PCode::UDec(opr);
			return tmp;
		}
	case kField:
		{
			PCode::Oprand * opr = genExpr(context,((OpField*)expr)->base,block,tVarIndex);
			if(opr->cd==PCode::cFIELD )
			{
				int tcount = 0;
				if(((PCode::FIELD*)opr)->base->cd==PCode::cItem &&
						((PCode::Item*)((PCode::FIELD*)opr)->base)->tempId)tcount++;
				tVarIndex += 1 - tcount;
				*block += PCode::Let(PCode::tTmp(tVarIndex),opr);
				return PCode::Field(PCode::tTmp(tVarIndex),((OpField*)expr)->hField);
				
			}
			else if(opr->cd==PCode::cELEMENT)
			{
				int tcount = 0;
				if(((PCode::ELEMENT*)opr)->base->cd==PCode::cItem &&
						((PCode::Item*)((PCode::ELEMENT*)opr)->base)->tempId)tcount++;
				if(((PCode::ELEMENT*)opr)->rank->cd==PCode::cItem &&
						((PCode::Item*)((PCode::ELEMENT*)opr)->base)->tempId)tcount++;
				tVarIndex += 1 - tcount;
				*block += PCode::Let(PCode::tTmp(tVarIndex),opr);
				return PCode::Field(PCode::tTmp(tVarIndex),((OpField*)expr)->hField);
			}
			else
				return PCode::Field(genExpr(context,((OpField*)expr)->base,block,tVarIndex),((OpField*)expr)->hField);
		}
	case kElement:
		return PCode::Element(
			genExpr(context,((OpElement*)expr)->exp,block,tVarIndex),
			genExpr(context,((OpElement*)expr)->rank,block,tVarIndex));
	case kCall:
		{
			TNode* mt = ((OpCall*)expr)->mt;
			std::vector<TNode*> terms;
			if(((OpCall*)expr)->args)
			{
				if(((OpCall*)expr)->args->kind!=kList)
				{
					terms.push_back(((OpCall*)expr)->args);
				}else
				{
					TNodeList* args = (TNodeList*)((OpCall*)expr)->args;
					while(true)
					{
						if(args->node && args->node->kind!=kEmpty)terms.push_back(args->node);
						if(!args->next) break;
						args = args->next;
					}
				}
			}
			for(int i= (int)terms.size() - 1;i>=0;i--)
			{
				*block += PCode::Push(genExpr(context,terms[i],block,tVarIndex));
			}
			if(mt->kind==kItem)
			{
				xlang::Handle hTerm = ((OpItem*)mt)->hTerm;
				return PCode::Call(0,0,hTerm); 
			}else if(mt->kind==kField)
			{
				OpField * exp =  (OpField*)mt;
				return PCode::Call(genExpr(context,exp->base,block,tVarIndex),0,exp->hField);
			}else
			{
				return PCode::Call(0,genExpr(context,mt,block,tVarIndex));
			}
		}
	//unary
	case kUInc:
		{
			PCode::Oprand* opr = genExpr(context,((OpUInc*)expr)->expr,block,tVarIndex);
			*block += PCode::UInc(opr);
			return opr;
		}
	case kUDec:
		{
			PCode::Oprand* opr = genExpr(context,((OpUDec*)expr)->expr,block,tVarIndex);
			*block += PCode::UDec(opr);
			return opr;
		}
	case kNeg:
		{
		return PCode::Neg(genExpr(context,((OpNeg*)expr)->expr,block,tVarIndex));
		}
	case kNew:
		return PCode::New(((OpNew*)expr)->typ->hType,
			((OpNew*)expr)->rank? genExpr(context,((OpNew*)expr)->rank,block,tVarIndex):0);
	case kDelete:
		return PCode::Del(0,genExpr(context,((OpDelete*)expr)->exp,block,tVarIndex));
	case kNot:
		return PCode::Not(0,genExpr(context,((OpNot*)expr)->expr,block,tVarIndex));
	case kBNot:
		return PCode::Bnot(0,genExpr(context,((OpBNot*)expr)->expr,block,tVarIndex));
	//cast
	case kCast:
		//PCode::Cast(
		return PCode::Cast(expr->hType,((OpCast*)expr)->expr->hType,genExpr(context,((OpCast*)expr)->expr,block,tVarIndex));
	case kEmpty:
		return 0;
	//binary
	case kBinary:
		{
			OpBinary* term = (OpBinary*) expr;
			PCode::Oprand* cterm = 0;
			PCode::Oprand* rhs = genExpr(context,term->rhs,block,tVarIndex);
			PCode::Oprand* lhs = genExpr(context,term->lhs,block,tVarIndex);
			if(term->op != OpBinary::asn)
			{
				int tcount = 0;
				if(lhs->cd==PCode::cItem && ((PCode::Item*)lhs)->tempId)tcount++;
				if(rhs->cd==PCode::cItem && ((PCode::Item*)rhs)->tempId)tcount++;
				
				if(lhs->cd==PCode::cFIELD && ((PCode::FIELD*)lhs)->base->cd==PCode::cItem &&
						((PCode::Item*)((PCode::FIELD*)lhs)->base)->tempId)tcount++;
				if(rhs->cd==PCode::cFIELD && ((PCode::FIELD*)rhs)->base->cd==PCode::cItem &&
						((PCode::Item*)((PCode::FIELD*)rhs)->base)->tempId)tcount++;
				
				if(lhs->cd==PCode::cELEMENT && ((PCode::ELEMENT*)lhs)->base->cd==PCode::cItem &&
						((PCode::Item*)((PCode::ELEMENT*)lhs)->base)->tempId)tcount++;
				if(rhs->cd==PCode::cELEMENT && ((PCode::ELEMENT*)rhs)->base->cd==PCode::cItem &&
						((PCode::Item*)((PCode::ELEMENT*)rhs)->base)->tempId)tcount++;
				if(lhs->cd==PCode::cELEMENT && ((PCode::ELEMENT*)lhs)->rank->cd==PCode::cItem &&
						((PCode::Item*)((PCode::ELEMENT*)lhs)->base)->tempId)tcount++;
				if(rhs->cd==PCode::cELEMENT && ((PCode::ELEMENT*)rhs)->rank->cd==PCode::cItem &&
						((PCode::Item*)((PCode::ELEMENT*)rhs)->rank)->tempId)tcount++;

				tVarIndex += 1 - tcount; 
				assert(tVarIndex > 0);
				cterm = PCode::Let(PCode::tTmp(tVarIndex),term->op, lhs,rhs);
				*block += cterm; 
			}else
			{
				cterm = PCode::Let(lhs,rhs);
				if(rhs->cd==PCode::cItem && ((PCode::Item*)rhs)->tempId && block->stmts.size())
				{
					if(block->stmts.back()->cd==PCode::cLET && ((PCode::LET*)block->stmts.back())->dest->cd==PCode::cItem &&
						((PCode::Item*)((PCode::LET*)block->stmts.back())->dest)->tempId== ((PCode::Item*)rhs)->tempId)
					{
						((PCode::LET*)block->stmts.back())->dest = lhs;
					}
					else
						*block += cterm;
				}else
					*block += cterm;
			}
			return ((PCode::LET*)cterm)->dest;
		}
	default:
		assert(false);
		return 0;
	}*/
}

PCode::Oprand * genExpr(TContext & context,TNode* expr, PCode::Block* block)
{
	int tVarIndex = 0;
	PCode::Oprand * node =  genExpr(context,expr,block,tVarIndex);
	//if(expr->kind != kBinary) *block += node;
	return node;   
}

PCode::Oprand * genCondExpr(TContext & context,TNode* expr,PCode::Block* block, PCode::Block* tblock,PCode::Block* fblock,int & tVarIndex)
{
	switch(expr->kind)
	{
	case kNot:
		genCondExpr(context,((OpNot*)expr)->expr,block,fblock,tblock,tVarIndex);
		break;
	case kBinary:
		{
			OpBinary* term = (OpBinary*)expr;
			switch(term->op)
			{
			case OpBinary::or:
				{
					PCode::Block* tmpblock = PCode::tBlock();
					genCondExpr(context,term->lhs,block,tblock,tmpblock,tVarIndex);
					genCondExpr(context,term->rhs,tmpblock,tblock,fblock,tVarIndex);
				}
				break;
			case OpBinary::and: 
				{
					PCode::Block* tmpblock = PCode::tBlock();
					genCondExpr(context,term->lhs,block,tmpblock,fblock,tVarIndex);
					genCondExpr(context,term->rhs,tmpblock,tblock,fblock,tVarIndex);
				}
				break; 
			case OpBinary::eq:
			case OpBinary::neq:
			case OpBinary::lt:
			case OpBinary::gt:
			case OpBinary::lte:
			case OpBinary::gte:
				{
					PCode::Oprand* lhs = genExpr(context,term->lhs,block,tVarIndex);
					PCode::Oprand* rhs = genExpr(context,term->rhs,block,tVarIndex);
					switch(term->op)
					{
						case OpBinary::eq:
							*block+=PCode::Jcc(PCode::Eq(0,lhs,rhs),tblock,fblock);
							break;
						case OpBinary::neq:
							*block+=PCode::Jcc(PCode::Neq(0,lhs,rhs),tblock,fblock);
							break;
						case OpBinary::gt:
							*block+=PCode::Jcc(PCode::Gt(0,lhs,rhs),tblock,fblock);
							break;
						case OpBinary::gte:
							*block+=PCode::Jcc(PCode::Gte(0,lhs,rhs),tblock,fblock);
							break;
						case OpBinary::lt:
							*block+=PCode::Jcc(PCode::Lt(0,lhs,rhs),tblock,fblock);
							break;
						case OpBinary::lte:
							*block+=PCode::Jcc(PCode::Lte(0,lhs,rhs),tblock,fblock);
							break;
					}
				}
				break;
			default:
				PCode::Oprand* opr = genExpr(context,expr,block,tVarIndex);
				*block += PCode::Jcc(PCode::Neq(0,opr,PCode::tImm(0)),tblock,fblock);
				break;
			}			
		}
		break;
	default:
		PCode::Oprand* opr = genExpr(context,expr,block,tVarIndex);
		*block += PCode::Jcc(PCode::Neq(0,opr,PCode::tImm(0)),tblock,fblock);
		break;
	}
	return 0;
}

PCode::Oprand * genCondExpr(TContext & context,TNode* expr,PCode::Block* block, PCode::Block* tblock,PCode::Block* fblock)
{
	int tVarIndex = 0;
	return genCondExpr(context,expr,block,tblock,fblock,tVarIndex);
}

int  EnsureBlock(std::vector<PCode::Block*> & blocks, PCode::Block * block)
{
	for(int i=0; i<(int)blocks.size(); i++)
	{
		if(blocks[i]==block) return i + 1;
	}
	blocks.push_back(block);
	return (int)blocks.size();
}

PCode::Oprand* genStmt(TContext & context,TNode* stmt,PCode::Block* & block,
			 PCode::Block* continueBlock,PCode::Block* breakBlock,PCode::Block* exitBlock)
{
	TNode* node = stmt;
	if(!node) return 0;

	switch(node->kind)
	{
	case kIf:
		{
			OpIf * term = (OpIf*)node;
			PCode::Block* tblock = PCode::tBlock();
			PCode::Block* fblock = PCode::tBlock();
			PCode::Block * bellow = PCode::tBlock();

			genCondExpr(context,term->cond,block,tblock,term->fbody?fblock:bellow);
			if(term->tbody)genStmt(context,term->tbody,tblock,continueBlock,breakBlock,exitBlock);
			*tblock += PCode::Jmp(bellow);
			if(term->fbody)
			{
				genStmt(context,term->fbody,fblock,continueBlock,breakBlock,exitBlock);
				*fblock += PCode::Jmp(bellow);
			}
			block = bellow;
		}
		break;
	case kFor:
		{
			OpFor * term = (OpFor*)node;
			if(term->init)
			{
				if(term->init->kind==kVar || term->init->kind==kList)
					genStmt(context,term->init,block,continueBlock,breakBlock,exitBlock); 
				else
					genExpr(context,term->init,block);
			}

			PCode::Block * body = PCode::tBlock();
			PCode::Block * bellow = PCode::tBlock();
			PCode::Block * cond = PCode::tBlock();

			*block += PCode::Jmp(cond);

			if(term->cond)
				genCondExpr(context,term->cond,cond,body,bellow);

			genStmt(context,term->body,body,cond,bellow,exitBlock);
			if(term->inc)genExpr(context,term->inc,body);
			*body += PCode::Jmp(term->cond?cond:body);
			block = bellow;
		}
		break;
	case kWhile:
		{
			OpWhile * term = (OpWhile*)node;

			PCode::Block * body = PCode::tBlock();
			PCode::Block * cond = block->stmts.size()?PCode::tBlock():block;
			PCode::Block * bellow = PCode::tBlock();

			if(block!=cond)*block += PCode::Jmp(cond);
			genCondExpr(context,term->cond,cond,body,bellow);
			genStmt(context,term->body,body,cond,bellow,exitBlock);
			*body += PCode::Jmp(cond);

			block = bellow;
		}
		break;
	case kUntil:
		{
			OpUntil * term = (OpUntil*)node;

			PCode::Block * body =  block->stmts.size()? PCode::tBlock():block;
			PCode::Block * bellow = PCode::tBlock();

			if(block!=body)*block += PCode::Jmp(body);
			genStmt(context,term->body,body,body,bellow,exitBlock);
			genCondExpr(context,term->cond,body,body,bellow);
			
			block = bellow;	
		}
		break;
	case kReturn:
		{
			OpReturn * term = (OpReturn*)node;
			if(term->expr)
			{
				*block += PCode::VRet(genExpr(context,term->expr,block));
			}
			*block += PCode::Jmp(exitBlock);
		}
		break;
	case kContinue:
		*block += PCode::Jmp(continueBlock);
		break;
	case kBreak:
		*block += PCode::Jmp(breakBlock);
		break;
	case kCompStmt:
		{
			OpCompStmt * term = (OpCompStmt*)node;
			genStmt(context,term->stmts,block,continueBlock,breakBlock,exitBlock);
		}
		break;
	case kExprStmt:
		{
			OpExprStmt* term = (OpExprStmt*)node;
			if(term->expr)
				genExpr(context,term->expr,block);
		}
		break;
	case kVar:
		{
			OpVar*	term = (OpVar *) node;
			if(term->expr)
				genExpr(context,opBinary(0,OpBinary::asn,opItem(term->hVariant),term->expr),block);
		}
		break;
	case kList:
		{
			OpList * terms = (OpList *)node;
			while(terms)
			{
				if(terms->node)genStmt(context,terms->node,block,continueBlock,breakBlock,exitBlock);
				terms = terms->next;
			}
		}
		break;
	}
	return 0;
}

std::wstring TraceNode(TContext &context,PCode::Oprand* stmt,std::wstring  space)
{
	if(!stmt) return L"";
	std::wstring str;
	switch(stmt->cd)
	{
	case PCode::cItem:
		{
			PCode::Item * item = (PCode::Item*)stmt;
			xlang::Handle hItem = item->hTerm;
			if(item->tempId)
			{
				wchar_t buf[3];
				if(item->tempId>9)
				{
					buf[2] = 0;
					buf[1] = '0' + (item->tempId - 1) % 10;
					buf[0] = '0' + (item->tempId  - 1)/10;
				}else
				{
					buf[1] = 0;
					buf[0] = '0' + (item->tempId -1)% 10;
				}
				str += (std::wstring)L"t" + buf;
			}
			if(hItem.kind==skVariant)
				str += util::getHashString(context.trap->getVariant(hItem)->name);
			else if(hItem.kind==skParam)
				str += util::getHashString(context.trap->getParam(hItem)->name);
			else if(hItem.kind==skField)
				str += util::getHashString(context.trap->getField(hItem)->name);
			else if(hItem.kind==skMethod)
				str += util::getHashString(context.trap->getMethod(hItem)->name);
			else if(hItem)
				str += L"#v";
			break;
		}
	case PCode::cImm:
		{
			PCode::Imm * term = (PCode::Imm*)stmt;
			wchar_t buf[80];
			switch(term->vt)
			{
			case PCode::Imm::I32:
			case PCode::Imm::UI32:
				_ltow_s(term->val_i32,buf,80,10);
				str += buf;
				break;
			case PCode::Imm::D32:
			case PCode::Imm::D64:
				{
					swprintf_s(buf,80,L"%f",term->val_d64);
					str += buf;
				}
				break;
			case PCode::Imm::BOOL:
				str += term->val_bool?L"true":L"false";
				break;
			case PCode::Imm::STR:
				str += (std::wstring)L"\"" + term->val_wstr + L"\"";
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case PCode::cNill:
		str += L"nill";
		break;
	case PCode::cThis:
		str += L"this";
		break;
	//case PCode::Oprand::kTmp:
	//	break;
	//case PCode::Oprand::kVariant:
	//	break;
	//case PCode::Oprand::kParam:
	//	break;
	//case PCode::Oprand::kField:
	//	break;
	case PCode::cVRET:
		str += space + L"#result" +L" "+ TraceNode(context,((PCode::VRET*)stmt)->opr);
		break;
	case PCode::cJCC:
		str += space + L"jcc";
		break;
	case PCode::cJMP:
		str += space + L"jmp";
		break;
	case PCode::cLDA:
		str += space + L"lda";
		break;
	case PCode::cLD:
		str += space + L"ld";
		break;
	case PCode::cST:
		str += space + L"st";
		break;
	case PCode::cADD:
		str += space + L"add";
		break;
	case PCode::cSUB:
		str += space + L"sub";
		break;
	case PCode::cMUL:
		str += space + L"mul";
		break;
	case PCode::cDIV:
		str += space + L"div";
		break;
	case PCode::cSHR:
		str += space + L"shr";
		break;
	case PCode::cSHL:
		str += space + L"shl";
		break;
	case PCode::cXOR:
		str += space + L"xor";
		break;
	case PCode::cSADD:
		str += space + L"sadd";
		break;
	case PCode::cBOR:
		str += space + L"bor";
		break;
	case PCode::cBAND:
		str += space + L"band";
		break;
	case PCode::cBNOT:
		str += space + L"bnot";
		break;

	case PCode::cNOP:
		str += space + L"nop";
		break;
	case PCode::cBR:
		str += space + L"br";
		break;

	case PCode::cRET:
		str += space + L"ret";
		break;
	case PCode::cLET:
		{
			PCode::LET* term = (PCode::LET*)stmt;
			if(term->op==OpBinary::nil)
			{
				if(term->lhs->cd==PCode::cCALL && ((PCode::CALL*)term->lhs)->args)
				{
					PCode::CALL* cterm = (PCode::CALL*)term->lhs;
					if(cterm->args)
					{
						std::vector<PCode::Oprand*>& stmts = cterm->args->stmts;
						for(int i=0;i<(int)stmts.size();i++)
						{
							str += space + TraceNode(context,stmts[i],i==0?L"":L"\n\t");
						}
						str += space+ L"\n\t";
					}
				}
				str += space + TraceNode(context,term->dest) +L" = " 
					+ TraceNode(context,term->lhs);
			}else
			{
				str += space + TraceNode(context,term->dest) +L" = " 
					+ TraceNode(context,term->lhs) 
					+ L" " + opValue::getOperateText(term->op) + L" " 
					+ TraceNode(context,term->rhs);
			}
			break;
		}
	case PCode::cCALL:
		{
			PCode::CALL* term = (PCode::CALL*)stmt;
			if(term->hTerm)
			{
				str += space + L"call " + 
					(term->hTerm.kind==skMethod?(std::wstring)context.trap->getTermName(context.trap->getParent(term->hTerm)) + L"::":L"")+
					context.trap->getTermName(term->hTerm);
			}
			else 
				str += space + L"call " + TraceNode(context,term->opr) ;
		}
		break;
	case PCode::cVCALL:
		{
			PCode::VCALL* term = (PCode::VCALL*)stmt;
			str += space + L"vcall " + TraceNode(context,term->opr) ;
		}
		break;
	case PCode::cNCALL:
		{
			PCode::NCALL* term = (PCode::NCALL*)stmt;
			str += space + L"ncall " + TraceNode(context,term->opr) ;
		}
		break;
	case PCode::cPUSH:
		str += space + L"push " + TraceNode(context,((PCode::PUSH*)stmt)->opr);
		break;
	case PCode::cPOP:
		str += space + L"pop" + TraceNode(context,((PCode::POP*)stmt)->opr);
		break;

	case PCode::cBIND:
		str += space + L"bind";
		break;
	case PCode::cUNBIND:
		str += space + L"unbind";
		break;
	case PCode::cSPILL:
		str += space + L"spill";
		break;

	case PCode::cNEQ:
		str += (((PCode::NEQ*)stmt)->dest? space + TraceNode(context,((PCode::NEQ*)stmt)->dest)+L" = ":L"")
			+ TraceNode(context,((PCode::NEQ*)stmt)->lhs) 
			+ L" != "
			+ TraceNode(context,((PCode::NEQ*)stmt)->rhs);
		break;
	case PCode::cEQ:
		str += (((PCode::EQ*)stmt)->dest? space + TraceNode(context,((PCode::EQ*)stmt)->dest)+L" = ":L"")
			+ TraceNode(context,((PCode::EQ*)stmt)->lhs) 
			+ L" == "
			+ TraceNode(context,((PCode::EQ*)stmt)->rhs);
		break;
	case PCode::cLT:
		str += (((PCode::LT*)stmt)->dest? space + TraceNode(context,((PCode::LT*)stmt)->dest)+L" = ":L"")
			+ TraceNode(context,((PCode::LT*)stmt)->lhs) 
			+ L" < "
			+ TraceNode(context,((PCode::LT*)stmt)->rhs);
		break;
	case PCode::cLTE:
		str += (((PCode::LTE*)stmt)->dest? space + TraceNode(context,((PCode::LTE*)stmt)->dest)+L" = ":L"")
			+ TraceNode(context,((PCode::LTE*)stmt)->lhs) 
			+ L" <= "
			+ TraceNode(context,((PCode::LTE*)stmt)->rhs);
		break;
	case PCode::cGT:
		str += (((PCode::GT*)stmt)->dest? space + TraceNode(context,((PCode::GT*)stmt)->dest)+L" = ":L"")
			+ TraceNode(context,((PCode::GT*)stmt)->lhs) 
			+ L" > "
			+ TraceNode(context,((PCode::GT*)stmt)->rhs);
		break;
	case PCode::cGTE:
		str += (((PCode::GTE*)stmt)->dest? space + TraceNode(context,((PCode::GTE*)stmt)->dest)+L" = ":L"")
			+ TraceNode(context,((PCode::GTE*)stmt)->lhs) 
			+ L" >= "
			+ TraceNode(context,((PCode::GTE*)stmt)->rhs);
		break;
	case PCode::cNOT:
		str += (((PCode::NOT*)stmt)->lhs? space + TraceNode(context,((PCode::GTE*)stmt)->lhs)+L" = ":L"")
			+ L" !("
			+ TraceNode(context,((PCode::GTE*)stmt)->rhs) 
			+ L")";
		break;

	case PCode::cFIELD:
		{
			PCode::FIELD* term = (PCode::FIELD*)stmt;
			str += TraceNode(context,term->base) + L"." + 
				context.trap->getTermName(term->hField);
			break;
		}
		break;
	case PCode::cELEMENT:
		{
			PCode::ELEMENT* term = (PCode::ELEMENT*)stmt;
			str += TraceNode(context,term->base) + L"[" + TraceNode(context,term->rank)+L"]" ;
			break;
		}
		break;
	case PCode::cBLOCK:
		str += space + L"block";
		break;

	case PCode::cINC:
		str += space + TraceNode(context,((PCode::INC*)stmt)->opr)+L"++";
		break;
	case PCode::cDEC:
		str += space + TraceNode(context,((PCode::INC*)stmt)->opr)+L"--";
		break;

	case PCode::cNEW:
		str += space + L"new " + context.trap->getTermName(((PCode::NEW*)stmt)->type);
		if(((PCode::NEW*)stmt)->rank) str +=(std::wstring)L"[" 
				+TraceNode(context,((PCode::NEW*)stmt)->rank) + L"]";
		break;
	case PCode::cDEL:
		str += space + L"del " + TraceNode(context,((PCode::DEL*)stmt)->opr);
		break;
	case PCode::cNEG:
		str += space + L"- " + TraceNode(context,((PCode::NEG*)stmt)->opr);
		break;

	case PCode::cUINC:
		str += space + L"++" + TraceNode(context,((PCode::UINC*)stmt)->opr);
		break;
	case PCode::cUDEC:	
		str += space + L"--" + TraceNode(context,((PCode::UDEC*)stmt)->opr);
		break;
	case PCode::cCAST:
		str += space + L"cast( " + TraceNode(context,((PCode::CAST*)stmt)->opr) +L" as " +
			context.trap->getTermName(((PCode::CAST*)stmt)->destType)+L" )";
		break;
	default:
		assert(false);
		break;
	}
/*
	case kNot:
		{
			OpNot * term = (OpNot *)stmt;
			str += (std::wstring)L"!(" + TraceNode(context,term->expr)+L")";
		}
		break;
	case kBNot:
		{
			OpBNot * term = (OpBNot *)stmt;
			str += (std::wstring)L"~" +TraceNode(context,term->expr);
		}
		break;
	case kNeg:
		{
			OpNeg * term = (OpNeg *)stmt;
			str += (std::wstring)L" - " +TraceNode(context,term->expr)+L"";
		}
		break;
	case kCast:
		{
			OpCast * term = (OpCast *)stmt;
			str += (std::wstring)L"case(" + TraceNode(context,term->expr)+L" as "+ 
				TraceNode(context,term->typ) +L")";
		}
		break;*/

	return str;
}

PCode::Oprand* genStmtEx(TContext & context,TNode* stmt)
{
	PCode::Block * block = PCode::tBlock();
	PCode::Block * exitBlock = PCode::tBlock();

	PCode::Block * enter = block;

	genStmt(context,stmt,block,0,0,exitBlock);
	block = enter;

	std::wstring str;
	std::wstring space=L"\n\t";
	wchar_t buf[60];
	std::vector<PCode::Block*> blocks;
	blocks.push_back(block);
	blocks.push_back(exitBlock);
	int index = 0;
	while(index < (int)blocks.size())
	{
		if(index==1) 
		{
			str += L"\n\nblock 2 " + space +L"ret;";
			index++;
			continue;
		}
		swprintf_s(buf,60,L"\n\nblock %d:",index + 1 );
		str += buf;
		if(!blocks[index])
		{
			index++;
			continue;
		}
		std::vector<PCode::Oprand*>& stmts = blocks[index]->stmts;
		for(int i=0;i<(int)stmts.size();i++)
		{
			if(stmts[i]->cd == PCode::cJMP)
			{
				swprintf_s(buf,60,L"jmp %d ;",EnsureBlock(blocks,((PCode::JMP*)stmts[i])->dest));
				str += space + buf;
			}
			else if(stmts[i]->cd == PCode::cJCC)
			{
				EnsureBlock(blocks,((PCode::JCC*)stmts[i])->tblock);
				str += space + L"test "+ TraceNode(context,((PCode::JCC*)stmts[i])->cond);
				if(((OpJcc*)stmts[i])->fblock)
				{
					blocks.push_back(((PCode::JCC*)stmts[i])->fblock);
					swprintf_s(buf,60,L"jcc %d %d ;",EnsureBlock(blocks,((PCode::JCC*)stmts[i])->tblock),
						EnsureBlock(blocks,((PCode::JCC*)stmts[i])->fblock));
				}
				else
					swprintf_s(buf,60,L"jcc %d ;",EnsureBlock(blocks,((PCode::JCC*)stmts[i])->tblock));
				str += space + buf;
			}
			else
				str += space + TraceNode(context,stmts[i]);
		}
		index++;
	}
	printf("\n%S",str.c_str());

	return block;
}



TNode* genExpr(TContext & context,TNode* expr, OpBlock* block,int & tVarIndex)
{
	switch(expr->kind)
	{
	//primary
	case kThis:
	case kSuper:
	case kValue:
	case kNill:
	case kItem:
		return expr;
	//postfix
	case kInc:
		((OpInc*)expr)->expr = genExpr(context,((OpInc*)expr)->expr,block,tVarIndex);
		return expr;
	case kDec:
		((OpInc*)expr)->expr = genExpr(context,((OpDec*)expr)->expr,block,tVarIndex);
		return expr;
	case kField:
		((OpField*)expr)->base = genExpr(context,((OpField*)expr)->base,block,tVarIndex);
		return expr;
	case kElement:
		((OpElement*)expr)->exp = genExpr(context,((OpElement*)expr)->exp,block,tVarIndex);
		return expr;
	case kCall:
		{
			TNode* mt = ((OpCall*)expr)->mt;
			std::vector<TNode*> terms;
			if(((OpCall*)expr)->args)
			{
				if(((OpCall*)expr)->args->kind!=kList)
				{
					terms.push_back(((OpCall*)expr)->args);
				}else
				{
					TNodeList* args = (TNodeList*)((OpCall*)expr)->args;
					while(true)
					{
						if(args->node)terms.push_back(args->node);
						if(!args->next) break;
						args = args->next;
					}
				}
			}
			for(int i= (int)terms.size() - 1;i>=0;i--)
			{
				TNode* arg = genExpr(context,terms[i],block,tVarIndex);
				arg->isArg = true;
				*block += arg;
			}
		}
		return expr;
	//unary
	case kUInc:
		((OpUInc*)expr)->expr = genExpr(context,((OpUInc*)expr)->expr,block,tVarIndex);
		return expr;
	case kUDec:
		((OpUDec*)expr)->expr = genExpr(context,((OpUDec*)expr)->expr,block,tVarIndex);
		return expr;
	case kNeg:
		((OpNeg*)expr)->expr = genExpr(context,((OpNeg*)expr)->expr,block,tVarIndex);
		return expr;
	case kNew:
		//((OpNeg*)expr)->expr 
		return expr;
	case kDelete:
		((OpDelete*)expr)->exp = genExpr(context,((OpDelete*)expr)->exp,block,tVarIndex);
		return expr;
	case kNot:
		((OpNot*)expr)->expr = genExpr(context,((OpNot*)expr)->expr,block,tVarIndex);
		return expr;
	case kBNot:
		((OpBNot*)expr)->expr = genExpr(context,((OpBNot*)expr)->expr,block,tVarIndex);
		return expr;
	//cast
	case kCast:
		((OpCast*)expr)->expr = genExpr(context,((OpCast*)expr)->expr,block,tVarIndex);
		return expr;
	//binary
	case kBinary:
		{
			OpBinary* term = (OpBinary*) expr;
			term->rhs = genExpr(context,term->rhs,block,tVarIndex);
			term->lhs = genExpr(context,term->lhs,block,tVarIndex);
			if(term->op != OpBinary::asn)
			{
				int tcount = 0;
				if(term->lhs->kind==kItem && ((OpItem*)term->lhs)->tempId)tcount++;
				if(term->rhs->kind==kItem && ((OpItem*)term->rhs)->tempId)tcount++;
				tVarIndex += 1 - tcount; 
				assert(tVarIndex > 0);
				term = opBinary(0,OpBinary::asn,opItem(tVarIndex), term);
			}
			*block += term;
			return term->lhs;
		}
	default:
		assert(false);
		return 0;
	}
}

TNode* genExpr(TContext & context,TNode* expr, OpBlock* block)
{
	int tVarIndex = 0;
	TNode * node =  genExpr(context,expr,block,tVarIndex);
	if(expr->kind != kBinary) *block += node;
	return node;
}

int  EnsureBlock(std::vector<OpBlock*> & blocks, OpBlock * block)
{
	for(int i=0; i<(int)blocks.size(); i++)
	{
		if(blocks[i]==block) return i + 1;
	}
	blocks.push_back(block);
	return (int)blocks.size();
}

TNode* genStmt(TContext & context,TNode* stmt)
{
	OpBlock * block = opBlock();
	OpBlock * exitBlock = opBlock();

	OpBlock * enter = block;

	genStmt(context,stmt,block,0,0,exitBlock);
	block = enter;

	std::wstring str;
	std::wstring space=L"\n\t";
	wchar_t buf[60];
	std::vector<OpBlock*> blocks;
	blocks.push_back(block);
	blocks.push_back(exitBlock);
	int index = 0;
	while(index < (int)blocks.size())
	{
		if(index==1) 
		{
			str += L"\n\nblock 2 " + space +L"ret;";
			index++;
			continue;
		}
		swprintf_s(buf,60,L"\n\nblock %d:",index + 1 );
		str += buf;
		if(!blocks[index])
		{
			index++;
			continue;
		}
		OpList* stmts = blocks[index]->stmts;
		while(stmts)
		{
			if(stmts->node)
			{
				if(stmts->node->kind == kJmp)
				{
					swprintf_s(buf,60,L"jmp %d ;",EnsureBlock(blocks,((OpJmp*)stmts->node)->dest));
					str += space + buf;
				}
				else if(stmts->node->kind == kJcc)
				{
					EnsureBlock(blocks,((OpJcc*)stmts->node)->tblock);
					if(((OpJcc*)stmts->node)->fblock)
					{
						blocks.push_back(((OpJcc*)stmts->node)->fblock);
						swprintf_s(buf,60,L"jcc %d %d ;",EnsureBlock(blocks,((OpJcc*)stmts->node)->tblock),
							EnsureBlock(blocks,((OpJcc*)stmts->node)->fblock));
					}
					else
						swprintf_s(buf,60,L"jcc %d ;",EnsureBlock(blocks,((OpJcc*)stmts->node)->tblock));
					str += space + buf;
				}
				else
					str += space + TraceNode(context,stmts->node);
			}
			stmts = stmts->next;
		}
		index++;
	}
	printf("\n%S",str.c_str());

	return block;
}

TNode* genStmt(TContext & context,TNode* stmt,OpBlock* & block,
			  OpBlock* continueBlock,OpBlock* breakBlock,OpBlock* exitBlock)
{
	TNode* node = stmt;
	if(!node) return 0;

	switch(node->kind)
	{
	case kIf:
		{
			OpIf * term = (OpIf*)node;
			OpBlock* tblock = opBlock();
			OpBlock* fblock = opBlock();
			OpBlock * bellow = opBlock();

			*block += opJcc(genExpr(context,term->cond,block),tblock,term->fbody?fblock:bellow);
			if(term->tbody)genStmt(context,term->tbody,tblock,continueBlock,breakBlock,exitBlock);
			*tblock += opJmp(bellow);
			if(term->fbody)
			{
				genStmt(context,term->fbody,fblock,continueBlock,breakBlock,exitBlock);
				*fblock += opJmp(bellow);
			}
			block = bellow;
		}
		break;
	case kFor:
		{
			OpFor * term = (OpFor*)node;
			if(term->init)
			{
				if(term->init->kind==kVar || term->init->kind==kList)
					genStmt(context,term->init,block,continueBlock,breakBlock,exitBlock); 
				else
					genExpr(context,term->init,block);
			}

			OpBlock * body = opBlock();
			OpBlock * bellow = opBlock();
			OpBlock * cond = opBlock();

			*block += opJmp(cond);

			if(term->cond)
				*cond += opJcc(genExpr(context,term->cond,cond),body,bellow);
			else
				*cond += opJmp(body);

			genStmt(context,term->body,body,cond,bellow,exitBlock);

			if(term->inc)genExpr(context,term->inc,body);
			*body += opJmp(cond);

			block = bellow;
		}
		break;
	case kWhile:
		{
			OpWhile * term = (OpWhile*)node;

			OpBlock * body = opBlock();
			OpBlock * cond = block->stmts->node?opBlock():block;
			OpBlock * bellow = opBlock();

			if(block!=cond)*block += opJmp(cond);
			*cond += opJcc(genExpr(context,term->cond,cond),body,bellow);
			genStmt(context,term->body,body,cond,bellow,exitBlock);
			*body += opJmp(cond);

			block = bellow;
		}
		break;
	case kUntil:
		{
			OpUntil * term = (OpUntil*)node;

			OpBlock * body =  block->stmts->node? opBlock():block;
			OpBlock * bellow = opBlock();

			if(block!=body)*block += opJmp(body);
			genStmt(context,term->body,body,body,bellow,exitBlock);
			*body += opJcc(genExpr(context,term->cond,body),body,bellow);
			
			block = bellow;	
		}
		break;
	case kReturn:
		{
			OpReturn * term = (OpReturn*)node;
			if(term->expr)
			{
				term->expr = genExpr(context,term->expr,block);
			}
			*block += opJmp(exitBlock);
		}
		break;
	case kContinue:
		*block += opJmp(continueBlock);
		break;
	case kBreak:
		*block += opJmp(breakBlock);
		break;
	case kCompStmt:
		{
			OpCompStmt * term = (OpCompStmt*)node;
			genStmt(context,term->stmts,block,continueBlock,breakBlock,exitBlock);
		}
		break;
	case kExprStmt:
		{
			OpExprStmt* term = (OpExprStmt*)node;
			if(term->expr)
				genExpr(context,term->expr,block);
		}
		break;
	case kVar:
		{
			OpVar*	term = (OpVar *) node;
			if(term->expr)
				genExpr(context,opBinary(0,OpBinary::asn,opItem(term->hVariant),term->expr),block);
		}
		break;
	case kList:
		{
			OpList * terms = (OpList *)node;
			while(terms)
			{
				if(terms->node)genStmt(context,terms->node,block,continueBlock,breakBlock,exitBlock);
				terms = terms->next;
			}
		}
		break;
	}
	return 0;
}


PCode::Oprand* genCode(TContext & context, PCode::Oprand* node)
{
	assert(methodes.size());
	HMethod hMethod = methodes.back();

	GenSvr genSvr(context);
	GenSvr::RegSvr regSvr(genSvr);
	genSvr.regSvr = &regSvr;

	//regs
	//eax,ecx,edx,ebx,esp,ebp,esi,edi
	regSvr.regs.push_back(GenSvr::RegItem(1,true)); //ecx
	regSvr.regs.push_back(GenSvr::RegItem(0,true)); //eax
	regSvr.regs.push_back(GenSvr::RegItem(3,false)); //ebx
	regSvr.regs.push_back(GenSvr::RegItem(6,false)); //esi
	regSvr.regs.push_back(GenSvr::RegItem(7,false)); //edi
	
	//tmps
	for(int i=0;i<32;i++)
	{
		regSvr.tmps.push_back(GenSvr::VarTmpItem());
		GenSvr::VarTmpItem & item = regSvr.tmps.back();
		item.reg = -1;
		item.addr = false;
		item.offset = - regSvr.varOffset - context.trap->getMethodVariantSize(hMethod,sizeof(void*)) - regSvr.tmpOffset - (i<<3);
		if(abs(item.offset)>128)
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
		else
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B8>(item.offset));
	}

	//param
	for(int i=0;i<context.trap->getMethod(hMethod)->paramCount;i++)
	{
		regSvr.params.push_back(GenSvr::VarItem());
		GenSvr::VarItem & item = regSvr.params.back();
		item.hTerm = context.trap->getMethod(hMethod)->hParam;
		item.hTerm.index += i;
		item.reg = -1;
		item.addr = true;
		item.offset = context.trap->getFrameOffset(hMethod,item.hTerm) - regSvr.paramOffset;
		if(abs(item.offset)>128)
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
		else
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B8>(item.offset));
	}
	//variant
	//this
	regSvr.vars.push_back(GenSvr::VarItem());
	GenSvr::VarItem & item = regSvr.vars.back();
	item.hTerm = classes.size()?classes.back():0;
	item.reg = -1;
	item.addr = false;
	item.offset = -4;
	item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
	regSvr.UseReg(0,GenSvr::VarIndex(GenSvr::VarIndex::kVar,0));//bind ecx this
	//other
	for(int i=0;i<context.trap->getMethod(hMethod)->variantCount;i++)
	{
		regSvr.vars.push_back(GenSvr::VarItem());
		GenSvr::VarItem & item = regSvr.vars.back();
		item.hTerm = context.trap->getMethod(hMethod)->hVariant;
		item.hTerm.index += i;
		item.reg = -1;
		item.addr = true;
		item.offset = - context.trap->getFrameOffset(hMethod,item.hTerm) - regSvr.varOffset;
		if(abs(item.offset)>128)
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
		else
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B8>(item.offset));
	}
	//
	unsigned char * pc = context.trap->getCoder()->hCodeSpace->pc;
	genCode(context,node,&genSvr);

	std::wstring str;
	context.trap->getCoder()->Disassemble(pc,str);
	printf("\ncode:%S",str.c_str());

	//variant
	for(int i=0;i<(int)regSvr.tmps.size();i++)delete (X86::AddressX86<B32>*)regSvr.tmps[i].addrTerm;
	for(int i=0;i<(int)regSvr.params.size();i++)delete (X86::AddressX86<B32>*)regSvr.params[i].addrTerm;
	for(int i=0;i<(int)regSvr.vars.size();i++)delete (X86::AddressX86<B32>*)regSvr.vars[i].addrTerm;

	return 0;
}

TNode* genCode(TContext & context, TNode* node)
{
	assert(methodes.size());
	HMethod hMethod = methodes.back();

	GenSvr genSvr(context);
	GenSvr::RegSvr regSvr(genSvr);

	//regs
	//eax,ecx,edx,ebx,esp,ebp,esi,edi
	regSvr.regs.push_back(GenSvr::RegItem(1,true)); //ecx
	regSvr.regs.push_back(GenSvr::RegItem(0,true)); //eax
	regSvr.regs.push_back(GenSvr::RegItem(3,false)); //ebx
	regSvr.regs.push_back(GenSvr::RegItem(6,false)); //esi
	regSvr.regs.push_back(GenSvr::RegItem(7,false)); //edi
	
	//tmps
	for(int i=0;i<32;i++)
	{
		regSvr.tmps.push_back(GenSvr::VarTmpItem());
		GenSvr::VarTmpItem & item = regSvr.tmps.back();
		item.reg = -1;
		item.addr = false;
		item.offset = - context.trap->getMethodVariantSize(hMethod,sizeof(void*)) - (i<<3);
		if(abs(item.offset)>128)
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
		else
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B8>(item.offset));
	}

	//param
	for(int i=0;i<context.trap->getMethod(hMethod)->paramCount;i++)
	{
		regSvr.params.push_back(GenSvr::VarItem());
		GenSvr::VarItem & item = regSvr.params.back();
		item.hTerm = context.trap->getMethod(hMethod)->hParam;
		item.hTerm.index += i;
		item.reg = -1;
		item.addr = true;
		item.offset = context.trap->getFrameOffset(hMethod,item.hTerm) + regSvr.paramOffset;
		if(abs(item.offset)>128)
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
		else
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B8>(item.offset));
	}
	//variant
	//this
	regSvr.vars.push_back(GenSvr::VarItem());
	GenSvr::VarItem & item = regSvr.vars.back();
	item.hTerm = classes.size()?classes.back():0;
	item.reg = -1;
	item.addr = false;
	item.offset = -4;
	item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
	regSvr.UseReg(0,GenSvr::VarIndex(GenSvr::VarIndex::kVar,0));//bind ecx this
	//other
	for(int i=0;i<context.trap->getMethod(hMethod)->variantCount;i++)
	{
		regSvr.vars.push_back(GenSvr::VarItem());
		GenSvr::VarItem & item = regSvr.vars.back();
		item.hTerm = context.trap->getMethod(hMethod)->hVariant;
		item.hTerm.index += i;
		item.reg = -1;
		item.addr = true;
		item.offset = context.trap->getFrameOffset(hMethod,item.hTerm)+regSvr.varOffset;
		if(abs(item.offset)>128)
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B32>(item.offset));
		else
			item.addrTerm = new X86::AddressX86<B32>(X86::ebp,Disp<B8>(item.offset));
	}
	//
	unsigned char * pc = context.trap->getCoder()->hCodeSpace->pc;
	genCode(context,node,&genSvr);

	std::wstring str;
	context.trap->getCoder()->Disassemble(pc,str);
	printf("\ncode:%S",str.c_str());

	//variant
	for(int i=0;i<(int)regSvr.tmps.size();i++)delete (X86::AddressX86<B32>*)regSvr.tmps[i].addrTerm;
	for(int i=0;i<(int)regSvr.params.size();i++)delete (X86::AddressX86<B32>*)regSvr.params[i].addrTerm;
	for(int i=0;i<(int)regSvr.vars.size();i++)delete (X86::AddressX86<B32>*)regSvr.vars[i].addrTerm;

	return 0;
}

Term* genCodeTerm(TContext & context,PCode::Oprand * item, GenSvr* genSvr,int &maskRegister);

Term* genOlePropPut(TContext & context,PCode::Oprand * item, Term* value, GenSvr* genSvr,int &maskRegister)
{
	/*
		//ole field
		let $type = fn:symGetTermType($hitem);
		MOV(ebx,$lhs);
		PUSH($rhs);
		let $nmethod = fn:symGetOlePropPut($hitem);
		//push this
		MOV(ecx,ebx);
		PUSH(ecx);
		//find vtable
		MOV(eax,Address(ecx));
		ADD(eax,Imm(fn:Mul(4,fn:Dec($nmethod))));
		MOV(eax,Address(eax));
		CALL(eax);
		return eax();
	*/
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	//only process olefield
	PCode::FIELD * term = (PCode::FIELD *)item;
	assert(term->hField.kind==skOleField);
	Term* base = genCodeTerm(context,term->base,genSvr,maskRegister);

	//put value
	genSvr->regSvr->SpillReg(0); //ecx
	if(value->kind==Term::kAddress)
		pc = X86::PUSH(pc,*(X86::AddressX86<B32>*)value);
	else if(value->kind==Term::kRegister)
		pc = X86::PUSH(pc, *(Register<B32>*)value);
	else if(value->kind==Term::kImm)
		pc = X86::PUSH(pc,*(Imm<B32>*)value);
	else
		assert(false);

	//put this
	genSvr->regSvr->SpillReg(0); //ecx
	if(base->kind==Term::kAddress)
		pc = X86::MOV(pc, X86::ecx,*(X86::AddressX86<B32>*)base);
	else if(base->kind==Term::kRegister)
		pc = X86::MOV(pc, X86::ecx,*(Register<B32>*)base);
	else
		assert(false);
	pc = X86::PUSH(pc,X86::ecx);

	genSvr->regSvr->SpillCallerReg();
	int hMethod = ((SymOleField *)context.trap->getField(term->hField))->propPut;
	assert(hMethod);
	//call
	//lookup vtable
	pc = X86::MOV(pc,X86::eax,X86::AddressX86<B32>(X86::ecx));
	pc = X86::ADD(pc,X86::eax,Imm<B32>(4*(hMethod - 1)));
	pc = X86::MOV(pc,X86::eax,X86::AddressX86<B32>(X86::eax));
	pc = X86::CALL(pc,X86::eax);

	return (Term*)genSvr->regSvr->regs[1].regTerm; //eax
}

Term* genOlePropFetch(TContext & context,PCode::Oprand * item, GenSvr* genSvr,int &maskRegister)
{
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	//only process olefield
	PCode::FIELD * term = (PCode::FIELD *)item;
	assert(term->hField.kind==skOleField);
	Term* base = genCodeTerm(context,term->base,genSvr,maskRegister);

	/*
	//bug: 如ole返回浮点字段会出错.
	//ole field
	let $type = fn:symGetTermType(hitem);
	let $size = fn:symGetTypeTermFrameSize(hitem);
	//push Ret
	MOV(ebx,$r);
	
	SUB(esp,Imm($size));
	MOV(eax, esp);	//Address(ebp,Imm(fn:Neg(0x80))));
	MOV(Address(eax),Imm(0));
	PUSH(eax);

	let $nmethod = fn:symGetOlePropGet(hitem);
	//push this
	MOV(ecx,ebx);
	PUSH(ecx);
	//find vtable
	MOV(eax,Address(ecx));
	ADD(eax,Imm(fn:Mul(4,fn:Dec($nmethod))));
	MOV(eax,Address(eax));
	CALL(eax);
	if(fn:Eq($type,T("variant")))
	{
		let $vt_r8 = 5;
		XOR(eax,eax);
		MOV(ebx,esp);
		MOV(ax,Address(ebx));
		//CMP(eax,Imm($vt_r8));
		//JCC(Label(labelBreakDouble),cNotEqual);
		MOV(eax,Address(ebx,Imm(8)));
		//MOVD(xmm0,Address(ebx,8));
		//LABEL(labelBreakDouble);
	}else
	{
		MOV(eax,esp);
		MOV(eax,Address(eax));//Address(ebp,Imm(fn:Neg(0x80))));
	}
	ADD(esp,Imm($size));
	MOV($r0,eax);
	
	MOV(ecx,Address(ebp,Imm(fn:Neg(4))));
	return $r0;

	*/
	HType hType= context.trap->getTermType(term->hField);
	int msize = 0;
	int sz = context.trap->getAllocSize(hType,msize);
	//put return value
	pc = X86::SUB(pc,X86::esp,Imm<B32>(sz));
	pc = X86::MOV(pc,X86::AddressX86<B32>(X86::esp),Imm<B32>(0));
	pc = X86::PUSH(pc,X86::esp);
	//put this
	genSvr->regSvr->SpillReg(0); //ecx
	if(base->kind==Term::kAddress)
		pc = X86::MOV(pc, X86::ecx,*(X86::AddressX86<B32>*)base);
	else if(base->kind==Term::kRegister)
		pc = X86::MOV(pc, X86::ecx,*(Register<B32>*)base);
	else
		assert(false);
	pc = X86::PUSH(pc,X86::ecx);

	genSvr->regSvr->SpillCallerReg();
	int hMethod = ((SymOleField *)context.trap->getField(term->hField))->propGet;
	assert(hMethod);
	//call
	//lookup vtable
	pc = X86::MOV(pc,X86::eax,X86::AddressX86<B32>(X86::ecx));
	pc = X86::ADD(pc,X86::eax,Imm<B32>(4*(hMethod - 1)));
	pc = X86::MOV(pc,X86::eax,X86::AddressX86<B32>(X86::eax));
	pc = X86::CALL(pc,X86::eax);

	//处理返回值
	if(context.trap->getTypeHandle(util::hashString(L"variant"))==hType)
	{
		pc = X86::MOV(pc,X86::eax,X86::AddressX86<B32>(X86::esp,Disp<B32>(8)));
	}else
	{
		pc = X86::MOV(pc,X86::eax,X86::AddressX86<B32>(X86::esp));
	}
	pc = X86::ADD(pc,X86::esp,Imm<B32>(sz));

	//放弃临时变量对寄存器的引用
	if(term->base->cd==PCode::cItem)
	{
		GenSvr::VarIndex xIndex = genSvr->regSvr->GetVarIndex(context,(PCode::Item *)term->base);
		if(genSvr->isTmpVar(xIndex))genSvr->regSvr->UnUsedReg(xIndex);
	}
	return (Term*)genSvr->regSvr->regs[1].regTerm; //eax
}

void* genCodeStmt(TContext & context,PCode::Oprand * node, GenSvr* genSvr);
Term* genCodeTerm(TContext & context,PCode::Oprand * item, GenSvr* genSvr,int &maskRegister)
{
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	switch(item->cd)
	{
	case PCode::cItem:
		{
			PCode::Oprand* xNode = item;
			GenSvr::VarIndex xIndex = genSvr->regSvr->GetVarIndex(context,(PCode::Item *)xNode);
			GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
			if(xItem->reg!=-1)
				return (Term*)genSvr->regSvr->regs[xItem->reg].regTerm;
			else
			{
				if(xItem->addrTerm) return (Term*)xItem->addrTerm;

				if(xIndex.kind ==GenSvr::VarIndex::kField)
				{
					GenSvr::FieldItem * term = (GenSvr::FieldItem *)&genSvr->regSvr->GetVarItem(xIndex);
					GenSvr::VarItemBase & item = genSvr->regSvr->GetVarItem(term->baseIndex);
					if(item.reg==-1)
					{
						pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)item.addrTerm);
						return new X86::AddressX86<B32>(X86::edx,Disp<B32>(term->offset));
					}else
					{
						return new X86::AddressX86<B32>(*(Register<B32>*)genSvr->regSvr->regs[item.reg].regTerm,Disp<B32>(term->offset));
					}
				}else
				{
					assert(false);
					return 0;
				}
			}
		}
	case PCode::cImm:
		return new Imm<B32>(((PCode::Imm*)item)->val_i32);
	case PCode::cThis:
		{
			GenSvr::VarIndex xIndex(GenSvr::VarIndex::kVar,0);
			GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
			if(xItem->reg!=-1)
				return (Term*)genSvr->regSvr->regs[xItem->reg].regTerm;
			else
				return (Term*)xItem->addrTerm;
		}
	case PCode::cFIELD:
		{
			//only process olefield
			PCode::FIELD * term = (PCode::FIELD *)item;
			Term* base = genCodeTerm(context,term->base,genSvr,maskRegister);
			if(term->hField.kind==skOleField)
				return genOlePropFetch(context,item,genSvr,maskRegister);
			else
				assert(false);
			return 0;
		}
	case PCode::cCALL:
	{
		/*
				//函数
		apply RValue(OpMethod(regs ty base)) ->
		{
			let $fn = ty;
			let $fnspec = fn:symGetMethodSpec($fn);
			let $addr = fn:symGetMethodAddr(ty);
			
			
			if($addr)
			{
				MOV(eax,Imm($addr));
			}else
			{
				PUSH(ecx);
				if(fn:Band($fnspec,type_spec("native")))
				{
					//get extern address by used LoadProcAddress
					PUSH(Imm(fn:symGetMethodNativeAlias($fn)));
					PUSH(Imm(fn:symGetMethodNativeFrom($fn)));
					MOV(eax,Fct(LoadProcAddress));
					CALL(eax);
					ADD(esp,Imm(8));
				}else
				{
					//重新获取函数地址
					PUSH(Imm($fn));
					PUSH(Imm(fn:symGetSymTable()));
					MOV(eax,Fct(GetMethodAddr));
					CALL(eax);
				}
				POP(ecx);
				//MOV(ecx,Address(ebp,Imm(fn:Neg(4))));
			}
			return eax();
		}	

		//函数调用
		apply RValue(OpCall(regs typ mt args) )->
		{
			//fn:print("--call--",regs);
			let $r0 = GetRegister(regs,typ);
			let $fn = TypeOfTerm(mt);
			let $reg1 = RegsOfTerm(mt);
			
			//断点函数
			if(fn:Eq(fn:symGetTermName($fn),"debugbreak"))
			{
				BR();
				return Imm(0);
			}
				
			let $nargs = fn:size(args);
			let $cargs = fn:symGetMethodParamCount($fn);
			let $i = 0;
			let $argsize = 0;
			
			//这些对象的第一个参数是本身的值
			if(IsPrimaryTypeMethod($fn))
			{
				fn:bind($cargs,fn:Dec($cargs));
				if(IsDoubleTypeMethod($fn))
					fn:bind($argsize,8);
				else
					fn:bind($argsize,4);
			}
			
			let $rettype = fn:symGetTermType($fn);
			let $fsize = fn:symGetTypeTermFrameSize($fn);
			let $fspec = fn:symGetMethodSpec($fn);
			
			//对于ole对象函数调用，预留空间用于保存返回变量
			if(fn:symIsOleItem($fn))
			{
				//fn:print("\nreturn: ",fn:symGetTermName($rettype)," size:",$fsize);
				if($fsize)
				{
					//push Ret
					SUB(esp,Imm($fsize));
					MOV(eax,esp); 
					MOV(Address(eax),Imm(0));
					PUSH(eax);
				}
			}	
			
			//如返回结构，则需在栈上创建一个结构。如结构被另一变量接收，应该直接使用该变量的地址
			else if(fn:And($rettype, IsStructItem($rettype)))
			{
				//如果只是返回引用指针，则不需要。
				if(fn:Not(fn:Band($fspec, type_spec("ref"))))
				{
					SUB(esp,Imm($fsize));
					fn:bind($argsize,fn:Add($argsize,Imm($fsize)));
					
					MOV(ebx,ebp);
					PUSH(Address(ebx));
					fn:bind($argsize,fn:Add($argsize,4));
				}
			}
			//对于外部浮点运算,预留空间用于把浮点寄存器的数据转化为xmm数据.
			 else if(fn:And($rettype,fn:Eq($rettype, T("double"))))
			{
				SUB(esp,Imm($fsize));
			}

			//缺省值入栈
			while(fn:Gt($cargs , fn:Add($nargs,$i)))
			{
				let $hparam = fn:symGetMethodParamHandleByIndex($fn,fn:Sub($cargs,fn:Inc($i)));
				let $typ 	 = 	fn:symGetTermType($hparam);
				let $spec = 	fn:symGetParamSpec($hparam);
				let $size  = 	fn:symGetTypeTermFrameSize($hparam);
				PUSH(Imm(fn:symGetParamInitValue($hparam)));
				fn:bind($argsize,fn:Add($argsize,$size));
				fn:bind($size,fn:Sub($size,4));
				while(fn:Gt($size,0))
				{
					PUSH(Imm(0));
					fn:bind($size,fn:Sub($size,4));
				}
				fn:bind($i,fn:Inc($i));
			}
			
			//参数入栈
			let $fnspec = fn:symGetMethodSpec($fn);
			for $t in args
			{
				let $hparam = fn:symGetMethodParamHandleByIndex($fn,fn:Sub($cargs,fn:Inc($i)));
			
				let $typ 		 = fn:symGetTermType($hparam);
				let $spec 	 = fn:symGetParamSpec($hparam);
				let $size 	 = fn:symGetTypeTermFrameSize($hparam);
				
				let $r 		= fn:branch(fn:And(fn:Band($spec, type_spec("ref")),fn:Not(IsStructItem($typ)))
										,LValue($t),RValue($t));
				let $type 	= TypeOfTerm($t);
				
				if(fn:Eq($typ,$type))
				{
					//如果参数是引用调用
					//假定结构内部与外部一致。
					if(fn:Band($spec, type_spec("ref")))
					{
						//bug:如果本身是引用类型，则应该求值
						if(IsStructItem($typ))
						{
							PUSH($r);
						}else
						{
							PtrTo(ebx,$r);
							PUSH(ebx);
						}
					}
					else
					{
						if(fn:Or(IsClassItem($typ),IsInterfaceItem($typ)))
						{
							if(NeedNativePtr($fnspec,$spec))
							{
								if(IsNativeClass($typ))
								{
									MOV(ebx,$r);
									ADD(ebx,Imm(4));
									MOV(ebx,Address(ebx));
									PUSH(ebx);
								}else
									PUSH($r);
							}else
							{
								PUSH($r);
							}
						}else
							Push($typ,$r);
					}
				}else if(fn:Eq($typ,T("variant")))
				{
					PushVariant($type,$size,$r);
				}else if(fn:Band($spec, type_spec("ref")))
				{
					//bug:如果本身是引用类型，则应该求值
					if(IsStructItem($typ))
					{
						PUSH($r);
					}else
					{
						PtrTo(ebx,$r);
						PUSH(ebx);
					}
				}
				else
				{
					//转换数据类型
					genOpCast(1, $typ,$type,$r);
					//可能是函数指针
					Push($type, $r); 
					//fn:throw(fn:Add("\n----need cast type at call " ,fn:symGetTermName($fn)));
				}
				fn:bind($argsize,fn:Add($argsize,$size));
				fn:bind($i,fn:Inc($i));
			}
		
			//判断是否需要this
			//如果非静态函数，需要设置this
			let $pThis = fn:branch(IsNodeName(mt,"OpMethod"), RValue(fn:ChildNext(mt, 2)) ,ecx());
			if(fn:Not($pThis))fn:bind($pThis,ecx());
			
				
			//primary object ptr
			if(IsPrimaryTypeMethod($fn))
			{
				if(IsDoubleTypeMethod($fn))
				{
					Push(T("double"), $pThis);
				}else
					PUSH($pThis);
			}
			
			//调用
			if(fn:symIsOleItem($fn))
			{
				MOV(ecx,$pThis);
				let $nmethod = fn:symGetOleMethod($fn);
				PUSH(ecx);
				//find vtable
				MOV(eax,Address(ecx));
				ADD(eax,Imm(fn:Mul(4,fn:Dec($nmethod))));
				MOV(eax,Address(eax));
				CALL(eax);
				//bug
				if(fn:Eq($rettype,T("variant")))
				{
					let $vt_r8 = 5;
					XOR(eax,eax);
					MOV(ebx,esp);
					//MOV(eax,Address(ebx));
					//CMP(eax,Imm($vt_r8));
					//JCC(Label(labelBreakDouble),cNotEqual);
					MOV(eax,Address(ebx,Imm(8)));
					//MOVD(xmm0,Address(ebx,8));
					//LABEL(labelBreakDouble);
					
				}else
				{
					if($fsize)
					{
						MOV(eax,esp);
						MOV(eax,Address(eax));//Address(ebp,Imm(fn:Neg(0x80))));
					}
				}
				if($fsize)ADD(esp,Imm($fsize));
				MOV(ecx,Address(ebp,Imm(fn:Neg(4))));
			}else
			{

				//设置this
				if(fn:Not(fn:Band($fnspec,type_spec("static"))))
				{
					MOV(ecx,$pThis);
					MOV(edx,ecx);
					if(fn:Band($fnspec,type_spec("native")))
					{
						//提取本地指针
						ADD(ecx,Imm(4));
						MOV(ecx,Address(ecx));
						CMP(ecx,Imm(0));
						JCC(Label(label_chknativeptr),cNotEqual);
						//空指针
						PUSH(eax);
						PUSH(Address(edx));
						PUSH(Imm($fn));
						PUSH(Imm($hmethod));
						PUSH(Imm(fn:symGetContext()));
						PUSH(Imm(3)); //errorcode
						MOV(eax,Fct(WarnToCall));
						CALL(eax);
						ADD(esp,Imm(12));
						XOR(ecx,ecx);
						POP(eax);
						LABEL(label_chknativeptr);
					}
				}

				if(fn:Band($fnspec,type_spec("static")))
				{
					let $addr = 0; 
					if(fn:Or(
						fn:Band($fnspec,type_spec("static")),
						fn:Not(fn:And(fn:Band($fnspec,type_spec("virtual")),
											fn:Not(IsNativeMethod($fn))))))
					{
						fn:bind($addr,RValue(mt)); 
					}
					CALL($addr);
					if(fn:Not(fn:Band($fnspec,type_spec("stdcall"))))
						if($argsize) ADD(esp,Imm($argsize));
				}else 
				{//this call
					if(fn:Band($fnspec,type_spec("stdcall")))
						PUSH(ecx); //push this
					if(fn:Band($fnspec,type_spec("virtual")))
					{
						if(fn:Not(IsNativeMethod($fn)))
						{
							PUSH(ecx); //save
							//call FindMatchVirtualMethodAddr
							PUSH(Imm($fn));
							PUSH(Address(ecx)); //wc
							PUSH(Imm(fn:symGetContext()));
							MOV(eax,Fct(FindMatchVirtualMethodAddr));
							CALL(eax);
							ADD(esp,Imm(12));
							POP(ecx); //restore
							CALL(eax);
						}else
						{
							let $addr = 0; 
							if(fn:Or(
								fn:Band($fnspec,type_spec("static")),
								fn:Not(fn:And(fn:Band($fnspec,type_spec("virtual")),
													fn:Not(IsNativeMethod($fn))))))
							{
								fn:bind($addr,RValue(mt)); 
							}
							CALL($addr);	
						}
					}else
					{
						let $addr = 0; 
						if(fn:Or(
							fn:Band($fnspec,type_spec("static")),
							fn:Not(fn:And(fn:Band($fnspec,type_spec("virtual")),
												fn:Not(IsNativeMethod($fn))))))
						{
							fn:bind($addr,RValue(mt)); 
						}
						CALL($addr);
					}
				}
				if(fn:Not(fn:Band(fn:symGetMethodSpec($hmethod),type_spec("static"))))
					MOV(ecx,Address(ebp,Imm(fn:Neg(4))));
			}

			//如返回结构，则退出结构。
			 if(fn:And($rettype,IsStructItem($rettype)))
			{
				ADD(esp,Imm($fsize));
				return eax();
			}
			
			//如返回浮点数，则需要判断是否为外部函数，如外部函数则在浮点寄存器上，安全函数则在xmm寄存器中。
			else if(fn:And($rettype,IsDoubleType($rettype)))
			{
				if(fn:Band($fnspec,type_spec("native")))
				{
					//从浮点寄存器中取数据
					ADD(esp,Imm($fsize));
					MOV(ebx,esp);
					FSTP(Address(ebx));
					MOV(xmm0,Address(ebx));
					ADD(esp,Imm(8));
				}
				return xmm0();
			}
			
			//其他则假定在eax中	
			//如果本地函数返回本地类对象，则生成类对象。
			else if(IsNativeMethod($fn))
			{
				if(IsNativeClass($rettype))
				{
					//新建类对象
					MOV(esi,eax); //会不会有冲突
					
					//判断是否为空指针
					CMP(eax,Imm(0));
					JCC(Label(label_chknativeptr1),cNotEqual);
					//空指针
					PUSH(eax);
					PUSH(Imm(0)); //type
					PUSH(Imm($fn));
					PUSH(Imm($hmethod));
					PUSH(Imm(fn:symGetContext()));
					PUSH(Imm(4)); //errorcode return nill ptr
					MOV(eax,Fct(WarnToCall));
					CALL(eax);
					ADD(esp,Imm(12));
					POP(eax);
					LABEL(label_chknativeptr1);
					
					PUSH(Imm(fn:symGetTypeAllocSize($rettype)));
					MOV(eax,Fct(alloc));
					CALL(eax);
					MOV(ecx,Address(ebp,Imm(fn:Neg(4))));
					ADD(esp,Imm(4));
					//设置本地指针
					MOV(ebx,eax);
					MOV(Address(ebx),Imm($rettype));
					ADD(ebx,Imm(4));
					MOV(Address(ebx),esi);
				}
			}
			return eax();
		*/
		PCode::CALL * term = (PCode::CALL *)item;
		HMethod hMethod = term->hTerm;

		//断点函数
		if(wcscmp(context.trap->getTermName(hMethod),L"debugbreak")==0)
		{
			pc = X86::BR(pc);
			return 0;
		}

		HType hClass = (HType)context.trap->getParent(hMethod);
		
		//缺省值入栈
		int argCount = context.trap->getMethod(hMethod)->paramCount;
		HParam hParam = context.trap->getMethod(hMethod)->hParam;
		for(int i=argCount; i>term->argCount;i--)
		{
			hParam.index += i - 1;
			int sz = context.trap->getTypeTermFrameSize(hParam);
			pc = X86::PUSH(pc,Imm<B32>(context.trap->getParam(hParam)->initValue));
			sz -=4;
			while(sz)
			{
				pc = X86::PUSH(pc,Imm<B32>(0));
				sz -=4;
			}
		}
		void * Addr = context.trap->getMethodAddr(hMethod);
		unsigned int spec = context.trap->getMethodSpec(hMethod);

		//gen param
		if(term->args)
		{
			std::vector<PCode::Oprand*>& stmts = term->args->stmts;
			for(int i=0;i<(int)stmts.size();i++)
			{
				genCodeStmt(context,stmts[i],genSvr);
			}
		}
		Term* base =term->base? genCodeTerm(context,term->base,genSvr,maskRegister):0;
		if(!(spec & StorageSpec::static_) && hClass.kind==skType) //this call
		{
			if(base)
			{
				genSvr->regSvr->SpillReg(0);//spill ecx
				if(base->kind==Term::kAddress)
					pc = X86::MOV(pc, X86::ecx,*(X86::AddressX86<B32>*)base);
				else if(base->kind==Term::kRegister)
					pc = X86::MOV(pc, X86::ecx,*(Register<B32>*)base);
				else
					pc = X86::MOV(pc, X86::ecx,*(Imm<B32>*)base);
				if(spec & MethodSpec::native_)
					pc = X86::MOV(pc, X86::ecx,X86::AddressX86<B32>(X86::ecx,Disp<B8>(4)));
			}
			else
			{
				if(genSvr->regSvr->vars[0].reg==-1)
				{
					genSvr->regSvr->UseReg(0/*ecx*/,GenSvr::VarIndex(GenSvr::VarIndex::kVar,0));
					genSvr->regSvr->LoadData(0/*ecx*/,GenSvr::VarIndex(GenSvr::VarIndex::kVar,0));
				}
				if(spec & MethodSpec::native_)
				{
					genSvr->regSvr->SpillReg(0);//spill ecx
					pc = X86::MOV(pc, X86::ecx,X86::AddressX86<B32>(X86::ecx,Disp<B8>(4)));
				}
			}
			//this stdcall call
			if(spec & MethodSpec::stdcall_)
			{
				pc = X86::PUSH(pc,*(Register<B32>*)genSvr->regSvr->regs[0].regTerm);
			}
		}
		genSvr->regSvr->SpillCallerReg();
		pc = X86::MOV(pc,X86::eax,Imm<B32>((int)Addr));
		//pc = X86::CALL(pc,X86::AddressX86<B32>(X86::eax));
		pc = X86::CALL(pc,X86::eax);
		if(!(spec & MethodSpec::stdcall_) && spec & MethodSpec::native_)
		{
			if(context.trap->getMethodParamSize(hMethod))pc = X86::ADD(pc,X86::esp,Imm<B32>(context.trap->getMethodParamSize(hMethod)));
		}
		return (Term*)genSvr->regSvr->regs[1].regTerm; //eax
	}
	case PCode::cELEMENT:
		{
			PCode::ELEMENT * ele = (PCode::ELEMENT*)item;
			Term* base = genCodeTerm(context,ele->base,genSvr,maskRegister);
			int rx = genSvr->regSvr->LookupReg(base);
			if(rx!=-1) maskRegister |= 1<<rx;
			Term* rank = genCodeTerm(context,ele->rank,genSvr,maskRegister);
			int ry = genSvr->regSvr->LookupReg(rank);
			if(ry!=-1) maskRegister |= 1<<ry;
			int elesize = ele->eleSize;
			//load base to register
			if(rx ==-1)
			{
				rx = genSvr->regSvr->GetReg(maskRegister);
				 maskRegister |= 1<<rx;
				if(base->kind==Term::kAddress)
				{
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
						*(X86::AddressX86<B32>*)base);
				}
				else if(base->kind==Term::kImm)
				{
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
						*(Imm<B32>*)base);
				}
			}
			//当rank是地址时,装入数据到寄存器
			if(ry==-1 && rank->kind==Term::kAddress)
			{
				ry = genSvr->regSvr->GetReg(maskRegister);
				maskRegister |= 1<<ry;
				pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[ry].regTerm,
						*(X86::AddressX86<B32>*)rank);
			}

			//放弃临时变量对寄存器的引用
			if(ele->rank->cd==PCode::cItem)
			{
				GenSvr::VarIndex xIndex = genSvr->regSvr->GetVarIndex(context,(PCode::Item *)ele->rank);
				if(genSvr->isTmpVar(xIndex))genSvr->regSvr->UnUsedReg(xIndex);
			}
			if(ele->base->cd==PCode::cItem)
			{
				GenSvr::VarIndex xIndex = genSvr->regSvr->GetVarIndex(context,(PCode::Item *)ele->base);
				if(genSvr->isTmpVar(xIndex))genSvr->regSvr->UnUsedReg(xIndex);
			}

			if(elesize==1 || elesize==2|| elesize==4||elesize==8) 
			{
				if(ry!=-1)
					return new X86::AddressX86<B32>(
						*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
						*(Register<B32>*)genSvr->regSvr->regs[ry].regTerm,elesize);
				else //imm
					return new X86::AddressX86<B32>(*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,Disp<B32>(
					((Disp<B32>*)rank)->value*elesize));
			}
			else
			{
				if(ry==-1) //imm
					return new X86::AddressX86<B32>(*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
						Disp<B32>(((Disp<B32>*)rank)->value*elesize));
				else
				{
					pc = X86::IMUL(pc,*(Register<B32>*)genSvr->regSvr->regs[ry].regTerm,
							Imm<B32>(elesize));
					return new X86::AddressX86<B32>(
						*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
						*(Register<B32>*)genSvr->regSvr->regs[ry].regTerm,1);
				}
			}
		}
	case PCode::cINC:
	case PCode::cDEC:
	case PCode::cUINC:
	case PCode::cUDEC:
	{
		PCode::Oprand * node = ((PCode::INC*)item)->opr;
		
		assert(node->cd==PCode::cItem);
		PCode::Item * item1 = (PCode::Item*)node;
		GenSvr::VarIndex index = genSvr->regSvr->GetVarIndex(context,item1);
		GenSvr::VarItemBase & rhs =  genSvr->regSvr->GetVarItem(index);
		if(rhs.reg!=-1)
		{
			if(genSvr->regSvr->regs[rhs.reg].used.count > 1)
			{
				int r = genSvr->regSvr->GetReg();
				pc = X86::MOV(pc, *(Register<B32>*)genSvr->regSvr->regs[r].regTerm,
					*(Register<B32>*)genSvr->regSvr->regs[rhs.reg].regTerm);
				genSvr->regSvr->UnUsedReg(index);
				genSvr->regSvr->UseReg(r,index);
			}
		}
		Term* term = (Term*)(rhs.reg!=-1? genSvr->regSvr->regs[rhs.reg].regTerm:rhs.addrTerm); //genCodeTerm(context,node,genSvr);
		if(term->kind==Term::kRegister)
		{
			if(item->cd==PCode::cUINC || item->cd==PCode::cINC)
				pc = X86::INC(pc,*(Register<B32>*)term);
			else
				pc = X86::DEC(pc,*(Register<B32>*)term);
		}
		else
			if(item->cd==PCode::cUINC || item->cd==PCode::cINC)
				pc = X86::INC(pc,*(X86::AddressX86<B32>*)term);
			else
				pc = X86::DEC(pc,*(X86::AddressX86<B32>*)term);
		return term;
	}
	case PCode::cPUSH:
	{
		PCode::Oprand * node = ((PCode::PUSH*)item)->opr;
		HParam hParam = ((PCode::PUSH*)item)->hParam;
		unsigned int spec = context.trap->getParam(hParam)->spec;

		Term* term = 0;
		int maskRegister = 0;
		if(spec & ParamSpec::byref_)
		{
			if(node->cd==PCode::cItem || node->cd==PCode::cFIELD)
			{
				GenSvr::VarIndex xIndex= genSvr->regSvr->GetVarIndex(context,(PCode::Item*)node);
				GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
				assert(xItem);
				if(xItem->reg!=-1) genSvr->regSvr->UnUsedReg(xIndex);
				term = genCodeTerm(context,node,genSvr,maskRegister);
			}
			else if(node->cd==PCode::cELEMENT)
			{
				term = genCodeTerm(context,node,genSvr,maskRegister);
			}else
				assert(false);
			if(term->kind==Term::kRegister)
				pc = X86::PUSH(pc,*(Register<B32>*)term);
			else if(term->kind==Term::kAddress)
			{
				pc = X86::LEA(pc,X86::edx,*(X86::AddressX86<B32>*)term);
				pc = X86::PUSH(pc,X86::edx);
			}
			else 
				assert(false);
		}else
		{
			term = genCodeTerm(context,node,genSvr,maskRegister);
			if(term->kind==Term::kRegister)
				pc = X86::PUSH(pc,*(Register<B32>*)term);
			else if(term->kind==Term::kAddress)
				pc = X86::PUSH(pc,*(X86::AddressX86<B32>*)term);
			else if(term->kind==Term::kImm)
				pc = X86::PUSH(pc,*(Imm<B32>*)term);
			else //if(term->kind==Term::kFRegister)
				assert(false);
		}
		return term;
	}
	case PCode::cNEW:
	{
		PCode::NEW * node = (PCode::NEW *)item;
		HType hType = node->type;
		if(context.trap->isOleItem(hType))
		{
			/*
			PUSH(Imm(fn:symGetOleTypeInfo(typ)));
			PUSH(Imm(fn:symGetTypeOleName(typ)));
			MOV(eax,Fct(OleCreateInstanceEx));
			CALL(eax);
			ADD(esp,Imm(8));
			*/
			pc = X86::PUSH(pc,Imm<B32>((int)context.trap->getTypeOleTerm(hType)->typeInfo));
			wchar_t* name = util::getHashString(context.trap->getTypeNSOleTerm(hType)->oleName);
			pc = X86::PUSH(pc,Imm<B32>((int)name));
			pc = X86::MOV(pc,X86::eax,Imm<B32>((int)vlOle::OleCreateInstanceEx));
			pc = X86::CALL(pc,X86::eax);
			return (Term*)genSvr->regSvr->regs[1].regTerm; //eax
		}else
		{
			int msize = 0;
			int size = 	node->rank && context.trap->getTypeKind(hType)==TypeKind::tkClass?
					4 : context.trap->getAllocSize(hType,msize);
			pc = X86::PUSH(pc, Imm<B32>(size));
			if(node->rank)
			{
				if(node->rank->cd==PCode::cImm)
				{
					pc = X86::PUSH(pc,Imm<B32>(((PCode::Imm*)node->rank)->val_i32));
				}else
				{
					int maskRegister = 0;
					Term * term = genCodeTerm(context,node->rank,genSvr,maskRegister);
					if(term->kind == Term::kRegister)
						pc = X86::PUSH(pc,*(Register<B32>*)term);
					else
						pc = X86::PUSH(pc,*(X86::AddressX86<B32>*)term);
				}
			}else
				pc = X86::PUSH(pc,Imm<B32>(1));

			genSvr->regSvr->SpillCallerReg();
			pc = X86::MOV(pc,X86::eax,Imm<B32>((int)&calloc));
			pc = X86::CALL(pc,X86::eax);
			pc = X86::ADD(pc,X86::esp,Imm<B32>(8));
			return (Term*)genSvr->regSvr->regs[1].regTerm; //eax
		}
	}
	case PCode::cDEL:
	{
		PCode::DEL * node = (PCode::DEL *)item;
		int maskRegister = 0;
		Term * term = genCodeTerm(context,node->opr,genSvr,maskRegister);
		if(term->kind == Term::kRegister)
			pc = X86::PUSH(pc,*(Register<B32>*)term);
		else
			pc = X86::PUSH(pc,*(X86::AddressX86<B32>*)term);
		genSvr->regSvr->SpillCallerReg();
		pc = X86::MOV(pc,X86::eax,Imm<B32>((int)&free));
		pc = X86::CALL(pc,X86::eax);
		pc = X86::ADD(pc,X86::esp,Imm<B32>(4));
		return (Term*)&X86::eax;
	}
	case PCode::cPOP:
	{
		assert(false);
	}
	default:
		assert(false);
	}
	return 0;
}

Term* genCodeTerm(TContext & context,TNode * item, GenSvr* genSvr,int maskRegister=0)
{
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	switch(item->kind)
	{
	case kItem:
		{
			TNode* xNode = item;
			GenSvr::VarIndex xIndex = xNode->kind==kItem?genSvr->regSvr->GetVarIndex(context,(OpItem *)xNode):GenSvr::VarIndex();
			GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
			if(xItem->reg!=-1)
				return (Term*)genSvr->regSvr->regs[xItem->reg].regTerm;
			else
				return (Term*)xItem->addrTerm;
		}
	case kValue:
		return new Imm<B32>(((OpValue*)item)->val_i32);
	case kCall:
		{
			OpCall * term = (OpCall *)item;
			HMethod hMethod = term->mt->hType;
			HType hClass = (HType)context.trap->getParent(hMethod);
			void * Addr = context.trap->getMethodAddr(hMethod);
			unsigned int spec = context.trap->getMethodSpec(hMethod);

			if(!(spec & StorageSpec::static_) && hClass.kind==skType) //this call
			{
				if(genSvr->regSvr->vars[0].reg==-1)
				{
					genSvr->regSvr->UseReg(0/*ecx*/,GenSvr::VarIndex(GenSvr::VarIndex::kVar,0));
					genSvr->regSvr->LoadData(0/*ecx*/,GenSvr::VarIndex(GenSvr::VarIndex::kVar,0));
				}
				//this native spec

				//this stdcall call
				if(spec & MethodSpec::stdcall_)
				{
					pc = X86::PUSH(pc,*(Register<B32>*)genSvr->regSvr->regs[0].regTerm);
				}
			}
			genSvr->regSvr->SpillCallerReg();
			pc = X86::MOV(pc,X86::eax,Imm<B32>((int)Addr));
			//pc = X86::CALL(pc,X86::AddressX86<B32>(X86::eax));
			pc = X86::CALL(pc,X86::eax);
			return (Term *)&X86::eax;
		}
	case kThis:
		{
			GenSvr::VarIndex xIndex(GenSvr::VarIndex::kVar,0);
			GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
			if(xItem->reg!=-1)
				return (Term*)genSvr->regSvr->regs[xItem->reg].regTerm;
			else
				return (Term*)xItem->addrTerm;
		}
	case kInc:
		{
			OpInc * node = (OpInc *)((OpInc*)item)->expr;
			Term* term = genCodeTerm(context,node,genSvr);
			if(term->kind==Term::kRegister)
			{
				pc = X86::INC(pc,*(Register<B32>*)term);
			}
			else
				pc = X86::INC(pc,*(X86::AddressX86<B32>*)term);
			int reg = genSvr->regSvr->GetReg(maskRegister);
			if(term->kind==Term::kRegister)
				pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[reg].regTerm,*(Register<B32>*)term);
			else
				pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[reg].regTerm,*(X86::AddressX86<B32>*)term);
			return (Register<B32>*)genSvr->regSvr->regs[reg].regTerm;
		}
	case kDec:
		{
			OpInc * node = (OpInc *)((OpInc*)item)->expr;
			Term* term = genCodeTerm(context,node,genSvr);
			if(term->kind==Term::kRegister)
				pc = X86::DEC(pc,*(Register<B32>*)term);
			else
				pc = X86::DEC(pc,*(X86::AddressX86<B32>*)term);
			int reg = genSvr->regSvr->GetReg(maskRegister);
			if(term->kind==Term::kRegister)
				pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[reg].regTerm,*(Register<B32>*)term);
			else
				pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[reg].regTerm,*(X86::AddressX86<B32>*)term);
			return (Register<B32>*)genSvr->regSvr->regs[reg].regTerm;
		}
	case kUInc:
		{
			OpInc * node = (OpInc *)((OpInc*)item)->expr;
			Term* term = genCodeTerm(context,node,genSvr);
			if(term->kind==Term::kRegister)
				pc = X86::INC(pc,*(Register<B32>*)term);
			else
				pc = X86::INC(pc,*(X86::AddressX86<B32>*)term);
			return term;
		}
	case kUDec:
		{
			OpInc * node = (OpInc *)((OpInc*)item)->expr;
			Term* term = genCodeTerm(context,node,genSvr);
			if(term->kind==Term::kRegister)
				pc = X86::DEC(pc,*(Register<B32>*)term);
			else
				pc = X86::DEC(pc,*(X86::AddressX86<B32>*)term);
			return term;
		}
	}
	return 0;
}

Term* genCodeBinary(TContext & context, GenSvr* genSvr,OpBinary::Op op,Term* lhs,Term* rhs)
{
#define EmitOp2(cd,lhs,rhs) if(rhs->kind==Term::kRegister) \
			pc = X86::cd(pc,*(Register<B32>*)lhs,*(Register<B32>*)rhs); \
		else if(rhs->kind==Term::kAddress)	\
			pc = X86::cd(pc,*(Register<B32>*)lhs,*(X86::AddressX86<B32>*)rhs);	\
		else if(rhs->kind==Term::kImm)	\
			pc = X86::cd(pc,*(Register<B32>*)lhs,*(Imm<B32>*)rhs);

#define EmitSHx(cd,lhs,rhs) if(rhs->kind==Term::kImm)	\
			pc = X86::cd(pc,*(Register<B32>*)lhs,*(Imm<B8>*)rhs);	\
		else	\
		{	\
			pc = X86::MOV(pc,X86::edx,X86::ecx);	\
			if(rhs->kind==Term::kAddress)	\
			{	\
				pc = X86::MOV(pc, X86::ecx, *(X86::AddressX86<B32>*)rhs);	\
			}else	\
			{	\
				assert(rhs->kind==Term::kRegister);	\
				pc = X86::MOV(pc, X86::ecx, *(Register<B32>*)rhs);	\
			}	\
			pc = X86::cd(pc,*(Register<B32>*)lhs);	\
			pc = X86::MOV(pc,X86::ecx,X86::edx);	\
		}	


	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	switch(op)
	{
	case OpBinary::add:
		EmitOp2(ADD,lhs,rhs);
		break;
	case OpBinary::sub:
		EmitOp2(SUB,lhs,rhs);
		break;
	case OpBinary::mul:
		EmitOp2(MUL,lhs,rhs);
		//need eax
		break;
	case OpBinary::div:
		EmitOp2(DIV,lhs,rhs);
		//need eax
		break;
	case OpBinary::shl:
		//need cl
		EmitSHx(SHL,lhs,rhs);
		break;
	case OpBinary::shr:
		//need cl
		EmitSHx(SHR,lhs,rhs);
		break;
	case OpBinary::asn:
		assert(false);
		break;
	case OpBinary::mod:
		break;
	case OpBinary::band:
		EmitOp2(AND,lhs,rhs);
		break;
	case OpBinary::bor:
		EmitOp2(OR,lhs,rhs);
		break;
	case OpBinary::xor:
		EmitOp2(XOR,lhs,rhs);
		break;
	//case OpBinary::or
	//case OpBinary::and:
	//case OpBinary::eq:
	//case OpBinary::neq:
	//case OpBinary::gt:
	//case OpBinary::gte:
	//case OpBinary::lt:
	//case OpBinary::lte:
	}
	return 0;
}

Term* genCodeBinary(TContext & context,PCode::Oprand * item, GenSvr* genSvr)
{
	assert(false); //not reach
	return 0;
}

Term* genCodeBinary(TContext & context,TNode * item, GenSvr* genSvr)
{
	assert(false); //not reach
	return 0;
}

GenSvr::VarIndex GetTermVarIndex(TContext & context,PCode::Oprand * node, GenSvr* genSvr)
{
	if(node->cd==PCode::cFIELD && ((PCode::FIELD*)node)->hField.kind==skOleField)
		return GenSvr::VarIndex();
	if(node->cd==PCode::cFIELD&& ((PCode::FIELD*)node)->base->cd==PCode::cItem)
	{
		return genSvr->regSvr->SureFieldIndex(context,
			genSvr->regSvr->GetVarIndex(context,(PCode::Item*)((PCode::FIELD *)node)->base),
			((PCode::FIELD *)node)->hField);
	}
	return node->cd==PCode::cItem?genSvr->regSvr->GetVarIndex(context,(PCode::Item *)node):GenSvr::VarIndex();
}

Term* genCodeLet(TContext & context,PCode::Oprand * node, GenSvr* genSvr)
{
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	PCode::LET* term = (PCode::LET*)node;
	PCode::Oprand* rhs = term->rhs;
	PCode::Oprand* lhs = term->lhs;
	PCode::Oprand* dest = term->dest;
	Term * rhsTerm = 0;
	Term * lhsTerm = 0;
	Term * destTerm = 0;
	Term * result = 0;
	bool   wbreak = false;

	//item
	if(term->op != OpBinary::nil)
	{
		//x=y+z
		//y,z只可能是变量或直接数
		//x可能是变量、字段、数组元
		
		//1、如果x是变量，直接把计算结果存入与x变量对应寄存器即可。
		PCode::Oprand* xNode = dest;
		PCode::Oprand* yNode = lhs;
		PCode::Oprand* zNode = rhs;
		if(xNode->cd==PCode::cELEMENT)
		{
			//x=y+z
			int maskRegister = 0;
			Term * ty = genCodeTerm(context,lhs,genSvr,maskRegister);
			Term * tz = genCodeTerm(context,rhs,genSvr,maskRegister);
			

			//if(ty->kind!=Term::kRegister)
			//{
				int reg = genSvr->regSvr->GetReg(maskRegister);
				if(ty->kind==Term::kImm)
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[reg].regTerm,*(Imm<B32>*)ty);
				else if(ty->kind==Term::kRegister)
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[reg].regTerm,*(Register<B32>*)ty);
				else
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[reg].regTerm,*(X86::AddressX86<B32>*)ty);
				ty = (Term *)genSvr->regSvr->regs[reg].regTerm;
			//}
			
			genCodeBinary(context,genSvr,term->op,ty,tz);

			//free tmp register
			if(lhs->cd==PCode::cItem)
			{
				GenSvr::VarIndex xIndex = genSvr->regSvr->GetVarIndex(context,(PCode::Item *)lhs);
				if(genSvr->isTmpVar(xIndex))genSvr->regSvr->UnUsedReg(xIndex);
			}
			if(rhs->cd==PCode::cItem)
			{
				GenSvr::VarIndex xIndex = genSvr->regSvr->GetVarIndex(context,(PCode::Item *)rhs);
				if(genSvr->isTmpVar(xIndex))genSvr->regSvr->UnUsedReg(xIndex);
			}

			maskRegister = 1<<reg;
			Term * tx = genCodeTerm(context,dest,genSvr,maskRegister);
			if(ty->kind==Term::kImm)
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,*(Imm<B32>*)ty);
			else if(ty->kind==Term::kRegister)
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,*(Register<B32>*)ty);
			else //address
			{
				pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)ty);
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,X86::edx);
			}
			result = tx;
		}
		else
		{
			GenSvr::VarIndex xIndex = GetTermVarIndex(context,xNode,genSvr);
			GenSvr::VarIndex yIndex = GetTermVarIndex(context,yNode,genSvr);
			GenSvr::VarIndex zIndex = GetTermVarIndex(context,zNode,genSvr);

			GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
			GenSvr::VarItemBase * yItem = yIndex?&genSvr->regSvr->GetVarItem(yIndex):0;
			GenSvr::VarItemBase * zItem = zIndex?&genSvr->regSvr->GetVarItem(zIndex):0;
			OpBinary::Op op = term->op;

			int rx = xIndex?xItem->reg:-1;
			int ry = yIndex?yItem->reg:-1;
			int rz = zIndex?zItem->reg:-1;

			/*交换操作数
				情况一：x=y+x -> x=x+y这样可以直接使用rx为目标操作数,如add rx,ry，
					否则需要一个额外的寄存器暂存ry，产生如下代码序列: mov rt, ry; add rt,rx; mov rx,rt
				情况二: x=dy+z,
			*/
			if(OpBinary::canSwap(op) && ((xIndex == zIndex && xIndex != yIndex)
				|| (!yIndex && zIndex)))
			{
				std::swap(ry,rz);
				std::swap(yIndex,zIndex);
				std::swap(yItem,zItem);
				std::swap(zNode,yNode);
			}
			
			//选择合适的目标寄存器
			//假定左操作数必须为寄存器
			int reg = -1;
			//如x单独使用寄存器并且rz!=rx，则使用x的寄存器
			if(reg ==-1 && rx>=0 && !genSvr->isShared(rx) && rz!=rx)reg = rx;

			//如ry是临时变量并且使用寄存器，则选择该寄存器
			if(reg ==-1 && ry>=0 && !genSvr->isShared(ry) && genSvr->isTmpVar(yIndex))reg = ry;

			//如仍未有合适的目标寄存器，则申请一个除ry,rz外的寄存器。
			if(reg == -1) reg = genSvr->regSvr->GetReg((ry==-1?0:1<<ry)|(rz==-1?0:1<<rz));

			//对于mul和div操作，目标寄存器必须为eax。
			if(op==OpBinary::mul||op==OpBinary::div)
			{
				if(reg!=1/*eax*/)
				{
					genSvr->regSvr->SwapReg(1,reg);
					reg = 1;//eax
					
					//交换后y，z的状态可能发生了变化。
					yItem = yIndex?&genSvr->regSvr->GetVarItem(yIndex):0;
					zItem = zIndex?&genSvr->regSvr->GetVarItem(zIndex):0;
					ry = yIndex?yItem->reg:-1;
					rz = zIndex?zItem->reg:-1;
				}
			}

			rx = reg;

			//sure left oprand
			if(ry ==-1)
			{
				if(!genSvr->isTmpVar(yIndex))
				{
					//申请一个寄存器，把数据加载到寄存器以备后用。
					ry = genSvr->regSvr->GetReg((rx==-1?0:1<<rx)|(rz==-1?0:1<<rz));
					//load data to ry
					genSvr->regSvr->LoadData(ry,yIndex);
					genSvr->regSvr->UseReg(ry,yIndex);
				}else
				{
					//临时变量不再使用，直接从内存中加载数据到rx
					genSvr->regSvr->LoadData(rx,yIndex);
					ry = rx;
				}
			}
			if(ry != rx )
			{
				//rx <- ry
				pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
					*(Register<B32>*)genSvr->regSvr->regs[ry].regTerm);
			}
			lhsTerm = (Register<B32>*)genSvr->regSvr->regs[rx].regTerm;

			if(zIndex) 
			{//变量
				//寄存器有效
				if(zItem->reg!=-1)
					rhsTerm = (Register<B32>*)genSvr->regSvr->regs[zItem->reg].regTerm;
				else
				{
					if(zIndex.kind==GenSvr::VarIndex::kField)
					{
						int rz = genSvr->regSvr->GetReg((1<<rx) | (1<<ry));
						genSvr->regSvr->LoadData(rz,zIndex);
						rhsTerm=(Term*)genSvr->regSvr->regs[rz].regTerm;
					}else
						rhsTerm = (X86::AddressX86<B32>*)zItem->addrTerm;
				}

				genCodeBinary(context,genSvr,op,lhsTerm,rhsTerm);
				//释放变量所占的寄存器
				if(zIndex.kind == GenSvr::VarIndex::kTmpVar && zItem->reg)
					genSvr->regSvr->UnUsedReg(zIndex);
			}else
			{
				int maskRegister = rx==-1?0:1<<rx;
				rhsTerm = genCodeTerm(context,zNode,genSvr,maskRegister);
				genCodeBinary(context,genSvr,op,lhsTerm,rhsTerm);
			}

			if(xItem->reg !=-1 && xItem->reg != rx)
			{
				genSvr->regSvr->UnUsedReg(xIndex);
			}
			if(xItem->reg != rx /*&& rx != yItem->reg*/)genSvr->regSvr->UseReg(rx,xIndex);
			result = (Register<B32>*)genSvr->regSvr->regs[rx].regTerm;

			if(xNode->cd==PCode::cFIELD)
			{
				GenSvr::VarIndex xIndex = GetTermVarIndex(context,xNode,genSvr);
				GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
				if(xItem)
				{
					PCode::FIELD * tm = (PCode::FIELD*)xNode;
					if(tm->base && tm->base->cd==PCode::cItem && ((PCode::Item*)tm->base)->tempId >0)
					{
						if(xItem->reg!=-1)
						{
							genSvr->regSvr->StoreData(xItem->reg,xIndex);
							genSvr->regSvr->UnUsedReg(xIndex);
							GenSvr::VarIndex bIndex = GetTermVarIndex(context,tm->base,genSvr);
							if(bIndex)genSvr->regSvr->UnUsedReg(bIndex);
						}

					}
				}
			}
		}
	}
	else
	{
		if(dest->cd==PCode::cELEMENT)
		{
			//x = y
			int maskRegister = 0;
			Term * ty  = genCodeTerm(context,lhs,genSvr,maskRegister);
			Term * tx = genCodeTerm(context,dest,genSvr,maskRegister);
			if(ty->kind==Term::kImm)
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,*(Imm<B32>*)ty);
			else if(ty->kind==Term::kRegister)
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,*(Register<B32>*)ty);
			else //address
			{
				pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)ty);
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,X86::edx);
			}
			result = 0;

		}
		else if(lhs->cd == PCode::cFIELD && ((PCode::Item*)lhs)->hTerm.kind==skOleField)
		{
			//x = y
			int maskRegister = 0;
			Term * ty  = genCodeTerm(context,lhs,genSvr,maskRegister);
			Term * tx  = genCodeTerm(context,dest,genSvr,maskRegister);

			if(ty->kind==Term::kImm)
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,*(Imm<B32>*)ty);
			else if(ty->kind==Term::kRegister)
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,*(Register<B32>*)ty);
			else //address
			{
				pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)ty);
				pc = X86::MOV(pc,*(X86::AddressX86<B32>*)tx,X86::edx);
			}
			result = 0;

		}
		else if(lhs->cd == PCode::cItem || lhs->cd == PCode::cFIELD)
		{
			PCode::Item * item = (PCode::Item *)lhs;
			if(dest->cd == PCode::cItem || lhs->cd == PCode::cFIELD)
			{
				//x = x
				 if((int)item->hTerm && (int)((PCode::Item *)dest)->hTerm == (int)item->hTerm)  return 0;
				//x = y
				GenSvr::VarIndex lhsIndex = GetTermVarIndex(context,dest,genSvr);
				GenSvr::VarIndex rhsIndex = GetTermVarIndex(context,lhs,genSvr);
				GenSvr::VarItemBase * lhsItem = lhsIndex?&genSvr->regSvr->GetVarItem(lhsIndex):0;
				GenSvr::VarItemBase * rhsItem = rhsIndex?&genSvr->regSvr->GetVarItem(rhsIndex):0;
				//如果y的数据在寄存器上，则使用y寄存器。同时释放x寄存器。
				if(rhsItem && rhsItem->reg>=0)
				{
					if(lhsItem->reg>=0)genSvr->regSvr->UnUsedReg(lhsIndex);
					//如果y是临时变量，临时变量遵循用过即毁的原则，也没必要再保存。
					if(rhsIndex.kind==GenSvr::VarIndex::kTmpVar)
					{
						lhsItem->reg = rhsItem->reg;
						lhsItem->addr = false;
						genSvr->regSvr->regs[rhsItem->reg].used.varIndex[0] = lhsIndex;
						rhsItem->reg = -1;
					}
					else
					{
						genSvr->regSvr->UseReg(rhsItem->reg,lhsIndex);
						lhsItem->addr = false;
					}
					wbreak = true;

				}
				if(!wbreak)
				{
					//如果y未使用寄存器，如x单独使用寄存器，则使用x的寄存器，否则申请新的寄存器，
					//同时释放x寄存器(如使用)。
					//另外如果y是临时变量，则不需要把y添加到寄存器的使用表中。
					int useReg = -1;
					if(lhsItem && lhsItem->reg>=0)
					{
						if(genSvr->regSvr->regs[lhsItem->reg].used.count < 2)
						{
							useReg = lhsItem->reg;
						}
					}
					if(useReg==-1)useReg=genSvr->regSvr->GetReg();
					genSvr->regSvr->LoadData(useReg,rhsIndex);
					lhsItem->addr = false;
					if(rhsIndex.kind!=GenSvr::VarIndex::kTmpVar)
					{
						genSvr->regSvr->UseReg(useReg,rhsIndex);
						rhsItem->addr = true;
					}
					if(useReg!=lhsItem->reg)
					{
						if(lhsItem->reg!=-1)genSvr->regSvr->UnUsedReg(lhsIndex);
						genSvr->regSvr->UseReg(useReg,lhsIndex);
					}

					result = (Register<B32>*)genSvr->regSvr->regs[useReg].regTerm;
				}
			}else
				assert(false);
		}
		else if(lhs->cd == PCode::cImm)
		{
			GenSvr::VarIndex lhsIndex = GetTermVarIndex(context,dest,genSvr);
			GenSvr::VarItemBase & lhsItem = genSvr->regSvr->GetVarItem(lhsIndex);
			int useReg = -1;
			if(lhsItem.reg>=0)
			{
				if(genSvr->regSvr->regs[lhsItem.reg].used.count>1)
				{
					genSvr->regSvr->UnUsedReg(lhsIndex);
					lhsItem.reg = -1;
					lhsItem.addr = false;
				}else
					useReg = lhsItem.reg;
			}
			if(useReg ==-1)
				useReg = genSvr->regSvr->GetReg();
			genSvr->regSvr->UseReg(useReg,lhsIndex);
			pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[useReg].regTerm,Imm<B32>(((PCode::Imm*)lhs)->val_i32));
			result = (Register<B32>*)genSvr->regSvr->regs[useReg].regTerm;
		}
		else
		{
			int maskRegister = 0;
			rhsTerm = genCodeTerm(context,lhs,genSvr,maskRegister);
			GenSvr::VarIndex lhsIndex = GetTermVarIndex(context,dest,genSvr);
			GenSvr::VarItemBase & lhsItem = genSvr->regSvr->GetVarItem(lhsIndex);
			int useReg = -1;
			if(lhsItem.reg>=0)
			{
				if(genSvr->regSvr->regs[lhsItem.reg].used.count>1)
				{
					genSvr->regSvr->UnUsedReg(lhsIndex);
					lhsItem.reg = -1;
					lhsItem.addr = false;
				}else
					useReg = lhsItem.reg;
			}
			if(useReg ==-1)
			{
				useReg = genSvr->regSvr->LookupReg(rhsTerm);
				if(useReg!=-1)
				{
					if(genSvr->regSvr->regs[useReg].used.count >= 1) useReg = -1;
				}
				if(useReg ==-1)useReg = genSvr->regSvr->GetReg();
			}
			genSvr->regSvr->UseReg(useReg,lhsIndex);
			if(rhsTerm->kind == Term::kRegister)
			{
				if((Register<B32>*)genSvr->regSvr->regs[useReg].regTerm !=
					(Register<B32>*)rhsTerm)
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[useReg].regTerm,
						*(Register<B32>*)rhsTerm);
			}else
			{
				pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[useReg].regTerm,
					*(X86::AddressX86<B32>*)rhsTerm);
			}
			result = (Register<B32>*)genSvr->regSvr->regs[useReg].regTerm;
		}

		if(dest->cd==PCode::cFIELD)
		{
			GenSvr::VarIndex xIndex = GetTermVarIndex(context,dest,genSvr);
			GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
			if(xItem)
			{
				PCode::FIELD * tm = (PCode::FIELD*)dest;
				if(tm->base && tm->base->cd==PCode::cItem && ((PCode::Item*)tm->base)->tempId >0)
				{
					if(xItem->reg!=-1)
					{
						genSvr->regSvr->StoreData(xItem->reg,xIndex);
						genSvr->regSvr->UnUsedReg(xIndex);
						GenSvr::VarIndex bIndex = GetTermVarIndex(context,tm->base,genSvr);
						if(bIndex)genSvr->regSvr->UnUsedReg(bIndex);
					}

				}
			}
		}

	}
	return result;
}

Term* genCodeLet(TContext & context,TNode * node, GenSvr* genSvr)
{
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	OpBinary* term = (OpBinary*)node;
	assert(term->op==OpBinary::asn);
	TNode* rhs = term->rhs;
	TNode* lhs = term->lhs;
	Term * rhsTerm = 0;
	Term * lhsTerm = 0;
	Term * result = 0;

	//item
	switch(rhs->kind)
	{
	case kBinary:
		{
			//x=y+z
			TNode* xNode = lhs;
			TNode* yNode = ((OpBinary *)rhs)->lhs;
			TNode* zNode = ((OpBinary *)rhs)->rhs;
			GenSvr::VarIndex xIndex = xNode->kind==kItem?genSvr->regSvr->GetVarIndex(context,(OpItem *)xNode):GenSvr::VarIndex();
			GenSvr::VarIndex yIndex = yNode->kind==kItem?genSvr->regSvr->GetVarIndex(context,(OpItem *)yNode):GenSvr::VarIndex();
			GenSvr::VarIndex zIndex = zNode->kind==kItem?genSvr->regSvr->GetVarIndex(context,(OpItem *)zNode):GenSvr::VarIndex();

			GenSvr::VarItemBase * xItem = xIndex?&genSvr->regSvr->GetVarItem(xIndex):0;
			GenSvr::VarItemBase * yItem = yIndex?&genSvr->regSvr->GetVarItem(yIndex):0;
			GenSvr::VarItemBase * zItem = zIndex?&genSvr->regSvr->GetVarItem(zIndex):0;
			OpBinary::Op op = ((OpBinary *)rhs)->op;

			int rx = xIndex?xItem->reg:-1;
			int ry = yIndex?yItem->reg:-1;
			int rz = zIndex?zItem->reg:-1;

			//x=y+x x=dy+z,在可能的情况下交换操作数。 
			if(OpBinary::canSwap(op) && ((xIndex == zIndex && xIndex != yIndex)
				|| (!yIndex && zIndex)))
			{
				std::swap(ry,rz);
				std::swap(yIndex,zIndex);
				std::swap(yItem,zItem);
			}
			
			//假定左操作数必须为寄存器
			int reg = -1;
			//如x单独使用寄存器并且rz!=rx，则使用x的寄存器
			if(reg ==-1 && rx>=0 && genSvr->regSvr->regs[rx].used.count==1 && rz!=rx)reg = rx;
			//如ry是临时变量并且使用寄存器，则选择该寄存器
			if(reg ==-1 && ry>=0 && yIndex.kind==GenSvr::VarIndex::kTmpVar)reg = ry;

			if(reg == -1) reg = genSvr->regSvr->GetReg((ry==-1?0:1<<ry)|(rz==-1?0:1<<rz));

			if(op==OpBinary::mul||op==OpBinary::div)
			{
				if(reg!=1/*eax*/)
				{
					genSvr->regSvr->SwapReg(1,reg);
					reg = 1;//eax
					
					yItem = yIndex?&genSvr->regSvr->GetVarItem(yIndex):0;
					zItem = zIndex?&genSvr->regSvr->GetVarItem(zIndex):0;
					ry = yIndex?yItem->reg:-1;
					rz = zIndex?zItem->reg:-1;
				}
			}
			rx = reg;

			//sure left oprand
			if(ry ==-1)
			{
				if(yIndex.kind != GenSvr::VarIndex::kTmpVar)
				{
					ry = genSvr->regSvr->GetReg((rx==-1?0:1<<rx)|(rz==-1?0:1<<rz));
					//load data to ry
					genSvr->regSvr->LoadData(ry,yIndex);
					genSvr->regSvr->UseReg(ry,yIndex);

				}else
				{
					//rx <- [my]
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,*(X86::AddressX86<B32>*)yItem->addrTerm);
				}
			}
			if(ry!=-1)
			{
				if(ry != rx )
				{
					//rx <- ry
					pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
						*(Register<B32>*)genSvr->regSvr->regs[ry].regTerm);
				}
			}else 
			{
				if(yIndex)
					genSvr->regSvr->LoadData(rx,yIndex);
				else
				{
					//pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[rx].regTerm,
					//	genCodeTerm(context,zNode,genSvr));
				}
			}
			lhsTerm = (Register<B32>*)genSvr->regSvr->regs[rx].regTerm;

			if(zIndex)
			{
				if(zItem->reg!=-1)
					rhsTerm = (Register<B32>*)genSvr->regSvr->regs[zItem->reg].regTerm;
				else
					rhsTerm = (X86::AddressX86<B32>*)zItem->addrTerm;

				genCodeBinary(context,genSvr,op,lhsTerm,rhsTerm);
				if(zIndex.kind == GenSvr::VarIndex::kTmpVar && zItem->reg)
					genSvr->regSvr->UnUsedReg(zIndex);
			}else
			{
				rhsTerm = genCodeTerm(context,zNode,genSvr,(rx==-1?0:1<<rx));
				genCodeBinary(context,genSvr,op,lhsTerm,rhsTerm);
			}
			if(xItem->reg !=-1 && xItem->reg != rx)
			{
				genSvr->regSvr->UnUsedReg(xIndex);
			}
			if(xItem->reg != rx && rx != yItem->reg)genSvr->regSvr->UseReg(rx,xIndex);
			result = (Register<B32>*)genSvr->regSvr->regs[rx].regTerm;
			break;
		}
	case kItem:
		{
			OpItem * item = (OpItem *)rhs;
			if(lhs->kind==kItem)
			{
				//x = x
				 if((int)item->hTerm && (int)((OpItem *)lhs)->hTerm == (int)item->hTerm)  return 0;
				//x = y
				GenSvr::VarIndex lhsIndex = genSvr->regSvr->GetVarIndex(context,(OpItem *)lhs);
				GenSvr::VarIndex rhsIndex = genSvr->regSvr->GetVarIndex(context,(OpItem *)rhs);
				GenSvr::VarItemBase & lhsItem = genSvr->regSvr->GetVarItem(context,(OpItem *)lhs);
				GenSvr::VarItemBase & rhsItem = genSvr->regSvr->GetVarItem(context,(OpItem *)rhs);
				//如果y的数据在寄存器上，则使用y寄存器。同时释放x寄存器。
				if(rhsItem.reg>=0)
				{
					if(lhsItem.reg>=0)genSvr->regSvr->UnUsedReg(lhsIndex);
					//如果y是临时变量，临时变量遵循用过即毁的原则，也没必要再保存。
					if(rhsIndex.kind==GenSvr::VarIndex::kTmpVar)
					{
						lhsItem.reg = rhsItem.reg;
						lhsItem.addr = false;
						genSvr->regSvr->regs[rhsItem.reg].used.varIndex[0] = lhsIndex;
						rhsItem.reg = -1;
					}
					else
					{
						genSvr->regSvr->UseReg(rhsItem.reg,lhsIndex);
						lhsItem.addr = false;
					}
					break;

				}
				//如果y未使用寄存器，如x单独使用寄存器，则使用x的寄存器，否则申请新的寄存器，
				//同时释放x寄存器(如使用)。
				//另外如果y是临时变量，则不需要把y添加到寄存器的使用表中。
				int useReg = -1;
				if(lhsItem.reg>=0)
				{
					if(genSvr->regSvr->regs[lhsItem.reg].used.count>1)
					{
						genSvr->regSvr->UnUsedReg(lhsIndex);
					}else
					{
						useReg = rhsItem.reg;
					}
				}
				if(useReg==-1)useReg=genSvr->regSvr->GetReg();
				genSvr->regSvr->UseReg(useReg,lhsIndex);
				genSvr->regSvr->LoadData(useReg,rhsIndex);
				lhsItem.addr = false;
				if(lhsIndex.kind!=GenSvr::VarIndex::kTmpVar)
				{
					genSvr->regSvr->UseReg(useReg,rhsIndex);
					rhsItem.addr = true;
				}
				result = (Register<B32>*)genSvr->regSvr->regs[useReg].regTerm;
			}else
				assert(false);

			break;
		}
	case kValue:
		{
			GenSvr::VarIndex lhsIndex = genSvr->regSvr->GetVarIndex(context,(OpItem *)lhs);
			GenSvr::VarItemBase & lhsItem = genSvr->regSvr->GetVarItem(context,(OpItem *)lhs);
			int useReg = -1;
			if(lhsItem.reg>=0)
			{
				if(genSvr->regSvr->regs[lhsItem.reg].used.count>1)
				{
					genSvr->regSvr->UnUsedReg(lhsIndex);
					lhsItem.reg = -1;
					lhsItem.addr = false;
				}else
					useReg = lhsItem.reg;
			}
			if(useReg ==-1)
				useReg = genSvr->regSvr->GetReg();
			genSvr->regSvr->UseReg(useReg,lhsIndex);
			pc = X86::MOV(pc,*(Register<B32>*)genSvr->regSvr->regs[useReg].regTerm,Imm<B32>(((OpValue*)rhs)->val_i32));
			result = (Register<B32>*)genSvr->regSvr->regs[useReg].regTerm;
			break;
		}
	default:
		{
			rhsTerm = genCodeTerm(context,rhs,genSvr);
			result = rhsTerm;
			break;
		}
	}
	return result;
}

void* genCodeStmt(TContext & context,PCode::Oprand * node, GenSvr* genSvr)
{
	Term * result = 0;
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	if(node->cd==PCode::cVRET)
	{
		int maskRegister = 0;
		result = genCodeTerm(context,((PCode::VRET*)node)->opr,genSvr,maskRegister);
		if(((PCode::VRET*)node)->opr->cd==PCode::cItem &&
			((PCode::Item*)((PCode::VRET*)node)->opr)->tempId)
		{
			GenSvr::VarIndex varIndex = genSvr->regSvr->GetVarIndex(context,(PCode::Item*)((PCode::VRET*)node)->opr);
			genSvr->regSvr->UnUsedReg(varIndex);
		}
		if(result != (void*)&X86::eax)
		{
			if(result->kind == Term::kRegister)
			{
				if(X86::eax.value!= (*(Register<B32>*)result).value)
					pc = X86::MOV(pc,X86::eax,*(Register<B32>*)result);
			}
			else if(result->kind == Term::kAddress)
				pc = X86::MOV(pc,X86::eax,*(X86::AddressX86<B32>*)result);
			else if(result->kind == Term::kImm)
				pc = X86::MOV(pc,X86::eax,*(Imm<B32>*)result);
			else
				assert(false &&"error term kind");
		}
	}
	else if(node->cd==PCode::cLET)
	{
		result =  genCodeLet(context,node,genSvr);
	}else
	{
		int maskRegister = 0;
		result =  genCodeTerm(context,node,genSvr,maskRegister);
	}
	return result;
}

void* genCodeStmt(TContext & context,TNode * node, GenSvr* genSvr)
{
	Term * result = 0;
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	bool isArg = node->isArg;
	switch(node->kind)
	{
	case kReturn:
		if(((OpReturn*)node)->expr && (void*)&X86::eax!=result)
		{
			if(result->kind == Term::kRegister)
				pc = X86::MOV(pc,X86::eax,*(Register<B32>*)result);
			else if(result->kind == Term::kAddress)
				pc = X86::MOV(pc,X86::eax,*(X86::AddressX86<B32>*)result);
			else if(result->kind == Term::kImm)
				pc = X86::MOV(pc,X86::eax,*(Imm<B32>*)result);
			else
				assert(false &&"error term kind");
		}
		result = (Term *) &X86::eax;
		break;
	case kBinary: //let
		result =  genCodeLet(context,node,genSvr);
		break;
	default:
		result =  genCodeTerm(context,node,genSvr);
		break;
	}
	if(isArg )
	{
		assert(result);
		if(result->kind==Term::kRegister)
			pc = X86::PUSH(pc,*(Register<B32>*)result);
		else if(result->kind==Term::kAddress)
			pc = X86::PUSH(pc,*(X86::AddressX86<B32>*)result);
		else if(result->kind==Term::kImm)
			pc = X86::PUSH(pc,*(Imm<B32>*)result);
		else 
			assert(false);
	}
	return result;
}


int  EnterFrame(GenSvr* genSvr)
{
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	pc = X86::PUSH(pc,X86::ebp);
	pc = X86::MOV(pc,X86::ebp,X86::esp);
	//pc = X86::PUSH(pc,X86::esp);
	pc = X86::SUB(pc,X86::esp,Imm<B32>(0x128));
	pc = X86::PUSH(pc,X86::ebx);
	pc = X86::PUSH(pc,X86::esi);
	pc = X86::PUSH(pc,X86::edi);
	//save this
	//pc = X86::MOV(pc,X86::AddressX86<B32>(X86::ebp, Disp<B32>(-4)),X86::ecx);

	return 1;
}

int LeaveFrame(GenSvr* genSvr, int ret = 0)
{
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	//MOVD(xmm0,eax);
	//MOVD(eax,xmm0);
	
	pc = X86::POP(pc,X86::edi);
	pc = X86::POP(pc,X86::esi);
	pc = X86::POP(pc,X86::ebx);
	//pc = ADD(X86::esp,X86::Imm(0x128));
	pc = X86::MOV(pc,X86::esp,X86::ebp); 
	pc = X86::POP(pc,X86::ebp);
	pc = X86::RET(pc,Imm<B16>(ret));	
	return 0;
}

PCode::Oprand* genCode(TContext & context, PCode::Oprand* node, GenSvr* genSvr)
{
	if(!node)return 0;
	assert(node->cd==PCode::cBlock);
	assert(methodes.size());

	HMethod hMethod = methodes.back();

	unsigned char* addr = genSvr->coder->hCodeSpace->pc;
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	unsigned char* & code = genSvr->coder->hCodeSpace->code;
	PCode::Block * block = (PCode::Block*)node;
	PCode::Block * wblock  = 0;
	std::vector<PCode::Block*> blocks;

	blocks.push_back(block);
	EnterFrame(genSvr);

	int index = 0;
	while(index < (int)blocks.size())
	{
		wblock = blocks[index];
		wblock->startpc= pc - code;
		for(int i=0;i<(int)wblock->jmpSrc.size();i++)
		{
			*(int*)(code + wblock->jmpSrc[i])
				= pc - code - wblock->jmpSrc[i] - 4;
		}

		std::vector<PCode::Oprand*>& stmts = blocks[index]->stmts;
		for(int i=0;i<(int)stmts.size();i++)
		{
			if(stmts[i]->cd==PCode::cJMP)
			{
				wblock = ((PCode::JMP*)stmts[i])->dest;
				int wIndex = EnsureBlock(blocks,wblock);

				genSvr->regSvr->SpillRegs();
				pc = X86::JMP(pc,Imm<B32>(0));
				if(wIndex>index)
				{
					wblock->jmpSrc.push_back(pc - code - 4);
				}else
				{ 
					*(int*)(pc - 4) = code + wblock->startpc - pc;
				}
			}
			else if(stmts[i]->cd==PCode::cJCC)
			{
				wblock = ((PCode::JCC*)stmts[i])->tblock;
				int wIndex = EnsureBlock(blocks,wblock);
				
				PCode::Oprand * cond = ((PCode::JCC*)stmts[i])->cond;

#define		CMPBRANCH(x,Ty,cnd) case x: \
				{\
					Ty* exp = (Ty*)cond;	\
					int maskRegister = 0;\
					Term* lhs = genCodeTerm(context,exp->lhs,genSvr,maskRegister);	\
					Term* rhs = genCodeTerm(context,exp->rhs,genSvr,maskRegister);	\
					if(lhs->kind==Term::kRegister) \
					{	\
						if(rhs->kind==Term::kRegister)\
						{\
							pc = X86::CMP(pc,*(Register<B32>*)lhs,*(Register<B32>*)rhs);\
						}\
						else if(rhs->kind==Term::kAddress)\
						{\
							pc = X86::CMP(pc,*(Register<B32>*)lhs,*(X86::AddressX86<B32>*)rhs);\
						}\
						else if(rhs->kind==Term::kImm)\
						{\
							pc = X86::CMP(pc,*(Register<B32>*)lhs,*(Imm<B32>*)rhs);\
						}\
					}\
					else if(lhs->kind==Term::kAddress) \
					{	\
						if(rhs->kind==Term::kRegister)\
						{\
							pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)lhs);\
							pc = X86::CMP(pc,X86::edx,*(Register<B32>*)rhs);\
						}\
						else if(rhs->kind==Term::kAddress)\
						{\
							pc = X86::MOV(pc,X86::edx,*(X86::AddressX86<B32>*)lhs);\
							pc = X86::CMP(pc,X86::edx,*(X86::AddressX86<B32>*)rhs);\
						}\
						else if(rhs->kind==Term::kImm)\
						{\
							pc = X86::CMP(pc,*(X86::AddressX86<B32>*)lhs,*(Imm<B32>*)rhs);\
						}\
					}\
					else if(lhs->kind==Term::kImm) \
					{	\
						pc = X86::MOV(pc,X86::edx,*(Imm<B32>*)lhs);\
						\
						if(rhs->kind==Term::kRegister)\
						{\
							pc = X86::CMP(pc,X86::edx,*(Register<B32>*)rhs);\
						}\
						else if(rhs->kind==Term::kAddress)\
						{\
							pc = X86::CMP(pc,X86::edx,*(X86::AddressX86<B32>*)rhs);\
						}\
						else if(rhs->kind==Term::kImm)\
						{\
							pc = X86::CMP(pc,X86::edx,*(Imm<B32>*)rhs);\
						}\
					}\
					else \
					{	\
						assert(false); \
					}	\
					genSvr->regSvr->SpillRegs();\
					pc = X86::JCC(pc,cnd,Imm<B32>(0));\
					break;\
				}

				switch(cond->cd)
				{
					CMPBRANCH(PCode::cGT,	PCode::GT,	TCond::cGreater)
					CMPBRANCH(PCode::cGTE,	PCode::GTE,	TCond::cGreaterEqual)
					CMPBRANCH(PCode::cLT,	PCode::LT,	TCond::cLess)
					CMPBRANCH(PCode::cLTE,	PCode::LTE,	TCond::cLessEqual)
					CMPBRANCH(PCode::cEQ,	PCode::EQ,	TCond::cEqual)
					CMPBRANCH(PCode::cNEQ,	PCode::GT,	TCond::cNotEqual)
				default:
					assert(false);
					break;
				}

				if(wIndex>index){
					wblock->jmpSrc.push_back(pc - 4 - code);
				}else
				{
					*(int*)(pc - 4) =code + wblock->startpc - pc;
				}
				if(((PCode::JCC*)stmts[i])->fblock)
				{
					wblock = ((PCode::JCC*)stmts[i])->fblock;
					wIndex = EnsureBlock(blocks,((PCode::JCC*)stmts[i])->fblock);
					pc = X86::JMP(pc,Imm<B32>(0));
					if(wIndex>index){
						wblock->jmpSrc.push_back(pc - 4 - code);
					}
					else
					{
						*(int*)(pc - 4) = code + wblock ->startpc - pc;
					}
				}
			}
			else
			{ 
				genCodeStmt(context,stmts[i],genSvr);
				if(i==stmts.size() - 1)
					genSvr->regSvr->SpillRegs();
			}
		}
		index++;
	}

	LeaveFrame(genSvr, context.trap->getMethodParamSize(hMethod));

	if(pc > addr)
		context.trap->setMethodAddr(hMethod,addr);

	return 0;
}

TNode* genCode(TContext & context, TNode* node, GenSvr* genSvr)
{
	if(!node)return 0;
	assert(node->kind==kBlock);
	assert(methodes.size());

	HMethod hMethod = methodes.back();

	unsigned char* addr = genSvr->coder->hCodeSpace->pc;
	unsigned char* & pc = genSvr->coder->hCodeSpace->pc;
	unsigned char* & code = genSvr->coder->hCodeSpace->code;
	OpBlock * block = (OpBlock*)node;
	OpBlock * wblock  = 0;
	std::vector<OpBlock*> blocks;

	blocks.push_back(block);
	EnterFrame(genSvr);

	int index = 0;
	while(index < (int)blocks.size())
	{
		wblock = blocks[index];
		wblock->startpc= pc - code;
		for(int i=0;i<(int)wblock->jmpSrc.size();i++)
		{
			*(int*)(code + wblock->jmpSrc[i])
				= pc - code - wblock->jmpSrc[i] - 4;
		}

		OpList* stmts = blocks[index]->stmts;
		while(stmts)
		{
			if(stmts->node)
			{
				if(stmts->node->kind == kJmp)
				{
					wblock = ((OpJmp*)stmts->node)->dest;
					int wIndex = EnsureBlock(blocks,wblock);
					pc = X86::JMP(pc,Imm<B32>(0));
					if(wIndex>index)
					{
						wblock->jmpSrc.push_back(pc - code - 4);
					}else
					{ 
						*(int*)(pc - 4) = code + wblock->startpc - pc;
					}
				}
				else if(stmts->node->kind == kJcc)
				{
					wblock = ((OpJcc*)stmts->node)->tblock;
					int wIndex = EnsureBlock(blocks,wblock);
					pc = X86::JCC(pc,TCond::cNotZero,Imm<B32>(0));
					if(wIndex>index){
						wblock->jmpSrc.push_back(pc - 4 - code);
					}else
					{
						*(int*)(pc - 4) =code + wblock->startpc - pc;
					}
					if(((OpJcc*)stmts->node)->fblock)
					{
						wblock = ((OpJcc*)stmts->node)->fblock;
						wIndex = EnsureBlock(blocks,((OpJcc*)stmts->node)->fblock);
						pc = X86::JMP(pc,Imm<B32>(0));
						if(wIndex>index){
							wblock->jmpSrc.push_back(pc - 4 - code);
						}
						else
						{
							*(int*)(pc - 4) = code + wblock ->startpc - pc;
						}
					}
				}
				else
				{ 
					genCodeStmt(context,stmts->node,genSvr);
				}
			}
			stmts = stmts->next;
		}
		index++;
	}

	LeaveFrame(genSvr, context.trap->getMethodParamSize(hMethod));

	if(pc > addr)
		context.trap->setMethodAddr(hMethod,addr);

	return 0;
}

}//namespace complier
}//namespace trap