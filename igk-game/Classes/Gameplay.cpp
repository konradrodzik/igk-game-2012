#include "Gameplay.h"
#include "Input.h"

Gameplay::Gameplay() {
	scheduleUpdate();
}

Gameplay::~Gameplay() {
	unscheduleUpdate();
}

CCScene* Gameplay::scene()
{
	CCScene * scene = NULL;
	do 
	{
		// 'scene' is an autorelease object
		scene = CCScene::node();
		CC_BREAK_IF(! scene);

		// 'layer' is an autorelease object
		Gameplay *layer = Gameplay::node();
		CC_BREAK_IF(! layer);

		// add layer as a child to scene
		scene->addChild(layer);
	} while (0);

	// return the scene
	return scene;
}

bool Gameplay::init() 
{
	setIsTouchEnabled(true);
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	player = new Player();
	player->setPosition(ccp(size.width / 2, size.height / 2));
	this->addChild(player, 1);

	world = CCNode::node();
	world->setContentSize(size);
	world->setAnchorPoint(ccp(0.5, 0.5));
	world->setPosition(ccp(size.width / 2, size.height / 2));
	addChild(world);

	// setup sun
	sun = new Sun();
	sun->setPosition(ccp(-sun->getContentSize().width / 2 + 400, world->getContentSize().height / 2));
	world->addChild(sun);

	// setup world rotation around sun
	float sunAnchorPositionX = (sun->getPositionX())  / world->getContentSize().width;
	world->setAnchorPoint(ccp(sunAnchorPositionX, 0.5));
	world->setPosition(ccp(world->getPositionX() - (0.5 + fabs(sunAnchorPositionX)) * world->getContentSize().width, world->getPositionY()));

	return true;
}

void Gameplay::update(ccTime dt) 
{
	Input::instance()->update();
	if(Input::instance()->keyDown(VK_UP)) {
		world->setRotation(world->getRotation() + 1);
	}

	if(Input::instance()->keyDown(VK_DOWN)) {
		world->setRotation(world->getRotation() - 1);
	}
}

void Gameplay::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	//world->setRotation(world->getRotation() + 5);
}

void Gameplay::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{

}

void Gameplay::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{

}