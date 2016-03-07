#include "vlang5.h"
#include "vlparser.hpp"
#include "vlplatform.hpp"
#include "vlcontext.hpp"
#include "vlutils.hpp"
#include "vltrap.hpp"

#include <map>
#include <string> 

using namespace std;
using namespace xlang; 


int try_unify(wstring_t src)
{
	bool traceSyntax = false;
	bool traceAst = false;

	vlContext * context = new vlContext;
	vlParser * parser = new vlParser; 
	parser->getLexer()->setSource(src,0);
	parser->getLexer()->nextch();
	parser->nextToken();

	os_t::print(L"\nstart compile:\n");
	parser->parse_xlang_translation_unit(context);
	os_t::print(L"\nfinish compile.\n");

	if(traceSyntax)
	{
		os_t::print(L"syntax:\n");
		map<int ,vlAstAxis*>::const_iterator iter = context->rulers.begin();
		while(iter!=context->rulers.end())
		{
			os_t::print(L"%s = ",util::getHashString(iter->first));
			parser->printAstAxis(iter->second);
			os_t::print(L"\n");
			iter++;
		}
	}
	/*
	if(traceAst)
	{
		if(ctx.nodes.size())
			os_t::print(L"\n%s",ctx.nodes[0]->printOwner().c_str());
	}*/

	return 1;
}

