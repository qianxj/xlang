#ifndef WPP_CONTEXT_H
#define WPP_CONTEXT_H

#include <vector>
#include "wpdatapool.hpp"
#include "wpstringpool.hpp"
#include "wpruntime.hpp"

namespace trap {
namespace complier {

class context_t
{
public:
	virtual runtime_t* 		getRuntime() = 0;
public:
	virtual DataPool * 		getDataPool() = 0;
	virtual StringPool*		getStringPool() = 0;
public:
	virtual HSymbol sym(wstring_t) = 0;
};
	
class Context : public context_t
{
	public:
		Context();
		~Context();
	public:
		runtime_t* 		getRuntime();
	public:
		DataPool * 		getDataPool() ;
		StringPool*		getStringPool() ;
	public:
		HSymbol 		sym(const wstring_t); 
	private:
		runtime_t*		runtime_;
		DataPool*		dataPool_;
		StringPool*		stringPool_;
};


}	//namespace trap
} //namespace ctrap

#endif //WP_CONTEXT_H