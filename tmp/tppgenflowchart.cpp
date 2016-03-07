#include "tppsymnode.hpp"
#include "tppgenflowchart.hpp"
#include "vlutils.hpp"
#include <assert.h>

using namespace trap::complier;

int setBlockPiror(FlowChart* flowChart,int follow, int piror)
{
	if(follow < 0 || piror < 0) return -1;
	if(!flowChart->piror[follow]) 
		flowChart->piror[follow] = new std::vector<int>;
	flowChart->piror[follow]->push_back(piror);
	
	if(!flowChart->succ[piror]) 
		flowChart->succ[piror] = new std::vector<int>;
	flowChart->succ[piror]->push_back(follow);

	return 1;
}

int makeBlock(FlowChart* flowChart)
{
	BaseBlock * tBlock = new BaseBlock;	
	flowChart->blocks.push_back(tBlock);
	flowChart->piror.push_back(0);
	flowChart->succ.push_back(0);
	return (int)flowChart->blocks.size() - 1;
}
int makeBlock(FlowChart* flowChart,int piror)
{
	int index = makeBlock(flowChart);
	if(piror==-1) return index;
	setBlockPiror(flowChart,index,piror);
	return index;
}

int makeBlock(FlowChart* flowChart,std::vector<int>& pirors)
{
	int index = makeBlock(flowChart);
	if(pirors.size()<1) return index;
	for(int i =0;i<(int)pirors.size();i++)
	{
		int piror = pirors[i];
		setBlockPiror(flowChart,index,piror);
	}
	return index;
}



bool updateBlock(FlowChart* flowChart, int curBlock)
{
	if(flowChart->blocks[curBlock]->start==-1)
	{
		flowChart->blocks[curBlock]->start = (int)flowChart->stmts.size() - 1;
		flowChart->blocks[curBlock]->last = flowChart->blocks[curBlock]->start;
	}else
		flowChart->blocks[curBlock]->last++;
	return true;
}

int trap::complier::MakeFlowChart(FlowChart* flowChart, int curBlock, SNode * astmt,int loopEnter,int loopExit)
{
	SNode * stmt = astmt;
	switch(stmt->kind)
	{
		case kIf:
			{
				flowChart->stmts.push_back(((OpIf*)stmt)->cond);
				updateBlock(flowChart,curBlock);

				std::vector<int> pirors;
				if(((OpIf*)stmt)->tbody)
				{
					int block = makeBlock(flowChart,curBlock);
					block = MakeFlowChart(flowChart,block,((OpIf*)stmt)->tbody,loopEnter,loopExit);
					pirors.push_back(block);
				}
				
				if(((OpIf*)stmt)->fbody)
				{
					int block = makeBlock(flowChart,curBlock);
					block = MakeFlowChart(flowChart,block,((OpIf*)stmt)->fbody,loopEnter,loopExit);
					pirors.push_back(block);
				}

				curBlock = makeBlock(flowChart,pirors);
			}
			break;
		case kFor:
			{
				flowChart->stmts.push_back(((OpFor*)stmt)->init);
				updateBlock(flowChart,curBlock);

				int cond = makeBlock(flowChart,curBlock);
				flowChart->stmts.push_back(((OpFor*)stmt)->cond);
				updateBlock(flowChart,cond);

				int body = makeBlock(flowChart,cond);
				int inc = makeBlock(flowChart,body);
				curBlock = makeBlock(flowChart,cond);

				body = MakeFlowChart(flowChart,body,((OpFor*)stmt)->body,inc,curBlock);
				flowChart->stmts.push_back(((OpFor*)stmt)->inc);
				setBlockPiror(flowChart,cond,inc);
			}
			break;
		case kWhile:
			{
				int cond = makeBlock(flowChart,curBlock);
				flowChart->stmts.push_back(((OpWhile*)stmt)->cond);
				updateBlock(flowChart,cond);
				
				curBlock = makeBlock(flowChart,cond);

				int body = makeBlock(flowChart,cond);
				body = MakeFlowChart(flowChart,body,((OpWhile*)stmt)->body,cond,curBlock);

				setBlockPiror(flowChart,cond,body);
			}
			break;
		case kUntil:
			{
				int body = makeBlock(flowChart,curBlock);

				int cond = makeBlock(flowChart,body);
				flowChart->stmts.push_back(((OpWhile*)stmt)->cond);
				updateBlock(flowChart,cond);

				curBlock = makeBlock(flowChart,cond);

				body = MakeFlowChart(flowChart,body,((OpUntil*)stmt)->body,cond,curBlock);

				setBlockPiror(flowChart,body,cond);
			}
			break;
		case kReturn:
			if(((OpReturn*)stmt)->expr)
			{
				flowChart->stmts.push_back(((OpReturn*)stmt)->expr);
				updateBlock(flowChart,curBlock);
				setBlockPiror(flowChart,flowChart->exitblock,curBlock);
			}
			curBlock = makeBlock(flowChart,curBlock);
			break;
		case kContinue:
			setBlockPiror(flowChart,loopEnter,curBlock);
			curBlock = makeBlock(flowChart,curBlock);
			break;
		case kBreak:
			setBlockPiror(flowChart,loopExit,curBlock);
			curBlock = makeBlock(flowChart,curBlock);
			break;
		case kCompStmt:
			{
				/*OpCompStmt * cstmt = (OpCompStmt *)stmt;
				for(int i=0;i<(int)cstmt->stmts.size();i++)
				{
					curBlock = MakeFlowChart(flowChart,curBlock,cstmt->stmts[i],loopEnter,loopExit);
				}*/
			}
			break;
		case kExprStmt:
			flowChart->stmts.push_back(stmt);
			updateBlock(flowChart,curBlock);
			break;
		case kNew:
		case kDelete:
			flowChart->stmts.push_back(stmt);
			updateBlock(flowChart,curBlock);
			break;
		case kVar:
			if(((OpVar*)stmt)->expr)
			{
				flowChart->stmts.push_back(stmt);
				updateBlock(flowChart,curBlock);
			}
			break;
		default:
			//expr
			flowChart->stmts.push_back(stmt);
			updateBlock(flowChart,curBlock);
			break;
	}
	return curBlock;
};

