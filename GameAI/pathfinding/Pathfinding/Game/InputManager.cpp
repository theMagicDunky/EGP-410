#include "InputManager.h"
#include "GameApp.h"
#include "GameMessageManager.h"
#include "Vector2D.h"
#include "PathToMessage.h"

InputManager::InputManager() {}
InputManager::~InputManager() {}

void InputManager::update()
{
	al_get_mouse_state(&mMouseState);
	al_get_keyboard_state(&mKeyState);

	//mouse inout
	if (al_mouse_button_down(&mMouseState, 1))//left mouse click
	{
		static Vector2D lastPos(0.0f, 0.0f);
		Vector2D pos(mMouseState.x, mMouseState.y);
		if (lastPos.getX() != pos.getX() || lastPos.getY() != pos.getY())
		{
			GameMessage* pMessage = new PathToMessage(lastPos, pos);
			gpGameApp->getMessageManager()->addMessage(pMessage, 0);
			lastPos = pos;
		}
	}

	//keypresses
	//calling appropriate event for the keypress
	if (al_key_down(&mKeyState, ALLEGRO_KEY_ESCAPE))
	{
		gpGame->markForExit();
	}

	//save previous keystate to have ability to check for key up
	mPrevKeyState = mKeyState;
}

//functions identical to Unity's keyDown function
//checks if key was up last frame and down on current frame
bool InputManager::keyDown(int key, ALLEGRO_KEYBOARD_STATE& currKeyState, ALLEGRO_KEYBOARD_STATE& prevKeyState)
{
	return (al_key_down(&currKeyState, key) && !al_key_down(&prevKeyState, key));
}