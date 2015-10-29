#include "lib/universal_include.h"
#include "game_obj.h"

#include "lib/gfx/shape.h"
#include "lib/matrix34.h"


void GameObj::Render()
{
    Matrix34 mat(m_front, g_upVector, m_pos);
    m_shape->Render(0.0f, mat);
}