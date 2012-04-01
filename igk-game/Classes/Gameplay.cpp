#include "Gameplay.h"
#include "Input.h"

#define PTM_RATIO 32

const int MaxPlanets = 64;
const float MinPlanetDistance = 600;
const float MinShowPlanetDistance = 1500;
const float MaxPlanetDistance = 2000;
const float PlanetsDistance = 200;

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

	playerPos = CCLabelTTF::labelWithString("text", "Verdana", 16);
	playerPos->setAnchorPoint(ccp(0,0));
	playerPos->setPosition(ccp(100, 100));

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
	world->addChild(playerPos);

	// setup world rotation around sun
	float sunAnchorPositionX = (sun->getPositionX())  / world->getContentSize().width;
	world->setAnchorPoint(ccp(sunAnchorPositionX, 0.5));
	world->setPosition(ccp(world->getPositionX() - (0.5 + fabs(sunAnchorPositionX)) * world->getContentSize().width, world->getPositionY()));
	

	initPhysicalWorld();
	scheduleUpdate();
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	createPlayer(screenSize.width/2, screenSize.height/2);
	
	//addPlanet("planet_01.png", ccp(500, 100));
	//addPlanet("planet_02.png", ccp(500, 200));
	//addPlanet("planet_03.png", ccp(500, 300));

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

bool Gameplay::outsideView(const CCPoint &pos, float* distance2, b2Vec2* normalized)
{
	b2Vec2 posCenter(pos.x, pos.y);
	b2Vec2 sunCenter = b2Vec2(sun->getPosition().x, sun->getPosition().y);
	float distance = (sunCenter - posCenter).Length();
	if(distance2)
		*distance2 = (distance - MinPlanetDistance) / (MaxPlanetDistance - MinPlanetDistance);
	if(normalized)
	{
		*normalized = (sunCenter - posCenter);
		normalized->Normalize();
	}
	if(distance < MinPlanetDistance || MaxPlanetDistance < distance)
		return true;
	return false;
}

bool Gameplay::hasPlanetsNear(const CCPoint &pos, float radius)
{
	b2Vec2 posCenter(pos.x, pos.y);
	for(int i = mPlanets.size(); i-- > 0; ) 
	{
		Planet* planet = mPlanets[i];
		b2Vec2 planetCenter = planet->mPlanetBody->GetPosition();
		planetCenter.x *= PTM_RATIO;
		planetCenter.y *= PTM_RATIO;
		float distance = (planetCenter - posCenter).Length();
		if(distance < radius)
			return true;
	}
	return false;
}

void Gameplay::updatePlanets(ccTime dt)
{
	for(int i = mPlanets.size(); i-- > 0; ) 
	{
		Planet* planet = mPlanets[i];
		b2Vec2 planetCenter = planet->mPlanetBody->GetPosition();
		CCPoint planetScreen(planetCenter.x * PTM_RATIO, planetCenter.y * PTM_RATIO);

		float dist = 1.0f;
		b2Vec2 normalized;

		if(outsideView(planetScreen, &dist, &normalized))
		{
			removePlanet(i);
			continue;
		}

		planet->setPos(planetScreen);
		planet->setAngle(planet->mPlanetBody->GetAngle() / M_PI * 180.0f);

		// dist *= 1.0f - exp(- dist / dt);

		dist *= 0.3f;
		dist += 0.7f;

		float force = 0.2f / (dist * dist);

		normalized.x *= force;
		normalized.y *= force;

		planet->mPlanetBody->ApplyForceToCenter(normalized);
		
		// b2Vec2 velocity = planet->mPlanetBody->GetLinearVelocity();
		// b2Vec2 force(velocity.x * dt, velocity.y * dt);
		// planetCenter += velocity * dt;
		// planet->mPlanetBody->ApplyForceToCenter(force);
		// planet->mPlanetBody->ApplyTorque(planet->mPlanetBody->GetAngularVelocity());
		// planet->mPlanetBody->ApplyTorque(M_PI);
		// planet->mPlanetBody->ApplyAngularImpulse(180.0f);
	}

	srand(GetTickCount());

	CCPoint center = sun->getPosition();

	while(mPlanets.size() < MaxPlanets) 
	{
		float angle = 2 * M_PI * (float)rand() / RAND_MAX;
		float distance = (float)rand() / RAND_MAX;
		distance = MinShowPlanetDistance + distance * (MaxPlanetDistance - MinShowPlanetDistance);
		CCPoint dir(cos(angle) * distance, sin(angle) * distance);
		CCPoint planet(center.x + dir.x, center.y + dir.y);

		if(outsideView(planet))
			continue;
		if(hasPlanetsNear(planet, PlanetsDistance))
			continue;

		const int MaxImages = 3;
		static const char* images[MaxImages] = {
			"planet_01.png",
			"planet_02.png",
			"planet_03.png"
		};

		Planet* planetObj = addPlanet(images[rand() % MaxImages], planet);
		
		const float AngleDiff = 2.0f * M_PI / 30.0f;
		const float AngularMin = 60.0f / 180.0f * M_PI;
		const float AngularMax = 180.0f / 180.0f * M_PI;
		const float VelocityMin = 1 / PTM_RATIO;
		const float VelocityMax = 3 / PTM_RATIO;
		
		angle = 2 * M_PI * (float)rand() / RAND_MAX;
		float vel = VelocityMin + (float)rand() / RAND_MAX * (VelocityMax - VelocityMin);
		float avel = AngularMin + (float)rand() / RAND_MAX * (AngularMax - AngularMin);
		planetObj->mPlanetBody->SetLinearVelocity(b2Vec2(cos(angle) * -vel, sin(angle) * -vel));
		planetObj->mPlanetBody->SetAngularVelocity(avel);
	}
}

