#include "LcdLayouts.hpp"

LcdLayouts::LcdLayouts() : m_lcd(0x27, 20, 4) // set the LCD address to 0x27 for a 16 chars and 2 line display
{
}

void LcdLayouts::init()
{
    m_lcd.init();
    m_lcd.backlight();
}

void LcdLayouts::selectKeyBoardMode(const String& p_firstLine)
{
    // 0 M:PG_DU  PG:1000
    // 1 W:OFF BT:PG_DW

    m_lcd.clear();
    m_lcd.setCursor(0, 0);
    m_lcd.print(p_firstLine);
    m_lcd.setCursor(0, 1);
    m_lcd.print("     ^^^^");
}

void LcdLayouts::defaultL(const String& p_mode,
                          int p_pageId,
                          bool p_wireless,
                          bool p_wirelessError,
                          const String& p_buttonText /*= ""*/)
{
    // 0 M:PG_DU  PG:1000
    // 1 W:OFF BT:PG_DW

    String firstLine    = "M:" + p_mode + "  PG:" + String(p_pageId);
    String wirelessText = p_wireless ? "ON " : "OFF";
    if (p_wireless && p_wirelessError)
    {
        wirelessText = "ERR";
    }
    String secondLine = "W:" + wirelessText + "   ";
    if (p_buttonText.length() > 0)
    {
        secondLine += "BT:" + p_buttonText;
    }
    else
    {
    }

    m_lcd.clear();
    m_lcd.setCursor(0, 0);
    m_lcd.print(firstLine);
    m_lcd.setCursor(0, 1);
    m_lcd.print(secondLine);
}