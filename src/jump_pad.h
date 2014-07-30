#ifndef INCLUDED_JUMP_PAD_H
#define INCLUDED_JUMP_PAD_H


#include "game_obj.h"


class JumpPad: public GameObj
{
public:
	JumpPad(Vector3 pos);

	void Advance();
	void Render();
};


#endif
