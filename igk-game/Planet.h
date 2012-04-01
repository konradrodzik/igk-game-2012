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
	void setAngle(float angle);

	void setGravityRadius(float radius);

	CCSprite* planetSprite;
	CCSprite* gravitySprite;
	float gravityRadius;
	float maxGravityRadius;
	// Physical representation
	b2Body* mPlanetBody;
	b2Fixture* mPlanetFixture;
};

class Trash {
public:
	Trash(std::string trashSpriteName);

	~Trash();

	CCSprite* getSprite();

	void setPos(CCPoint pos);
	void setAngle(float angle);

	CCSprite* trashSprite;
	b2Body* mTrashBody;
	b2Fixture* mTrashFixture;
};
