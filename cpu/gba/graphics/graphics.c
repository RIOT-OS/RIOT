#include "periph_cpu.h"
#include "graphics.h"
#include <string.h>

void setBitmapMode(void)
{
    GBA_DISPCNT = GBA_DISPCNT_BGMODE_3 | GBA_DISPCNT_SDBG_2;
    clearScreen();
}

void clearScreen(void)
{
    memset(GBA_VRAM, 0x00, 240 * 160 * 2);
}

void drawBuffer(unsigned short x, unsigned short y, const unsigned char *buffer, unsigned int size)
{
    uint32_t offset = x + y * 240;
    if (offset + size < 240 * 160 * 2)
        memcpy(GBA_VRAM, buffer, size);
}

void drawPixel(unsigned short x, unsigned short y, unsigned short color)
{
    GBA_VRAM[x + y * 240] = color;
}

void drawChar(unsigned char c, int x, int y, unsigned short fgColour, unsigned short bgColour)
{
    unsigned char mask;
    const unsigned char *font = systemFont + (c * FONT_HEIGHT);
    unsigned short colourToDraw;

    for (int h = 0; h < FONT_HEIGHT; h++)
    {
        mask = 0x01;

        // This loop draws up to 1 byte (8 bit) per line; so 8 pixels at most
        for (int w = 0; w < FONT_WIDTH; w++)
        {
            if ((*font) & mask)
            {
                colourToDraw = fgColour;
            }
            else
            {
                colourToDraw = bgColour;
            }

            drawPixel(x + w, y + h, colourToDraw);

            mask <<= 1;
        }
        font++;
    }
}

void drawText(const char *txt, unsigned short x, unsigned short y, unsigned short fgColour, unsigned short bgColour)
{
    unsigned short len = strlen(txt);
    for (size_t cursor = 0; cursor < len; cursor++)
    {
        char c = txt[cursor];

        drawChar(c, x + (cursor * FONT_WIDTH), y, fgColour, bgColour);
    }
}
