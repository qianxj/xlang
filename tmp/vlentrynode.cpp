#include "vlentrynode.hpp"

using namespace xlang;


int OneOfNameList::addItem(TValue v, EntryNode* node)
{
	items.insert(pair<int,EntryNode*>(v.val,node));
	return 1;
}
EntryNode * OneOfNameList::findItem(TValue v)
{
	if(items.find(v.val)!=items.end()) return items[v.val];
	return 0;
}

int OneOfMatchList::addItem(TValue v, EntryNode* node)
{
	items.push_back(pair<TValue,EntryNode *>(v,node));
	return 1;
}
EntryNode * OneOfMatchList::findItem(TValue v)
{
	for(int i=0; i<(int)items.size();i++)
	{
		if(v.ty==items[i].first.ty && v.val64==items[i].first.val64) return items[i].second;
	}
	return 0;
}

int OneOfCountList::addItem(TValue v, EntryNode* node)
{
	items.push_back(pair<int,EntryNode *>(v.val,node));
	return 1;
}

EntryNode * OneOfCountList::findItem(TValue v)
{
	for(int i=0; i<(int)items.size();i++)
	{
		if(v.val==items[i].first) return items[i].second;
	}
	return 0;
}