#include "Gameplay.h"

Gameplay::Gameplay() {
	scheduleUpdate();
}

Gameplay::~Gameplay() {
	unscheduleUpdate();
}

void Gameplay::update(ccTime dt) {

}