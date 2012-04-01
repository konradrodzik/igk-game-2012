#include "WelcomeScene.h"
#include "MenuScene.h"

CCScene* WelcomeScene::scene()
{
    CCScene * scene = NULL;
    do 
    {
        // 'scene' is an autorelease object
        scene = CCScene::node();
        CC_BREAK_IF(! scene);

        // 'layer' is an autorelease object
        WelcomeScene *layer = WelcomeScene::node();
        CC_BREAK_IF(! layer);

        // add layer as a child to scene
        scene->addChild(layer);
    } while (0);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool WelcomeScene::init()
{
    bool bRet = false;
    do 
    {
        CC_BREAK_IF(! CCLayer::init());

		CCSize size = CCDirector::sharedDirector()->getWinSize();
		CCSprite* pSprite = CCSprite::spriteWithFile("title.png");
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

void WelcomeScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionMoveInR::transitionWithDuration(0.5f, MenuScene::scene()));
}