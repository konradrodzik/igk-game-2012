#include "MenuScene.h"
#include "Gameplay.h"

using namespace cocos2d;

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

		CCSize size = CCDirector::sharedDirector()->getWinSize();
		CCSprite* pSprite = CCSprite::spriteWithFile("HelloWorld.png");
        CC_BREAK_IF(! pSprite);

        // Place the sprite on the center of the screen
        pSprite->setPosition(ccp(size.width/2, size.height/2));

        // Add the sprite to HelloWorld layer as a child layer.
        this->addChild(pSprite, 0);

		this->setIsTouchEnabled(true);
        bRet = true;
    } while (0);

    return bRet;
}

void MenuScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionMoveInR::transitionWithDuration(0.5f, Gameplay::scene()));
}