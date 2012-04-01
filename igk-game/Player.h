#pragma once

#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace cocos2d;

class BaseObject
{
public:
	virtual ~BaseObject() {}
};

class Player : public BaseObject
{
public:
	Player(void);
	~Player(void);

public:
	CCSprite* mPlayer;
	b2Body* mPlayerBody;
	b2Fixture* mPlayerFixture;
	CCPoint direction;
	CCPoint mLastPos;
	CCPoint mLastVel;
	CCPoint mOptimizedVel;
	CCPoint mOptimizedPos;
	CCPoint mOptimizedPos2;
	float mLastAngle;
	

	int score;
};

