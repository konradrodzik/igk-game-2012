#include "Gameplay.h"
#include "Input.h"
#include "SimpleAudioEngine.h"
#define PTM_RATIO (1.0f/32.0f)

const int MaxPlanets = 70;
const int MaxTrashes = 60;
const float MinPlanetDistance = 903;
const float MinShowPlanetDistance = 1500;
const float MaxPlanetDistance = 2000;
const float PlanetsDistance = 200;
const float BoundsDistance = 1640.0f;

DWORD PlaySoundThread(void* ptr)
{
	PlaySoundA((const char*)ptr, NULL, 0);
	return 0;
}

void PlaySoundThreaded(const char* ptr)
{
	// CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PlaySoundThread, (LPVOID)ptr, 0, 0);
}

Gameplay::Gameplay() {
	impulseFuel = 100;
	drainImpulseFuel = false;
	nextRocketTimer = 0;
	gayplay = this;
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
	gameIsPlaying = 1;
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("explosion.mp3");
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
	
	for(int i = 0; i < 3; ++i)
	{
		CCParticleSystemQuad* galaxy = new CCParticleGalaxy();
		galaxy->initWithFile("ExplodingRing.plist");
		galaxy->setEmitterMode(1);
		galaxy->setPosition(sun->getPosition());
		galaxy->setStartRadius(BoundsDistance - 1 + (i & 3) * 5);
		galaxy->setEndRadius(BoundsDistance + 1 + (i & 3) * 5);
		galaxy->setDuration(100000.0f);
		galaxy->setStartSize(35);
		galaxy->setEndSize(50);
		world->addChild(galaxy);
	}
	
	for(int i = 0; i < 3; ++i)
	{
		CCParticleSystemQuad* galaxy = new CCParticleGalaxy();
		galaxy = new CCParticleGalaxy();
		galaxy->initWithFile("ExplodingRing.plist");
		galaxy->setEmitterMode(1);
		galaxy->setPosition(sun->getPosition());
		galaxy->setStartRadius(MinPlanetDistance - 1 + i * 5);
		galaxy->setEndRadius(MinPlanetDistance + 1 + i * 5);
		galaxy->setDuration(100000.0f);
		galaxy->setStartSize(30);
		galaxy->setEndSize(60);
		world->addChild(galaxy);
	}

	initPhysicalWorld();
	scheduleUpdate();
	CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
	createPlayer(screenSize.width/2, screenSize.height/2);
	
	hud = CCSprite::spriteWithFile("gui.png");
	hud->setAnchorPoint(ccp(0.0f, 0.0f));
	hud->setPosition(ccp(0,0));
	addChild(hud);
	

	for(int i = 0; i < 3; ++i) {
		CCSprite* life = CCSprite::spriteWithFile("life.png");
		mLifeSprites.push_back(life);
		life->setPosition(ccp(hud->getContentSize().width-3*(10+life->getContentSize().width) + i*(10+life->getContentSize().width), hud->getContentSize().height/2.0f));
		hud->addChild(life);
	}


	//CCParticleSystem *particleSystem = ParticleFactory::stars();
	//particleSystem->setPosition(screenSize.width / 2, screenSize.height / 2);
	//world->addChild(particleSystem, 1);
	trail = ParticleFactory::meteor(); 
	trail->setPositionType(kCCPositionTypeRelative);
	trail->setPosition(mPlayer->mPlayer->getPosition());
	trail->setDuration(20.0f);
	world->addChild(trail, 1);

	

	grid = CCSprite::spriteWithFile("siatka.png");
	grid->setAnchorPoint(ccp(0,1));
	grid->setPosition(ccp(296, size.height-664));
	addChild(grid);

	impulseFuelIndicator = CCLayerColor::layerWithColor(ccc4(1, 255, 1, 128));
	impulseFuelIndicator->setAnchorPoint(ccp(0, 1));
	impulseFuelIndicator->setContentSize(grid->getContentSize());
	impulseFuelIndicator->setPosition(grid->getPosition());
	impulseFuelIndicator->setPositionY(impulseFuelIndicator->getPositionY() - grid->getContentSize().height);
	addChild(impulseFuelIndicator);
	
	timeLabel = CCLabelTTF::labelWithString("Time: 0", "Verdana",30);
	timeLabel->setAnchorPoint(ccp(0.5f, 0.5f));
	timeLabel->setPosition(ccp(grid->getContentSize().width * 0.5f,grid->getContentSize().height * 0.5f));
	grid->addChild(timeLabel);

	scoreText = CCLabelTTF::labelWithString("Score: 0", "Verdana",30);
	scoreText->setAnchorPoint(ccp(0.0f, 0.5f));
	scoreText->setPosition(ccp(50,hud->getContentSize().height/2.0f));
	hud->addChild(scoreText);


	// Init explosion anim
	/*CCSprite* mExplosionAnimTexture;
	CCAnimation* mExplosionpakAnimation;
	mExplosionAnimTexture = new CCSprite;
	mExplosionAnimTexture->initWithFile("explosion.png");
	mExplosionpakAnimation = new CCAnimation();
	mExplosionpakAnimation->initWithFrames(NULL, 0.03f);
	for(int y = 0; y < 7; ++y) {
		for(int x = 0; x < 8; ++x) {
			float width = 256;
			float height = 256.0f;
			CCSpriteFrame* frame = CCSpriteFrame::frameWithTexture(mExplosionAnimTexture->getTexture(), CCRect(x*width, y*height, width, height));
			mExplosionpakAnimation->addFrame(frame);
		}
	}*/

	return true;
}


