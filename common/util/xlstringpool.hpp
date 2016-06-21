#ifndef XLSTRINGPOOL_H
#define XLSTRINGPOOL_H

#include <vector>
#include "xldatapool.hpp"

namespace xl {
	
typedef struct StringEntry{
	const wchar_t* str;
	StringEntry* next;
} * HStringEntry;

typedef HStringEntry HSymbol;

class StringPool
{
public:
	StringPool() ;
	~StringPool();
public:
	HStringEntry Ensure(const wchar_t* str) ;
	HStringEntry Ensure(const wchar_t* str, int size);

public:
	const wchar_t* GetString(HStringEntry  h);
public:
	int HashCode(const wchar_t* str);
	int HashCode(const wchar_t* str, int size) ;
private:
	DataPool * dataPool;
	int bucketSize;
	HStringEntry*  buckets;
};


}	//namespace xl

#endif //XLSTRINGPOOL_H