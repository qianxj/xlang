#include "wpstringpool.hpp"
#include <tchar.h>

using namespace trap::complier;

StringPool::StringPool() : data_pool_(new DataPool),bucketsize_(4096){
	int len = bucketsize_ * sizeof(HStringEntry);
	void * p = data_pool_->alloc(len);
	buckets_ = (HStringEntry *)p;
}
StringPool::~StringPool() { 
	delete data_pool_;
}

HStringEntry StringPool::insert(wstring_t str) { return insert(str,wcslen(str)); } 
HStringEntry StringPool::insert(wstring_t str, int size) {
	if(!str)	return 0;

	int h = hashCode(str,size);
	
	HStringEntry e = buckets_[h];
	while(e)
	{
		if(e->str_ && !e->str_[size] && wcsncmp(e->str_,str,size)==0) return e;
		e = e->next_;
	}
	
	//not found then append string to data pool and insert a bucket
	str = data_pool_->append(str,size);
	e = (HStringEntry)data_pool_->alloc(sizeof(StringEntry));
	e->str_ = str;
	e->next_ = buckets_ [h];
	buckets_[h] = e;
	
	return buckets_[h];
}

wstring_t StringPool::getString(HStringEntry  h) { return h->str_; }

int StringPool::hashCode(wstring_t str) { return hashCode(str,wcslen(str)); }
int StringPool::hashCode(wstring_t str, int size) {
	int index = 0;
	int i = 0;
	while (i++ < size) {
		index = index << 1 ^ *str++;
	}
	index %= bucketsize_;
	if (index < 0) 	index = -index;
	return index + 1;
}