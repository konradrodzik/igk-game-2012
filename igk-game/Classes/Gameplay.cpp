#include "Gameplay.h"
#include "Input.h"

#define PTM_RATIO (1.0f/32.0f)

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

	impulseTimer = 2;

	playerPos = CCLabelTTF::labelWithString("text", "Verdana", 16);
	playerPos->setAnchorPoint(ccp(0,0));
	playerPos->setPosition(ccp(100, 100));

	world = CCNode::node();
	world->setContentSize(size);
	world->setAnchorPoint(ccp(0.5, 0.5));
	world->setPosition(ccp(size.width / 2, size.height / 2));
	addChild(world);
	
	// Background
	for(int x = -3; x <= 3; ++x) {
		for(int y = -3; y <= 3; ++y) {
			mBackground = CCSprite::spriteWithFile("space.png");
			mBackground->setAnchorPoint(ccp(0,0));
			mBackground->setPosition(ccp(1024*x,768*y));
			mBackground->setFlipX((x&1) != 0);
			mBackground->setFlipY((y&1) != 0);
			world->addChild(mBackground);
		}
	}

	// setup sun
	sun = new Sun();
	sun->setPosition(ccp(-sun->getContentSize().width / 2 + 400, world->getContentSize().height / 2));
	world->addChild(sun);
	world->addChild(playerPos);

	// setup world rotation around sun
	float sunAnchorPositionX = (sun->getPositionX())  / world->getContentSize().width;
	world->setAnchorPoint(ccp(sunAnchorPositionX, 0.5));
	world->setPosition(ccp(world->getPositionX() - (0.5 + fabs(sunAnchorPositionX)) * world->getContentSize().width, world->getPositionY()));

	cursor = CCSprite::spriteWithFile("player.png");
	cursor->retain();

	initPhysicalWorld();
	scheduleUpdate();
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	createPlayer(screenSize.width/2, screenSize.height/2);
	
	hud = CCSprite::spriteWithFile("gui.png");
	hud->setAnchorPoint(ccp(0.0f, 0.0f));
	hud->setPosition(ccp(0,0));
	addChild(hud);

	/*for(int i = 0; i < 3; ++i) {
		mLifeSprites[0] = CCSprite::spriteWithFile("life.png");
	}
	
	mLifeSprites[1] = CCSprite::spriteWithFile("life.png");
	mLifeSprites[2] = CCSprite::spriteWithFile("life.png");
	hud->addChild(mLifeSprites[0]);
	hud->addChild(mLifeSprites[0]);
	hud->addChild(mLifeSprites[0]);*/

	grid = CCSprite::spriteWithFile("siatka.png");
	grid->setAnchorPoint(ccp(0,1));
	grid->setPosition(ccp(296, 664));
	addChild(grid);

	scoreText = CCLabelTTF::labelWithString("Score: 0", "Verdana",30);
	scoreText->setAnchorPoint(ccp(0.0f, 0.5f));
	scoreText->setPosition(ccp(50,hud->getContentSize().height/2.0f));
	hud->addChild(scoreText);

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
		planetCenter.x /= PTM_RATIO;
		planetCenter.y /= PTM_RATIO;
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
		CCPoint planetScreen(planetCenter.x / PTM_RATIO, planetCenter.y / PTM_RATIO);

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
		const float VelocityMin = 0.25 * PTM_RATIO;
		const float VelocityMax = 1.0f * PTM_RATIO;

		int directionLeft = rand()%2;
		
		angle = 2 * M_PI * (float)rand() / RAND_MAX;
		float vel = VelocityMin + (float)rand() / RAND_MAX * (VelocityMax - VelocityMin);
		float avel = AngularMin + (float)rand() / RAND_MAX * (AngularMax - AngularMin);
		planetObj->mPlanetBody->SetLinearVelocity(b2Vec2(cos(angle) * -vel, sin(angle) * -vel));
		planetObj->mPlanetBody->SetAngularVelocity(avel * directionLeft==0 ? -1 : 1);
	}
}

