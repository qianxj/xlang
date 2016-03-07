#include "vlDataPool.hpp"
#include "vlplatform.hpp"
#include "vldebuger.hpp"

using namespace xlang;

vlDataPool::vlDataPool():space_alloc_(new std::vector<ptr_data_size_t>),space_free_(new std::vector<ptr_data_size_t>)
	,data_count_(0),data_used_(0),alloc_size_(0),free_size_(0)
{

}

vlDataPool::~vlDataPool()
{
	std::vector<ptr_data_size_t>::const_iterator iter = space_alloc_->begin();
	for(;iter!=space_alloc_->end();iter++)
	{
		os_t::heap_free((*iter).ptr_);
	}
	space_alloc_->clear();
}

void * vlDataPool::append_data(const void * ptr_data, int size)
{
	void * p = ensure_space(size);
	os_t::mem_copy(p,ptr_data,size);
	return p;
}

void * vlDataPool::ensure_space(int size)
{
	ASSERT(size > 0);
	std::vector<ptr_data_size_t>::iterator iter = space_free_->begin();
	std::vector<ptr_data_size_t>::iterator iter_prev = space_free_->end();
	int bsize = 0;
	for(;iter!=space_free_->end();iter++)
	{
		int csize = (*iter).size_ - size;
		if(csize < 0) continue;
		iter_prev = iter;
		if(csize == 0) break;
		if(csize < bsize)bsize = csize;
	}

	if(iter_prev == space_free_->end())
	{ //no space to fit
		vlDataPool::alloc_space(size);
		iter_prev =  --space_free_->end();
		bsize = (*iter_prev).size_ - size;
	}
	void * p = (*iter_prev).ptr_;
	if(bsize < 16) 
		space_free_->erase(iter_prev);
	else
	{
		(*iter_prev).size_ = bsize;
		(*iter_prev).ptr_  = ((unsigned char *)(*iter_prev).ptr_) + size;
	}

	free_size_ -= size;
	data_used_ += size;
	data_count_+= 1;

	return p;
}

void * vlDataPool::alloc_space(int size)
{
	int alloc_size = ((size + os_t::pageSize() -1 ) / os_t::pageSize())* os_t::pageSize();
	void * p = os_t::heap_alloc(0,alloc_size);

	space_alloc_->push_back(ptr_data_size_t(p,alloc_size));
	space_free_->push_back(ptr_data_size_t(p,alloc_size));

	free_size_  += alloc_size;
	alloc_size_ += alloc_size;

	return p;
}
