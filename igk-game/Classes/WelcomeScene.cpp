#include "WelcomeScene.h"
#include "MenuScene.h"
#include "Gameplay.h"

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
		title = CCSprite::spriteWithFile("title.png");
        CC_BREAK_IF(! title);

        // Place the sprite on the center of the screen
		title->setAnchorPoint(ccp(0,1));
        title->setPosition(ccp(160, size.height-150));

        // Add the sprite to HelloWorld layer as a child layer.
        this->addChild(title, 0);


		text = CCSprite::spriteWithFile("text.png");
		CC_BREAK_IF(! text);

		// Place the sprite on the center of the screen
		text->setAnchorPoint(ccp(0,1));
		text->setPosition(ccp(105, size.height-320));

		// Add the sprite to HelloWorld layer as a child layer.
		this->addChild(text, 0);

		text->setIsVisible(false);
		title->setIsVisible(false);


		this->setIsTouchEnabled(true);
        bRet = true;
    } while (0);

    return bRet;
}

void WelcomeScene::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCDirector::sharedDirector()->replaceScene(CCTransitionMoveInR::transitionWithDuration(0.5f, Gameplay::scene()));
}

void WelcomeScene::onEnterTransitionDidFinish()
{
	title->setIsVisible(true);
	text->setIsVisible(true);
	CCFadeIn* fade = CCFadeIn::actionWithDuration(0.5);
	title->runAction(fade);
	text->runAction((CCFadeIn*)fade->copy());
}
