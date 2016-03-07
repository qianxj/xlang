#ifndef WPP5_ASTAXIS_H
#define WPP5_ASTAXIS_H

#include "vlAstNode.hpp"

namespace xlang{

class vlAstAxis : public vlAstNode
{
public:
	vlAstAxis(vlAstNode * node);
public:
	vlAstAxis * appendSibling(vlAstAxis* node) throw(...);
	vlAstAxis * appendChild(vlAstAxis* node) throw(...);
	vlAstAxis * appendNext(vlAstAxis* node) throw(...);
	vlAstAxis * appendChildNext(vlAstAxis* node) throw(...);
public:
	bool removeChildNode(vlAstAxis* node);

	std::wstring printOwner();
	std::wstring printOwnerOnly();
public:
	vlAstAxis * findNode(int ide);
public:
	vlAstNode * Node();
	vlAstAxis * Next(int count = 1);
	vlAstAxis * Sibling(int count = 1);
	vlAstAxis * Child(int count = 1);
	vlAstAxis * LastChild();
	vlAstAxis * LastFirstChild();
	//等价于LastChild()->node
	vlAstNode * LastChildNode();
	//等价于child(1).sibling(index)
	vlAstAxis * ChildSibling(int index = 1);
	//等价于child(1).next(index)
	vlAstAxis * ChildNext(int index = 1);
	vlAstAxis * GetRoot();
public:
	void setParent(vlAstAxis * node,bool relation = true);
	void setChild(vlAstAxis * node,bool relation = true);
	void setSibling(vlAstAxis * node,bool relation = true);
	void setNext(vlAstAxis * node,bool relation = true);
	void setPiror(vlAstAxis * node,bool relation = true);
	void setBefore(vlAstAxis * node,bool relation = true);
public:
	inline vlAstAxis * vlAstAxis::getParent(){return parent;}
	inline vlAstAxis * vlAstAxis::getChild(){return child;}
	inline vlAstAxis * vlAstAxis::getSibling(){return sibling;}
	inline vlAstAxis * vlAstAxis::getNext(){return next;}
	inline vlAstAxis * vlAstAxis::getPiror(){return piror;}
	inline vlAstAxis * vlAstAxis::getBefore(){return before;}
private:
	void comeAwaySibling();
	void comeAwayPirorNext();
	void comeAwayParent();
	void comeAwayChild();
public:
	void comeAwayButChild();
	void comeAwayAll();

public:
	vlAstAxis * parent;
	vlAstAxis * child;
	vlAstAxis * piror;
	vlAstAxis * next;
	vlAstAxis * before;
	vlAstAxis * sibling;
public:
	vlAstNode * node;
};

} //namespace xlang

#endif //WPP5_ASTAXIS_H