#include "xlscope.hpp"

using namespace xl;

#define STR_BUCKET_SIZE 0x4000
Scope::Scope(void):bucketsize(STR_BUCKET_SIZE),count(0),stringPool(NULL)
{
	bucket.resize(bucketsize);
	for(int i = 0; i < bucketsize; i++)bucket[i] = 0;
	//stringPool = util::getGlobalHashString();
}

Scope::~Scope(void)
{
}

void Scope::SetStringPool(StringPool* hashString)
{
	stringPool = hashString;
}

StringPool* Scope::GetStringPool()
{
	return stringPool;
}

int Scope::HashCode(const wchar_t* str)
{
	int index = 0;
	while (*str != '\0') {
		index = index << 1 ^ *str++;
	}
	index %= bucketsize;
	if (index < 0) {
		index = -index;
	}
	return index + 1;
}

int Scope::GetCount()
{
	return count;
}

void Scope::EnterScope()
{
	countCurrents.push(0);
}

void Scope::LeaveScope()
{
	if(countCurrents.size()<=1) return ;

	//pop current word scope block
	int count = countCurrents.top();
	countCurrents.pop();

	while(count>0)
	{
		int nIndex = currents.back();
		currents.pop_back();
		if(items[nIndex - 1].piror < 0  ) //firstnode
		{
			bucket[- items[nIndex - 1].piror - 1] = items[nIndex - 1].next;
		}else
		{
			items[items[nIndex - 1].piror - 1].next = items[nIndex - 1].next;
		}
		if(items[nIndex - 1].next)
			items[items[nIndex - 1].next - 1].piror = items[nIndex - 1].piror;
		//enter this node to freelist
		freelist.push(nIndex);
		//next node
		count--;
	}
}

int Scope::Push(int  nWord,HSymbol ide)
{
	int nHashIndex = HashCode(stringPool->GetString(ide));
	int nIndex = 0;
	//need a node
	if(freelist.size()>0)
	{
		nIndex = freelist.top();
		freelist.pop();
	}else
	{
		Item item;
		items.push_back(item);
		nIndex = (int)items.size() ;
	}

	items[nIndex - 1].nWord = nWord;
	items[nIndex - 1].ide = ide;
	items[nIndex - 1].piror = - nHashIndex;
	items[nIndex - 1].next = bucket[nHashIndex - 1];
	if(bucket[nHashIndex - 1])items[bucket[nHashIndex - 1] - 1].piror = nIndex;
	bucket[nHashIndex - 1] = nIndex;

	currents.push_back(nIndex);
	countCurrents.top() += 1;
	return nIndex;
}

int Scope::Push(void*  nWord,HSymbol ide)
{
	int nHashIndex = HashCode(stringPool->GetString(ide));
	int nIndex = 0;
	//need a node
	if(freelist.size()>0)
	{
		nIndex = freelist.top();
		freelist.pop();
	}else
	{
		Item item;
		this->items.push_back(item);
		nIndex = (int)items.size() ;
	}

	items[nIndex - 1].ptr = nWord;
	items[nIndex - 1].ide = ide;
	items[nIndex - 1].piror = - nHashIndex;
	items[nIndex - 1].next = bucket[nHashIndex - 1];
	if(bucket[nHashIndex - 1])items[bucket[nHashIndex - 1] - 1].piror = nIndex;
	bucket[nHashIndex - 1] = nIndex;

	currents.push_back(nIndex);
	countCurrents.top() += 1;
	return nIndex;
}

int Scope::Push(int  w,const wchar_t* str)
{
	return Push(w,stringPool->Insert(str));
}

int Scope::Push(void*  w,const wchar_t* str)
{
	return Push(w,stringPool->Insert(str));
}

int  Scope::GetWord(int nIndex)
{
	if(nIndex < 1) return int();
	return items[nIndex - 1].nWord;
}

void*  Scope::GetPtr(int nIndex)
{
	if(nIndex < 1) return int();
	return items[nIndex - 1].ptr;
}

int Scope::FindToken(HSymbol ide)
{
	int nHashIndex = HashCode(stringPool->GetString(ide));
	int nIndex = bucket[nHashIndex - 1]; 
	while(nIndex > 0)
	{
		if(items[nIndex - 1].ide == ide) return nIndex;
		nIndex = items[nIndex - 1].next;
	}
	return -1;
}

void* Scope::LookupCurrentScope(HSymbol ide)
{
	int count = countCurrents.top();
	if(count == 0) return 0;
	int start = (int)currents.size() - count ;
	for(int i=start; i<(int)currents.size();i++)
	{
		if(items[currents[i] - 1].ide == ide) return items[currents[i]].ptr;
	}
	return 0;
}

int Scope::FindToken(const wchar_t* str)
{
	HSymbol ide = stringPool->Insert(str);
	int nHashIndex = HashCode(stringPool->GetString(ide));

	int nIndex = bucket[nHashIndex - 1]; 
	while(nIndex > 0)
	{
		if(items[nIndex - 1].ide == ide) return nIndex;
		nIndex = items[nIndex - 1].next;
	}
	return -1;
}

int Scope::FindNextToken(int nIndex,const wchar_t* str)
{
	return FindNextToken(nIndex,stringPool->Insert(str));
}

int Scope::FindNextToken(int nIndex, HSymbol ide)
{
	nIndex = items[nIndex - 1].next;
	while(nIndex > 0)
	{
		if(items[nIndex - 1].ide == ide) return nIndex;
		nIndex = items[nIndex - 1].next;
	}
	return -1;
}