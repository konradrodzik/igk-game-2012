#pragma once
#include "cocos2d.h"

using namespace cocos2d;

class MenuScene : public cocos2d::CCLayer
{
public:
	virtual bool init();
    static cocos2d::CCScene* scene();
	LAYER_NODE_FUNC(MenuScene);

	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
};

