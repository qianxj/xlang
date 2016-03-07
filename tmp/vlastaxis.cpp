#include "vlAstAxis.hpp"
#include "vlutils.hpp"

using namespace xlang;

vlAstAxis::vlAstAxis(vlAstNode * node):vlAstNode(akAxisNode),node(node),next(0),child(0),sibling(0),parent(0),before(0),piror(0){}

vlAstNode * vlAstAxis::Node()
{
	if(!this) return 0;
	return node;
}

vlAstAxis * vlAstAxis::Next(int count)
{
	vlAstAxis * node = this;
	if(!node) return 0;
	while(count >0)
	{
		if(!node->next)break;
		node = node->next;
		count--;
	}
	return count==0 ? node : 0;
}

vlAstAxis * vlAstAxis::Sibling(int count)
{
	vlAstAxis * node = this;
	if(!node) return 0;
	while(count >0)
	{
		if(!node->sibling)break;
		node = node->sibling;
		count--;
	}
	return count==0 ? node : 0;
}

vlAstAxis * vlAstAxis::LastFirstChild()
{
	vlAstAxis * node = this;
	if(!node) return 0;
	while(node->child) node = node->child;
	return node;
}

vlAstAxis * vlAstAxis::LastChild()
{
	vlAstAxis * node = this;
	if(!node) return 0;
	while(node->child && !node->child->sibling && !node->child->next) node = node->child;
	return node;
}

vlAstNode * vlAstAxis::LastChildNode()
{
	return LastChild()->Node();
}

vlAstAxis * vlAstAxis::Child(int count)
{
	vlAstAxis * node = this;
	if(!node) return 0;
	while(count >0)
	{
		if(!node->child)break;
		node = node->child;
		count--;
	}
	return count==0 ? node : 0;
}

vlAstAxis * vlAstAxis::GetRoot()
{
	vlAstAxis * root = this;
	while(root && root->getParent()) root = root->getParent();
	return root;
}

vlAstAxis * vlAstAxis::ChildNext(int index)
{
	if(!this) return 0;
	if(Child(1)) return Child(1)->Next(index);
	return 0;
}
vlAstAxis * vlAstAxis::ChildSibling(int index)
{
	if(!this) return 0;
	if(Child(1)) return Child(1)->Sibling(index);
	return 0;
}

vlAstAxis * vlAstAxis::appendNext(vlAstAxis* node)  throw(...)
{
	if(!node)
		throw(L"error argument for appendNext");
	if(!next) 
		next = node;
	else
	{
		vlAstAxis* it = next;
		while(it->next) it = it->next;
		it->next = node;
	}
	node->parent = parent;
	return this;
}
vlAstAxis * vlAstAxis::appendChildNext(vlAstAxis* node)  throw(...)
{
	if(!node)
		throw(L"error argument for appendChildNex");
	if(!child) 
		child = node;
	else
	{
		vlAstAxis* it = child;
		while(it->next) it = it->next;
		it->next = node;
	}
	node->parent = parent;
	return this;
}

vlAstAxis * vlAstAxis::appendSibling(vlAstAxis* node)  throw(...)
{
	if(!node)
		throw(L"error argument for appendSibling");
	if(!sibling) 
		sibling = node;
	else
	{
		vlAstAxis* it = sibling;
		while(it->sibling) it = it->sibling;
		it->sibling = node;
	}
	node->parent = parent;
	return this;
}

vlAstAxis * vlAstAxis::appendChild(vlAstAxis* node) throw(...)
{
	if(!node)
		throw(L"error argument for appendChild");
	if(!child) 
		child = node;
	else
	{
		vlAstAxis* it = child;
		while(it->sibling) it = it->sibling;
		it->sibling = node;
	}
	node->parent = parent;
	return this;
}

bool vlAstAxis::removeChildNode(vlAstAxis* node)
{
	vlAstAxis* parent = this;
	if(parent->child==node)
	{
		parent->child = node->getSibling();
		parent->child->parent = parent;
		parent->before = 0 ;
		return true;
	}
	else
	{
		vlAstAxis* item = parent->child;
		vlAstAxis* pitem = 0;
		while(item)
		{
			if(item==node)
			{
				pitem ->sibling = node->sibling;
				pitem ->sibling->before = pitem;
				return true;
			}
			pitem = item;
			item = item->sibling;
		}
	}
	return false;
}

vlAstAxis * vlAstAxis::findNode(int ide)
{
	if(getIdent()==ide) return this;

	vlAstAxis* siblingnode = this;
	vlAstAxis* nextnode = this;

	while(siblingnode)
	{
		nextnode = siblingnode;
		while(nextnode)
		{
			//if(nextnode->getIdent())
			//	printf("\n%S",util::getHashString(nextnode->getIdent()));
			if(nextnode->getIdent()==ide) return nextnode;
			if(nextnode->node->kind==akAxisNode)
			{
				vlAstAxis* node = ((vlAstAxis *)nextnode->node)->findNode(ide);
				if(node) return node;
			}
			if(nextnode->child)
			{
				vlAstAxis* node = nextnode->child->findNode(ide);
				if(node) return node;
			}
			nextnode = nextnode->next;
		}
		siblingnode = siblingnode->sibling;
	}
	return 0;
}

