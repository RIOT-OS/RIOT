/*
 * SPDX-FileCopyrightText: 2017 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_dsp0401
 * @{
 *
 * @file
 * @brief       Device driver implementation for dsp0401 alphanumeric display
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#include <inttypes.h>
#include <string.h>

#include "dsp0401.h"
#include "timex.h"
#include "ztimer.h"
#include "periph/gpio.h"
#include "periph/pwm.h"

#define ENABLE_DEBUG        0
#include "debug.h"

#define LATCH_DELAY         (50U) /* 50 us */
#define SCK                 (dev->params.clk)
#define SDI                 (dev->params.sdi)
#define LAT                 (dev->params.lat)
#define PWM_PERIPH          (dev->params.pwm)
#define PWM_CHAN            (dev->params.pwm_channel)
#define BRIGHTNESS          (dev->params.brightness)
#define PWM_FREQ            (1000U)
#define PWM_STEPS           (256U)
#define MOD_COUNT           (dev->params.module_count)

/* Internal Ascii char map */
#define CHAR_MAP_MIN         (30U)
#define CHAR_MAP_MAX         (127U)

static const uint8_t dsp0401_charmap[][2] = {
                     /* dec: char */
    { 0x00, 0x00 },  /*  30:      */
    { 0x00, 0x00 },  /*  31:      */
    { 0x00, 0x00 },  /*  32:  ' ' */
    { 0x00, 0x00 },  /*  33:   !  */
    { 0x00, 0x00 },  /*  34:   "  */
    { 0x00, 0x00 },  /*  35:   #  */
    { 0xB5, 0xB5 },  /*  36:   $  */
    { 0xB3, 0xB3 },  /*  37:   %  */
    { 0x00, 0x00 },  /*  38:   &  */
    { 0x01, 0x00 },  /*  39:   `  */
    { 0x81, 0x00 },  /*  40:   (  */
    { 0x00, 0x42 },  /*  41:   )  */
    { 0x53, 0x53 },  /*  42:   *  */
    { 0x11, 0x11 },  /*  43:   +  */
    { 0x00, 0x00 },  /*  44:   ,  */
    { 0x10, 0x10 },  /*  45:   -  */
    { 0x00, 0x04 },  /*  46:   .  */
    { 0x02, 0x02 },  /*  47:   /  */
    { 0xAC, 0xAC },  /*  48:   0  */
    { 0x01, 0x01 },  /*  49:   1  */
    { 0x9C, 0x9C },  /*  50:   2  */
    { 0xBC, 0x84 },  /*  51:   3  */
    { 0x38, 0x30 },  /*  52:   4  */
    { 0xB4, 0xB4 },  /*  53:   5  */
    { 0xB4, 0xBC },  /*  54:   6  */
    { 0x16, 0x92 },  /*  55:   7  */
    { 0xBC, 0xBC },  /*  56:   8  */
    { 0xBC, 0xB4 },  /*  57:   9  */
    { 0x04, 0x04 },  /*  58:   :  */
    { 0x04, 0x02 },  /*  59:   ;  */
    { 0x00, 0x00 },  /*  60:   <  */
    { 0x90, 0x14 },  /*  61:   =  */
    { 0x00, 0x00 },  /*  62:   >  */
    { 0x00, 0x00 },  /*  63:   ?  */
    { 0xAC, 0x9D },  /*  64:   @  */
    { 0x3C, 0xB8 },  /*  65:   A  */
    { 0xB1, 0xBC },  /*  66:   B  */
    { 0x84, 0xAC },  /*  67:   C  */
    { 0xAD, 0x85 },  /*  68:   D  */
    { 0x84, 0xBC },  /*  69:   E  */
    { 0x04, 0xB8 },  /*  70:   F  */
    { 0xB4, 0xAC },  /*  71:   G  */
    { 0x38, 0x38 },  /*  72:   H  */
    { 0x85, 0x85 },  /*  73:   I  */
    { 0xA8, 0x0C },  /*  74:   J  */
    { 0x42, 0x38 },  /*  75:   K  */
    { 0x80, 0x2C },  /*  76:   L  */
    { 0x2A, 0x68 },  /*  77:   M  */
    { 0x68, 0x68 },  /*  78:   N  */
    { 0xAC, 0xAC },  /*  79:   0  */
    { 0x1C, 0xB8 },  /*  80:   P  */
    { 0xEC, 0xAC },  /*  81:   Q  */
    { 0x5C, 0xB8 },  /*  82:   R  */
    { 0xB4, 0xB4 },  /*  83:   S  */
    { 0x05, 0x81 },  /*  84:   T  */
    { 0xA8, 0x2C },  /*  85:   U  */
    { 0x02, 0x2A },  /*  86:   V  */
    { 0x68, 0x2A },  /*  87:   W  */
    { 0x42, 0x42 },  /*  88:   X  */
    { 0x02, 0x42 },  /*  89:   Y  */
    { 0x86, 0x86 },  /*  90:   Z  */
    { 0x00, 0xAC },  /*  91:   [  */
    { 0x40, 0x40 },  /*  92:   \  */
    { 0xAC, 0x00 },  /*  93:   ]  */
    { 0x00, 0x00 },  /*  94:   ^  */
    { 0x80, 0x04 },  /*  95:   _  */
    { 0x00, 0x40 },  /*  96:   `  */
    { 0x00, 0x00 },  /*  97:   a  */
    { 0x00, 0x00 },  /*  98:   b  */
    { 0x00, 0x00 },  /*  99:   c  */
    { 0x00, 0x00 },  /* 100:   d  */
    { 0x00, 0x00 },  /* 101:   e  */
    { 0x00, 0x00 },  /* 102:   f  */
    { 0x00, 0x00 },  /* 103:   g  */
    { 0x00, 0x00 },  /* 104:   h  */
    { 0x00, 0x00 },  /* 105:   i  */
    { 0x00, 0x00 },  /* 106:   j  */
    { 0x00, 0x00 },  /* 107:   k  */
    { 0x00, 0x00 },  /* 108:   l  */
    { 0x00, 0x00 },  /* 109:   m  */
    { 0x00, 0x00 },  /* 110:   n  */
    { 0x00, 0x00 },  /* 111:   o  */
    { 0x00, 0x00 },  /* 112:   p  */
    { 0x00, 0x00 },  /* 113:   q  */
    { 0x00, 0x00 },  /* 114:   r  */
    { 0x00, 0x00 },  /* 115:   s  */
    { 0x00, 0x00 },  /* 116:   t  */
    { 0x00, 0x00 },  /* 117:   u  */
    { 0x00, 0x00 },  /* 118:   v  */
    { 0x00, 0x00 },  /* 119:   w  */
    { 0x00, 0x00 },  /* 120:   x  */
    { 0x00, 0x00 },  /* 121:   y  */
    { 0x00, 0x00 },  /* 122:   z  */
    { 0x85, 0x11 },  /* 123:   {  */
    { 0x01, 0x01 },  /* 124:   |  */
    { 0x11, 0x85 },  /* 125:   }  */
    { 0x00, 0x00 },  /* 126:   ~  */
    { 0x00, 0x00 },  /* 127:      */
};

