#ifndef WPSYMNODE_H
#define WPSYMNODE_H

#include <string>

namespace xl {namespace wp {

enum SymKind : char
{
	kSymUnknown 	= 0,
	kSymAxis 			= 1,
	kSymOneof 		= 2,
	kSymOption 		= 3,
	kSymList 			= 4,
	kSymVar				= 5,
	kSymAssign			= 6,
	kSymAction			= 7,
	kSymKeyword 		= 8,
	kSymLiteral   		= 9,
	kSymOperate		= 10,
	kSymSynTerm		= 11,
	kSymGuard			= 12,
	kSymSatisfy  		= 13
};

template<class TKind>	
struct SymNode
{
	TKind kind;
};

template<class TKind class Ty>
struct AxisNode : public SymNode<TKind>
{
	AxisNode * parent;
	AxisNode * child;
	AxisNode * next;
	T data;
	
	AxisNode() :parent(0),child(0),next(0),data(0){}
	AxisNode(T val) : parent(0),child(0),next(0),data(val){}
	
	 AxisNode & addChild(AxisNode * node)
	{
		AxisNode* term = this->child;
		if(!term)
		{
			this->child = node;
			node->parent = this;
			return *this;
		}else
		{
			return term->addSibling(node);
		}
	}
	
	 AxisNode & addSibling(AxisNode * node)
	{
		AxisNode* term = this;
		while(term->next) term = term->next;
		term->next = node;
		node->parent = term->parent;
		return *this;
	}
	
	std::wstring dumpNode(AxisNode * node, std::wstring space)
	{
		std::wstring str;
		str = space + L"***";
		AxisNode * term = node->child;
		while(term)
		{
			str += dumpNode(term,space+L"   ");
			term = term->next;
		}
		return str;
	}
	
	std::wstring dump(std::wstring space)
	{
		std::wstring str;
		AxisNode * term = this;
		while(term)
		{
			str += dumpNode(term,space);
			term = term->next;
		}
		return str;
	}
	
	 AxisNode& operator +( AxisNode &rhs)
	{
		return addSibling(&rhs);
	}
	
	 AxisNode& operator +=(AxisNode &rhs)
	{
		return addSibling(&rhs);
	}
	
	 AxisNode& operator *(AxisNode &rhs)
	{
		return addChild(&rhs);
	}
	
	 AxisNode& operator *=(AxisNode &rhs)
	{
		return addChild(&rhs);
	}
};

struct SymOneof
{
};

}} //namespace xl::wp

#endif //WPSYMNODE_H