void Gameplay::updatePhysic( ccTime dt )
{
	int32 velocityIterations = 8;
	int32 positionIterations = 1;

	updatePlanets(dt);

	mWorld->Step(dt, velocityIterations, positionIterations);

#if 0
	for(int i = 0; i < mPlanets.size(); ++i) {
		Planet* planet = mPlanets[i];
		b2Body* planetBody = planet->mPlanetBody;
		b2CircleShape* planetShape = (b2CircleShape*)planet->mPlanetFixture->GetShape();

		b2Vec2 planetCenter = planetBody->GetWorldPoint(planetShape->m_p);

		b2Vec2 distance = planetCenter - mPlayer->mPlayerBody->GetPosition();

		float force = 250.0f / distance.LengthSquared(); // 150 can be changed to adjust the amount of force
		distance.Normalize();
		b2Vec2 F = force * distance;
		// Finally apply a force on the body in the direction of the "Planet"
		mPlayer->mPlayerBody->ApplyForce(F, mPlayer->mPlayerBody->GetPosition());


		mPlayer->mPlayer->setPosition(ccp( mPlayer->mPlayerBody->GetPosition().x * PTM_RATIO, mPlayer->mPlayerBody->GetPosition().y * PTM_RATIO));
		mPlayer->mPlayer->setRotation(-1 * CC_RADIANS_TO_DEGREES(mPlayer->mPlayerBody->GetAngle()));
	}
#endif
}

void Gameplay::update(ccTime dt) {
	Input::instance()->update();

	if(Input::instance()->keyDown(VK_UP)) {
		mPlayer->mPlayer->setPositionY(mPlayer->mPlayer->getPositionY() + 100 * dt);
	}

	if(Input::instance()->keyDown(VK_DOWN)) {
		mPlayer->mPlayer->setPositionY(mPlayer->mPlayer->getPositionY() - 100 * dt);
	}

	if(Input::instance()->keyDown(VK_LEFT)) {
		mPlayer->mPlayer->setPositionX(mPlayer->mPlayer->getPositionX() - 100 * dt);
	}

	if(Input::instance()->keyDown(VK_RIGHT)) {
		mPlayer->mPlayer->setPositionX(mPlayer->mPlayer->getPositionX() + 100 * dt);
	}

	CCPoint sub = ccpSub(mPlayer->mPlayer->getPosition(), sun->getPosition());
	float angle =  CC_RADIANS_TO_DEGREES(ccpToAngle(sub));
	world->setRotation(angle);

	char buf[256];
	sprintf(buf, "%f %f", mPlayer->mPlayer->getPositionX(), mPlayer->mPlayer->getPositionY());
	playerPos->setString(buf);

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

void Gameplay::removePlanet(int i)
{
	if(i < 0 || i >= mPlanets.size())
		return;

	if(mPlanets[i]) {
		world->removeChild(mPlanets[i]->getSprite(), true);
		mWorld->DestroyBody(mPlanets[i]->mPlanetBody);
		delete mPlanets[i];
		mPlanets[i] = NULL;
	}

	if(i + 1 < mPlanets.size())
		mPlanets[i] = mPlanets.back();
	mPlanets.pop_back();
}

Planet* Gameplay::addPlanet( std::string planetSpriteName, CCPoint position )
{
	Planet* planet = new Planet(planetSpriteName);
	planet->setPos(position);
	world->addChild(planet->getSprite());

	mPlanets.push_back(planet);

	// Physical representation

	b2BodyDef planetBodyDef;
	planetBodyDef.type = b2_dynamicBody;
	planetBodyDef.position.Set(position.x/PTM_RATIO, position.y/PTM_RATIO);
	planetBodyDef.userData = mPlayer;
	b2Body* planetBody = mWorld->CreateBody(&planetBodyDef);

	b2CircleShape shape;
	shape.m_radius = planet->getSprite()->getContentSize().width / 2 / PTM_RATIO;
	//shape.m_p.Set(8.0f, 8.0f);
	b2FixtureDef fd;
	fd.shape = &shape;
	fd.restitution = 1.0f;
	fd.friction = 0.0f;
	b2Fixture* planetFixture = planetBody->CreateFixture(&fd);

	planet->mPlanetBody = planetBody;
	planet->mPlanetFixture = planetFixture;

	return planet;
}

void Gameplay::clearLevel()
{
	while(mPlanets.size())
		removePlanet(0);
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


