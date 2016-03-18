#ifndef XLBITVECTOR_H
#define XLBITVECTOR_H

namespace xl {
	
class BitVector
{
public:
	explicit BitVector(int length);
	~BitVector();

public:
	bool Test(unsigned int pos) ;
	
	void Set(unsigned int pos, bool val=true);
	bool Any() ;
	bool None() ;
	void Reset();
	void Flip(unsigned int pos) ;
	bool Forward(int& current);

public:
	unsigned int Size() const ;
public:
	void Union(const BitVector & rhs);
	
	void Subtract(const BitVector & rhs);
	bool UnionChanged(const BitVector & rhs) ;
	void Intersect(const BitVector & rhs);
	void CopyFrom(const BitVector & rhs) ;
	bool Equals(const BitVector& rhs) ;

private:
	static int allocSize(int length);
	
private:
	unsigned int * data;
	unsigned int length;
	unsigned int dataLength;
public:
	inline unsigned int GetLength() { return length ;}
	inline unsigned int GetDataLength() { return dataLength; }
	inline unsigned int* GetData() { return data; }
};


} //namespace xl

#endif //XLBITVECTOR_H