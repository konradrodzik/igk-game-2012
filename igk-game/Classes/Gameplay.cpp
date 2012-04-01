#include "Gameplay.h"
#include "Input.h"

#define PTM_RATIO 32

Gameplay::Gameplay() {
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

	// Background
	mBackground = CCSprite::spriteWithFile("space.png");
	mBackground->setAnchorPoint(ccp(0,0));
	mBackground->setPosition(ccp(0,0));
	addChild(mBackground);

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
	

	initPhysicalWorld();
	scheduleUpdate();
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	createPlayer(screenSize.width/2, screenSize.height/2);
	
	addPlanet("planet_01.png", ccp(800, 100));
	addPlanet("planet_02.png", ccp(500, 300));
	addPlanet("planet_03.png", ccp(800, 600));


	return true;
}

void Gameplay::initPhysicalWorld()
{
	// Create Box2d world
	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
	bool doSleep = false;
	mWorld = new b2World(gravity);
	mWorld->SetContinuousPhysics(true);

	// Debug Draw functions
	GLESDebugDraw* m_debugDraw = new GLESDebugDraw( PTM_RATIO );
	mWorld->SetDebugDraw(m_debugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_pairBit;
	m_debugDraw->SetFlags(flags);	
}

void Gameplay::updatePhysic( ccTime dt )
{
	int32 velocityIterations = 8;
	int32 positionIterations = 1;

	mWorld->Step(dt, velocityIterations, positionIterations);

	for(int i = 0; i < mPlanets.size(); ++i) {
		Planet* planet = mPlanets[i];

		b2Body* planetBody = planet->mPlanetBody;
		b2CircleShape* planetShape = (b2CircleShape*)planet->mPlanetFixture->GetShape();

		b2Vec2 planetCenter = planetBody->GetWorldPoint(planetShape->m_p);
		b2Vec2 distance = planetCenter - mPlayer->mPlayerBody->GetPosition();
		float distanceLength = sqrt(distance.x*distance.x+distance.y*distance.y);

		float maxRadius = planet->getSprite()->getContentSize().width * PTM_RATIO;
		float smallRadius = planet->getSprite()->getContentSize().width/2 * 1.6;
		
		float force = 500;// / distance.LengthSquared(); // 150 can be changed to adjust the amount of force
		force = clampf(1-(distanceLength)/maxRadius, 0, 1) * force;


		b2Vec2 gravityVec;
		float gravityForce = 9.8;
		float gravityStrength = clampf(1-(distanceLength)/maxRadius, 0, 1) * gravityForce;;
		if(distanceLength > smallRadius) {
			  gravityVec = b2Vec2(-distance.x, -distance.y);
		} else {
			 gravityVec = b2Vec2(distance.x, distance.y);
		}
		gravityVec.Normalize();
		gravityVec *= gravityForce;

		b2Vec2 forceVector = b2Vec2(-distance.y, distance.x);
		forceVector.Normalize();
		b2Vec2 F = force * forceVector;
		F += gravityVec;
		// Finally apply a force on the body in the direction of the "Planet"
		mPlayer->mPlayerBody->SetLinearVelocity(b2Vec2(0,0));
		mPlayer->mPlayerBody->SetAngularVelocity(0);
		mPlayer->mPlayerBody->ApplyForce(F, mPlayer->mPlayerBody->GetPosition()/*+b2Vec2(0,5*PTM_RATIO)*/);



		mPlayer->mPlayer->setPosition(ccp( mPlayer->mPlayerBody->GetPosition().x * PTM_RATIO, mPlayer->mPlayerBody->GetPosition().y * PTM_RATIO));
		
		float oldRotation  = mPlayer->mPlayer->getRotation();
		mPlayer->mPlayer->setRotation(-1 * CC_RADIANS_TO_DEGREES(mPlayer->mPlayerBody->GetAngle()));
	}
}

void Gameplay::update(ccTime dt) {
Input::instance()->update();
	if(Input::instance()->keyDown(VK_UP)) {
		mPlayer->mPlayer->setPositionY(mPlayer->mPlayer->getPositionY() + 3);
	}

	if(Input::instance()->keyDown(VK_DOWN)) {
		mPlayer->mPlayer->setPositionY(mPlayer->mPlayer->getPositionY() - 3);
	}

	CCPoint sub = ccpSub(mPlayer->mPlayer->getPosition(), sun->getPosition());
	float angle =  CC_RADIANS_TO_DEGREES(ccpToAngle(sub));
	world->setRotation(angle);

	updatePhysic(dt);
}

void Gameplay::createPlayer(float posx, float posy)
{
	// CREATE SPRITE
	CCPoint position = ccp(posx, posy);
	mPlayer = new Player;
	mPlayer->mPlayer = CCSprite::spriteWithFile("astro.png");
	mPlayer->mPlayer->setPosition(ccp(position.x, position.y));
	world->addChild(mPlayer->mPlayer);

	// PHYSICAL REPRESENTATION
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position.Set(position.x/PTM_RATIO, position.y/PTM_RATIO);
	bodyDef.userData = mPlayer;
	b2Body *body = mWorld->CreateBody(&bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.5f, 0.5f);
	//dynamicBox.SetAsBox(mPlayer->getContentSize().width / 2.0f / PTM_RATIO, mPlayer->getContentSize().height / 2.0f / PTM_RATIO);//These are mid points for our 1m box

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;	
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	b2Fixture* playerFixture = body->CreateFixture(&fixtureDef);

	mPlayer->mPlayerBody = body;
	mPlayer->mPlayerFixture = playerFixture;
}

void Gameplay::addPlanet( std::string planetSpriteName, CCPoint position )
{
	Planet* planet = new Planet(planetSpriteName);
	planet->setPos(position);
	world->addChild(planet->getSprite());

	mPlanets.push_back(planet);

	// Physical representation

	b2BodyDef planetBodyDef;
	planetBodyDef.position.Set(position.x/PTM_RATIO, position.y/PTM_RATIO);
	planetBodyDef.userData = planet;
	b2Body* planetBody = mWorld->CreateBody(&planetBodyDef);

	b2CircleShape shape;
	shape.m_radius = planet->getSprite()->getContentSize().width / 2 / PTM_RATIO;
	//shape.m_p.Set(8.0f, 8.0f);
	b2FixtureDef fd;
	fd.shape = &shape;
	b2Fixture* planetFixture = planetBody->CreateFixture(&fd);

	planet->mPlanetBody = planetBody;
	planet->mPlanetFixture = planetFixture;
}

void Gameplay::clearLevel()
{
	for(int i = 0; i < mPlanets.size(); ++i) {
		if(mPlanets[i]) {
			removeChild(mPlanets[i]->getSprite(), true);
			delete mPlanets[i];
			mPlanets[i] = NULL;
		}
	}
	mPlanets.clear();
}

void Gameplay::draw()
{
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	mWorld->DrawDebugData();

	// restore default GL states
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
}

void Gameplay::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)pTouches->anyObject();
	CCPoint p = CCDirector::sharedDirector()->convertToGL(touch->locationInView(touch->view()));

	CCParticleSystem* particle = ParticleFactory::explosion();
	particle->setPosition(p);
	world->addChild(particle, 1);
}