void Gameplay::initPhysicalWorld()
{
	totalTime = 0;
	// Create Box2d world
	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
	bool doSleep = false;
	mWorld = new b2World(gravity);
	mWorld->SetContinuousPhysics(true);
	listener = new Listener();
	mWorld->SetContactListener(listener);
	// Debug Draw functions
	GLESDebugDraw* m_debugDraw = new GLESDebugDraw( PTM_RATIO );
	mWorld->SetDebugDraw(m_debugDraw);

	uint32 flags = 0;
	flags += b2Draw::e_shapeBit;
	flags += b2Draw::e_pairBit;
	m_debugDraw->SetFlags(flags);	
}

bool Gameplay::outsideView(const CCPoint &pos, float* distance2, b2Vec2* normalized, bool inner)
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
	if(inner)
	{
		if(distance < MinPlanetDistance || BoundsDistance < distance)
			return true;
	}
	else
	{
		if(distance < MinPlanetDistance || MaxPlanetDistance < distance)
			return true;
	}
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

void Gameplay::updateRockets(ccTime dt)
{
	for(int i = mRockets.size(); i-- > 0; ) 
	{
		Rocket* rocket = mRockets[i];
		b2Vec2 rocketCenter = rocket->mRocketBody->GetPosition();
		CCPoint rocketScreen(rocketCenter.x / PTM_RATIO, rocketCenter.y / PTM_RATIO);

		float dist = 1.0f;
		b2Vec2 normalized;

		if(outsideView(rocketScreen, &dist, &normalized))
		{
			CCParticleSystemQuad* quad = ParticleFactory::explosion();
			quad->setPosition(rocketScreen);
			quad->setDuration(0.4f);
			world->addChild(quad);
			PlaySoundThreaded("resources/explosion.mp3");

						
			removeRocket(i);
			continue;
		}

		rocket->setPos(rocketScreen);
		rocket->setAngle(rocket->mRocketBody->GetAngle() / M_PI * 180.0f);

		// dist *= 1.0f - exp(- dist / dt);

		dist *= 3.0f;
		dist += 0.8f;

		float force = 50.0f / (dist);

		normalized.x *= force;
		normalized.y *= force;

		rocket->mRocketBody->ApplyForceToCenter(normalized);
		rocket->mRocketTrail->setPosition(rocketScreen);

		for(int i = 0; i < mPlanets.size(); ++i)
		{
			Planet* planet = mPlanets[i];
			b2Vec2 planetCenter = planet->mPlanetBody->GetPosition();
			CCPoint planetScreen(planetCenter.x / PTM_RATIO, planetCenter.y / PTM_RATIO);
			float distance = ccpDistance(rocketScreen, planetScreen);

			if(distance > planet->maxGravityRadius)
				continue;

			distance /= planet->maxGravityRadius;

			CCPoint velocity = ccpNormalize(ccpSub(planetScreen, rocketScreen));

			rocket->mRocketBody->ApplyForceToCenter(10.0f / distance / distance * b2Vec2(velocity.x, velocity.y));
		}
	}
}