static void _shift_char(const dsp0401_t *dev, uint8_t c)
{
    /* Unsupported chars not displayed */
    if ((c < CHAR_MAP_MIN) || (c > CHAR_MAP_MAX)) {
        c = ' ';
    }

    for (unsigned p = 0 ; p < 2 ; ++p) {
        for (unsigned i = 0; i < 8; ++i)  {
            gpio_write(SDI, !!(dsp0401_charmap[c - CHAR_MAP_MIN][p] & (1 << (7 - i))));
            gpio_set(SCK);
            gpio_clear(SCK);
        }
    }
}

static void _latch(const dsp0401_t *dev)
{
    ztimer_sleep(ZTIMER_USEC, LATCH_DELAY);
    gpio_set(LAT);
    ztimer_sleep(ZTIMER_USEC, LATCH_DELAY);
    gpio_clear(LAT);
    ztimer_sleep(ZTIMER_USEC, LATCH_DELAY);
}

int dsp0401_init(dsp0401_t *dev, const dsp0401_params_t *params)
{
    dev->params = *params;

    /* Initialize clock pin and set it to low */
    if (gpio_init(SCK, GPIO_OUT) < 0) {
        DEBUG("[ERROR] CLK GPIO initialization failed.");
        return -DSP0401_ERR_CLK_GPIO;
    }
    gpio_clear(SCK);

    /* Initialize data pin */
    if (gpio_init(SDI, GPIO_OUT) < 0) {
        DEBUG("[ERROR] SDI GPIO initialization failed.");
        return -DSP0401_ERR_SDI_GPIO;
    }
    gpio_clear(SDI);

    /* Initialize latch pin and set it to low */
    if (gpio_init(LAT, GPIO_OUT) < 0) {
        DEBUG("[ERROR] LAT GPIO initialization failed.");
        return -DSP0401_ERR_LAT_GPIO;
    }
    gpio_clear(LAT);

    /* Initialize PWM used for LED brightness */
    if (!pwm_init(PWM_PERIPH, PWM_LEFT, PWM_FREQ, PWM_STEPS)) {
        DEBUG("[ERROR] PWM initialization failed.");
        return -DSP0401_ERR_PWM;
    }
    pwm_set(PWM_PERIPH, PWM_CHAN, BRIGHTNESS);

    dsp0401_clear_text(dev);

    return DSP0401_OK;
}

void dsp0401_display_text(const dsp0401_t *dev, char *text)
{
    unsigned text_len = strlen(text);
    for (unsigned i = 0 ; i < MOD_COUNT * 4; ++i) {
        if (i < text_len) {
           _shift_char(dev, text[i]);
        } else {
            _shift_char(dev, ' ');
        }
    }

    _latch(dev);
}

void dsp0401_clear_text(const dsp0401_t *dev)
{
    /* each module has 4 alphanumeric displays */
    for (unsigned i = 0; i < MOD_COUNT * 4; ++i) {
        _shift_char(dev, ' ');
    }
    _latch(dev);
}

void dsp0401_scroll_text(const dsp0401_t *dev, char *text, uint16_t delay)
{
    dsp0401_clear_text(dev);

    for (unsigned i = 0; i < strlen(text); ++i) {
        _shift_char(dev, text[i]);
        _latch(dev);
        ztimer_sleep(ZTIMER_USEC, delay * US_PER_MS);
    }

    for (unsigned i = 0; i < MOD_COUNT * 4; ++i) {
        _shift_char(dev, ' ');
        _latch(dev);
        ztimer_sleep(ZTIMER_USEC, delay * US_PER_MS);
    }
}
