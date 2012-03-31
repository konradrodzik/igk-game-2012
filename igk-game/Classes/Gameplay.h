#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "cocos2d.h"
#include "Sun.h"

using namespace cocos2d;

class Gameplay : public cocos2d::CCLayer {
	Sun* sun;

public:
	Gameplay();

	~Gameplay();

	virtual bool init();

	static CCScene* scene();
	LAYER_NODE_FUNC(Gameplay);

	void update(ccTime dt);
};

#endif