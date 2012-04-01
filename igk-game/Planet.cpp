#include "Planet.h"

Planet::Planet( std::string planetSpriteName )
{
	planetSprite = new CCSprite;
	planetSprite->initWithFile(planetSpriteName.c_str());

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