int visitBlock(FlowChart* flowChart,int curIndex,SNode* node)
{
	switch(node->kind)
	{
		case kExprStmt:
			{
				OpExprStmt* expr = (OpExprStmt*)node;
				return visitBlock(flowChart,curIndex,expr->expr);
			}
		case kItem:
			flowChart->uses[curIndex]++;
			return 1;
		case kThis:
			return 1;
		case kSuper:
			return 1;
		case kValue:
			{
				OpValue * value = (OpValue *)node;
				if(value->vt==OpValue::D32 || value->vt==OpValue::D64 )
					flowChart->expstate[curIndex] |= ExprState::hasdouble;
			}
			return 1;
		case kNill:
			return 1;
		case kField:
			return 1;
		case kElement:
			return 1;
		case kMethod:
			return 1;
		case kCall:
			flowChart->expstate[curIndex] |= ExprState::hascall;
			return 1;
		case kArgList:
			break;
		case kType:
			break;
		case kInc:
		case kDec:
			flowChart->expstate[curIndex] |= ExprState::hasinc;
			return visitBlock(flowChart,curIndex,((OpInc*)node)->expr);
		case kUInc:
		case kUDec:
			flowChart->expstate[curIndex] |= ExprState::hasuinc;
			return visitBlock(flowChart,curIndex,((OpUInc*)node)->expr);
		case kNew:
			return 1;
		case kDelete:
			return 0;
		case kNot:
			flowChart->expstate[curIndex] |= ExprState::hasjmp;
			return visitBlock(flowChart,curIndex,((OpNot*)node)->expr);
		case kBNot:
			return visitBlock(flowChart,curIndex,((OpBNot*)node)->expr);
		case kNeg:
			return visitBlock(flowChart,curIndex,((OpNeg*)node)->expr);
		case kCast:
			return visitBlock(flowChart,curIndex,((OpCast*)node)->expr);
		case kBinary:
			{
				OpBinary* expr = (OpBinary*)node;
				switch(expr->op)
				{
				case OpBinary::asn:
					flowChart->expstate[curIndex] |= ExprState::isassign;
				case OpBinary::or:
				case OpBinary::and:
					flowChart->expstate[curIndex] |= ExprState::hasjmp;
				break;
				}
				int lhs = visitBlock(flowChart,curIndex,expr->lhs);
				int rhs = visitBlock(flowChart,curIndex,expr->rhs);
				return lhs==rhs? lhs + 1 : lhs<rhs ? rhs : lhs;
			}
			break;
		case kVar:
			{
				flowChart->expstate[curIndex] |= ExprState::hasdef;
				OpVar* expr = (OpVar*)node;
				flowChart->vars.push_back(expr->ident);
				return visitBlock(flowChart,curIndex,expr->expr)+1;
			}
			break;
		default:
			assert(false); //not reach hear
			break;
	}
	return -99;
}

int trap::complier::visitBlocks(FlowChart* flowChart)
{
	flowChart->expstate.resize(flowChart->stmts.size());
	flowChart->regs.resize(flowChart->stmts.size());
	flowChart->uses.resize(flowChart->stmts.size());
	flowChart->defs.resize(flowChart->stmts.size());

	for(int i=0;i<(int)flowChart->stmts.size();i++)
	{
		int regs = visitBlock(flowChart,i,flowChart->stmts[i]);
		flowChart->regs[i] = regs;
	}
	return 1;
}

int trap::complier::DFSBlocks(FlowChart* flowChart, int index,int& n,std::vector<bool>& marks, bool usesucc)
{
	if(!marks[index])
	{
		marks[index] = true;
		if(usesucc)
		{
			if(flowChart->succ[index])
			{
				for(int i=0;i<(int)flowChart->succ[index]->size();i++)
				{
					DFSBlocks(flowChart,(*flowChart->succ[index])[i],n,marks,usesucc);
				}
			}
			flowChart->sorts[n - 1] = index;
		}else
		{
			if(flowChart->piror[index])
			{
				for(int i=0;i<(int)flowChart->piror[index]->size();i++)
				{
					DFSBlocks(flowChart,(*flowChart->piror[index])[i],n,marks,usesucc);
				}
			}
			flowChart->vsorts[n - 1] = index;
		}
		n--;
	}

	return 1;
}

int trap::complier::topsortBlocks(FlowChart* flowChart)
{
	int n =  (int)flowChart->blocks.size();
	std::vector<bool> marks(n,false);
	std::vector<bool> marks1(n,false);
	flowChart->sorts.resize(n);
	flowChart->vsorts.resize(n);

	DFSBlocks(flowChart,flowChart->enterblock,n,marks,true);
	n =  (int)flowChart->blocks.size();
	DFSBlocks(flowChart,flowChart->enterblock,n,marks1,false);

	return 1;
}


