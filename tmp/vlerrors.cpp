#include "vlerrors.hpp"

namespace xlang {

#undef  WPP5Error
#define WPP5Error(x,y,z,u,v) {x,y,z,u,L##v},
error::erritem error::erroritems[]={
	#include "vlerrorl.hpp"
	{0,0}
}; 

 VLANG_API const wstring_t  error::geterrtext(err e)
{
	return error::erroritems[ e - err_start - 1].text;
}

} // namespace xlang