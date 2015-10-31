#ifndef INCLUDED_BOMB_H
#define INCLUDED_BOMB_H


#include "game_obj.h"


class Bomb: public GameObj
{
public:
    Bomb(Vector3 const &pos);

    virtual void TakeHit(float force);

    virtual RgbaColour GetRadarColour();
};


#endif
