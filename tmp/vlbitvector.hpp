#ifndef  WPP5_BITVECTOR_H
#define  WPP5_BITVECTOR_H

#include "vlang5.h"
#include "vldebuger.hpp"

class VLANG_API vlBitVector
{
public:
	class Iterator 
	{
   public:
    explicit Iterator(vlBitVector* target)
        : target_(target),
          current_index_(0),
          current_value_(target->data_[0]),
          current_(-1)
	{
      ASSERT(target->data_length_ > 0);
      forward();
    }
    ~Iterator() { }

	int operator *()
	{
		return current();
	}

	int current()
	{
		ASSERT(!is_end());
		return current_; 
	}
	Iterator & operator ++()
	{
		forward();
		return *this;
	}
    bool is_end() const { return current_>= target_->length_ || current_index_ >= target_->data_length_; }
	Iterator & first() 
	{
		current_index_ = 0;
		current_value_ = target_->data_[0];
		current_ = -1;
		forward();
		return *this;
	}
    void forward()
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

	}

   private:
    vlBitVector* target_;
    int current_index_;
    unsigned current_value_;
    int current_;
  };

private:
	int* data_;
	int length_;
	int data_length_;
public:
	explicit vlBitVector(int length)
		: length_(length),
		data_length_(SizeFor(length)),
		data_(new int[SizeFor(length)]) {
			ASSERT(length > 0);
			Clear();
	}
	
	vlBitVector(const vlBitVector& other)
		: length_(other.length()),
		data_length_(SizeFor(other.length())),
		data_(new int[SizeFor(other.length())]) {
			CopyFrom(other);
	}

	~vlBitVector()
	{
		if(data_)delete[] data_;
	}

	static int SizeFor(int length) {
		return 1 + ((length - 1) / 32);
	}

	vlBitVector& operator=(const vlBitVector& rhs) {
		if (this != &rhs) CopyFrom(rhs);
		return *this;
	}

	void CopyFrom(const vlBitVector& other) {
		ASSERT(other.length() <= length());
		for (int i = 0; i < other.data_length_; i++) {
			data_[i] = other.data_[i];
		}
		for (int i = other.data_length_; i < data_length_; i++) {
			data_[i] = 0;
		}
	}

	bool Get(int i) const {
		ASSERT(i >= 0 && i < length());
		return (data_[i / 32] & (1U << (i % 32))) != 0;
	}

	void Add(int i,bool b) {
		ASSERT(i >= 0 && i < length());
		data_[i / 32] |= ((b?1U:0U) << (i % 32));
	}

	void Remove(int i) {
		ASSERT(i >= 0 && i < length());
		data_[i / 32] &= ~(1U << (i % 32));
	}

	void Union(const vlBitVector& other) {
		ASSERT(other.length() == length());
		for (int i = 0; i < data_length_; i++) {
			data_[i] |= other.data_[i];
		}
	}

	bool UnionIsChanged(const vlBitVector& other) {
		ASSERT(other.length() == length());
		bool changed = false;
		for (int i = 0; i < data_length_; i++) {
			int old_data = data_[i];
			data_[i] |= other.data_[i];
			if (data_[i] != old_data) changed = true;
		}
		return changed;
	}

	void Intersect(const vlBitVector& other) {
		ASSERT(other.length() == length());
		for (int i = 0; i < data_length_; i++) {
			data_[i] &= other.data_[i];
		}
	}

	void Subtract(const vlBitVector& other) {
		ASSERT(other.length() == length());
		for (int i = 0; i < data_length_; i++) {
			data_[i] &= ~other.data_[i];
		}
	}

	void Clear() {
		for (int i = 0; i < data_length_; i++) {
			data_[i] = 0;
		}
	}

	bool IsEmpty() const {
		for (int i = 0; i < data_length_; i++) {
			if (data_[i] != 0) return false;
		}
		return true;
	}

	bool Equals(const vlBitVector& other) {
		for (int i = 0; i < data_length_; i++) {
			if (data_[i] != other.data_[i]) return false;
		}
		return true;
	}

	int length() const { return length_; }

	friend class BitElementVector;
};

