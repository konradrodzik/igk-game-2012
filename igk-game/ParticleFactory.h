#pragma once

#include "cocos2d.h"

using namespace cocos2d;

class ParticleFactory
{
public:
	static CCParticleSystemQuad* explosion();
	static CCParticleSystem* meteor();
	static CCParticleGalaxy* galaxy();
	static CCParticleSun* sun();
	static CCParticleSystem * stars();
};

