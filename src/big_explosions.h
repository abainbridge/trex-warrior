#ifndef INCLUDED_BIG_EXPLOSIONS_H
#define INCLUDED_BIG_EXPLOSIONS_H


#include "game_obj.h"


class MissileDeath: public GameObj
{
public:
    double m_endTime;

    MissileDeath(Vector3 const &pos);

    void Advance();
    void Render();

    virtual RgbaColour GetRadarColour();
};


class Boom: public MissileDeath
{
private:
    float m_nextDamageCheckTime;
    float GetSize();

public:
    Boom(Vector3 const &pos);

    void Advance();
    void Render();
};


#endif