void Gameplay::updateTrash(ccTime dt)
{
	for(int i = mTrashes.size(); i-- > 0; ) 
	{
		Trash* trash = mTrashes[i];
		b2Vec2 trashCenter = trash->mTrashBody->GetPosition();
		CCPoint trashScreen(trashCenter.x / PTM_RATIO, trashCenter.y / PTM_RATIO);

		float dist = 1.0f;
		b2Vec2 normalized;

		if(outsideView(trashScreen, &dist, &normalized))
		{
			CCParticleSystemQuad* quad = ParticleFactory::explosion();
			quad->setPosition(trashScreen);
			quad->setDuration(0.4f);
			world->addChild(quad);
			
			
			/*CCSprite* explosion = CCSprite::spriteWithTexture(mExplosionAnimTexture->getTexture());
			world->addChild(explosion);
			 
				
			
			CCActionInterval* expAction = CCAnimate::actionWithAnimation(mExplosionpakAnimation);
			CCCallFuncND* callfunc = CCCallFuncND::actionWithTarget(this, callfuncND_selector(Gameplay::explosionCallback), NULL);
			CCSequence* seq = CCSequence::actionOneTwo(expAction, callfunc);
			explosion->stopAllActions();
			explosion->setIsVisible(true);
			explosion->runAction(seq);*/
			


			world->removeChild(mTrashes[i]->getSprite(), true);
			mWorld->DestroyBody(mTrashes[i]->mTrashBody);
			delete mTrashes[i];

			if(i + 1 < mTrashes.size())
				mTrashes[i] = mTrashes.back();
			mTrashes.pop_back();
			continue;
		}

		trash->setPos(trashScreen);
		trash->setAngle(trash->mTrashBody->GetAngle() / M_PI * 180.0f);

		// dist *= 1.0f - exp(- dist / dt);

		dist *= 3.0f;
		dist += 0.8f;

		float force = 50.0f / (dist);

		normalized.x *= force;
		normalized.y *= force;

		trash->mTrashBody->ApplyForceToCenter(normalized);
	}

	srand(GetTickCount());

	CCPoint center = sun->getPosition();

	while(mTrashes.size() < MaxTrashes) 
	{
		float angle = 2 * M_PI * (float)rand() / RAND_MAX;
		float distance = (float)rand() / RAND_MAX;
		distance = MinShowPlanetDistance + distance * (MaxPlanetDistance - MinShowPlanetDistance);
		CCPoint dir(cos(angle) * distance, sin(angle) * distance);
		CCPoint trashPos(center.x + dir.x, center.y + dir.y);

		if(outsideView(trashPos))
			continue;
		if(hasPlanetsNear(trashPos, PlanetsDistance))
			continue;

		const int MaxImages = 1;
		static const char* images[MaxImages] = {
			"satellite.png"
		};

		
		Trash* trash = new Trash(images[rand() % MaxImages]);
		trash->setPos(trashPos);
		trash->trashSprite->setScale(0.5f);
		world->addChild(trash->getSprite());

		mTrashes.push_back(trash);

		// Physical representation

		b2BodyDef trashBodyDef;
		trashBodyDef.type = b2_dynamicBody;
		trashBodyDef.position.Set(trashPos.x*PTM_RATIO, trashPos.y*PTM_RATIO);
		trashBodyDef.userData = trash;
		b2Body* trashBody = mWorld->CreateBody(&trashBodyDef);

		b2CircleShape shape;
		shape.m_radius = trash->getSprite()->getContentSize().width / 2 * PTM_RATIO / 2;
		//shape.m_p.Set(8.0f, 8.0f);
		b2FixtureDef fd;
		fd.shape = &shape;
		fd.density = 10.0f;
		fd.restitution = 1.0f;
		fd.friction = 0.0f;
		b2Fixture* trashFixture = trashBody->CreateFixture(&fd);

		trash->mTrashBody = trashBody;
		trash->mTrashFixture = trashFixture;
		
		const float AngleDiff = 2.0f * M_PI / 30.0f;
		const float AngularMin = 20.0f / 180.0f * M_PI;
		const float AngularMax = 60.0f / 180.0f * M_PI;
		const float VelocityMin = 0.25 * PTM_RATIO;
		const float VelocityMax = 1.0f * PTM_RATIO;

		int directionLeft = rand()%2;
		
		angle = 2 * M_PI * (float)rand() / RAND_MAX;
		float vel = VelocityMin + (float)rand() / RAND_MAX * (VelocityMax - VelocityMin);
		float avel = AngularMin + (float)rand() / RAND_MAX * (AngularMax - AngularMin);
		trash->mTrashBody->SetLinearVelocity(b2Vec2(cos(angle) * -vel, sin(angle) * -vel));
		trash->mTrashBody->SetAngularVelocity(avel * directionLeft==0 ? -1 : 1);
	}

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
			CCParticleSystemQuad* quad = ParticleFactory::explosion();
			quad->setPosition(planetScreen);
			quad->setDuration(0.4f);
			world->addChild(quad);

			removePlanet(i);
			
			PlaySoundThreaded("resources/explosion.mp3");

			//CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect("explosion.mp3");
			continue;
		}

		planet->setPos(planetScreen);
		planet->setAngle(planet->mPlanetBody->GetAngle() / M_PI * 180.0f);

		// dist *= 1.0f - exp(- dist / dt);

		dist *= 3.0f;
		dist += 0.8f;

		float force = 5000.0f / (dist);

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
		const float AngularMin = 20.0f / 180.0f * M_PI;
		const float AngularMax = 60.0f / 180.0f * M_PI;
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
	totalTime += dt;

	CCSize size = CCDirector::sharedDirector()->getWinSize();
	int32 velocityIterations = 8;
	int32 positionIterations = 1;

	updatePlanets(dt);
	updateTrash(dt);
	updateRockets(dt);

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
		float force = 800;
		force = clampf(1-(distanceLength)/maxRadius, 0, 1) * force;
		b2Vec2 forceVector = b2Vec2(-distance.y, distance.x);
		forceVector.Normalize();
		b2Vec2 F = force * forceVector;

		// gravity
		float bla;
		float bla2;
		float gravityForce = 100;
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
	if(drainImpulseFuel) {
		engineForceVector *= 400;
	} else {
		engineForceVector *= 100;
	}
	globalForce += engineForceVector;

	// sun gravity
	float sunRadius = 500 * PTM_RATIO;
	b2Vec2 sunPosition = b2Vec2(sun->getPosition().x * PTM_RATIO, sun->getPosition().y * PTM_RATIO);
	b2Vec2 sunGravityVector = sunPosition - mPlayer->mPlayerBody->GetPosition();
	float playerSunDistance = sunGravityVector.Length() - sunRadius;
	//if(playerSunDistance > 0)
	{
		float maxSunDistance = size.width * PTM_RATIO + fabs(sun->getPositionX() * PTM_RATIO) - sunRadius;
		float sunGravityFactor = playerSunDistance / maxSunDistance;
		float sunGravityForce = 400 * sunGravityFactor;

		sunGravityVector.Normalize();

		sunGravityVector *= sunGravityForce;
		globalForce += sunGravityVector;
	}
	// Finally apply a force on the body in the direction of the "Planet"
	mPlayer->mPlayerBody->SetLinearVelocity(b2Vec2(0,0));
	mPlayer->mPlayerBody->SetAngularVelocity(0);
	mPlayer->mPlayerBody->ApplyForce(globalForce, mPlayer->mPlayerBody->GetPosition());

	CCPoint pos(mPlayer->mPlayerBody->GetPosition().x / PTM_RATIO, mPlayer->mPlayerBody->GetPosition().y / PTM_RATIO);
	float t1 = 1.0f - exp(- dt / 0.5f);
	float t2 = 1.0f - exp(- dt / 0.25f);

	mPlayer->mOptimizedPos = ccpLerp(mPlayer->mOptimizedPos, pos, t1);
	mPlayer->mOptimizedPos2 = ccpLerp(mPlayer->mOptimizedPos2, pos, t2);

	mPlayer->mPlayer->setPosition(mPlayer->mOptimizedPos2);

	//mPlayer->mPlayer->setRotation(mPlayer->mPlayerBody->GetAngle());

	//if(!drainImpulseFuel) 
	//{
	//	b2Vec2 velocityVec = mPlayer->mPlayerBody->GetLinearVelocity();
	//	if(velocityVec.Length() > 0) 
	//	{
	//		CCPoint velVect = ccp(velocityVec.x, velocityVec.y);
	//		velVect = ccpNormalize(velVect);
	//		mPlayer->direction = velVect;

	//		float angle = -CC_RADIANS_TO_DEGREES(ccpToAngle(velVect));
	//		mPlayer->mPlayer->setRotation(angle);
	//	}
	//}

	if(outsideView(mPlayer->mOptimizedPos2, NULL, NULL, true))
	{
		showGameOver();
	}
}

