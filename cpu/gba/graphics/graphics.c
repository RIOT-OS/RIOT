#include "periph_cpu.h"
#include "graphics.h"
#include <string.h>

void setTiledMode(void)
{
    GBA_DISPCNT = GBA_DISPCNT_BGMODE_0 | GBA_DISPCNT_SDBG_0;
}

void setBG0Control(void)
{
    GBA_BG0CNT = 0    |       /* priority, 0 is highest, 3 is lowest */
            (0 << 2)  |       /* the char block the image data is stored in */
            (0 << 6)  |       /* the mosaic flag */
            (1 << 7)  |       /* color mode, 0 is 16 colors, 1 is 256 colors */
            (16 << 8) |       /* the screen block the tile data is stored in */
            (1 << 13) |       /* wrapping flag */
            (0 << 14);        /* bg size, 0 is 256x256 */
}

void setupBackgroundPalette(const unsigned short *palette, unsigned int size)
{
    for (unsigned int i = 0; i < size; i++)
        GBA_VRAM_PALETTE[i] = palette[i];
}

void setupCharacterBlock(unsigned int block, const unsigned char *characters, unsigned int size)
{
    volatile unsigned char* addr = ((volatile unsigned char*  )GBA_VRAM + (block * 0x4000));
    for (unsigned int i = 0; i < size; i ++) {
        addr[i] = characters[i];
    }
}

void setupMapBlock(unsigned int block, const unsigned short *map, unsigned int size)
{
    volatile unsigned short* addr = GBA_VRAM + (block * 0x400);
    for (unsigned int i = 0; i < size; i++)
        addr[i] = map[i];
}

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
