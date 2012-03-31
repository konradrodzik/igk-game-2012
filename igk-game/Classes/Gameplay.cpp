#include "Gameplay.h"

#define PTM_RATIO 32

Gameplay::Gameplay() {
	createPlayer(100,100);
	initPhysicalWorld();
	scheduleUpdate();
}

Gameplay::~Gameplay() {
	unscheduleUpdate();
}

void Gameplay::initPhysicalWorld()
{
	// Create Box2d world
	b2Vec2 gravity = b2Vec2(0.0f, 0.0f);
	bool doSleep = false;
	//mWorld = new b2World(gravity, doSleep);
	mWorld = new b2World(gravity);
	mWorld->SetContinuousPhysics(true);

	/*uint32 flags = 0;
	flags += b2DebugDraw::e_shapeBit;
	//flags += b2DebugDraw::e_jointBit;
	//flags += b2DebugDraw::e_aabbBit;
	flags += b2DebugDraw::e_pairBit;
	//flags += b2DebugDraw::e_centerOfMassBit;
	m_debugDraw->SetFlags(flags);	*/
}

void Gameplay::updatePhysic( ccTime dt )
{
	/*int32 velocityIterations = 8;
	int32 positionIterations = 1;

	mWorld->Step(dt, velocityIterations, positionIterations);

	for (b2Body* b = mWorld->GetBodyList(); b; b = b->GetNext())
	{
		b2Body* ground = planet->GetBody();
		b2CircleShape* circle = (b2CircleShape*)planet->GetShape();
		// Get position of our "Planet"
		b2Vec2 center = ground->GetWorldPoint(circle->m_p);
		// Get position of our current body in the iteration
		b2Vec2 position = b->GetPosition();
		// Get the distance between the two objects.	
		b2Vec2 d = center - position;
		// The further away the objects are, the weaker the gravitational force is
		float force = 250.0f / d.LengthSquared(); // 150 can be changed to adjust the amount of force
		d.Normalize();
		b2Vec2 F = force * d;
		// Finally apply a force on the body in the direction of the "Planet"
		b->ApplyForce(F, position);

		if (b->GetUserData() != NULL) {
			CCSprite *myActor = (CCSprite*)b->GetUserData();
			myActor.position = CGPointMake( b->GetPosition().x * PTM_RATIO, b->GetPosition().y * PTM_RATIO);
			myActor.rotation = -1 * CC_RADIANS_TO_DEGREES(b->GetAngle());
		}	
	}*/
}

void Gameplay::update(ccTime dt) {
	updatePhysic(dt);
}

void Gameplay::createPlayer(float posx, float posy)
{
	// CREATE SPRITE
	// create & set position
	/*mPlayer = CCSprite::spriteWithFile("CloseNormal.png");
	mPlayer->setPosition(ccp(posx, posy));
	addChild

	// PHYSICAL REPRESENTATION
	// Define the dynamic body.
	//Set up a 1m squared box in the physics world
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;

	bodyDef.position.Set(p.x/PTM_RATIO, p.y/PTM_RATIO);
	bodyDef.userData = sprite;
	b2Body *body = world->CreateBody(&bodyDef);

	// Define another box shape for our dynamic body.
	b2PolygonShape dynamicBox;
	dynamicBox.SetAsBox(.5f, .5f);//These are mid points for our 1m box

	// Define the dynamic body fixture.
	b2FixtureDef fixtureDef;
	fixtureDef.shape = &dynamicBox;	
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 0.3f;
	body->CreateFixture(&fixtureDef);*/
}


