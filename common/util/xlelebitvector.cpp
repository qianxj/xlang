#include <assert.h>
#include "xlelebitvector.hpp"

using namespace xl;

BitElementVector::BitElementVector(int elementLength,int length)
	: length(length),
	elementLength(elementLength),
	elementDataLength(1 + (elementLength - 1) / 32),
	dataLength(SizeFor(elementLength,length)),
	data(new int[SizeFor(elementLength,length)])
{
		assert(length > 0);
		Clear();
}

BitElementVector::BitElementVector(const BitElementVector& other)
	: length(other.GetLength()),
	elementLength(other.GetElementLength()),
	dataLength(SizeFor(other.elementLength,other.length)),
	data(new int[SizeFor(other.elementLength,other.length)]) {
		CopyFrom(other); 
}

BitElementVector::~BitElementVector()
{
	if(data)delete[] data;
}

int BitElementVector::SizeFor(int elementLength,int length) {
	return (1 + ((elementLength - 1) / 32))*length;
}

BitElementVector& BitElementVector::operator=(const BitElementVector& rhs) {
	if (this != &rhs) CopyFrom(rhs);
	return *this;
}

void BitElementVector::CopyFrom(const BitElementVector& other) {
	assert(other.GetLength() <= GetLength());
	for (int i = 0; i < other.dataLength; i++) {
		data[i] = other.data[i];
	}
	for (int i = other.dataLength; i < dataLength; i++) {
		data[i] = 0;
	}
}

bool BitElementVector::Get(int element, int i) const {
	assert(element>=0 && element < GetLength());
	assert(i >= 0 && i < GetElementLength() );

	return (data[element * elementDataLength + i / 32] & (1U << (i % 32)))!=0;
}

inline void BitElementVector::Add(int element, int i,bool b) {
	assert(element>=0 && element < GetLength());
	assert(i >= 0 && i < GetElementLength() );

	data[element * elementDataLength + i / 32] |= ((b?1U:0U) << (i % 32));
}

void BitElementVector::Remove(int element, int i) {
	assert(element>=0 && element < GetLength());
	assert(i >= 0 && i < GetElementLength() );

	data[element * elementDataLength + i / 32] &= ~(1U << (i % 32));
}

void BitElementVector::Union(int element1,int element2) {
	assert(element1>=0 && element1 < GetLength());
	assert(element2>=0 && element2 < GetLength());

	for (int i = 0; i < elementDataLength; i++) {
		data[element1 * elementDataLength + i] |= 
			data[element2 * elementDataLength + i];
	}
}
void BitElementVector::Union(int element1,BitElementVector & other, int element2) {
	assert(element1>=0 && element1 < GetLength());
	assert(element2>=0 && element2 < other.GetLength());
	assert(elementDataLength == other.elementDataLength);

	for (int i = 0; i < elementDataLength; i++) {
		data[element1 * elementDataLength + i] |= 
			other.data[element2 * elementDataLength + i];
	}
}
void BitElementVector::UnionTo(int element1,BitVector & other) {
	assert(element1>=0 && element1 < GetLength());
	assert(elementDataLength == other.GetDataLength());

	for (int i = 0; i < elementDataLength; i++) {
		other.GetData()[i] |= 
			data[element1 * elementDataLength + i];
	}
}

bool BitElementVector::UnionIsChanged(int element1,int element2) {
	assert(element1>=0 && element1 < GetLength());
	assert(element2>=0 && element2 < GetLength());
	bool changed = false;
	for (int i = 0; i < elementDataLength; i++) {
		int olddata = data[element1 * elementDataLength+i];
		data[element1 * elementDataLength+i] |= 
			data[element2 * elementDataLength+i];
		if (data[element1 * elementDataLength+i] != olddata) changed = true;
	}
	return changed;
}

void BitElementVector::Intersect(int element1,int element2) {
	assert(element1>=0 && element1 < GetLength());
	assert(element2>=0 && element2 < GetLength());
	for (int i = 0; i < elementDataLength; i++) {
		data[element1 * elementDataLength+i] &=
			data[element2 * elementDataLength+i];
	}
}

void BitElementVector::Subtract(int element1,int element2) {
	assert(element1>=0 && element1 < GetLength());
	assert(element2>=0 && element2 < GetLength());
	for (int i = 0; i < elementDataLength; i++) {
		data[element1 * elementDataLength+i] &=
			~data[element2 * elementDataLength+i];
	}
}

void BitElementVector::Clear() {
	for (int i = 0; i < dataLength; i++) {
		data[i] = 0;
	}
}

bool BitElementVector::IsEmpty() const {
	for (int i = 0; i < dataLength; i++) {
		if (data[i] != 0) return false;
	}
	return true;
}

bool BitElementVector::IsEmpty(int element) const {
	assert(element>=0 && element < GetLength());
	for (int i = 0; i < elementDataLength; i++) {
		if (data[element * elementDataLength + i] != 0) return false;
	}
	return true;
}

bool BitElementVector::Equals(int element1,int element2) {
	assert(element1>=0 && element1 < GetLength());
	assert(element2>=0 && element2 < GetLength());
	for (int i = 0; i < elementDataLength; i++) {
		if(data[element1 * elementDataLength+i] !=
			~data[element2 * elementDataLength+i]) return false;
	}
	return true;
}

#define BitElementVectorGet(element,i) ((data[element * elementDataLength + i / 32] & (1U << (i % 32)))!=0)		
#define BitElementVectorAdd(element,i,b) (data[element * elementDataLength + i / 32] |= ((b?1U:0U) << (i % 32)))
void BitElementVector::TransmitClosure(int pSorts[],int &count)
{
	assert(length == elementLength);
	int len = GetLength();
	
	bool* pFlag  = new bool[len];
	int top = 0 ;
	while(top<len){pFlag[top++]=false;}
	pSorts[0] = 0;
	pFlag[0] = true;
	top = 0;
	int index = -1;
	int topbase = top;
	int tokenIndex = 0;
	while(true)
	{
		while(index < top)
		{
			index++;
			int current = pSorts[index];
			for(int k=0;k<len;k++) 
			{

				if(!pFlag[k] && BitElementVectorGet(current,k))
				{
					pFlag[k] = true;
					pSorts[++top] = k;
				}
			}
		}
		
		while(true)
		{
			bool notneedloop = true;
			index = top;
			while(index >= topbase) 
			{
				bool changed = true;
				int current = pSorts[index];
				while(changed)
				{
					changed = false;
					for(int k=0;k<len;k++) 
					{
						if(BitElementVectorGet(current,k))
						{
							if(UnionIsChanged(current,k))
							{
								changed = true; 
								notneedloop = false;
							}
						}
					};
				}
				index--;
			}
			if(notneedloop)break; //检查环路
		}

		//添加一个分隔
		while(tokenIndex <len && pFlag[tokenIndex])tokenIndex++;
		if(tokenIndex<len)
		{
			//计算下一个闭包集合
			pFlag[tokenIndex] = true;
			top++;
			topbase = top;
			pSorts[top] = tokenIndex;
			index = top - 1;
		}else
			break;
	}
	//delete pSorts;
	delete pFlag;
	count = top + 1;
}

#undef BitElementVectorGet
#undef BitElementVectorAdd