std::wstring vlAstAxis::printOwnerOnly()
{
	if(!this) return L"";

	std::wstring str;
	vlAstAxis* siblingnode = this;
	vlAstAxis* nextnode = this;

	if(sibling) str += (std::wstring)L"[";
	while(siblingnode)
	{
		nextnode = siblingnode;
		if(siblingnode!=this)
			str += (std::wstring)L",";
		if(nextnode->next) str += (std::wstring)L"\n(";
		while(nextnode)
		{
			if(nextnode!=siblingnode)str += (std::wstring)L" "; 
			std::wstring fstr = nextnode->node->printOwner();
			str += fstr;
			if(nextnode->child)
			{
				//nextnode->child->next节点已经包含括号
				if(fstr!=L"" && !nextnode->child->next)
					str += (std::wstring)L"(" + nextnode->child->printOwner() + L")";
				else
					str += nextnode->child->printOwner(); 
			}
			//nextnode = 0;
			nextnode = nextnode->next;
		}
		if(siblingnode->next) str += (std::wstring)L")";
		//siblingnode = siblingnode->sibling;
		siblingnode =0;
	}
	if(sibling) str += (std::wstring)L"]";

	return str;
}
std::wstring vlAstAxis::printOwner()
{
	if(!this) return L"";

	std::wstring str;
	vlAstAxis* siblingnode = this;
	vlAstAxis* nextnode = this;

	if(sibling) str += (std::wstring)L"[";
	while(siblingnode)
	{
		nextnode = siblingnode;
		if(siblingnode!=this)
			str += (std::wstring)L",";
		if(nextnode->next) str += (std::wstring)L"\n(";
		while(nextnode)
		{
			if(nextnode!=siblingnode)str += (std::wstring)L" "; 
			std::wstring fstr = nextnode->node->printOwner();
			str += fstr;
			if(nextnode->child)
			{
				//nextnode->child->next节点已经包含括号
				if(fstr!=L"" && !nextnode->child->next)
					str += (std::wstring)L"(" + nextnode->child->printOwner() + L")";
				else
					str += nextnode->child->printOwner(); 
			}
			nextnode = nextnode->next;
		}
		if(siblingnode->next) str += (std::wstring)L")";
		siblingnode = siblingnode->sibling;
	}
	if(sibling) str += (std::wstring)L"]";

	return str;
}

void vlAstAxis::setParent(vlAstAxis * node,bool relation)
{
	this->parent = node;
	if(!relation)node->child = this;
	if(!relation) return;

	node->child = this;
	vlAstAxis * anode = this->sibling;
	while(anode)
	{
		anode->parent = node;
		anode = anode->sibling;
	}
	anode = this->next;
	while(anode)
	{
		anode->parent = node;
		anode = anode->next;
	}
}

void vlAstAxis::setChild(vlAstAxis * child,bool relation)
{
	child->setParent(this,relation);
}

void vlAstAxis::setSibling(vlAstAxis * node,bool relation)
{
	sibling = node;
	if(!relation)node->child = this;
	if(relation)
	{
		node->before = this;
		node->parent = parent;
	}
}

void vlAstAxis::setNext(vlAstAxis * node,bool relation)
{
	next = node;
	if(!relation)node->piror = this;
	if(relation)
	{
		node->piror = this;
		node->parent = parent;
	}
}

void vlAstAxis::setPiror(vlAstAxis * node,bool relation)
{
	node->setNext(this,relation);
}

void vlAstAxis::setBefore(vlAstAxis * node,bool relation)
{
	node->setSibling(this,relation);
}

void vlAstAxis::comeAwaySibling()
{
	if(this->parent)
	{
		if(parent->child==this)
		{
			parent->child = sibling;
			sibling->before = 0;
			this->sibling = 0;
		}else
		{
			if(before) before->sibling = sibling;
			if(sibling)sibling->before = before;
			sibling = 0;
			before = 0;
		}
	}else
	{
		if(before) before->sibling = sibling;
		if(sibling)sibling->before = before;
		sibling = 0;
		before = 0;
	}
}

void vlAstAxis::comeAwayPirorNext()
{
	if(this->parent)
	{
		if(parent->child==this)
		{
			parent->child = next;
			next->piror = 0;
			this->next = 0;
		}else
		{
			if(piror)piror->next = next;
			if(next) next->piror = piror;
			piror = 0;
			next = 0;
		}
	}else
	{
		if(piror)piror->next = next;
		if(next) next->piror = piror;
		piror = 0;
		next = 0;
	}
}

void vlAstAxis::comeAwayParent()
{
}

void vlAstAxis::comeAwayChild()
{
}

void vlAstAxis::comeAwayButChild()
{
	this->parent = 0;
	this->sibling = 0;
	this->before = 0;
	this->next = 0;
	this->piror = 0;
}

void vlAstAxis::comeAwayAll()
{
	this->parent = 0;
	this->sibling = 0;
	this->before = 0;
	this->next = 0;
	this->piror = 0;
	this->child = 0;
}



