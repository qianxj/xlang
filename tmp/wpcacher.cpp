#include "wpcacher.hpp"

using namespace trap::complier;

bool StringCacher::isDirty(const std::wstring & key, const FILETIME & dt)
{
	std::map<std::wstring,CacherItem>::iterator tr = data_.find(key);
	if(tr==data_.end()) return true; //not caher

	FILETIME ft = tr->second.dt;
	LONG  ret = CompareFileTime(&ft, &dt); 
	if(ret==-1) return true; 
	return false;
}

void StringCacher::update(const std::wstring & key, const FILETIME & dt, const std::wstring &text)
{
	std::map<std::wstring,CacherItem>::iterator tr = data_.find(key);
	if(tr!=data_.end()) {
		tr->second.dt = dt;
		tr->second.text = text;
	}else
		insert(key, dt, text);  
}

void StringCacher::insert(const std::wstring & key, const FILETIME & dt, const std::wstring &text)
{
	CacherItem it = {dt,text};
	data_.insert(std::pair<std::wstring,CacherItem>(key,it));
}

void StringCacher::erase(const std::wstring & key)
{
	std::map<std::wstring,CacherItem>::iterator tr = data_.find(key);
	if(tr!=data_.end()) data_.erase(tr);
}

std::wstring & StringCacher::getData(const std::wstring & key)
{
	std::map<std::wstring,CacherItem>::iterator tr = data_.find(key);
	if(tr!=data_.end()) return tr->second.text;
	static std::wstring empty(L"");
	return empty;
}
