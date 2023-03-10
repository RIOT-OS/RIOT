#ifndef GRAPHICS_H
#define GRAPHICS_H
static const unsigned char systemFont[] =
    {
#include "font_terminal.h"
};

#define BLUE 0x7C00
#define RED 0x001F
#define GREEN 0x03E0
#define BLACK 0x0000
#define WHITE 0xFFFF

void setBitmapMode(void);

void clearScreen(void);

void drawBuffer(unsigned short x, unsigned short y, const unsigned char *buffer, unsigned int size);

void drawPixel(unsigned short x, unsigned short y, unsigned short color);

void drawChar(unsigned char c, int x, int y, unsigned short fgColour, unsigned short bgColour);

void drawText(const char *txt, unsigned short x, unsigned short y, unsigned short fgColour, unsigned short bgColour);
#endif