void Gameplay::update(ccTime dt) {
	impulseTimer += dt;
	nextRocketTimer += dt;
	updateScore();
	if(drainImpulseFuel) {
		impulseFuel -= dt * 20;
		if(impulseFuel < 0) {
			impulseFuel = 0;
		}
	} else {
		impulseFuel += dt * 10;
		if(impulseFuel > 100) {
			impulseFuel = 100;
		}
	}

	if(impulseFuel < 20) {
		impulseFuelIndicator->setColor(ccc3(255, 1, 1));
	} else {
		impulseFuelIndicator->setColor(ccc3(1, 255, 1));
	}

	impulseFuelIndicator->setContentSize(CCSizeMake(grid->getContentSize().width * (impulseFuel / 100.0f), 
		impulseFuelIndicator->getContentSize().height));

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

	if(GetAsyncKeyState(VK_RBUTTON)) 
	{
		if(nextRocketTimer >= 0)
		{
			float angle = mPlayer->mPlayer->getRotation() * M_PI / 180.0f;
			float speed = 500.0f;
			addRocket("rocket.png", mPlayer->mPlayer->getPosition(), CCPoint(speed * sinf(angle), speed * cosf(angle)));
			nextRocketTimer = -0.25f;
		}
	}


	CCPoint sub = ccpSub(mPlayer->mOptimizedPos, sun->getPosition());
	float angle =  CC_RADIANS_TO_DEGREES(ccpToAngle(sub));
	world->setRotation(angle);

	char buf[256];
	sprintf(buf, "%f %f", mPlayer->mPlayer->getPositionX(), mPlayer->mPlayer->getPositionY());
	playerPos->setString(buf);
	
	updatePhysic(dt);

 	trail->setPosition(mPlayer->mPlayer->getPosition());
	//trail->setGravity(ccpMult(mPlayer->direction, 10));
}

