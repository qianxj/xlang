#include "../common/util/xlvalue.cpp"
#include "../wpp/wpsymnode.cpp"

using namespace xl::wp;

int main()
{
	SymLiteral * literal = makeLiteral("hello world!");
	printf("%S\n", literal->toString().c_str());
	
	literal = makeLiteral(true);
	printf("%S\n", literal->toString().c_str());
	
	return 1;
}

