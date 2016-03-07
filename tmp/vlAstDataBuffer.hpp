#ifndef WPP5_ASTDATABUFFER_H
#define WPP5_ASTDATABUFFER_H

#include "vlAstNode.hpp"
#include "vlutils.hpp"

namespace xlang{

struct vlAstDataBuffer : public vlAstNode
{
	vlTapeChar * data;
	vlAstDataBuffer() :vlAstNode(akDataBuffer),data(new vlTapeChar){}
	~vlAstDataBuffer() {if(data) delete data;};
};

}

#endif //WPP5_ASTDATABUFFER_H
