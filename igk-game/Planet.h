#pragma once
#include "cocos2d.h"

using namespace cocos2d;

class Planet {
public:
	Planet(std::string planetSpriteName);

	~Planet();

	CCSprite* getSprite();
private:
	CCSprite* planetSprite;
	float gravityRadius;

};