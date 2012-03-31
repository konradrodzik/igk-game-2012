#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "cocos2d.h"
#include "GLES-Render.h"
#include "Sun.h"
#include "Planet.h"
#include "Box2D/Box2D.h"



using namespace cocos2d;

class Gameplay : public cocos2d::CCLayer {
	Sun* sun;

public:
	Gameplay();

	~Gameplay();

	virtual bool init();

	static CCScene* scene();
	LAYER_NODE_FUNC(Gameplay);

	void createPlayer(float posx, float posy);

	void initPhysicalWorld();

	void updatePhysic(ccTime dt);

	void update(ccTime dt);
	
	void addPlanet(std::string planetSpriteName, CCPoint position);

	void clearLevel();

	void draw();

private:
	CCSprite* mBackground;
	b2World* mWorld;
	CCSprite* mPlayer;
	std::vector<Planet*> mPlanets;
};

#endif