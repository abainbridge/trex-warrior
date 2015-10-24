#include "lib/universal_include.h"
#include "radar.h"

#include "lib/window_manager.h"
#include "game_obj.h"
#include "level.h"
#include "ship_player.h"


void RenderRadar()
{
	int sw = g_windowManager->ScreenW();
	int sh = g_windowManager->ScreenH();
	int posX = sw / 2;
	int posY = sh * 0.87f;
	float scaleX = 0.00022f * sh;
	float scaleY = 0.00017f * sh;
	float size = sw * 0.002;

	Vector3 const &playerPos = g_level->m_playerShip->m_pos;
	Vector3 const &playerFront = g_level->m_playerShip->m_front;
	Vector3 playerRight = playerFront;
	playerRight.RotateAroundY(M_PI / 2.0f);

	int numObjects = g_level->m_objects.Size();
	for (int i = 0; i < numObjects; i++)
	{
		GameObj *o = g_level->m_objects[i];
        
        RgbaColour col = o->GetRadarColour();
        glColor3ub(col.r, col.g, col.b);

		float x = o->m_pos.x - playerPos.x;
		float y = o->m_pos.z - playerPos.z;

		float a = playerFront.x;
		float b = playerFront.z;
		float c = playerRight.x;
		float d = playerRight.z;

		float tx = x * a + y * b;
		float ty = x * c + y * d;
		x = -ty * scaleX;
		y = -tx * scaleY;

		x += posX;
		y += posY;

		glBegin(GL_QUADS);
		glVertex2f(x-size, y+size);
		glVertex2f(x+size, y+size);
		glVertex2f(x+size, y-size);
		glVertex2f(x-size, y-size);
		glEnd();
	}
}
