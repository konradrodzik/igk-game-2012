#include "Sun.h"


Sun::Sun(void)
{
	initWithFile("sun.png");
	createSunAnimation();
}

Sun::~Sun(void)
{
}

void Sun::createSunAnimation()
{
	/*sunAnimSprite = new CCSprite();
	sunAnimSprite->initWithFile("sun_anim.png");
	sunAnimation = new CCAnimation();
	sunAnimation->initWithFrames(NULL, 0.3f);
	for(int y = 0; y < 10; ++y) {
		for(int x = 0; x < 12; ++x) {
			float width = 400.0f;
			float height = 400.0f;
			CCSpriteFrame* frame = CCSpriteFrame::frameWithTexture(sunAnimSprite->getTexture(), CCRect(x*width, y*height, width, height));
			sunAnimation->addFrame(frame);
		}
	}*/
}

void Sun::playBurnAnim()
{
	CCActionInterval* sunAction = CCAnimate::actionWithAnimation(sunAnimation);

}
