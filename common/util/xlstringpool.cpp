#include "xlstringpool.hpp"
#include <tchar.h>

using namespace xl;

StringPool::StringPool() : dataPool(new DataPool),bucketSize(8192){
	int len = bucketSize * sizeof(HStringEntry);
	void * p = dataPool->Alloc(len);
	buckets = (HStringEntry *)p;
}
StringPool::~StringPool() { 
	delete dataPool;
}

HStringEntry StringPool::Insert(const wchar_t* str) { return Insert(str,wcslen(str)); } 
HStringEntry StringPool::Insert(const wchar_t* str, int size) {
	if(!str)	return 0;

	int h = HashCode(str,size);
	
	HStringEntry e = buckets[h];
	while(e)
	{
		if(e->str && !e->str[size] && wcsncmp(e->str,str,size)==0) return e;
		e = e->next;
	}
	
	//not found then append string to data pool and Insert a bucket
	str = dataPool->Append(str,size);
	e = (HStringEntry)dataPool->Alloc(sizeof(StringEntry));
	e->str = str;
	e->next = buckets [h];
	buckets[h] = e;
	
	return buckets[h];
}

const wchar_t* StringPool::GetString(HStringEntry  h) { return h->str; }

int StringPool::HashCode(const wchar_t* str) { return HashCode(str,wcslen(str)); }
int StringPool::HashCode(const wchar_t* str, int size) {
	int index = 0;
	int i = 0;
	while (i++ < size) {
		index = index << 1 ^ *str++;
	}
	index %= bucketSize;
	if (index < 0) 	index = -index;
	return index + 1;
}