void Gameplay::createPlayer(float posx, float posy)
{
	// CREATE SPRITE
	CCPoint position = ccp(posx, posy);
	mPlayer = new Player;
	mPlayer->mPlayer = CCSprite::spriteWithFile("astro.png");
	mPlayer->mPlayer->setPosition(ccp(position.x, position.y));
	mPlayer->mOptimizedPos = ccp(position.x, position.y);
	mPlayer->mOptimizedPos2 = ccp(position.x, position.y);
	world->addChild(mPlayer->mPlayer, 3);

	/*// Jetpack animation
	mJetpakAnimTexture = CCSprite::spriteWithFile("jetpack.png");
	mJetpakAnimation = new CCAnimation();
	mJetpakAnimation->initWithFrames(NULL, 0.03f);
	for(int y = 0; y < 4; ++y) {
		for(int x = 0; x < 8; ++x) {
			float width = 256;
			float height = 256.0f;
			CCSpriteFrame* frame = CCSpriteFrame::frameWithTexture(mJetpakAnimTexture->getTexture(), CCRect(x*width, y*height, width, height));
			mJetpakAnimation->addFrame(frame);
		}
	}

	mJetpakAnimTexture->setAnchorPoint(ccp(0.5f, 0.5f));
	mJetpakAnimTexture->setPosition(ccp(14, 36));
	world->addChild(mJetpakAnimTexture, 2);
	mJetpakAnimTexture->setIsVisible(false);
	*/
	


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
	fixtureDef.userData = mPlayer;
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

void Gameplay::removeRocket(int i)
{
	if(i < 0 || i >= mRockets.size())
		return;

	if(mRockets[i]) {
		world->removeChild(mRockets[i]->mRocketTrail, true);
		world->removeChild(mRockets[i]->getSprite(), true);
		mWorld->DestroyBody(mRockets[i]->mRocketBody);
		delete mRockets[i];
		mRockets[i] = NULL;
	}

	if(i + 1 < mRockets.size())
		mRockets[i] = mRockets.back();
	mRockets.pop_back();
}

void Gameplay::removeTrash(int i)
{
	if(i < 0 || i >= mTrashes.size())
		return;

	if(mTrashes[i]) {
		world->removeChild(mTrashes[i]->getSprite(), true);
		mWorld->DestroyBody(mTrashes[i]->mTrashBody);
		delete mTrashes[i];
		mTrashes[i] = NULL;
	}

	if(i + 1 < mTrashes.size())
		mTrashes[i] = mTrashes.back();
	mTrashes.pop_back();
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
	fd.density = 1000.0f;
	fd.restitution = 1.0f;
	fd.friction = 0.0f;
	fd.userData = planet;
	b2Fixture* planetFixture = planetBody->CreateFixture(&fd);

	planet->mPlanetBody = planetBody;
	planet->mPlanetFixture = planetFixture;

	return planet;
}

Rocket* Gameplay::addRocket( std::string rocketSpriteName, CCPoint position,  CCPoint velocity )
{
	float scale = 0.4f;
	Rocket* rocket = new Rocket(rocketSpriteName);
	rocket->setPos(position);
	rocket->getSprite()->setScale(scale);
	world->addChild(rocket->getSprite());
	
	rocket->mRocketTrail = ParticleFactory::meteor(); 
	rocket->mRocketTrail->setPositionType(kCCPositionTypeRelative);
	rocket->mRocketTrail->setPosition(mPlayer->mPlayer->getPosition());
	rocket->mRocketTrail->setDuration(20.0f);
	world->addChild(rocket->mRocketTrail, 1);

	mRockets.push_back(rocket);

	// Physical representation

	b2BodyDef rocketBodyDef;
	rocketBodyDef.type = b2_dynamicBody;
	rocketBodyDef.position.Set(position.x*PTM_RATIO, position.y*PTM_RATIO);
	rocketBodyDef.angle = -atan2f(velocity.y, velocity.x);
	rocketBodyDef.userData = rocket;
	b2Body* rocketBody = mWorld->CreateBody(&rocketBodyDef);

	b2CircleShape shape;
	shape.m_radius = scale * rocket->getSprite()->getContentSize().width / 2 * PTM_RATIO;
	//shape.m_p.Set(8.0f, 8.0f);
	b2FixtureDef fd;
	fd.shape = &shape;
	fd.density = 1000.0f;
	fd.restitution = 1.0f;
	fd.friction = 0.0f;
	fd.userData = rocket;
	b2Fixture* rocketFixture = rocketBody->CreateFixture(&fd);

	rocket->mRocketBody = rocketBody;
	rocket->mRocketFixture = rocketFixture;

	rocketBody->SetLinearVelocity(b2Vec2(velocity.x * PTM_RATIO, velocity.y * PTM_RATIO));

	return rocket;
}

void Gameplay::clearLevel()
{
	while(mPlanets.size())
		removePlanet(0);
	while(mTrashes.size())
		removePlanet(0);
	mPlanets.clear();
}

void Gameplay::draw()
{
#if 0
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	mWorld->DrawDebugData();

	// restore default GL states
	glEnable(GL_TEXTURE_2D);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
#endif
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
	drainImpulseFuel = true;
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
	drainImpulseFuel = false;
}

void Gameplay::updateScore()
{	
	char tab[255] = {0};
	sprintf(tab, "Score: %i", mPlayer->score);
	scoreText->setString(tab);
	sprintf(tab, "Time: %.2f", totalTime);
	timeLabel->setString(tab);

	static int lastScored = 0;
	int currentScore = (int)totalTime;
	if (currentScore % 10 == 0 && lastScored != currentScore) {
		lastScored = currentScore;
		sprintf(tab, "You survived %.0f seconds, Great!", totalTime);
		this->showAchievement(tab);
	}
}


void Gameplay::removeAchievement(CCNode *label) 
{
	label->removeFromParentAndCleanup(true);
}
	
void Gameplay::collide(Rocket* rocket, Trash* trash)
{

}

void Gameplay::showAchievement(const char *achievementName)
{
	if (!gameIsPlaying) {
		return;
	}
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	CCLabelTTF *label = CCLabelTTF::labelWithString(achievementName, "Comic Sans", 48);
	label->setPosition(ccp(size.width * 0.5f, size.height * 0.5f));
	label->setColor(ccc3(128,255, 0));
	this->addChild(label, 4);

	CCFadeIn *fadeIn = CCFadeIn::actionWithDuration(0.2f);
	CCScaleBy *scaleBy = CCScaleBy::actionWithDuration(1.0f, 4, 4);
	CCRotateBy *rotateBy = CCRotateBy::actionWithDuration(1.0f, rand() % 60 - 30);
	CCFadeOut *fadeOut = CCFadeOut::actionWithDuration(0.5f);
	CCCallFuncN *callFunc = CCCallFuncN::actionWithTarget(this, callfuncN_selector(Gameplay::removeAchievement));
	CCDelayTime *delayTime = CCDelayTime::actionWithDuration(0.2f);

	CCFiniteTimeAction *spawn = CCSpawn::actions(fadeIn, NULL);
	CCFiniteTimeAction *spawn2 = CCSpawn::actions(fadeOut, rotateBy, scaleBy, NULL);
	CCFiniteTimeAction *sequence = CCSequence::actions(spawn, delayTime, spawn2, callFunc, NULL);
	label->runAction(sequence);
}
Gameplay* Gameplay::gayplay;

void Gameplay::showGameOver()
{
	if (!gameIsPlaying) {
		return;
	}
	unscheduleUpdate();
	gameIsPlaying = 0;
	CCLayerColor *layer = new CCLayerColor();
	layer->initWithColor(ccc4(40, 40, 40, 255));
	CCSize size = CCDirector::sharedDirector()->getWinSize();
	CCLabelTTF *label = CCLabelTTF::labelWithString("GAME OVER", "Comic Sans", 72);
	label->setPosition(ccp(size.width * 0.5f, size.height * 0.5f));
	label->setColor(ccc3(128,255, 0));
	this->addChild(layer);
	layer->addChild(label, 4);

	CCFadeIn *fadeIn = CCFadeIn::actionWithDuration(0.2f);
	CCScaleBy *scaleBy = CCScaleBy::actionWithDuration(1.0f, 4, 4);
	CCRotateBy *rotateBy = CCRotateBy::actionWithDuration(1.0f, rand() % 60 - 30);
	CCFadeOut *fadeOut = CCFadeOut::actionWithDuration(1.0f);
	CCCallFuncN *callFunc = CCCallFuncN::actionWithTarget(this, callfuncN_selector(Gameplay::removeAchievement));
	CCDelayTime *delayTime = CCDelayTime::actionWithDuration(0.2f);
	
	CCFiniteTimeAction *spawn = CCSpawn::actions(fadeIn, NULL);
	CCFiniteTimeAction *spawn2 = CCSpawn::actions(fadeOut, rotateBy, scaleBy, NULL);
	CCFiniteTimeAction *sequence = CCSequence::actions(spawn, delayTime, spawn2, callFunc, NULL);
	label->runAction(sequence);
}

void Listener::BeginContact(b2Contact* contact)
{
	BaseObject* a = (BaseObject*)contact->GetFixtureA()->GetUserData();
	BaseObject* b = (BaseObject*)contact->GetFixtureB()->GetUserData();

	if(a && b)
	{
		Rocket* ra = dynamic_cast<Rocket*>(a);
		Trash* rb = dynamic_cast<Trash*>(b);
		if(ra && rb)
		{
			Gameplay::gayplay->collide(ra, rb);
		}
		else
		{
			ra = dynamic_cast<Rocket*>(b);
			rb = dynamic_cast<Trash*>(a);
			if(ra && rb)
			{
				Gameplay::gayplay->collide(ra, rb);
			}
		}
	}
}

void Listener::EndContact(b2Contact* contact)
{

}