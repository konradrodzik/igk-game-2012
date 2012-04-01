#include "ParticleFactory.h"

CCParticleSystemQuad* ParticleFactory::explosion()
{
	CCParticleSystemQuad* particle = new CCParticleSystemQuad();
	particle->initWithFile("ExplodingRing.plist");
	return particle;
}

CCParticleSystem* ParticleFactory::meteor()
{
	CCParticleMeteor* particle = CCParticleMeteor::node();
	//particle->setIsAutoRemoveOnFinish(true);
	particle->setTexture( CCTextureCache::sharedTextureCache()->addImage("fire.png") );
	return particle;
}

CCParticleGalaxy* ParticleFactory::galaxy()
{
	CCParticleGalaxy* particle = CCParticleGalaxy::node();
	particle->setIsAutoRemoveOnFinish(true);
	particle->setTexture( CCTextureCache::sharedTextureCache()->addImage("fire.png") );

	return particle;
}

CCParticleSun* ParticleFactory::sun()
{
	CCParticleSun* particle = CCParticleSun::node();
	particle->setTexture( CCTextureCache::sharedTextureCache()->addImage("fire.png") );

	return particle;
}

CCParticleSystem *ParticleFactory::stars()
{
	CCParticleSystem *particle = CCParticleSystem::particleWithFile("Stars.plist");
	return particle;
}