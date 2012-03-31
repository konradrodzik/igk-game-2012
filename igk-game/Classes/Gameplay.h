#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace cocos2d;

class Gameplay : public cocos2d::CCScene {
	Gameplay();

	~Gameplay();

	void createPlayer(float posx, float posy);

	void initPhysicalWorld();

	void updatePhysic(ccTime dt);

	void update(ccTime dt);

private:
	b2World* mWorld;
	CCSprite* mPlayer;
};

#endif