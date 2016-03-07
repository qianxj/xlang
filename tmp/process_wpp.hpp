#ifndef WPP5_PROCESS_WPP_H
#define WPP5_PROCESS_WPP_H

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <locale.h>
#include <vector>

#include "vlParser.hpp"

using namespace xlang;

namespace xlang
{
	vlAstAxis * WppProcess(vlContext * context, vlApply* pApply,vlParser* parser,vlAstAxis * anode,std::vector<vlAstAxis *> &args,std::map<int,vlAstAxis*> & paramBind);
}

#endif //WPP5_PROCESS_WPP_H
