#ifndef XLMEMORYH
#define XLMEMORYH

#include <memory.h>
#include<vector>

namespace xl {

class SpaceWithCheckPoint
{
public:
	SpaceWithCheckPoint();
	~SpaceWithCheckPoint();
private:
	unsigned char* EnsureBuffuer(int len);
public:
	int CheckPoint();
	int RestorePoint();
private:
	int blockUsed;
	int blockSize;
private:
	std::vector<unsigned char *>* blocks;
private:
	std::vector<unsigned char *>* blocksfree;
	struct FCheckPoint
	{
		int blockUsed;
		int blockSize;
		int indexBlock;
	};
	std::vector<FCheckPoint> checkPoints;
};

}	//namespace xl

#endif //XLMEMORYH	