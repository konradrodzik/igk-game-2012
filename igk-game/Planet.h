#pragma once
#include "cocos2d.h"
#include "Box2D/Box2D.h"

using namespace cocos2d;

class Planet {
public:
	Planet(std::string planetSpriteName);

	~Planet();

	CCSprite* getSprite();

	void setPos(CCPoint pos);
private:
	CCSprite* planetSprite;
	float gravityRadius;

public:
	// Physical representation
	b2Body* mPlanetBody;
	b2Fixture* mPlanetFixture;
};