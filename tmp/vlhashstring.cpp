#include "vlhashString.hpp"
#include <wchar.h>

namespace xlang {

vlHashString::vlHashString(void):pool_string_(new vlStringPool),_bucketsize(STR_BUCKET_SIZE),_count(0)
{
	_bucket.resize(_bucketsize);
	vlCHashStringItem item = {0,0};
	for(int i = 0; i < _bucketsize; i++)_bucket[i] = item;
}

vlHashString::~vlHashString(void)
{
	delete pool_string_;
}

int vlHashString::get_count()
{
	return _count;
}

int vlHashString::hash_value(const wchar_t* str)
{
	int index = 0;
	while (*str != '\0') {
		index = index << 1 ^ *str++;
	}
	index %= _bucketsize;
	if (index < 0) {
		index = -index;
	}
	return index + 1;
}

int vlHashString::hash_value(const wchar_t* str,int size)
{
	int index = 0;
	int i = 0;
	while (i++ < size) {
		index = index << 1 ^ *str++;
	}
	index %= _bucketsize;
	if (index < 0) {
		index = -index;
	}
	return index + 1;
}

int vlHashString::hash_token(const wchar_t* str,int size)
{
	if(!str)
		return 0;

	int h = hash_value(str,size);
	while(true)
	{
		if(!_bucket[h - 1].hItem)break;
		if(!_bucket[h - 1].hItem[size] && wcsncmp(_bucket[h - 1].hItem,str,size)==0)return h;
		if(!_bucket[h - 1].next)break;
		h = _bucket[h - 1].next;
	};
	wchar_t* ht = pool_string_->append_string(str,size);
	if(_bucket[h - 1].hItem)
	{
		vlCHashStringItem item = {ht,0};
		_bucket.push_back(item);
		_bucket[h - 1].next = (int)_bucket.size();
		h = (int)_bucket.size();
	}else
		_bucket[h - 1].hItem = ht;
	
	_count++;
	return h;
}

int vlHashString::hash_token(const wchar_t* str)
{
	if(!str)
		return 0;

	int h = hash_value(str);
	while(true)
	{
		if(!_bucket[h - 1].hItem)break;
		if(wcscmp(_bucket[h - 1].hItem,str)==0)return h;
		if(!_bucket[h - 1].next)break;
		h = _bucket[h - 1].next;
	};
	wchar_t* ht = pool_string_->append_string(str);
	if(_bucket[h - 1].hItem)
	{
		vlCHashStringItem item = {ht,0};
		_bucket.push_back(item);
		_bucket[h - 1].next = (int)_bucket.size();
		h = (int)_bucket.size();
	}else
		_bucket[h - 1].hItem = ht;
	
	_count++;
	return h;
}

int vlHashString::find_token(const wchar_t* str,int size)
{
	int h = hash_value(str,size);
	while(true)
	{
		if(!_bucket[h - 1].hItem)break;
		if(!_bucket[h - 1].hItem[size] && wcsncmp(_bucket[h - 1].hItem,str,size)==0)return h;
		if(!_bucket[h - 1].next)break;
		h = _bucket[h - 1].next;
	};
	return -1;
}

int vlHashString::find_token(const wchar_t* str)
{
	int h = hash_value(str);
	while(true)
	{
		if(!_bucket[h - 1].hItem)break;
		if(_wcsicmp(_bucket[h - 1].hItem,str)==0)return h;
		if(!_bucket[h - 1].next)break;
		h = _bucket[h - 1].next;
	};
	return -1;
}

wchar_t* vlHashString::get_string(int nIndex)
{
	if(nIndex < 1) return 0;
	return _bucket[nIndex -1].hItem;
}

} //namespace xlang
