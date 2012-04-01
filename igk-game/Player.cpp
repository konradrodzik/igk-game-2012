#include "Player.h"


Player::Player(void)
{
	mLastPos = CCPoint(0,0);
	mLastVel = CCPoint(0,0);
	mOptimizedVel = CCPoint(0,0);
	mOptimizedPos = CCPoint(0,0);
	mOptimizedPos2 = CCPoint(0,0);
	mLastAngle = 0;
}


Player::~Player(void)
{
}