int VLANG_API test_parser(wchar_t* src)
{
	return 	try_unify(src);
#if 0
	vlTrap trap;
	Handle hInt		= trap->getTypeHandle(vlTrap::Ident(L"int"));
	Handle hDouble	= trap->getTypeHandle(vlTrap::Ident(L"double"));
	Handle hString	= trap->getTypeHandle(vlTrap::Ident(L"string"));
	Handle hChar	= trap->getTypeHandle(vlTrap::Ident(L"char"));
	Handle hBoolean = trap->getTypeHandle(vlTrap::Ident(L"bool"));
	Handle hPointer	= trap->getTypeHandle(vlTrap::Ident(L"ptr"));
	Handle hClass	= trap->getTypeHandle(vlTrap::Ident(L"class"));
	Handle hStruct	= trap->getTypeHandle(vlTrap::Ident(L"struct"));
	Handle hEnum	= trap->getTypeHandle(vlTrap::Ident(L"enum"));
	Handle hArray	= trap->getTypeHandle(vlTrap::Ident(L"array"));
	Handle hVector	= trap->getTypeHandle(vlTrap::Ident(L"vector"));
	Handle hMap		= trap->getTypeHandle(vlTrap::Ident(L"map"));

	Handle hSymbol		= trap->addType(vlTrap::Ident(L"Symbol"));
	Handle hType		= trap->addType(vlTrap::Ident(L"SymType"));
	Handle hField		= trap->addType(vlTrap::Ident(L"SymField"));
	Handle hMethod		= trap->addType(vlTrap::Ident(L"Method"));
	Handle hParam		= trap->addType(vlTrap::Ident(L"Param"));
	Handle hTypeParam	= trap->addType(vlTrap::Ident(L"TypeParam"));
	Handle hVariant		= trap->addType(vlTrap::Ident(L"Variant"));
	Handle hItem		= trap->addType(vlTrap::Ident(L"Item"));
	Handle hThing		= trap->addType(vlTrap::Ident(L"Thing"));

	trap->push(hSymbol);

	Handle hMyType(skCompType,(int)trap->symTable.comptypes.size() + 1);
	trap->symTable.comptypes.push_back(hVector.index);
	trap->symTable.comptypes.push_back((unsigned char) hType.index);
	//trap->symTable.comptypes.push_back(0);
	trap->addField(vlTrap::Ident(L"types"),0,hMyType);

	hMyType.index = (int)trap->symTable.comptypes.size() + 1;
	trap->symTable.comptypes.push_back(hVector.index);
	trap->symTable.comptypes.push_back((unsigned char) hField.index);
	//trap->symTable.comptypes.push_back(0);
	trap->addField(vlTrap::Ident(L"fields"),0,hMyType);

	hMyType.index = (int)trap->symTable.comptypes.size() + 1;
	trap->symTable.comptypes.push_back(hVector.index);
	trap->symTable.comptypes.push_back((unsigned char) hMethod.index);
	//trap->symTable.comptypes.push_back(0);
	trap->addField(vlTrap::Ident(L"methods"),0,hMyType);

	hMyType.index = (int)trap->symTable.comptypes.size() + 1;
	trap->symTable.comptypes.push_back(hVector.index);
	trap->symTable.comptypes.push_back((unsigned char) hParam.index);
	//trap->symTable.comptypes.push_back(0);
	trap->addField(vlTrap::Ident(L"params"),0,hMyType);

	hMyType.index = (int)trap->symTable.comptypes.size() + 1;
	trap->symTable.comptypes.push_back(hVector.index);
	trap->symTable.comptypes.push_back((unsigned char) hTypeParam.index);
	//trap->symTable.comptypes.push_back(0);
	trap->addField(vlTrap::Ident(L"typeParams"),0,hMyType);

	hMyType.index = (int)trap->symTable.comptypes.size() + 1;
	//trap->symTable.comptypes.push_back((unsigned char) hPointer.index);
	//trap->symTable.comptypes.push_back((unsigned char) hPointer.index);
	trap->symTable.comptypes.push_back(hArray.index);
	trap->symTable.comptypes.push_back((unsigned char) 2);
	trap->symTable.comptypes.push_back(hArray.index);
	trap->symTable.comptypes.push_back((unsigned char) 8);
	trap->symTable.comptypes.push_back((unsigned char) hVariant.index);
	//trap->symTable.comptypes.push_back(0);
	trap->addField(vlTrap::Ident(L"variants"),0,hMyType);
	trap->pop(); //hSymbol;

	trap->push(hType);
	Handle hTypeName =  trap->addField(vlTrap::Ident(L"name"),0,hInt);
	Handle hTypeBase =  trap->addField(vlTrap::Ident(L"base"),0,hInt);
	Handle hTypeFlag =  trap->addField(vlTrap::Ident(L"flag"),0,hInt);
	Handle hTypeSize =  trap->addField(vlTrap::Ident(L"size"),0,hInt);
	Handle hTypeTypeParam		=  trap->addField(vlTrap::Ident(L"typeparam"),0,hInt);
	Handle hTypeTypeParamCount	=  trap->addField(vlTrap::Ident(L"typeparamCount"),0,hInt);
	Handle hTypeField			=  trap->addField(vlTrap::Ident(L"field"),0,hInt);
	Handle hTypeFieldCount		=  trap->addField(vlTrap::Ident(L"fieldCount"),0,hInt);
	trap->pop(); //hType;

	trap->push(hItem);
	trap->addField(vlTrap::Ident(L"name"),0,hInt);
	trap->addField(vlTrap::Ident(L"spec"),0,hInt);
	trap->addField(vlTrap::Ident(L"type"),0,hInt);
	trap->addField(vlTrap::Ident(L"offset"),0,hInt);
	trap->addField(vlTrap::Ident(L"size"),0,hInt);
	trap->pop(); //hItem;

	trap->getType(hParam)->baseType = hItem;
	trap->getType(hVariant)->baseType = hItem;
	trap->getType(hTypeParam)->baseType = hItem;
	
	trap->push(hMethod);
	trap->addField(vlTrap::Ident(L"name"),0,hInt);
	trap->addField(vlTrap::Ident(L"spec"),0,hInt);
	trap->addField(vlTrap::Ident(L"type"),0,hInt);
	trap->addField(vlTrap::Ident(L"typeparam"),0,hInt);
	trap->addField(vlTrap::Ident(L"typeparamCount"),0,hInt);
	trap->addField(vlTrap::Ident(L"param"),0,hInt);
	trap->addField(vlTrap::Ident(L"paramCount"),0,hInt);
	trap->addField(vlTrap::Ident(L"variant"),0,hInt);
	trap->addField(vlTrap::Ident(L"variantCount"),0,hInt);


	/*
	int offsetTypeName = trap->getFieldOffset(hType,hTypeName);
	int offsetTypeBase = trap->getFieldOffset(hType,hTypeBase);
	int offsetTypeFlag = trap->getFieldOffset(hType,hTypeFlag);

	//add class test;
	void * object = trap->makeType(vlTrap::Ident(L"SymType"));
	*(int *)(((unsigned char *)object) + offsetTypeName) = vlTrap::Ident(L"Object");

	void * rect = trap->makeType(vlTrap::Ident(L"SymType"));
	*(int  *)(((unsigned char *)rect) + offsetTypeName) = vlTrap::Ident(L"Rect");
	*(void **)(((unsigned char *)rect) + offsetTypeBase) = object;
	*/

	HType hInteger = trap->addType(util::hashString(L"Integer"));
	hMyType.index = (int)trap->symTable.comptypes.size()+1;
	trap->symTable.comptypes.push_back(hArray.index);
	trap->symTable.comptypes.push_back(8);
	trap->symTable.comptypes.push_back(hInt.index);
	trap->setTyperef(hInteger,hMyType);

	os_t::print(trap->dump().c_str());

	int msize = 0;
	int size = trap->getAllocSize(hSymbol,msize);

	int sInt = trap->getAllocSize(hInteger,msize);
	
	/*trap->freeType(object);
	trap->freeType(rect);
	*/
#endif
	return 1;

}
