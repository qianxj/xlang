#ifndef WPP_MEMORY_H
#define WPP_MEMORY_H

#include <memory.h>
#include<vector>

namespace trap {
namespace complier {

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
	int blockUsed_;
	int blockSize_;
private:
	std::vector<unsigned char *>* blocks_;
private:
	std::vector<unsigned char *>* blocksfree_;
	struct FCheckPoint
	{
		int blockUsed_;
		int blockSize_;
		int indexBlock;
	};
	std::vector<FCheckPoint> checkPoints_;
};

}	//namespace trap
} //namespace ctrap

#endif //WPP_MEMORY_H	