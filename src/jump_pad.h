#ifndef INCLUDED_JUMP_PAD_H
#define INCLUDED_JUMP_PAD_H


#include "game_obj.h"


class JumpPad: public GameObj
{
public:
	JumpPad(Vector3 pos);

    virtual RgbaColour GetRadarColour() { return RgbaColour(128, 128, 128); }

    void Advance();
};


#endif
