#include "lib/universal_include.h"
#include "app.h"

#include "lib/gfx/text_renderer.h"
#include "lib/sound/sound_system.h"
#include "lib/debug_utils.h"
#include "lib/hi_res_time.h"
#include "lib/input.h"
#include "lib/preferences.h"
#include "lib/profiler.h"
#include "lib/window_manager.h"

#include "camera.h"
#include "editor.h"
#include "level.h"
#include "particle_system.h"
#include "ship_player.h"
#include "renderer.h"



App *g_app = NULL;

App::App()
{
	m_state = StatePlayLocation;
	m_requestedState = StatePlayLocation;
	m_camera = NULL;
	m_renderer = NULL;
	m_paused = false;

	g_app = this;

	HiresTimeInit();
	g_prefsManager.RegisterInt("screenWidth", 1024);
	g_prefsManager.RegisterInt("screenHeight", 768);
	g_prefsManager.RegisterInt("windowed", 1);
	g_prefsManager.Load("prefs.txt");
#ifdef PROFILER_ENABLED
	m_profiler = new Profiler();
#endif
	m_renderer = new Renderer();
	g_inputManager = new InputManager;
	ReleaseAssert(g_gameFont.Initialize("data/bitmaps/font.bmp"), "Couldn't load game font");
	m_camera = new Camera();
	m_level = new Level();
	m_particleSystem = new ParticleSystem();
	g_soundSystem = new SoundSystem();
}


void App::MainLoopCommonStart()
{
	g_inputManager->PollForMessages();
	g_inputManager->Advance();

	g_soundSystem->Advance(g_level->m_playerShip->m_pos);

	// Toggle the editor?
	if (g_keyDeltas[KEY_F1])
		m_requestedState = StateEditLocation;

	if (g_app->m_requestedState == StateQuit)
		exit(0);

	if (g_inputManager->m_windowHasFocus)
	{
		HiresTimeUpdate();

		g_app->m_camera->Advance();
#ifdef PROFILER_ENABLED
		g_app->m_profiler->Advance();
#endif
	}
}


void App::Render()
{
	g_app->m_renderer->Render();
	Sleep(10);
}


void App::PlayLocationLoop()
{
	while (m_requestedState == StatePlayLocation)
	{
        if (g_keyDeltas[KEY_ENTER] && g_keys[KEY_ALT])
        {
            g_app->m_renderer->ToggleFullscreen();
            g_inputManager = new InputManager;
        }

		MainLoopCommonStart();

		if (g_inputManager->m_windowHasFocus)
			g_app->m_level->Advance();

		Render();
	}
}


void App::EditLocationLoop()
{
#if EDITOR_ENABLED == 1
	g_editor.Activate();

	while (m_requestedState == StateEditLocation)
	{
		MainLoopCommonStart();

		if (g_inputManager->m_windowHasFocus)
			g_editor.Advance();

		Render();
	}

	g_editor.Deactivate();
#endif
}


void App::GameOverLoop()
{
	g_app->m_renderer->StartFadeOut();
	float showMessageTime = g_gameTime + 2.0f;

	while (m_requestedState == StateGameOver)
	{
		MainLoopCommonStart();

		if (g_inputManager->m_windowHasFocus)
			g_app->m_level->Advance();

		Render();

		g_gameFont.BeginText2d();
		glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
		g_gameFont.DrawText2dCentre(g_windowManager->ScreenW()/2, g_windowManager->ScreenH()/2,
									40.0f, "EVERYTHING FADES\nAWAY.");
	}
}


void App::RunGame()
{
	while (1)
	{
		g_app->m_state = m_requestedState;

		switch (m_state)
		{
		case StatePlayLocation:
			g_app->PlayLocationLoop();
			break;
		case StateEditLocation:
			g_app->EditLocationLoop();
			break;
		case StateGameOver:
			g_app->GameOverLoop();
			break;
		}
	}
}


// Main Function
void AppMain()
{
	g_app = new App();
	g_app->RunGame();
}
