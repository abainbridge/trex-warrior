#include "lib/universal_include.h"

#include <math.h>

#include "lib/gfx/opengl_helper.h"
#include "lib/gfx/rgb_colour.h"
#include "lib/gfx/text_renderer.h"
#include "lib/debug_utils.h"
#include "lib/hi_res_time.h"
#include "lib/input.h"
#include "lib/math_utils.h"
#include "lib/preferences.h"
#include "lib/profiler.h"
#include "lib/resource.h"
#include "lib/window_manager.h" 

#include "app.h"
#include "camera.h"
#include "editor.h"
#include "hud_text.h"
#include "level.h"
#include "radar.h"
#include "renderer.h"
#include "script.h"
#include "ship_player.h"


float const FAR_PLANE = 1500.0f;
float const NEAR_PLANE = 0.5f;


Renderer::Renderer()
{
    m_fps = 60;

    m_fadedness = 0.0f;
	m_fadeRate = 0.0f;
	m_fadeDelay = 0.0f;

    bool windowed = g_prefsManager.GetInt("Windowed") ? true : false;
    CreateWin(windowed);
}


void Renderer::CreateWin(bool windowed)
{
    int targetScreenW = g_prefsManager.GetInt("ScreenWidth");
    int targetScreenH = g_prefsManager.GetInt("ScreenHeight");

    bool success = g_windowManager->CreateWin(targetScreenW, targetScreenH, windowed, "Trex Warrior");
    ReleaseAssert(success, "Failed to set screen mode" );
}


void Renderer::DestroyWin()
{
    g_windowManager->DestroyWin();
}


void Renderer::RenderBackground()
{
	START_PROFILE(g_app->m_profiler, "Render Background");

	glColor3ubv(g_colourWhite.GetData());
	glEnable(GL_TEXTURE_2D);
	int textureId = g_resourceManager.GetTexture("dash.bmp");
	if (textureId == -1)
		return;

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    int screenW = g_windowManager->ScreenW();
    int screenH = g_windowManager->ScreenH();
	glBegin(GL_QUADS);
	glTexCoord2f(0.95f, 1.0f);	glVertex2f(0, 0);
	glTexCoord2f(0.96f, 1.0f);	glVertex2f(screenW, 0);
	glTexCoord2f(0.96f, 0.61f);	glVertex2f(screenW, screenH * 0.67f);
	glTexCoord2f(0.95f, 0.61f);	glVertex2f(0, screenH * 0.67f);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	END_PROFILE(g_app->m_profiler, "Render Background");
}


void Renderer::RenderDash()
{
    int screenW = g_windowManager->ScreenW();
    int screenH = g_windowManager->ScreenH();

    glColor3ubv(g_colourWhite.GetData());
	glEnable(GL_TEXTURE_2D);
	int textureId = g_resourceManager.GetTexture("dash.bmp");
    if (textureId == -1)
		return;

	glBindTexture(GL_TEXTURE_2D, textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	{
        float u1 = 0.97f;
        float u2 = 0.96f;
        float u3 = 0.97f;
        float u4 = 0.96f;
        float v1 = 0.80f;
        float v2 = 0.78f;
        float v3 = 0.76f;
        float v4 = 0.74f;

		// Draw laser temp bar
		{
			float deltaX = 0.971f - 0.800f;
			int x1 = screenW * 0.800f;
			int x2 = x1 + screenW * deltaX * g_level->m_playerShip->m_laserTemp;
			int y1 = screenH * 0.82f;
			int y2 = screenH * 0.85f;
			glBegin(GL_QUADS);
			glTexCoord2f(u1, v1); glVertex2i(x1, y1);
			glTexCoord2f(u2, v2); glVertex2i(x2, y1);
			glTexCoord2f(u4, v4); glVertex2i(x2, y2);
			glTexCoord2f(u3, v3); glVertex2i(x1, y2);
			glEnd();
		}

		// Draw shields bar
		{
			float deltaX = 0.981f - 0.800f;
			float shields = g_level->m_playerShip->m_shields / PLAYER_SHIP_MAX_SHIELDS;
			if (shields < 0.0f)
				shields = 0.0f;
			int x1 = screenW * 0.811f;
			int x2 = x1 + screenW * deltaX * shields;
			int y1 = screenH * 0.88f;
			int y2 = screenH * 0.91f;
			glBegin(GL_QUADS);
			glTexCoord2f(u1, v1); glVertex2i(x1, y1);
			glTexCoord2f(u2, v2); glVertex2i(x2, y1);
			glTexCoord2f(u4, v4); glVertex2i(x2, y2);
			glTexCoord2f(u3, v3); glVertex2i(x1, y2);
			glEnd();
		}

		// Draw speed bar
		{
			float deltaX = 0.971f - 0.800f;
			float speed = g_level->m_playerShip->m_speed / PLAYER_SHIP_MAX_MAX_SPEED;
			if (speed < 0.0f)
				speed = 0.0f;
			int x1 = screenW * 0.832f;
			int x2 = x1 + screenW * deltaX * speed;
			int y1 = screenH * 0.96f;
			int y2 = screenH * 0.99f;
			glBegin(GL_QUADS);
			glTexCoord2f(u1, v1); glVertex2i(x1, y1);
			glTexCoord2f(u2, v2); glVertex2i(x2, y1);
			glTexCoord2f(u4, v4); glVertex2i(x2, y2);
			glTexCoord2f(u3, v3); glVertex2i(x1, y2);
			glEnd();
		}
	}

	// Draw the dash bitmap
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,   1.0f);	glVertex2f(0, 0);
	glTexCoord2f(0.625f, 1.0f);	glVertex2f(screenW, 0);
	glTexCoord2f(0.625f, 0.61f);glVertex2f(screenW, screenH);
	glTexCoord2f(0.0f,   0.61f);glVertex2f(0, screenH);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	RenderRadar();
}


