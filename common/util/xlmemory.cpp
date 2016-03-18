#include "xlmemory.hpp"

using namespace xl;

SpaceWithCheckPoint::SpaceWithCheckPoint():
	blocks(new std::vector<unsigned char * >),
	blockUsed(0),
	blockSize(0x8000),
	blocksfree(new std::vector<unsigned char * >)
{
	blocks->push_back(new unsigned char[blockSize]);
}

SpaceWithCheckPoint::~SpaceWithCheckPoint()
{
	for(int i=0;i<(int)blocks->size();i++) 
	{
		if((*blocks)[i]) delete (*blocks)[i]; 
	}
	delete blocks;

	for(int i=0;i<(int)blocksfree->size();i++) 
	{
		if((*blocksfree)[i]) delete (*blocksfree)[i];
	}
	delete blocksfree;
}

unsigned char* SpaceWithCheckPoint::EnsureBuffuer(int len)
{
	if(this->blockUsed + len >= this->blockSize)
	{
		if(blocksfree->size())
		{
			blocks->push_back(blocksfree->back());
			blocksfree->pop_back();
		}
		else
			blocks->push_back(new unsigned char[blockSize]);
		blockUsed = 0;
	}
	blockUsed += len;
	return blocks->back() + blockUsed - len;
}

int SpaceWithCheckPoint::CheckPoint()
{
	FCheckPoint chkPoint;
	chkPoint.blockSize = blockSize ;
	chkPoint.blockUsed = blockUsed;
	chkPoint.indexBlock = (int)blocks->size();
	checkPoints.push_back(chkPoint);
	return 1;
}

int SpaceWithCheckPoint::RestorePoint()
{
	if(!checkPoints.size()) return -1;
	if(checkPoints.back().indexBlock > (int)blocks->size()) return -1;
	for(int i=checkPoints.back().indexBlock;  i< (int)blocks->size();i++)
	{
		if(blocksfree->size()<32)
		{
			blocksfree->push_back((*blocks)[i]);
			memset((*blocks)[i],0,blockSize);
		}
		else
			delete (*blocks)[i];
	}
	blockUsed = checkPoints.back().blockUsed;
	blockSize = checkPoints.back().blockSize;
	blocks->resize(checkPoints.back().indexBlock);
	if(blocks->size())
		memset(blocks->back() + blockUsed, 0,blockSize - blockUsed);
	checkPoints.pop_back();

	return 1;
}