#include "Planet.h"

#define PTM_RATIO (1.0f/32.0f)

Planet::Planet( std::string planetSpriteName )
{
	planetSprite = new CCSprite;
	planetSprite->initWithFile(planetSpriteName.c_str());

	gravitySprite = new CCSprite;
	gravitySprite->initWithFile("gravity.png");
	gravitySprite->setAnchorPoint(ccp(0.5f, 0.5f));
	CCSize planetSize = planetSprite->getContentSize();
	gravitySprite->setPosition(ccp(planetSize.width/2.0f, planetSize.height/2.0f));

	planetSprite->addChild(gravitySprite);

	CCFadeOut* fadeout = CCFadeOut::actionWithDuration(1.5f);
	CCFadeIn* fadeint = CCFadeIn::actionWithDuration(1.5f);
	CCSequence* seq = CCSequence::actionOneTwo(fadeout, fadeint);
	gravitySprite->runAction(CCRepeatForever::actionWithAction(seq));

	gravityRadius = 10.0f;
	mPlanetBody = NULL;
	mPlanetFixture = NULL;
}

Planet::~Planet()
{
	if(planetSprite != NULL) {
		delete planetSprite;
		planetSprite = NULL;
	}
}

CCSprite* Planet::getSprite()
{
	return planetSprite;
}

void Planet::setPos( CCPoint pos )
{
	if(planetSprite) {
		planetSprite->setPosition(pos);
	}
}

void Planet::setAngle(float angle)
{
	if(planetSprite)
	{
		planetSprite->setRotation(angle);	
	}
}

void Planet::setGravityRadius( float radius )
{
	//int factor = (radius + 2*radius);
	//int randomRadius = rand()%factor + radius;
	gravityRadius = radius * PTM_RATIO;
	maxGravityRadius = gravityRadius*2;

	// change scale of gravity radius sprite
	float gravitySpriteSize = gravitySprite->getContentSize().width;

	float scale = radius * 2 / gravitySpriteSize;
	gravitySprite->setScale(scale);
}

