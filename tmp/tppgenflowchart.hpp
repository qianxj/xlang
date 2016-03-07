#ifndef STPP_GEN_H
#define STPP_GEN_H

#include <vector>
#include <map>

namespace trap {
namespace complier {

struct BaseBlock
{
	int start;
	int last;
	BaseBlock():start(-1),last(-1){}
};

struct ExprState
{
	static const unsigned int isconst		= 0x1;
	static const unsigned int hasvar		= 0x2;
	static const unsigned int hasdouble		= 0x3;
	static const unsigned int hascall		= 0x4;
	static const unsigned int isassign		= 0x5;
	static const unsigned int hasinc		= 0x6;	// or dec
	static const unsigned int hasuinc		= 0x7;	// or udec
	static const unsigned int hasjmp		= 0x8;	// || && !
	static const unsigned int hasdef		= 0x9;	//define variant
};

struct FlowChart
{
	std::vector<SNode *> stmts;
	std::vector<BaseBlock*> blocks;
	std::vector<std::vector<int>*> succ;
	std::vector<std::vector<int>*> piror;

	int enterblock;
	int exitblock;

	std::vector<int> expstate;
	std::vector<int> regs;

	std::vector<int> uses;
	std::vector<int> defs;
	
	std::vector<int> vars;
	std::map<int,int> mvars;

	std::vector<int> sorts;
	std::vector<int> vsorts;

	FlowChart()
	{
		blocks.push_back(new BaseBlock);
		piror.push_back(0);
		succ.push_back(0);

		blocks.push_back(new BaseBlock);
		piror.push_back(0);
		succ.push_back(0);

		enterblock = 0;
		exitblock = 1;
	}
};

//return curblock
int MakeFlowChart(FlowChart* flowChart, int curBlock, SNode * astmt,int loopEnter = -1,int loopExit = -1);
int visitBlocks(FlowChart* flowChart);
int DFSBlocks(FlowChart* flowChart,int index,int& n,std::vector<bool>& marks, bool usesucc = false);
int topsortBlocks(FlowChart* flowChart);

} // namespace complier
} // namespace trap


#endif //STPP_GEN_H