#ifndef INCLUDED_SHIELD_POWERUP_H
#define INCLUDED_SHIELD_POWERUP_H


#include "game_obj.h"


class ShieldPowerup: public GameObj
{
public:
    ShieldPowerup(Vector3 const &pos);

    virtual void Advance();

    virtual RgbaColour GetRadarColour();
};


#endif
