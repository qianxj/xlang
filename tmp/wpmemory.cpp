#include "wpmemory.hpp"

using namespace trap::complier;

SpaceWithCheckPoint::SpaceWithCheckPoint():
	blocks_(new std::vector<unsigned char * >),
	blockUsed_(0),
	blockSize_(0x8000),
	blocksfree_(new std::vector<unsigned char * >)
{
	blocks_->push_back(new unsigned char[blockSize_]);
}

SpaceWithCheckPoint::~SpaceWithCheckPoint()
{
	for(int i=0;i<(int)blocks_->size();i++) 
	{
		if((*blocks_)[i]) delete (*blocks_)[i]; 
	}
	delete blocks_;

	for(int i=0;i<(int)blocksfree_->size();i++) 
	{
		if((*blocksfree_)[i]) delete (*blocksfree_)[i];
	}
	delete blocksfree_;
}

unsigned char* SpaceWithCheckPoint::EnsureBuffuer(int len)
{
	if(this->blockUsed_ + len >= this->blockSize_)
	{
		if(blocksfree_->size())
		{
			blocks_->push_back(blocksfree_->back());
			blocksfree_->pop_back();
		}
		else
			blocks_->push_back(new unsigned char[blockSize_]);
		blockUsed_ = 0;
	}
	blockUsed_ += len;
	return blocks_->back() + blockUsed_ - len;
}

int SpaceWithCheckPoint::CheckPoint()
{
	FCheckPoint chkPoint;
	chkPoint.blockSize_ = blockSize_ ;
	chkPoint.blockUsed_ = blockUsed_;
	chkPoint.indexBlock = (int)blocks_->size();
	checkPoints_.push_back(chkPoint);
	return 1;
}

int SpaceWithCheckPoint::RestorePoint()
{
	if(!checkPoints_.size()) return -1;
	if(checkPoints_.back().indexBlock > (int)blocks_->size()) return -1;
	for(int i=checkPoints_.back().indexBlock;  i< (int)blocks_->size();i++)
	{
		if(blocksfree_->size()<32)
		{
			blocksfree_->push_back((*blocks_)[i]);
			memset((*blocks_)[i],0,blockSize_);
		}
		else
			delete (*blocks_)[i];
	}
	blockUsed_ = checkPoints_.back().blockUsed_;
	blockSize_ = checkPoints_.back().blockSize_;
	blocks_->resize(checkPoints_.back().indexBlock);
	if(blocks_->size())
		memset(blocks_->back() + blockUsed_, 0,blockSize_ - blockUsed_);
	checkPoints_.pop_back();

	return 1;
}