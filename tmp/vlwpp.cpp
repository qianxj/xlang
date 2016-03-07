#include "vlwpp.hpp"
#include "vlparser.hpp"
#include "vlwpp_context.hpp"
#include "vlcontext.hpp"

using namespace xlang;
using namespace xlang;
using namespace xlang;

Wpp5::Wpp5()
{

}

Wpp5::~Wpp5() 
{
}

bool Wpp5::parse(/*wpp_context*/ void * context,wchar_t* pSrc,bool btrace)
{
	vlParser * parser = new vlParser;  
	try {
		//mill::vill_asm m;
		wpp_context * context_ = (wpp_context *)context;
		vlContext * ctx = (vlContext *)context_->get_metatable();//new vlContext;
		ctx->wppcontext = (xlang::wpp_context*)context;
		parser->set_err_ptr(((xlang::wpp_context*)context)->get_error());
		parser->parse(ctx,pSrc,0);
		parser->getLexer()->source = 0;
	}
	catch(wchar_t * str )
	{
		parser->format_output(L"\nException raised:%s",str);
	}
	catch(const wchar_t * str )
	{
		parser->format_output(L"\nException raised:%s",str);
	}
	catch(... )
	{
		parser->format_output(L"\nException raised.");
	}

	return true;
}