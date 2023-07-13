/*
 * Copyright (C) 2018 Koen Zandberg <koen@bergzand.net>
 *               2021 Francisco Molina
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for lcd tft displays
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 *
 * @}
 */

#include <stdio.h>
#include "timex.h"
#include "ztimer.h"
#include "benchmark.h"
#include "board.h"
#include "lcd.h"
#include "lcd_internal.h"

#include "riot_logo.h"

#include "st77xx.h"
#include "st77xx_params.h"

#if MODULE_LCD_INIT_SEQUENCE

#if MODULE_ST7735

#include "st7735_internal.h"

static const uint8_t st7735_default_init[] =  {
    LCD_CMD_SWRESET, 0,              /* Soft Reset */
    LCD_DELAY, 120,                  /* Soft Reset needs 120 ms if in Sleep In mode */
    LCD_CMD_SLPOUT, 0,               /* Sleep Out leave Sleep In state after reset */
    LCD_DELAY, 120,                  /* Sleep Out needs 120 ms */
    LCD_CMD_PWCTRL1, 3,              /* Power Control 1 */
        0x82, 0x02, 0x84,            /* AVDD=4.9V, GVDD=4.6V, GVCL=-4.6V, AUTO mode */
    LCD_CMD_PWCTRL2, 1, 0xc5,        /* VGH=3*AVDD, VGL=-10V */
    LCD_CMD_VMCTRL1, 1, 0x04,        /* VCOM=-0.525V */
    LCD_CMD_PWCTRL3, 2,              /* Power Control Normal Mode */
        0x0a, 0x00,                  /* AP=Medium Low, SAP=Small */
    LCD_CMD_PWCTRL4, 2,              /* Power Control Idle Mode */
        0x8a, 0x2e,                  /* AP=Medium Low, SAP=Small */
    LCD_CMD_PWCTRL5, 2,              /* Power Control Partial Mode */
        0x8a, 0xaa,                  /* AP=Medium Low, SAP=Small */
    LCD_CMD_FRAMECTL1, 3,            /* Frame Control in Normal mode */
        0x01, 0x2c, 0x2d,            /* RNTA=1, FPA=44 lines, BPA=45 lines */
    LCD_CMD_FRAMECTL2, 3,            /* Frame Control in Idle mode */
        0x01, 0x2c, 0x2d,            /* RNTB=1, FPA=44 lines, BPA=45 lines */
    LCD_CMD_FRAMECTL3, 6,            /* Frame Control in partial mode */
        0x01, 0x2c, 0x2d,            /* RNTC=1, FPA=44 lines, BPA=45 lines */
        0x01, 0x2c, 0x2d,            /* RNTD=1, FPA=44 lines, BPA=45 lines */
    LCD_CMD_PGAMCTRL, 16,            /* Positive Voltage Gamma Control */
        0x02, 0x1c, 0x07, 0x12, 0x37, 0x32, 0x29, 0x2d,
        0x29, 0x25, 0x2B, 0x39, 0x00, 0x01, 0x03, 0x10,
    LCD_CMD_NGAMCTRL, 16,            /* Negative Voltage Gamma Control */
        0x03, 0x1d, 0x07, 0x06, 0x2E, 0x2C, 0x29, 0x2D,
        0x2E, 0x2E, 0x37, 0x3F, 0x00, 0x00, 0x02, 0x10,
    LCD_CMD_COLMOD, 1, 0x055,        /* 16 bit mode RGB & Control interface pixel format */
    LCD_CMD_MADCTL, 1, ST77XX_PARAM_ROTATION | (ST77XX_PARAM_RGB ? 0 : LCD_MADCTL_BGR),
#if ST77XX_PARAM_INVERTED
    LCD_CMD_DINVON, 0,               /* enable Inversion, reset default is off */
#endif
    LCD_CMD_SLPOUT, 0,               /* Sleep out (turn off sleep mode) */
    LCD_CMD_NORON, 0,                /* Normal display mode on */
    LCD_DELAY, 1,
    LCD_CMD_DISPON, 0,               /* Display on */
};

const uint8_t *seq = st7735_default_init;
size_t seq_len = ARRAY_SIZE(st7735_default_init);

#elif MODULE_ST7789

#include "st7789_internal.h"

