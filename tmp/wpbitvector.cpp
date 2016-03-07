#include "wpbitvector.hpp"
#include <assert.h>

using namespace trap::complier;

 BitVector::BitVector(int length) :
			length_(length),
			data_length_(allocSize(length)),
			data_(new unsigned int[allocSize(length)]) {}
	
BitVector::~BitVector() {
	if(data_)delete[] data_;
}

bool BitVector::test(std::size_t pos) {
	assert(pos>=0 && pos<length_);
	return (data_[pos / 32] & (1U << (pos % 32))) != 0;
}

void BitVector::set(std::size_t pos, bool val_) {
	assert(pos>=0 && pos<length_);
	if((data_[pos / 32] & (1U << (pos % 32))) == val_?1:0) return;
	if(val_)
		data_[ pos>>5] |= (1U << (pos % 32));
	else
		data_[ pos>>5] &= ~(1U << (pos % 32));
} 
bool BitVector::any() {
	for (std::size_t i = 0; i < data_length_ - 1; ++i ) 
		if (data_[i] != 0xffffffffU) return false;
	if(data_[data_length_ - 1] != (0xffffffffU >> (length_ % 32))) return false;
	return true;
}
bool BitVector::none() {
	for (std::size_t i = 0; i < data_length_; ++i ) 
		if (data_[i] != 0) return false;
	return true;
}
void BitVector::reset() {
	for (std::size_t i = 0; i < data_length_ ; ++i )  data_[i] = 0;
}
void BitVector::flip(std::size_t pos) {
	set(pos, !test(pos)); 
}

/*
 void BitVector::forward()
{
	current_++;
	unsigned int val = current_value_;
	while (val == 0) {
		current_index_++;
		if (is_end()) return;
		val = target_->data_[current_index_];
		current_ = current_index_ << 5;
	}
	//skip zero bytes
	while ((val & 0xFF) == 0) {
		val >>= 8;
		current_ += 8;
	}
	//skip zero bits
	while ((val & 0x1) == 0) {
		val >>= 1;
		current_++;
	}
	current_value_ = val >> 1;
}*/

std::size_t BitVector::size() const {
	return length_;
}

void BitVector::union_(const BitVector & rhs)
{
	assert(rhs.size() == length_);
	for (std::size_t i = 0; i < data_length_; i++) {
		data_[i] |= rhs.data_[i];
	}
}

void BitVector::subtract(const BitVector & rhs) {
	assert(rhs.size() == length_);
	for (std::size_t i = 0; i < data_length_; i++) {
		data_[i] &= ~rhs.data_[i];
	}
}
bool BitVector::unionChanged(const BitVector & rhs) {
	assert(rhs.size() == length_);
	bool changed = false;
	for (std::size_t i = 0; i < data_length_; i++) {
		int old_data = data_[i];
		data_[i] |= rhs.data_[i];
		if (data_[i] != old_data) changed = true;
	}
	return changed;
}

void BitVector::intersect(const BitVector & rhs) {
	assert(rhs.size() == length_);
	for (std::size_t i = 0; i < data_length_; i++) {
		data_[i] &= rhs.data_[i];
	}
}

void BitVector::copyFrom(const BitVector & rhs) {
	assert(rhs.size() <= length_);
	for (std::size_t i = 0; i < rhs.data_length_; i++) {
		data_[i] = rhs.data_[i];
	}
	for (std::size_t i = rhs.data_length_; i < data_length_; i++) {
		data_[i] = 0;
	}
}

bool BitVector::equals(const BitVector& rhs) {
	for (int i = 0; i < data_length_; i++) {
		if (data_[i] != rhs.data_[i]) return false;
	}
	return true;
}

int BitVector::allocSize(int length) {
	return 1 + ((length - 1) / 32);
}
