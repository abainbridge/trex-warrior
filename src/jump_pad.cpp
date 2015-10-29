#include "lib/universal_include.h"
#include "jump_pad.h"

#include "lib/hi_res_time.h"
#include "lib/resource.h"

#include "level.h"
#include "ship_player.h"


JumpPad::JumpPad(Vector3 pos)
: GameObj(ObjTypeJumpPad, pos)
{
	m_shape = g_resourceManager.GetShape("jump_pad.shp");
}


void JumpPad::Advance()
{
	m_front.RotateAroundY(g_advanceTime * 1.5f);
}
