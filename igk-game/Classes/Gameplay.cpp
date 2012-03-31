#include "Gameplay.h"

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
	CCSize size = CCDirector::sharedDirector()->getWinSize();

	sun = new Sun();
	sun->setPosition(ccp(-sun->getContentSize().width/2 + 300, size.height / 2));
	addChild(sun);

	return true;
}

void Gameplay::update(ccTime dt) {

}