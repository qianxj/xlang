#include "xlbitvector.hpp"
#include <assert.h>

using namespace xl;

 BitVector::BitVector(int length) :
			length(length),
			dataLength(allocSize(length)),
			data(new unsigned int[allocSize(length)]) {}
	
BitVector::~BitVector() {
	if(data)delete[] data;
}

bool BitVector::Test(unsigned int pos) {
	assert(pos>=0 && pos<length);
	return (data[pos / 32] & (1U << (pos % 32))) != 0;
}

void BitVector::Set(unsigned int pos, bool val) {
	assert(pos>=0 && pos<length);
	if((data[pos / 32] & (1U << (pos % 32))) == (val?1:0)) return;
	if(val)
		data[ pos>>5] |= (1U << (pos % 32));
	else
		data[ pos>>5] &= ~(1U << (pos % 32));
} 

bool BitVector::Any() {
	for (unsigned int i = 0; i < dataLength - 1; ++i ) 
		if (data[i] != 0xffffffffU) return false;
	if(data[dataLength - 1] != (0xffffffffU >> (length % 32))) return false;
	return true;
}

bool BitVector::None() {
	for (unsigned int i = 0; i < dataLength; ++i ) 
		if (data[i] != 0) return false;
	return true;
}

void BitVector::Reset() {
	for (unsigned int i = 0; i < dataLength ; ++i )  data[i] = 0;
}

void BitVector::Flip(unsigned int pos) {
	Set(pos, !Test(pos)); 
}

bool BitVector::Forward(int& current)
{
	current++;
	if(current >= (int)length) return false;
	int index = current >> 5;
	unsigned int val = data[index] >> current;
	while(val==0)
	{
		index++;
		if(index >= (int)dataLength) return false;
		current = index<<5;
	}
	
	//skip zero bytes
	while ((val & 0xFF) == 0) {
		val >>= 8;
		current += 8;
	}
	//skip zero bits
	while ((val & 0x1) == 0) {
		val >>= 1;
		current++;
	}
	return true;
}

unsigned int BitVector::Size() const {
	return length;
}

void BitVector::Union(const BitVector & rhs)
{
	assert(rhs.Size() == length);
	for (unsigned int i = 0; i < dataLength; i++) {
		data[i] |= rhs.data[i];
	}
}

void BitVector::Subtract(const BitVector & rhs) {
	assert(rhs.Size() == length);
	for (unsigned int i = 0; i < dataLength; i++) {
		data[i] &= ~rhs.data[i];
	}
}
bool BitVector::UnionChanged(const BitVector & rhs) {
	assert(rhs.Size() == length);
	bool changed = false;
	for (unsigned int i = 0; i < dataLength; i++) {
		int old_data = data[i];
		data[i] |= rhs.data[i];
		if (data[i] != old_data) changed = true;
	}
	return changed;
}

void BitVector::Intersect(const BitVector & rhs) {
	assert(rhs.Size() == length);
	for (unsigned int i = 0; i < dataLength; i++) {
		data[i] &= rhs.data[i];
	}
}

void BitVector::CopyFrom(const BitVector & rhs) {
	assert(rhs.Size() <= length);
	for (unsigned int i = 0; i < rhs.dataLength; i++) {
		data[i] = rhs.data[i];
	}
	for (unsigned int i = rhs.dataLength; i < dataLength; i++) {
		data[i] = 0;
	}
}

bool BitVector::Equals(const BitVector& rhs) {
	for (unsigned int  i = 0; i < dataLength; i++) {
		if (data[i] != rhs.data[i]) return false;
	}
	return true;
}

int BitVector::allocSize(int length) {
	return 1 + ((length - 1) / 32);
}
