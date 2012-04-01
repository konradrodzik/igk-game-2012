#pragma once

#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace cocos2d;

class Player : public CCSprite
{
public:
	Player(void);
	~Player(void);

public:
	CCSprite* mPlayer;
	b2Body* mPlayerBody;
	b2Fixture* mPlayerFixture;
};

