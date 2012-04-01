#include "MenuScene.h"
#include "Gameplay.h"

using namespace cocos2d;

#define START_ITEM 1
#define SOUND_ITEM 2
#define EXIT_ITEM 3

CCScene* MenuScene::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::node();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        MenuScene *layer = MenuScene::node();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MenuScene::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(! CCLayer::init());

		createMainMenu();

		/*CCSize size = CCDirector::sharedDirector()->getWinSize();
		CCSprite* pSprite = CCSprite::spriteWithFile("HelloWorld.png");
        CC_BREAK_IF(! pSprite);

        // Place the sprite on the center of the screen
        pSprite->setPosition(ccp(size.width/2, size.height/2));

        // Add the sprite to HelloWorld layer as a child layer.
        this->addChild(pSprite, 0);
		*/
		this->setIsTouchEnabled(true);
        bRet = true;
    } while (0);

    return bRet;
}

void MenuScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionMoveInR::transitionWithDuration(0.5f, Gameplay::scene()));
}

void MenuScene::createMainMenu() {
	CCMenuItemLabel* startLabel = CCMenuItemFont::itemFromString("Start", this, menu_selector(MenuScene::onItemClick));
	startLabel->setTag(1);
	CCMenuItemLabel* soundLabel = CCMenuItemFont::itemFromString("Sound", this, menu_selector(MenuScene::onItemClick));
	startLabel->setTag(2);
	CCMenuItemLabel* exitLabel = CCMenuItemFont::itemFromString("Exit", this, menu_selector(MenuScene::onItemClick));
	startLabel->setTag(3);

	mainMenu = CCMenu::menuWithItems(exitLabel, soundLabel, startLabel, NULL);
	addChild(mainMenu);
}

void MenuScene::onItemClick(CCObject* sender) {
	CCNode* item = (CCNode*)sender;
	switch(item->getTag()) {
	case START_ITEM:
		break;
	case SOUND_ITEM:
		break;
	case EXIT_ITEM:
		break;
	}
}

void MenuScene::mainMenuEnterAnimation() {
	CCSize winSize = CCDirector::sharedDirector()->getWinSize();
	CCArray* nodes = mainMenu->getChildren();
	for(int i = 0; i < nodes->count(); ++i) {
		CCNode* node = (CCNode*)nodes->objectAtIndex(i);

		bool left = i%2==0 ? true : false;

		int posx = left ? -200 : winSize.width + 200;
		int posy = 50+i*50;
		node->setAnchorPoint(ccp(0.5f, 0.5f));
		node->setPosition(ccp(posx, posy));

		CCMoveTo* moveTo = CCMoveTo::actionWithDuration(0.5f, ccp(winSize.width/2.0f, posy));
		CCEaseBounceOut* bounce = CCEaseBounceOut::actionWithAction(moveTo);
		node->runAction(bounce);
	}
}

void MenuScene::onEnter() {
	mainMenuEnterAnimation();
}