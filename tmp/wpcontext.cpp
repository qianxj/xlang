#include "wpcontext.hpp"

using namespace trap::complier;

Context::Context(): runtime_(new Runtime), dataPool_(new DataPool), stringPool_(new StringPool)
{
}

Context::~Context()
{
	delete runtime_;
	delete dataPool_;
	delete stringPool_;
}

runtime_t* 		Context::getRuntime()
{
	return runtime_;
}

DataPool * 		Context::getDataPool()
{
	return dataPool_;
}

StringPool*		Context::getStringPool()
{
	return stringPool_;
}

HSymbol 		Context::sym(const wstring_t val)
{
	return	(HSymbol)stringPool_->insert(val);
}