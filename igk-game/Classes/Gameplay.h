#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "cocos2d.h"
#include "GLES-Render.h"
#include "Sun.h"
#include "Planet.h"
#include "Player.h"
#include "ParticleFactory.h"

#include "Box2D/Box2D.h"

using namespace cocos2d;

class Gameplay : public cocos2d::CCLayer {
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


	void updatePlanets(ccTime dt);
	void removePlanet(int i);
	bool outsideView(const CCPoint &pos, float* distance = NULL, b2Vec2* normalized = NULL);
	bool hasPlanetsNear(const CCPoint &pos, float radius);
	
	Planet* addPlanet(std::string planetSpriteName, CCPoint position);

	void clearLevel();

	void draw();
	void playerJetpack();
	void playerLookAt(CCPoint p);

	virtual void ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent);
	virtual void ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent);

	void updateScore();

	void removeAchievement(CCNode *label);
	void showAchievement(const char*);

private:
	CCSprite* mBackground;
	b2World* mWorld;
	std::vector<Planet*> mPlanets;
	Player* mPlayer;
	CCNode* world;
	Sun* sun;
	CCLabelTTF* playerPos;

	CCSprite* cursor;

	float impulseTimer;

	CCSprite* hud;
	CCSprite* grid;
	CCLabelTTF* scoreText;
	CCLabelTTF* timeLabel;
	std::vector<CCSprite*> mLifeSprites;
	CCParticleSystem *trail;
	ccTime totalTime;
};

#endif