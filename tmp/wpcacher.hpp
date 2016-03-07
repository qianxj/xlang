#ifndef WPP_StringCacher_H
#define WPP_StringCacher_H

#include <vector>
#include <map>
#include <string>
#include "wptoken.hpp"
#include "windows.h"

namespace trap {
namespace complier {


class cacher_t
{
public:
	virtual bool isDirty(const std::wstring & key, const FILETIME & dt) = 0;
public:
	virtual void update(const std::wstring & key, const FILETIME & dt, const std::wstring &text) = 0;
	virtual void insert(const std::wstring & key, const FILETIME & dt, const std::wstring &text) = 0;
	virtual void erase(const std::wstring & key) = 0;
public:
	virtual std::wstring & getData(const std::wstring & key)  = 0;
};

class StringCacher : public cacher_t
{
public:
	bool isDirty(const std::wstring & key, const FILETIME & dt);
public:
	void update(const std::wstring & key, const FILETIME & dt, const std::wstring &text);
	void insert(const std::wstring & key, const FILETIME & dt, const std::wstring &text);
	void erase(const std::wstring & key);
public:
	std::wstring & getData(const std::wstring & key);
private:
	struct CacherItem
	{
		FILETIME dt;
		std::wstring text;
	};
	std::map<std::wstring,CacherItem> data_;
};

} // namespace complier
} // namespace trap

#endif // WPP_StringCacher_H