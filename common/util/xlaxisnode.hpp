#ifndef XLAXISNODE_H
#define XLAXISNODE_H

#include <string>

namespace xl {
namespace util{ namespace misc {
	
template<class T>
struct AxisNode 
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
		str = space + data.toString(); //data should have toString() method
		AxisNode * term = node->child;
		while(term)
		{
			str += term->dumpNode(term,space+L"   ");
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
			str += term->dumpNode(term,space);
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

}}	//namespace util::misc

} //namespace xl

#endif //XLAXISNODE_H