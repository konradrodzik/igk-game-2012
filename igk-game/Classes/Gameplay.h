#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "cocos2d.h"
#include "Sun.h"
#include "Player.h"

using namespace cocos2d;

class Gameplay : public cocos2d::CCLayer {
	CCNode* world;
	Sun* sun;
	Player* player;

public:
	Gameplay();

	~Gameplay();

	virtual bool init();

	static CCScene* scene();
	LAYER_NODE_FUNC(Gameplay);

	void update(ccTime dt);

	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);
};

#endif