void Renderer::Render()
{
#ifdef PROFILER_ENABLED
	g_app->m_profiler->RenderStarted();
#endif

	FpsMeterAdvance();
	SetOpenGLState();

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	RenderFrame();

	glColor4f(0,0,0,0.6);
	glBegin(GL_QUADS);
	glVertex2f(8.0, 1.0f);
	glVertex2f(70.0, 1.0f);
	glVertex2f(70.0, 15.0f);
	glVertex2f(8.0, 15.0f);
	glEnd();
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	g_gameFont.DrawText2d( 12, 10, DEF_FONT_SIZE, "FPS: %d", m_fps);

	g_gameFont.EndText2d();

	if (!g_inputManager->m_windowHasFocus)
		RenderPaused();	

	START_PROFILE(g_app->m_profiler, "GL Flip");
	g_windowManager->Flip();
	END_PROFILE(g_app->m_profiler, "GL Flip");

#ifdef PROFILER_ENABLED
	g_app->m_profiler->RenderEnded();
#endif
}


void Renderer::StartFadeOut()
{
	m_fadeDelay = 0.0f;
	m_fadeRate = 0.5f;
}


void Renderer::StartFadeIn(float delay)
{
	m_fadedness = 1.0f;
	m_fadeDelay = delay;
	m_fadeRate = -1.0f;
}


