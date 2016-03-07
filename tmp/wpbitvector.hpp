#ifndef WPP_BITVECTOR_H
#define WPP_BITVECTOR_H

#include <vector>

namespace trap {
namespace complier {
	
class BitVector
{
public:
	explicit BitVector(int length);
	~BitVector();

public:
	bool test(std::size_t pos) ;
	
	void set(std::size_t pos, bool val_=true);
	bool any() ;
	bool none() ;
	void reset();
	void flip(std::size_t pos) ;
	// void forward();

public:
	std::size_t size() const ;
public:
	void union_(const BitVector & rhs);
	
	void subtract(const BitVector & rhs);
	bool unionChanged(const BitVector & rhs) ;
	void intersect(const BitVector & rhs);
	void copyFrom(const BitVector & rhs) ;
	bool equals(const BitVector& rhs) ;

private:
	static int allocSize(int length);
	
private:
	unsigned int * data_;
	std::size_t length_;
	std::size_t data_length_;
};

}	//namespace trap
} //namespace ctrap

#endif //WPP_BITVECTOR_H