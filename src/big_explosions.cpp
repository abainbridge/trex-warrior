#include "lib/universal_include.h"
#include "big_explosions.h"

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

float const BOOM_DURATION = 1.5f;


Boom::Boom(Vector3 const &pos)
: MissileDeath(pos)
{
    m_endTime = g_gameTime + BOOM_DURATION;
}


void Boom::Render()
{
    float const halfLife = BOOM_DURATION/2.0f;
    double halfwayTime = m_endTime - halfLife;
    double startTime = m_endTime - BOOM_DURATION;
    float size;
    if (g_gameTime < halfwayTime)
        size = 20.0f * (g_gameTime - startTime) / halfLife;
    else
        size = 20.0f * (m_endTime - g_gameTime) / halfLife;

    int tid = g_resourceManager.GetTexture("bullet.bmp");

    // Red, Orange, White, Gray, Teal, Dark Teal, Very Dark Teal, 

    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    Render3dSprite(g_app->m_camera->m_up, g_app->m_camera->GetRight(), m_pos,
        size, size, tid, RgbaColour(255,0,0));
}
