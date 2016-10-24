#ifndef _PLAYERMOVETOMESSAGE_H
#define _PLAYERMOVETOMESSAGE_H

#include "GameMessage.h"
#include "Vector2D.h"

class Vector2D;

class PlayerMoveToMessage:public GameMessage
{
public:
	PlayerMoveToMessage( const Vector2D& pos );
	~PlayerMoveToMessage();

	void process();

private:
	Vector2D mPos;
};

#endif