class VLANG_API BitElementVector
{
	class Iterator 
	{
   public:
    explicit Iterator(BitElementVector* target,int element)
        : target_(target),
          current_index_(0),
          current_value_(target->data_[element * target->element_data_length_]),
          current_(-1),
		  element_(element)
	{
      ASSERT(target->data_length_ > 0);
      forward();
    }
    ~Iterator() { }

	int operator *()
	{
		return current();
	}

	int current()
	{
		ASSERT(!is_end());
		return current_;
	}
	Iterator & operator ++()
	{
		forward();
		return *this;
	}
    bool is_end() const { return current_>= target_->element_length_ || current_index_ >= target_->element_data_length_; }
	Iterator & first()
	{
		current_index_ = 0;
		current_value_ = target_->data_[element_ * target_->element_data_length_];
		current_ = -1;
		forward();
		return *this;
	}
    void forward()
	{
		current_++;
		unsigned int val = current_value_;
		while (val == 0) {
			current_index_++;
			if (is_end()) return;
			val = target_->data_[element_ * target_->element_data_length_ + current_index_];
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
	}

   private:
    BitElementVector* target_;
    int current_index_;
    unsigned int current_value_;
    int current_;
	int element_;
  };

private:
	int* data_;
	int length_;
	int element_length_;
	int data_length_;
	int element_data_length_;
public:
	explicit BitElementVector(int element_length,int length)
		: length_(length),
		element_length_(element_length),
		element_data_length_(1 + (element_length - 1) / 32),
		data_length_(SizeFor(element_length,length)),
		data_(new int[SizeFor(element_length,length)]) {
			ASSERT(length > 0);
			Clear();
	}
	
	BitElementVector(const BitElementVector& other)
		: length_(other.length()),
		element_length_(other.element_length()),
		data_length_(SizeFor(other.element_length_,other.length_)),
		data_(new int[SizeFor(other.element_length_,other.length_)]) {
			CopyFrom(other); 
	}

	~BitElementVector()
	{
		if(data_)delete[] data_;
	}

	static int SizeFor(int element_length,int length) {
		return (1 + ((element_length - 1) / 32))*length;
	}

	BitElementVector& operator=(const BitElementVector& rhs) {
		if (this != &rhs) CopyFrom(rhs);
		return *this;
	}

	void CopyFrom(const BitElementVector& other) {
		ASSERT(other.length() <= length());
		for (int i = 0; i < other.data_length_; i++) {
			data_[i] = other.data_[i];
		}
		for (int i = other.data_length_; i < data_length_; i++) {
			data_[i] = 0;
		}
	}

	inline bool Get(int element, int i) const {
		ASSERT(element>=0 && element < length());
		ASSERT(i >= 0 && i < element_length() );

		return (data_[element * element_data_length_ + i / 32] & (1U << (i % 32)))!=0;
	}

	inline void Add(int element, int i,bool b) {
		ASSERT(element>=0 && element < length());
		ASSERT(i >= 0 && i < element_length() );

		data_[element * element_data_length_ + i / 32] |= ((b?1U:0U) << (i % 32));
	}

	void Remove(int element, int i) {
		ASSERT(element>=0 && element < length());
		ASSERT(i >= 0 && i < element_length() );

		data_[element * element_data_length_ + i / 32] &= ~(1U << (i % 32));
	}

	void Union(int element1,int element2) {
		ASSERT(element1>=0 && element1 < length());
		ASSERT(element2>=0 && element2 < length());

		for (int i = 0; i < element_data_length_; i++) {
			data_[element1 * element_data_length_ + i] |= 
				data_[element2 * element_data_length_ + i];
		}
	}
	void Union(int element1,BitElementVector & other, int element2) {
		ASSERT(element1>=0 && element1 < length());
		ASSERT(element2>=0 && element2 < other.length());
		ASSERT(element_data_length_ == other.element_data_length_);

		for (int i = 0; i < element_data_length_; i++) {
			data_[element1 * element_data_length_ + i] |= 
				other.data_[element2 * element_data_length_ + i];
		}
	}
	void UnionTo(int element1,vlBitVector & other) {
		ASSERT(element1>=0 && element1 < length());
		ASSERT(element_data_length_ == other.data_length_);

		for (int i = 0; i < element_data_length_; i++) {
			other.data_[i] |= 
				data_[element1 * element_data_length_ + i];
		}
	}

