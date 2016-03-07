#ifndef WPP5_STRINGPOOL_H
#define WPP5_STRINGPOOL_H

#include "vlDataPool.hpp"

namespace xlang  {

class vlStringPool : public vlDataPool
{
public:
	wchar_t* append_string(const wchar_t* str);
	wchar_t* append_string(const wchar_t* str,int size);
};

}

#endif //WPP5_STRINGPOOL_H