static const uint8_t st7789_default_init[] =  {
    LCD_CMD_SWRESET, 0,              /* Soft Reset */
    LCD_DELAY, 120,                  /* Soft Reset needs 120 ms if in Sleep In mode */
    LCD_CMD_SLPOUT, 0,               /* Sleep Out leave Sleep In state after reset */
    LCD_DELAY, 120,                  /* Sleep Out needs 120 ms */
    LCD_CMD_VCOMS, 1, 0x20,          /* VCOM=0.9V */
    LCD_CMD_VRHS, 1, 0x0b,           /* VRH=4.1V */
    LCD_CMD_VDVS, 1, 0x20,           /* VDV=0V */
    LCD_CMD_VCMOFSET, 1, 0x20,       /* VCOMFS=0V */
    LCD_CMD_PWCTRL1X, 2, 0xa4, 0xa1, /* AVDD=6.8V, AVCL=4.8V, VDS=2.3 */
    LCD_CMD_PGAMCTRL, 14,            /* Positive Voltage Gamma Control */
        0xd0, 0x08, 0x11, 0x08, 0x0c, 0x15, 0x39,
        0x33, 0x50, 0x36, 0x13, 0x14, 0x29, 0x2d,
    LCD_CMD_NGAMCTRL, 14,            /* Negative Voltage Gamma Control */
        0xd0, 0x08, 0x10, 0x08, 0x06, 0x06, 0x39,
        0x44, 0x51, 0x0b, 0x16, 0x14, 0x2f, 0x32,
    LCD_CMD_COLMOD, 1, 0x055,        /* 16 bit mode RGB & Control interface pixel format */
    LCD_CMD_MADCTL, 1, ST77XX_PARAM_ROTATION | (ST77XX_PARAM_RGB ? 0 : LCD_MADCTL_BGR),
#if ST77XX_PARAM_INVERTED
    LCD_CMD_DINVON, 0,               /* enable Inversion, reset default is off */
#endif
    LCD_CMD_SLPOUT, 0,               /* Sleep out (turn off sleep mode) */
    LCD_CMD_NORON, 0,                /* Normal display mode on */
    LCD_DELAY, 1,
    LCD_CMD_DISPON, 0,               /* Display on */
};

const uint8_t *seq = st7789_default_init;
size_t seq_len = ARRAY_SIZE(st7789_default_init);

#else
#error "No init sequence defined for the ST77xx variant"
#endif

#endif /* MODULE_LCD_INIT_SEQUENCE */

int main(void)
{
    lcd_t dev;
    dev.driver = &lcd_st77xx_driver;

    puts("lcd TFT display test application");

    /* initialize the sensor */
    printf("Initializing display...");

    /* Enable backlight if macro is defined */
#ifdef BACKLIGHT_ON
    BACKLIGHT_ON;
#endif

    if (lcd_init(&dev, &st77xx_params[0]) == 0) {
        puts("[OK]");
    }
    else {
        puts("[Failed]");
        return 1;
    }

#if MODULE_LCD_INIT_SEQUENCE
    lcd_write_cmd_sequence(&dev, seq, seq_len);
#endif

#if MODULE_LCD_PARALLEL
    if (gpio_is_valid(st77xx_params[0].rdx_pin)) {
        uint8_t data[4];

        lcd_read_cmd(&dev, LCD_CMD_RDDIDIF, data, 3);
        printf("lcd ID: %02x %02x %02x\n", data[0], data[1], data[2]);

        lcd_read_cmd(&dev, LCD_CMD_RDDST, data, 4);
        printf("lcd status: %02x %02x %02x %02x\n", data[0], data[1], data[2], data[3]);
    }
#endif

    puts("lcd TFT display filling map");
    lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, 0x0000);
    puts("lcd TFT display map filled");

    /* Fill square with blue */
    puts("Drawing blue rectangle");
    lcd_fill(&dev, 0, dev.params->lines / 3, 0, dev.params->rgb_channels, 0x001F);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing green rectangle");
    lcd_fill(&dev, dev.params->lines / 3, 2 * (dev.params->lines / 3), 0,
             dev.params->rgb_channels, 0x07E0);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    puts("Drawing red rectangle");
    lcd_fill(&dev, 2 * (dev.params->lines / 3), dev.params->lines, 0,
             dev.params->rgb_channels, 0xf800);
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);

    lcd_invert_on(&dev);
    puts("lcd TFT display inverted");
    ztimer_sleep(ZTIMER_MSEC, 1 * MS_PER_SEC);
    lcd_invert_off(&dev);
    puts("lcd TFT display normal");

    puts("lcd TFT display clear screen with benchmarking");
    BENCHMARK_FUNC("fill", 1,
                   lcd_fill(&dev, 0, dev.params->lines, 0, dev.params->rgb_channels, 0x0000));

#ifndef CONFIG_NO_RIOT_IMAGE
    printf("Write pixmap of size %u x %u with benchmarking\n",
           RIOT_LOGO_WIDTH, RIOT_LOGO_HEIGHT);
    /* Approximate middle of the display */
    uint8_t x1 = (dev.params->lines / 2) - (RIOT_LOGO_WIDTH / 2);
    uint8_t y1 = (dev.params->rgb_channels / 2) - (RIOT_LOGO_HEIGHT / 2);
    BENCHMARK_FUNC("fill", 1,
                   lcd_pixmap(&dev,
                              x1, x1 + RIOT_LOGO_WIDTH - 1,
                              y1, y1 +  RIOT_LOGO_HEIGHT - 1,
                              (const uint16_t *)picture));
#endif
    while (1) {}

    return 0;
}
