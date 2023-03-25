/*
 * Copyright (C) 2023 Bennet Blischke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       RIOT demo
 *
 * @author      Bennet Blischke <bennet.blischke@haw-hamburg.de>
 *
 * @}
 */

#include "periph_cpu.h"
#include "gba_irq.h"
#include "graphics.h"
#include "dma.h"

#include "riot_logo_tiles.h"
#include "riot_logo.h"

#include "board.h"
#include "ball.h"


sprite_t sprites[NUM_SPRITES];

struct Ball {
    unsigned short x;
    unsigned short y;
    unsigned short vx;
    unsigned short vy;
    unsigned short width;
    unsigned short height;
    sprite_t *sprites;
};

volatile uint16_t key_pressed = 0;

void keypress_isr(void)
{
    uint16_t read = GBA_KEYPAD ^ 0x3FF;

    if (read) {
        key_pressed = read;
    }
}

volatile unsigned short *bg0_x_scroll = (unsigned short *)0x4000010;
volatile unsigned short *bg0_y_scroll = (unsigned short *)0x4000012;

void move_bg(void)
{
    static unsigned short xscroll = 0;
    static int short xadd = 1;
    static int short xlim = 270;

    *bg0_x_scroll = xscroll += xadd;
    if (xscroll == xlim) {
        xadd = -1;
    }
    else if (xscroll == 0) {
        xadd = 1;
    }
}

void init_board(int pos)
{
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < 8; i++) {
            sprites[i + k * 8].y = 140 + k * 8;
            sprites[i + k * 8].rotation = 0;
            sprites[i + k * 8].double_size = 0;
            sprites[i + k * 8].mode = 0;
            sprites[i + k * 8].mosaic = 0;
            sprites[i + k * 8].color_depth = 1;
            sprites[i + k * 8].shape = 0;
            sprites[i + k * 8].x = pos + i * 8;
            sprites[i + k * 8].hflip = 0;
            sprites[i + k * 8].vflip = 0;
            sprites[i + k * 8].size = 0;
            sprites[i + k * 8].name = (i + k * 8) * 2;
        }
    }
}

void move_board(int pos)
{
    for (int k = 0; k < 2; k++) {
        for (int i = 0; i < 8; i++) {
            sprites[i + k * 8].x = pos + i * 8;
        }
    }
}

void init_ball(struct Ball *ball)
{
    ball->sprites = &sprites[16];
    ball->x = 0;
    ball->y = 20;
    ball->vx = 1;
    ball->vy = 1;
    ball->width = 26;
    ball->height = 24;

    int y_off = 0;
    int x_off = 0;

    for (int i = 0; i < 16; i++) {
        ball->sprites[i].y = 20 + y_off;
        ball->sprites[i].rotation = 0;
        ball->sprites[i].double_size = 0;
        ball->sprites[i].mode = 0;
        ball->sprites[i].mosaic = 0;
        ball->sprites[i].color_depth = 1;
        ball->sprites[i].shape = 0;
        ball->sprites[i].x = x_off;
        ball->sprites[i].hflip = 0;
        ball->sprites[i].vflip = 0;
        ball->sprites[i].size = 0;
        ball->sprites[i].name = (16 + i) * 2;

        x_off += 8;
        if (i % 4 == 3) {
            y_off += 8;
            x_off = 0;
        }
    }
}

void move_ball(struct Ball *ball)
{
    ball->x += ball->vx;
    ball->y += ball->vy;
    for (int i = 0; i < 16; i++) {
        ball->sprites[i].y += ball->vy;
        ball->sprites[i].x += ball->vx;
    }
    if (ball->x <= 0 || ball->x >= SCREEN_WIDTH - ball->width) {
        ball->vx *= -1;
    }
    if (ball->y <= 0 || ball->y >= SCREEN_HEIGHT - ball->height) {
        ball->vy *= -1;
    }
}

void delay(unsigned int amount)
{
    for (unsigned int i = 0; i < amount * 1000; i++) {
        (void)amount;
    }
}

