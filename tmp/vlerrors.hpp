#ifndef WPP5ErrorS_H
#define WPP5ErrorS_H

#include "vlang5.h"
#include "vlglobal.hpp"

namespace xlang {

class error
{
public:
	enum lang {cn,en};
	#define WPP5Error(x,y,z,u,v) z,
	enum err
	{
		err_start = 0x2000,
		#include "vlerrorl.hpp"
		err_end
	};
	
	struct erritem
	{
		unsigned long	code;
		int				level;
		err				eitem;
		lang			elang;
		wchar_t*		text;
	};

public:
	static erritem		erroritems[];

public:
	//get error text by enum err
	VLANG_API static const wstring_t geterrtext(err e);
};

} // namespace xlang

#endif //WPP5ErrorS_H