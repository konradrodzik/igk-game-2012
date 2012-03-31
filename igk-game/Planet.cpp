#include "Planet.h"

Planet::Planet( std::string planetSpriteName )
{
	planetSprite = new CCSprite;
	planetSprite->initWithFile(planetSpriteName.c_str());
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

