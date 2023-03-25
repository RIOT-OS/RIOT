#ifndef GRAPHICS_H
#define GRAPHICS_H
static const unsigned char systemFont[] =
{
#include "font_terminal.h"
};

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 160
#define NUM_SPRITES 128

#define BLUE 0x7C00
#define RED 0x001F
#define GREEN 0x03E0
#define BLACK 0x0000
#define WHITE 0xFFFF

#pragma pack(2)
typedef struct {
    unsigned short y : 8;
    unsigned short rotation : 1;
    unsigned short double_size : 1;
    unsigned short mode : 2;
    unsigned short mosaic : 1;
    unsigned short color_depth : 1;
    unsigned short shape : 2;

    unsigned short x : 9;
    unsigned short not_used : 3;
    unsigned short hflip : 1;
    unsigned short vflip : 1;
    unsigned short size : 2;

    unsigned short name : 10;
    unsigned short attribute2 : 6;

    unsigned short padding;
} sprite_t;

void setTiledMode(void);

void setBG0Control(void);
void setupBackgroundPalette(const unsigned short *palette, unsigned int size);
void setupCharacterBlock(unsigned int block, const unsigned char *characters, unsigned int size);
void setupMapBlock(unsigned int block, const unsigned short *map, unsigned int size);

void setBitmapMode(void);

void clearScreen(void);

void drawBuffer(unsigned short x, unsigned short y, const unsigned char *buffer, unsigned int size);

void drawPixel(unsigned short x, unsigned short y, unsigned short color);

void drawChar(unsigned char c, int x, int y, unsigned short fgColour, unsigned short bgColour);

void drawText(const char *txt, unsigned short x, unsigned short y, unsigned short fgColour,
              unsigned short bgColour);
#endif
