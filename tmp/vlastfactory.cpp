#include "vlastfactory.hpp"
#include "vlplatform.hpp"

using namespace xlang;

vlAstFactory::vlAstFactory():blocks_(new std::vector<unsigned char * >),blockUsed_(0),
	blockSize_(0x8000),blocksfree_(new std::vector<unsigned char * >)
{
	blocks_->push_back(new unsigned char[blockSize_]);
}

vlAstFactory::~vlAstFactory()
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

unsigned char * vlAstFactory::EnsureBuffuer(int len)
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

vlAstNode * vlAstFactory::CreateNode(const wchar_t* nodeName)
{
	//os_t::print(L"%s",nodeName);
	return 0;
}

vlAstNode * vlAstFactory::CreateAstNode(vlAstKind kind)
{
	;
	return new (EnsureBuffuer(sizeof(vlAstNode))) vlAstNode(kind);
}

vlAstIdentifier * vlAstFactory::CreateAstIdentifier(vlAstKind kind,int ide)
{
	return new (EnsureBuffuer(sizeof(vlAstIdentifier))) vlAstIdentifier(kind,ide);
}

vlAstIdentifier * vlAstFactory::CreateAstIdentifier(vlAstKind kind,int ide,vlToken &token)
{
	vlAstIdentifier * pNode =  new (EnsureBuffuer(sizeof(vlAstIdentifier))) vlAstIdentifier(kind,ide);
	vlToken * tk = new (EnsureBuffuer(sizeof(vlToken))) vlToken(token);
	pNode->setToken(tk);
	return pNode;
}

vlAstIdentifier * vlAstFactory::CreateAstIdentifier(int ide,vlToken &token)
{
	vlAstIdentifier * pNode =  new (EnsureBuffuer(sizeof(vlAstIdentifier))) vlAstIdentifier(ide);
	vlToken * tk = new (EnsureBuffuer(sizeof(vlToken))) vlToken(token);
	pNode->setToken(tk);
	return pNode;
}

vlAstToken * vlAstFactory::CreateAstToken(vlAstKind kind,vlToken &token)
{
	return new (EnsureBuffuer(sizeof(vlAstToken))) vlAstToken(kind,token);
}
vlAstToken * vlAstFactory::CreateAstToken(vlToken &token)
{
	return new (EnsureBuffuer(sizeof(vlAstToken))) vlAstToken(token);
}
vlAstMissToken * vlAstFactory::CreateAstMissToken(int missKind,vlToken &token)
{
	return new (EnsureBuffuer(sizeof(vlAstMissToken))) vlAstMissToken(missKind,token);
}

vlAstAxis * vlAstFactory::CreateAxis(vlAstNode * node)
{
	return new (EnsureBuffuer(sizeof(vlAstAxis))) vlAstAxis(node);
}

vlAstDataBuffer* vlAstFactory::CreateAstDataBuffer() 
{
	return new (EnsureBuffuer(sizeof(vlAstDataBuffer))) vlAstDataBuffer();
}

vlAstTypeData* vlAstFactory::CreateAstTypeData()
{
	return new (EnsureBuffuer(sizeof(vlAstTypeData))) vlAstTypeData();
}


	std::vector<unsigned char * >* blocksfree_;
	struct FCheckPoint
	{
		int blockUsed_;
		int blockSize_ ;
		int indexBlock;
	};
int vlAstFactory::checkpoint()
{
	FCheckPoint chkPoint;
	chkPoint.blockSize_ = blockSize_ ;
	chkPoint.blockUsed_ = blockUsed_;
	chkPoint.indexBlock = (int)blocks_->size();
	checkPoints_.push_back(chkPoint);
	return 1;
}
int vlAstFactory::restorepoint()
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
	

