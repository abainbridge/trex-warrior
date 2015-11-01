#include "lib/universal_include.h"
#include "big_explosions.h"

#include "lib/gfx/shape.h"
#include "lib/gfx/simple_primitives.h"
#include "lib/hi_res_time.h"
#include "lib/resource.h"
#include "app.h"
#include "camera.h"
#include "level.h"


// ****************************************************************************
// MissileDeath
// ****************************************************************************

float const MISSILE_DEATH_DURATION = 0.5f;    // Seconds


MissileDeath::MissileDeath(Vector3 const &pos)
: GameObj(ObjTypeBullet, pos)
{
    m_endTime = g_gameTime + MISSILE_DEATH_DURATION;
}


void MissileDeath::Advance()
{
    if (g_gameTime > m_endTime)
        g_level->DeleteObj(this);
}


void MissileDeath::Render()
{
    int tid = g_resourceManager.GetTexture("bullet.bmp");

    float const halfLife = MISSILE_DEATH_DURATION/2.0f;
    double halfwayTime = m_endTime - halfLife;
    double startTime = m_endTime - MISSILE_DEATH_DURATION;
    float size;
    if (g_gameTime < halfwayTime)
        size = 10.0f * (g_gameTime - startTime) / halfLife;
    else
        size = 10.0f * (m_endTime - g_gameTime) / halfLife;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    Render3dSprite(g_app->m_camera->m_up, g_app->m_camera->GetRight(), m_pos,
        size, size, tid, RgbaColour(255,0,0));

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Render3dSprite(g_app->m_camera->m_up, g_app->m_camera->GetRight(), m_pos,
        size/2, size/2, tid, g_colourBlack);
}


RgbaColour MissileDeath::GetRadarColour()
{
    return g_colourBlack;
}


// ****************************************************************************
// Boom
// ****************************************************************************

float const BOOM_DURATION = 0.7f;
float const BOOM_SIZE = 150.0f;
double const BOOM_DAMAGE_CHECK_PERIOD = 0.05;


Boom::Boom(Vector3 const &pos)
: MissileDeath(pos)
{
    m_endTime = g_gameTime + BOOM_DURATION;
    m_nextDamageCheckTime = g_gameTime + BOOM_DAMAGE_CHECK_PERIOD;
}


float Boom::GetSize()
{
    float const halfLife = BOOM_DURATION/2.0f;
    double halfwayTime = m_endTime - halfLife;
    double startTime = m_endTime - BOOM_DURATION;
    float size;
    if (g_gameTime < halfwayTime)
        size = BOOM_SIZE * (g_gameTime - startTime) / halfLife;
    else
        size = BOOM_SIZE * (m_endTime - g_gameTime) / halfLife;
    return size;
}


void Boom::Advance()
{
    MissileDeath::Advance();

    if (g_gameTime > m_nextDamageCheckTime + 1.0f)
        m_nextDamageCheckTime = g_gameTime - 1.0f;

    while (g_gameTime > m_nextDamageCheckTime)
    {
        float const size = GetSize();

        int numObjs = g_level->m_objects.Size();
        for (int i = 0; i < numObjs; i++)
        {
            GameObj *o = g_level->m_objects[i];
            if (o == this)
                continue;

            if (!o->m_shape)
                continue;

            if (o->m_type == ObjTypeBullet || o->m_type == ObjTypeJumpPad)
                continue;

            SpherePackage sp(m_pos, size);
            Matrix34 osMat(o->m_front, g_upVector, o->m_pos);	
            if (o->m_shape->SphereHit(&sp, osMat, true))
                 o->TakeHit(1.0f);
        }

        m_nextDamageCheckTime += BOOM_DAMAGE_CHECK_PERIOD;
    }
}


void Boom::Render()
{
    float const size = GetSize();

    int tid = g_resourceManager.GetTexture("bullet.bmp");

    RgbaColour colours[] = 
    {
        RgbaColour(255,0,0),
        RgbaColour(255,178,0),
        RgbaColour(255,255,255),
        RgbaColour(200,200,200),
        RgbaColour(0x90, 0xd0, 0xd0),
        RgbaColour(0x40, 0x80, 0x80),
        RgbaColour(0x20, 0x40, 0x40)
    };
    int const numColours = sizeof(colours) / sizeof(colours[0]);

    double startTime = m_endTime - BOOM_DURATION;
    double fractionComplete = (g_gameTime - startTime) / BOOM_DURATION;
    int currentColour = fractionComplete * numColours;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    Vector3 pos = m_pos;
    pos.y += size * 0.1f;
    Render3dSprite(g_app->m_camera->m_up, g_app->m_camera->GetRight(), pos,
        size, size, tid, colours[currentColour]);
}
