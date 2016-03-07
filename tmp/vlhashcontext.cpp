#include "vlhashcontext.hpp"
#include "vlutils.hpp"

using namespace xlang;

vlHashContext::vlHashContext(void):_bucketsize(STR_BUCKET_SIZE),_count(0),_hashString(NULL)
{
	_bucket.resize(_bucketsize);
	for(int_t i = 0; i < _bucketsize; i++)_bucket[i] = 0;
	_hashString = util::getGlobalHashString();
}

vlHashContext::~vlHashContext(void)
{
}

void vlHashContext::set_hash_string(vlHashString* hashString)
{
	_hashString = hashString;
}

vlHashString* vlHashContext::get_hash_string()
{
	return _hashString;
}

int_t vlHashContext::hash_value(const wchar_t* str)
{
	int_t index = 0;
	while (*str != '\0') {
		index = index << 1 ^ *str++;
	}
	index %= _bucketsize;
	if (index < 0) {
		index = -index;
	}
	return index + 1;
}

int_t vlHashContext::get_count()
{
	return _count;
}

void vlHashContext::enter_scope()
{
	_countCurrents.push(0);
}

void vlHashContext::leave_scope()
{
	if(_countCurrents.size()<=1) return ;

	//pop current word scope block
	int_t count = _countCurrents.top();
	_countCurrents.pop();

	while(count>0)
	{
		int_t nIndex = _currents.back();
		_currents.pop_back();
		if(_items[nIndex - 1].piror < 0  ) //firstnode
		{
			_bucket[- _items[nIndex - 1].piror - 1] = _items[nIndex - 1].next;
		}else
		{
			_items[_items[nIndex - 1].piror - 1].next = _items[nIndex - 1].next;
		}
		if(_items[nIndex - 1].next)
			_items[_items[nIndex - 1].next - 1].piror = _items[nIndex - 1].piror;
		//enter this node to freelist
		_freelist.push(nIndex);
		//next node
		count--;
	}
}

int_t vlHashContext::push_word(int_t  nWord,int_t ide)
{
	int_t nHashIndex = hash_value(_hashString->get_string(ide));
	int_t nIndex = 0;
	//need a node
	if(_freelist.size()>0)
	{
		nIndex = _freelist.top();
		_freelist.pop();
	}else
	{
		vlCHashContextItem item;
		this->_items.push_back(item);
		nIndex = (int_t)_items.size() ;
	}

	_items[nIndex - 1].nWord = nWord;
	_items[nIndex - 1].ide = ide;
	_items[nIndex - 1].piror = - nHashIndex;
	_items[nIndex - 1].next = _bucket[nHashIndex - 1];
	if(_bucket[nHashIndex - 1])_items[_bucket[nHashIndex - 1] - 1].piror = nIndex;
	_bucket[nHashIndex - 1] = nIndex;

	_currents.push_back(nIndex);
	_countCurrents.top() += 1;
	return nIndex;
}

int_t vlHashContext::push_word(ptr_t  nWord,int_t ide)
{
	int_t nHashIndex = hash_value(_hashString->get_string(ide));
	int_t nIndex = 0;
	//need a node
	if(_freelist.size()>0)
	{
		nIndex = _freelist.top();
		_freelist.pop();
	}else
	{
		vlCHashContextItem item;
		this->_items.push_back(item);
		nIndex = (int_t)_items.size() ;
	}

	_items[nIndex - 1].ptr = nWord;
	_items[nIndex - 1].ide = ide;
	_items[nIndex - 1].piror = - nHashIndex;
	_items[nIndex - 1].next = _bucket[nHashIndex - 1];
	if(_bucket[nHashIndex - 1])_items[_bucket[nHashIndex - 1] - 1].piror = nIndex;
	_bucket[nHashIndex - 1] = nIndex;

	_currents.push_back(nIndex);
	_countCurrents.top() += 1;
	return nIndex;
}

int_t vlHashContext::push_word(int_t  w,const wstring_t str)
{
	return push_word(w,_hashString->hash_token(str));
}

int_t vlHashContext::push_word(ptr_t  w,const wstring_t str)
{
	return push_word(w,_hashString->hash_token(str));
}

int_t  vlHashContext::get_word(int_t nIndex)
{
	if(nIndex < 1) return int_t();
	return _items[nIndex - 1].nWord;
}

ptr_t  vlHashContext::get_ptr(int_t nIndex)
{
	if(nIndex < 1) return int_t();
	return _items[nIndex - 1].ptr;
}

int_t vlHashContext::find_token(int ide)
{
	int_t nHashIndex = hash_value(_hashString->get_string(ide));
	int_t nIndex = _bucket[nHashIndex - 1]; 
	while(nIndex > 0)
	{
		if(_items[nIndex - 1].ide == ide) return nIndex;
		nIndex = _items[nIndex - 1].next;
	}
	return -1;
}

ptr_t vlHashContext::lookup_currentscope(int ide)
{
	int_t count = _countCurrents.top();
	if(count == 0) return 0;
	int start = (int)_currents.size() - count ;
	for(int i=start; i<(int)_currents.size();i++)
	{
		if(_items[_currents[i] - 1].ide == ide) return _items[_currents[i]].ptr;
	}
	return 0;
}

int_t vlHashContext::find_token(const wstring_t str)
{
	int_t ide = _hashString->hash_token(str);
	int_t nHashIndex = hash_value(_hashString->get_string(ide));

	int_t nIndex = _bucket[nHashIndex - 1]; 
	while(nIndex > 0)
	{
		if(_items[nIndex - 1].ide == ide) return nIndex;
		nIndex = _items[nIndex - 1].next;
	}
	return -1;
}

int_t vlHashContext::find_next_token(int_t nIndex,const wstring_t str)
{
	return find_next_token(nIndex,_hashString->hash_token(str));
}

int_t vlHashContext::find_next_token(int_t nIndex, int_t ide)
{
	nIndex = _items[nIndex - 1].next;
	while(nIndex > 0)
	{
		if(_items[nIndex - 1].ide == ide) return nIndex;
		nIndex = _items[nIndex - 1].next;
	}
	return -1;
}
