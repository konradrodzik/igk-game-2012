#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "cocos2d.h"

using namespace cocos2d;

class Gameplay : public cocos2d::CCScene {
	Gameplay();

	~Gameplay();

	void update(ccTime dt);
};

#endif