#ifndef WPP5_ASTvlTapeChar_H
#define WPP5_ASTvlTapeChar_H

#include "vlAstNode.hpp"
#include <vector>

using namespace std;

namespace xlang{
	struct vlAstvlTapeChar : public vlAstNode
	{
		void * data;

	};
}

#endif //WPP5_ASTvlTapeChar_H