void Renderer::RenderFadeOut()
{
	static double lastTime = g_gameTime;
	double timeNow = g_gameTime;
	double timeIncrement = timeNow - lastTime;
	if (timeIncrement > 0.05f) timeIncrement = 0.05f;
	lastTime = timeNow;

	if (m_fadeDelay > 0.0f) 
	{
		m_fadeDelay -= timeIncrement;
	}
	else
	{
		m_fadedness += m_fadeRate * timeIncrement;
		if (m_fadedness < 0.0f)
		{
			m_fadedness = 0.0f;
			m_fadeRate = 0.0f;
		}
		else if (m_fadedness > 1.0f)
		{
			m_fadeRate = 0.0f;
			m_fadedness = 1.0f;
		}
	}
		
	if (m_fadedness > 0.0001f)
	{
		glEnable(GL_BLEND);
		glDepthMask(false);
		glDisable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
        int screenW = g_windowManager->ScreenW();
        int screenH = g_windowManager->ScreenH();
		glColor4ub(0, 0, 0, (int)(m_fadedness * 255.0f)); 
		glBegin(GL_QUADS);
			glVertex2i(0, 0);
			glVertex2i(screenW, 0);
			glVertex2i(screenW, screenH);
			glVertex2i(0, screenH);
		glEnd();
		
		glDisable(GL_BLEND);
		glDepthMask(true);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}


void Renderer::RenderPaused()
{
	const char *msg = "PAUSED";
	int x = g_windowManager->ScreenW()/2;
	int y = g_windowManager->ScreenH()/2;	
	
	g_gameFont.BeginText2d();
	
	glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
	g_gameFont.DrawText2dCentre( x, y, 80, msg );
	
	g_gameFont.EndText2d();
}


void Renderer::RenderFrame()
{
	if (g_app->m_camera->GetMode() == Camera::ModePlayerShip)
	{
		g_gameFont.BeginText2d();
 		RenderBackground();
 		g_gameFont.EndText2d();
	}

	SetupFor3D();

#ifdef DEBUG_RENDER_ENABLED
	g_debugRenderer.Render();
#endif
    
	if (g_app->m_camera->GetMode() == Camera::ModePlayerShip)
	{
        int screenW = g_windowManager->ScreenW();
        int screenH = g_windowManager->ScreenH();
		glScissor(0, screenH * 0.22f, screenW, screenH);
		glEnable(GL_SCISSOR_TEST);
	}
	g_app->m_level->Render();
	if (g_app->m_camera->GetMode() == Camera::ModePlayerShip)
		glDisable(GL_SCISSOR_TEST);
	UnsetObjectLighting();

#if EDITOR_ENABLED == 1
	g_editor.Render();
	CHECK_OPENGL_STATE();
#endif

    g_gameFont.BeginText2d();

	g_hudText.Render();

	// Render cross hair
	if (g_app->m_camera->GetMode() == Camera::ModePlayerShip)
	{
		int x = g_windowManager->ScreenW() / 2;
		int y = g_windowManager->ScreenH() / 2;

		//       ||
		//       ||
		//  ===      ===
		//       ||
		//       ||
		//  1 2  34  5 6

		int a = x * 0.05;
		int b = x * 0.02;
		int c = x * 0.005;
		int x1 = x - a;
		int x2 = x - b;
		int x3 = x - c;
		int x4 = x + c;
		int x5 = x + b;
		int x6 = x + a;
		int y1 = y - a;
		int y2 = y - b;
		int y3 = y - c;
		int y4 = y + c;
		int y5 = y + b;
		int y6 = y + a;

		glColor4f(255, 255, 0, 0.6);
		glBegin(GL_QUADS);
			glVertex2f(x3, y1);
			glVertex2f(x3, y2);
			glVertex2f(x4, y2);
			glVertex2f(x4, y1);

			glVertex2f(x3, y5);
			glVertex2f(x3, y6);
			glVertex2f(x4, y6);
			glVertex2f(x4, y5);

			glVertex2f(x1, y3);
			glVertex2f(x2, y3);
			glVertex2f(x2, y4);
			glVertex2f(x1, y4);

			glVertex2f(x5, y3);
			glVertex2f(x6, y3);
			glVertex2f(x6, y4);
			glVertex2f(x5, y4);
		glEnd();

		RenderDash();

		RenderFadeOut();
	}
}


void Renderer::SetupFor3D() const
{
    float screenW = g_windowManager->ScreenW();
    float screenH = g_windowManager->ScreenH();
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(g_app->m_camera->m_fov,
		screenW / screenH, // Aspect ratio
		NEAR_PLANE, FAR_PLANE);

	g_app->m_camera->SetupModelviewMatrix();


    //
    // Enable lighting

	glEnable(GL_LIGHTING);

    float ambient[] = { 0.2, 0.2, 0.2, 0 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    float black[] = {0,0,0,0};
    {
        float col[] = { 0.25f, 0.25f, 0.25f, 0.0f };
        Vector3 front(-1, -0.5, -1);
        front.Normalize();
        float tmp[4];
        tmp[0] = front.x;
        tmp[1] = front.y;
        tmp[2] = front.z;
        tmp[3] = 0;

        glLightfv(GL_LIGHT0, GL_POSITION, tmp);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, col);
        glLightfv(GL_LIGHT0, GL_SPECULAR, black);
        glLightfv(GL_LIGHT0, GL_AMBIENT, col);
    }

    {
        float col[] = { 0.25f, 0.25f, 0.25f, 0.0f };
        Vector3 front(1, -0.5, 1);
        front.Normalize();
        float tmp[4];
        tmp[0] = front.x;
        tmp[1] = front.y;
        tmp[2] = front.z;
        tmp[3] = 0;
        glLightfv(GL_LIGHT1, GL_POSITION, tmp);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, col);
        glLightfv(GL_LIGHT1, GL_SPECULAR, black);
        glLightfv(GL_LIGHT1, GL_AMBIENT, col);
    }

	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}


void Renderer::FpsMeterAdvance()
{
    static int framesThisSecond = 0;
    static double endOfSecond = HiresTimeGet() + 1.0;

    framesThisSecond++;

    double currentTime = HiresTimeGet();
    if (currentTime > endOfSecond)
    {
        if (currentTime > endOfSecond + 2.0)
            endOfSecond = currentTime + 1.0;
        else
            endOfSecond += 1.0;

		m_fps = framesThisSecond;
        framesThisSecond = 0;
    }
}


void Renderer::SetOpenGLState() const
{
	// Geometry
	glEnable		(GL_CULL_FACE);
	glFrontFace		(GL_CCW);
	glPolygonMode	(GL_FRONT, GL_FILL);
	glShadeModel	(GL_FLAT);

	// Colour
	glDisable		(GL_COLOR_MATERIAL);
	glColorMaterial	(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// Lighting
	glDisable		(GL_LIGHTING);
	
	// Blending, Anti-aliasing, Fog and Polygon Offset
	glDisable       (GL_BLEND);
	glBlendFunc     (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Texture Mapping
	glDisable		(GL_TEXTURE_2D);
	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri	(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexEnvi		(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	int colour[4] = { 0, 0, 0, 0 };
	glTexEnviv		(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, colour);

	// Frame Buffer
	glEnable		(GL_DEPTH_TEST);
	glDepthMask		(true);
	glDepthFunc		(GL_LEQUAL);

	glDisable		(GL_SCISSOR_TEST);

	// Hints
	glHint			(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
}


void Renderer::UnsetObjectLighting() const
{
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
}


void Renderer::ToggleFullscreen()
{
    bool wasWindowed = g_windowManager->IsWindowed();
    g_windowManager->DestroyWin();
    CreateWin(!wasWindowed);

    g_resourceManager.RegenerateOpenGlState();
    ReleaseAssert(g_gameFont.Initialize("data/bitmaps/font.bmp"), "Couldn't load game font");
}
