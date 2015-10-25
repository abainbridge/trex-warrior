#ifndef INCLUDED_HUD_TEXT_H
#define INCLUDED_HUD_TEXT_H


#include "lib/lite_string.h"


class HudText
{
public:
    float m_textEndTime1;
    float m_textEndTime2;
    LiteString m_text1;
    LiteString m_text2;

    HudText();
    void ShowText(char const *text);

    void Advance();
    void Render();
};


extern HudText g_hudText;


#endif
