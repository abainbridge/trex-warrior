#include "lib/universal_include.h"
#include "editor.h"

#if EDITOR_ENABLED == 1

#include "lib/gfx/debug_render.h"
#include "lib/gfx/text_renderer.h"
#include "lib/input.h"
#include "lib/window_manager.h"
#include "app.h"
#include "camera.h"
#include "level.h"
#include "navigation.h"
#include "renderer.h"


Editor g_editor;


Editor::Editor()
{
	m_active = false;
	m_previousCameraMode = Camera::ModeDebug;
}


void Editor::Activate()
{
	// Activate
	m_previousCameraMode = g_app->m_camera->GetMode();
	g_app->m_camera->SetMode(Camera::ModeDebug);
	m_active = true;
	g_windowManager->UnhideMousePointer();
}


void Editor::Deactivate()
{
	// Deactivate
	g_app->m_camera->SetMode(m_previousCameraMode);
	m_active = false;
	g_windowManager->HideMousePointer();
}


void Editor::Advance()
{
	if (!m_active)
		return;

	if (g_keyDeltas[KEY_S] && g_keys[KEY_SHIFT])
		Save();

	if (g_inputManager->m_lmbClicked)
	{
		NavRoute *nav = g_level->m_navRoutes[0];

		if (g_keys[KEY_SHIFT])
		{
			Vector3 start, dir;
			g_app->m_camera->GetClickRay(&start, &dir);

			// Find intersection with ground plane
			float distanceAlongDir = -start.y / dir.y;
			Vector3 end = start + (distanceAlongDir * dir);
			nav->m_wayPoints.PushBack(new Vector2(end.x, end.z));
		}
		else
		{
			for (int i = 0; i < nav->m_wayPoints.Size(); i++)
			{
			}
		}
	}
}


void Editor::Render()
{
	if (!m_active)
		return;

	char const *navRouteName = NULL;

	if (g_level->m_navRoutes.Size() > 0)
	{
		NavRoute *nav = g_level->m_navRoutes[0];
		navRouteName = nav->m_name.m_str;

		for (int i = 0; i < nav->m_wayPoints.Size(); i++)
		{
			Vector2 const *pos2 = nav->m_wayPoints[i];
			RenderSphere(Vector3(pos2->x, 0, pos2->y), 3.0f);
		}
	}

	g_gameFont.BeginText2d();
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	g_gameFont.DrawText2d(10, 30, 10, "Shift+Left Click - Create new nav point");
	if (navRouteName)
		g_gameFont.DrawText2d(10, 40, 10, "Current nav route = '%s'", navRouteName);
	g_gameFont.EndText2d();
}


bool Editor::Save()
{
	for (int i = 0; i < g_level->m_navRoutes.Size(); i++)
		if (!g_level->m_navRoutes[i]->Save())
			return false;

	return true;
}

#endif
