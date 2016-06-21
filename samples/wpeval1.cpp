//#include "../common/util/xlvalue.cpp"
#include "../wpp/wpsymnode.cpp"
#include "../wpp/wpcontext.cpp"
#include "../common/util/xlstringpool.cpp"
#include "../common/util/xldatapool.cpp"

using namespace xl;
using namespace xl::wp;

int main()
{
	/*SymLiteral * literal = makeLiteral(L"hello world!");
	printf("%S\n", literal->toString().c_str());
	
	literal = makeLiteral(true);
	printf("%S\n", literal->toString().c_str());
	*/

	WPContext context;
	SymLang * symLang = new SymLang;
	symLang->stringPool = new StringPool;
	context.symLang = symLang;

	return 1;
}