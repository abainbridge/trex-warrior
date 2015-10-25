#include "lib/universal_include.h"
#include "hud_text.h"

#include "lib/hi_res_time.h"
#include "lib/gfx/text_renderer.h"
#include "lib/window_manager.h"


#define TEXT_DISPLAY_TIME 5.0f


HudText g_hudText;


HudText::HudText()
{
}


void HudText::ShowText(char const *text)
{
    m_text2 = m_text1;
    m_textEndTime2 = m_textEndTime1;

    m_text1 = text;
    m_textEndTime1 = g_gameTime + TEXT_DISPLAY_TIME;
}


void HudText::Advance()
{
}


void HudText::Render()
{
    int x = g_windowManager->ScreenW() * 0.5f;
    int y = g_windowManager->ScreenH() * 0.65f;

    if (g_gameTime < m_textEndTime1)
        g_gameFont.DrawText2dCentre(x, y + 25, 25, m_text1.m_str);
    if (g_gameTime < m_textEndTime2)
        g_gameFont.DrawText2dCentre(x, y, 25, m_text2.m_str);
}