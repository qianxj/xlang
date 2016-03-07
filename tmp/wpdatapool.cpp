#include "wpdatapool.hpp"
#include <assert.h>
#include <memory.h>
#include <wchar.h>
using namespace trap::complier;


DataPool::DataPool() :space_alloc_(new std::vector<ptr_data_size_t>),space_free_(new std::vector<ptr_data_size_t>)
	,data_count_(0),data_used_(0),alloc_size_(0),free_size_(0){}
		
DataPool::~DataPool(){
	std::vector<ptr_data_size_t>::const_iterator iter = space_alloc_->begin();
	for(;iter!=space_alloc_->end();iter++) {
		free((*iter).ptr_);
	}
	space_alloc_->clear();
}

void* DataPool::append(const void * ptr_data, int size) { 
	void * p = ensureSpace(size);
	memcpy(p,ptr_data,size);
	return p;
}
void* DataPool::alloc( int size) {
	void * p =  ensureSpace(size);
	memset(p,0, size);
}
int* DataPool::append(int v) {
	return (int*)append(&v,sizeof(v));
}
double* DataPool::append(double v) {
	return (double*)append(&v,sizeof(v));
}
wstring_t  DataPool::append(wstring_t str, int len) {
	append(len);
	return (wstring_t)append((const void *)str, (len + 1)* sizeof(str[0]));
}
wstring_t  DataPool::append(wstring_t str) {
	return append(str,wcslen(str));
}

void * DataPool::ensureSpace(int size)
{
	assert(size > 0);
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
		allocSpace(size);
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
void * DataPool::allocSpace(int size)
{
	int alloc_size = ((size + 4096 -1 ) / 4096)* 4096;
	void * p = calloc(1,alloc_size);

	space_alloc_->push_back(ptr_data_size_t(p,alloc_size));
	space_free_->push_back(ptr_data_size_t(p,alloc_size));

	free_size_  += alloc_size;
	alloc_size_ += alloc_size;

	return p;
}