void Gameplay::updatePhysic( ccTime dt )
{
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	int32 velocityIterations = 8;
	int32 positionIterations = 1;

	updatePlanets(dt);

	mWorld->Step(dt, velocityIterations, positionIterations);

	b2Vec2 globalForce = b2Vec2_zero;
	for(int i = 0; i < mPlanets.size(); ++i) {
		Planet* planet = mPlanets[i];

		b2Body* planetBody = planet->mPlanetBody;
		b2CircleShape* planetShape = (b2CircleShape*)planet->mPlanetFixture->GetShape();

		b2Vec2 planetCenter = planetBody->GetWorldPoint(planetShape->m_p);
		b2Vec2 distance = planetCenter - mPlayer->mPlayerBody->GetPosition();
		float distanceLength = sqrt(distance.x*distance.x+distance.y*distance.y);

		float maxRadius = planet->maxGravityRadius;

		// rotating force
		float force = 1000;
		force = clampf(1-(distanceLength)/maxRadius, 0, 1) * force;
		b2Vec2 forceVector = b2Vec2(-distance.y, distance.x);
		forceVector.Normalize();
		b2Vec2 F = force * forceVector;

		// gravity
		float bla;
		float bla2;
		float gravityForce = 38;
		b2Vec2 gravityVec;
		b2Vec2 normalizedDistance = distance;
		normalizedDistance.Normalize();
		if(distanceLength < planet->gravityRadius) {
			bla = distanceLength / planet->gravityRadius; 
			gravityVec = b2Vec2(bla*gravityForce*-normalizedDistance.x, bla*gravityForce*-normalizedDistance.y);
		} else if(distanceLength < planet->maxGravityRadius) {
			bla = (distanceLength - planet->gravityRadius) / (planet->maxGravityRadius - planet->gravityRadius);
			bla2 = -pow(2*bla-1, 2) + 1;
			gravityVec = b2Vec2(bla2*gravityForce*normalizedDistance.x, bla2*gravityForce*normalizedDistance.y);
		} else {
			gravityVec = b2Vec2_zero;
		}

		F += gravityVec;

		globalForce += F;
	}

	// engine force
	CCPoint engineForceVectorCC = mPlayer->direction;
	b2Vec2 engineForceVector = b2Vec2(engineForceVectorCC.x, engineForceVectorCC.y);
	engineForceVector *= 200;
	globalForce += engineForceVector;

	// sun gravity
	float sunRadius = 800 * PTM_RATIO;
	b2Vec2 sunPosition = b2Vec2(sun->getPosition().x * PTM_RATIO, sun->getPosition().y * PTM_RATIO);
	b2Vec2 sunGravityVector = sunPosition - mPlayer->mPlayerBody->GetPosition();
	float playerSunDistance = sunGravityVector.Length() - sunRadius;
	//if(playerSunDistance > 0)
	{
		float maxSunDistance = size.width * PTM_RATIO + fabs(sun->getPositionX() * PTM_RATIO) - sunRadius;
		float sunGravityFactor = playerSunDistance / maxSunDistance;
		float sunGravityForce = 500 * sunGravityFactor;

		sunGravityVector.Normalize();

		sunGravityVector *= sunGravityForce;
		globalForce += sunGravityVector;
	}
	// Finally apply a force on the body in the direction of the "Planet"
	mPlayer->mPlayerBody->SetLinearVelocity(b2Vec2(0,0));
	mPlayer->mPlayerBody->SetAngularVelocity(0);
	mPlayer->mPlayerBody->ApplyForce(globalForce, mPlayer->mPlayerBody->GetPosition());

	mPlayer->mPlayer->setPosition(ccp( mPlayer->mPlayerBody->GetPosition().x / PTM_RATIO, mPlayer->mPlayerBody->GetPosition().y / PTM_RATIO));

	//float oldRotation  = mPlayer->mPlayer->getRotation();
	//mPlayer->mPlayer->setRotation(-1 * CC_RADIANS_TO_DEGREES(mPlayer->mPlayerBody->GetAngle()));
}

void Gameplay::update(ccTime dt) {
	impulseTimer += dt;

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
	bodyDef.position.Set(position.x*PTM_RATIO, position.y*PTM_RATIO);
	bodyDef.userData = mPlayer;
	b2Body *body = mWorld->CreateBody(&bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(0.5f, 0.5f);

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;	
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.0f;
	fixtureDef.restitution = 0.5f;
	b2Fixture* playerFixture = body->CreateFixture(&fixtureDef);

	mPlayer->mPlayerBody = body;
	mPlayer->mPlayerFixture = playerFixture;

	mPlayer->direction = ccp(0, 1);
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

	planet->setGravityRadius(planet->getSprite()->getContentSize().width);
	//planet->gravityRadius = planet->getSprite()->getContentSize().width * PTM_RATIO;
	//planet->maxGravityRadius = 2*planet->gravityRadius;

	mPlanets.push_back(planet);

	// Physical representation

	b2BodyDef planetBodyDef;
	planetBodyDef.type = b2_dynamicBody;
	planetBodyDef.position.Set(position.x*PTM_RATIO, position.y*PTM_RATIO);
	planetBodyDef.userData = planet;
	b2Body* planetBody = mWorld->CreateBody(&planetBodyDef);

	b2CircleShape shape;
	shape.m_radius = planet->getSprite()->getContentSize().width / 2 * PTM_RATIO;
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

void Gameplay::playerJetpack()
{
	if(impulseTimer > 2) 
	{
		impulseTimer = 0;
		CCPoint dir = ccpForAngle(CC_DEGREES_TO_RADIANS(90 + mPlayer->mPlayer->getRotation()));

		b2Vec2 direction = b2Vec2(dir.x, dir.y);
		direction *= 10;
		mPlayer->mPlayerBody->ApplyLinearImpulse(direction, mPlayer->mPlayerBody->GetPosition());
	}
}

void Gameplay::playerLookAt(CCPoint p)
{
	CCPoint dir = ccpSub(p, mPlayer->mPlayer->getPosition());
	mPlayer->direction = ccpNormalize(dir);
	float angle = -CC_RADIANS_TO_DEGREES(ccpToAngle(dir)) + 90;
	mPlayer->mPlayer->setRotation(angle);
}

void Gameplay::ccTouchesBegan(CCSet *pTouches, CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)pTouches->anyObject();
	CCPoint p = world->convertTouchToNodeSpace(touch);

	cursor->setPosition(p);
	world->addChild(cursor);

	playerLookAt(cursor->getPosition());
}

void Gameplay::ccTouchesMoved(CCSet *pTouches, CCEvent *pEvent)
{
	CCTouch* touch = (CCTouch*)pTouches->anyObject();
	CCPoint p = world->convertTouchToNodeSpace(touch);

	cursor->setPosition(p);
	playerLookAt(cursor->getPosition());
}

void Gameplay::ccTouchesEnded(CCSet *pTouches, CCEvent *pEvent)
{
	playerLookAt(cursor->getPosition());
	world->removeChild(cursor, false);
}

void Gameplay::updateScore()
{	
	char tab[255] = {0};
	sprintf(tab, "Score: %i", mPlayer->score);
	scoreText->setString(tab);
}