volatile unsigned short *scanline_counter = (volatile unsigned short *)0x4000006;
void wait_vblank(void)
{
    while (*scanline_counter < 160) {}
}

void sprite_clear(void)
{
    /* move all sprites offscreen to hide them */
    for (int i = 0; i < NUM_SPRITES; i++) {
        sprites[i].y = SCREEN_HEIGHT;
        sprites[i].x = SCREEN_WIDTH;
    }
}

int main(void)
{
    gba_irq_register_cb(keypress_isr, GBA_IRQ_KEYPAD);
    while (1) {
        setTiledMode();
        setBG0Control();

        setupBackgroundPalette(riot_logo_tiles_palette, 256);
        setupCharacterBlock(0, riot_logo_tiles_data, sizeof(riot_logo_tiles_data));
        setupMapBlock(16, riot_logo, sizeof(riot_logo));

        int frame_skip = 0;
        *bg0_y_scroll = 50;

        while (!key_pressed) {
            wait_vblank();
            frame_skip++;
            if (frame_skip == 10 * 240) {
                move_bg();
                frame_skip = 0;
            }
        }
        frame_skip = 0;
        key_pressed = 0;
        *bg0_x_scroll = 8;
        wait_vblank();

        dma_memcpy16(3, (unsigned short *)0x5000200, (unsigned short *)board_palette, 256);
        dma_memcpy16(3, (unsigned short *)0x6010000, (unsigned short *)(uintptr_t)board_data,
                     (board_width * board_height) / 2);
        dma_memcpy16(3, (unsigned short *)(0x6010000 + (board_width * board_height)),
                     (unsigned short *)(uintptr_t)ball_data, (ball_width * ball_height) / 2);

        sprite_clear();

        GBA_DISPCNT |= 0x1000;
        GBA_DISPCNT |= 0x40;
        dma_memcpy16(3, (unsigned short *)0x7000000, (unsigned short *)(uintptr_t)sprites,
                     NUM_SPRITES * 4);


        struct Ball ball;

        int pos = 140;
        int speed = 5 * 240;
        init_board(pos);
        init_ball(&ball);
        while (1) {
            wait_vblank();
            frame_skip++;
            if (frame_skip != speed) {
                continue;
            }
            frame_skip = 0;
            if (key_pressed) {
                switch (key_pressed) {
                case GBA_KEY_A:
                    break;
                case GBA_KEY_B:
                    break;
                case GBA_KEY_SELECT:
                    break;
                case GBA_KEY_START:
                    break;
                case GBA_KEY_RIGHT:
                    pos++;
                    break;
                case GBA_KEY_LEFT:
                    pos--;
                    break;
                case GBA_KEY_UP:
                    break;
                case GBA_KEY_DOWN:
                    break;
                case GBA_KEY_R:
                    break;
                case GBA_KEY_L:
                    break;
                default:
                    break;
                }
                move_board(pos);
                key_pressed = 0;
            }
            move_ball(&ball);
            if (ball.x + ball.width >= pos && ball.x <= pos + 60) {
                if (ball.y + ball.height == 140) {
                    speed -= 50;
                    ball.vy = -1;
                    if (ball.x + ball.width < pos + 16) {
                        ball.vx = -2;
                    }
                    else if (ball.x > pos + 42) {
                        ball.vx = 2;
                    }
                    else if (ball.vx * ball.vx > 1) {
                        ball.vx /= 2;
                    }
                }
            }
            if (ball.y >= SCREEN_HEIGHT - ball.height) {
                ball.vx = 0;
                ball.vy = 0;
                break;
            }
            wait_vblank();
            dma_memcpy16(3, (unsigned short *)0x7000000, (unsigned short *)(uintptr_t)sprites,
                         NUM_SPRITES * 4);
        }

        for (int i = 0; i < 10000; i++) {
            wait_vblank();
        }
    }

    return 0;
}
