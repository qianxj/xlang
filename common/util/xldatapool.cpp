#include "xldatapool.hpp"
#include <assert.h>
#include <memory.h>
#include <wchar.h>

using namespace xl;


DataPool::DataPool() :spaceAlloced(new std::vector<PtrDataSize >),spaceFree(new std::vector<PtrDataSize>)
	,dataCount(0),dataUsed(0),allocSize(0),freeSize(0){}
		
DataPool::~DataPool(){
	std::vector<PtrDataSize>::const_iterator iter = spaceAlloced->begin();
	for(;iter!=spaceAlloced->end();iter++) {
		free((*iter).ptr);
	}
	spaceAlloced->clear();
}

void* DataPool::Append(const void * ptr_data, int size) { 
	void * p = ensureSpace(size);
	memcpy(p,ptr_data,size);
	return p;
}

void* DataPool::Alloc( int size) {
	void * p =  ensureSpace(size);
	memset(p,0, size);
	return p;
}

int* DataPool::Append(int v) {
	return (int*)Append(&v,sizeof(v));
}

double* DataPool::Append(double v) {
	return (double*)Append(&v,sizeof(v));
}

wchar_t* DataPool::Append(wchar_t* str, int len) {
	Append(len);
	return (wchar_t*)Append((const void *)str, (len + 1)* sizeof(str[0]));
}
wchar_t*  DataPool::Append(wchar_t* str ) {
	return Append(str,wcslen(str));
}

void * DataPool::ensureSpace(int size)
{
	assert(size > 0);
	std::vector<PtrDataSize>::iterator iter = spaceFree->begin();
	std::vector<PtrDataSize>::iterator iter_prev = spaceFree->end();
	int bsize = 0;
	for(;iter!=spaceFree->end();iter++)
	{
		int asize = (*iter).size - size;
		if(asize < 0) continue;
		iter_prev = iter;
		if(asize == 0) break;
		if(asize < bsize)bsize = asize;
	}

	if(iter_prev == spaceFree->end())
	{ //no space to fit
		allocSpace(size);
		iter_prev =  --spaceFree->end();
		bsize = (*iter_prev).size - size;
	}
	void * p = (*iter_prev).ptr;
	if(bsize < 16) 
		spaceFree->erase(iter_prev);
	else
	{
		(*iter_prev).size = bsize;
		(*iter_prev).ptr  = ((unsigned char *)(*iter_prev).ptr) + size;
	}

	freeSize -= size;
	dataUsed += size;
	dataCount+= 1;

	return p;
}
void * DataPool::allocSpace(int size)
{
	int alloc_size = ((size + 4096 -1 ) / 4096)* 4096;
	void * p = calloc(1,alloc_size);

	spaceAlloced->push_back(PtrDataSize(p,alloc_size));
	spaceFree->push_back(PtrDataSize(p,alloc_size));

	freeSize  += alloc_size;
	allocSize += alloc_size;
	
	return p;
}