	bool UnionIsChanged(int element1,int element2) {
		ASSERT(element1>=0 && element1 < length());
		ASSERT(element2>=0 && element2 < length());
		bool changed = false;
		for (int i = 0; i < element_data_length_; i++) {
			int old_data = data_[element1 * element_data_length_+i];
			data_[element1 * element_data_length_+i] |= 
				data_[element2 * element_data_length_+i];
			if (data_[element1 * element_data_length_+i] != old_data) changed = true;
		}
		return changed;
	}

	void Intersect(int element1,int element2) {
		ASSERT(element1>=0 && element1 < length());
		ASSERT(element2>=0 && element2 < length());
		for (int i = 0; i < element_data_length_; i++) {
			data_[element1 * element_data_length_+i] &=
				data_[element2 * element_data_length_+i];
		}
	}

	void Subtract(int element1,int element2) {
		ASSERT(element1>=0 && element1 < length());
		ASSERT(element2>=0 && element2 < length());
		for (int i = 0; i < element_data_length_; i++) {
			data_[element1 * element_data_length_+i] &=
				~data_[element2 * element_data_length_+i];
		}
	}

	void Clear() {
		for (int i = 0; i < data_length_; i++) {
			data_[i] = 0;
		}
	}

	bool IsEmpty() const {
		for (int i = 0; i < data_length_; i++) {
			if (data_[i] != 0) return false;
		}
		return true;
	}

	bool IsEmpty(int element) const {
		ASSERT(element>=0 && element < length());
		for (int i = 0; i < element_data_length_; i++) {
			if (data_[element * element_data_length_ + i] != 0) return false;
		}
		return true;
	}

	bool Equals(int element1,int element2) {
		ASSERT(element1>=0 && element1 < length());
		ASSERT(element2>=0 && element2 < length());
		for (int i = 0; i < element_data_length_; i++) {
			if(data_[element1 * element_data_length_+i] !=
				~data_[element2 * element_data_length_+i]) return false;
		}
		return true;
	}

#define BitElementVectorGet(element,i) ((data_[element * element_data_length_ + i / 32] & (1U << (i % 32)))!=0)		
#define BitElementVectorAdd(element,i,b) (data_[element * element_data_length_ + i / 32] |= ((b?1U:0U) << (i % 32)))
	void TransmitClosure(int pSorts[],int &count)
	{
		ASSERT(length_ == element_length_);
		int len = length();
		/*
		for(int k=0;k<len;k++) 
		{
			for(int i=0;i<len;i++)
			{
				for(int j = 0;j<len;j++)
				{
					BitElementVectorAdd(i,j,BitElementVectorGet(i,k) &&
							BitElementVectorGet(k,j));
				}
			}
			//CopyFrom(bit);
		}*/

		
		bool* pFlag  = new bool[len];
		//int* pSorts = new int[len];
		int top = 0 ;
		while(top<len){pFlag[top++]=false;}
		pSorts[0] = 0;
		pFlag[0] = true;
		top = 0;
		int index = -1;
		int top_base = top;
		//toposort
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
				bool not_need_loop = true;
				index = top;
				while(index >= top_base) 
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
									not_need_loop = false;
								}
							}
						};
					}
					index--;
				}
				if(not_need_loop)break; //检查环路
			}

			//添加一个分隔
			while(tokenIndex <len && pFlag[tokenIndex])tokenIndex++;
			if(tokenIndex<len)
			{
				//计算下一个闭包集合
				pFlag[tokenIndex] = true;
				top++;
				top_base = top;
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

	inline int length() const { return length_; }
	inline int element_length() const {return element_length_;}
};

#endif //WPP5_BITVECTOR_H