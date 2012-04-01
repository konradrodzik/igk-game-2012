#include "Common.h"
#include "Input.h"

#define check(a) a

Input* Input::instance() {
	static Input* sharedInstance;
	if(!sharedInstance) {
		sharedInstance = new Input();
	}
	return sharedInstance;
}

void Input::init(HINSTANCE appInstance, bool isExclusive)
{
	// wyczyszczenie stanu klawiszy
	for(int i = 0; i < 256; i++) {
		keys[i] = tempKeys[i] = false;
	}
}

void Input::update()
{
	// update klawiszy
	for (int i = 0; i < 256; i++) 
	{
		bool state  = static_cast<bool>(GetAsyncKeyState(i));
		tempKeys[i] = (state != keys[i]);
		keys[i]     = state;
	}
}
