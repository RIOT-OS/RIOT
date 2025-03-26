#include <assert.h>
#include <string.h>
#include "byteorder.h"
#include "periph/spi.h"
#include "periph/pwm.h"
#include "periph/gpio.h"
#include "ztimer.h"
#include "kernel_defines.h"

#include "lpm013m126.h"
#include "lpm013m126_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if IS_USED(MODULE_LVGL)
#ifndef LVGL_UPDATE_FULL_LINES_ONLY
#error Must set -DLVGL_UPDATE_FULL_LINES_ONLY for use with LVGL
#endif
#endif

/*
 * since the display always expects full lines and
 * each line must be preceeded with a line address and trailed by dummy bytes
 * we can not just transfer a bitmap buffer but need to chop the lines up
 * and copy everything into a transfer buffer to speed up things,
 * the TRANSER_BUFFER_LINES defines how many lines will be copied in one transfer,
 * the more lines the faster it will be
*/
#define TRANSER_BUFFER_LINES 18
static uint8_t transfer_buffer[(TRANSER_BUFFER_LINES * ((LPM013M126_COLUMNS / 2) + 2)) + 3];

static uint8_t _display_on = 0;


static int _lpm013m126_write(const lpm013m126_t *dev, uint8_t *buf, int len)
{
    /* at least two bytes for commands */
    if (len<2)
        return -1;
    /* all writes must be 16bit aligned */
    if (len%2)
        return -1;

    gpio_set(dev->params->cs_pin);

    spi_acquire(dev->params->spi, SPI_CS_UNDEF, dev->params->spi_mode, dev->params->spi_clk);
    spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, false, buf, NULL, len);
    spi_release(dev->params->spi);

    gpio_clear(dev->params->cs_pin);

    return len;
}


/*
 * write cnt number of lines to the LCD,
 * starting from line start
 * Note: only full lines can be written
 * line buffer oly conntains pixel data, one nibble per pixel RGBx
*/
int lpm013m126_write_lines(const lpm013m126_t *dev, uint8_t *lines, uint8_t start, uint16_t cnt)
{
    uint8_t *bufp;

    if (cnt > TRANSER_BUFFER_LINES)
        cnt=TRANSER_BUFFER_LINES;

    if (dev->params->spi_mode != SPI_MODE_0)
        DEBUG("wl mode fail %d\n", dev->params->spi_mode);
    if (dev->params->spi_clk != SPI_CLK_5MHZ)
        DEBUG("wl clk fail %d\n", dev->params->spi_clk);

    bufp = transfer_buffer;
    *bufp++ = LPM013M126_CMD_UPDATE | (start >> 8);
    *bufp++ = (start & 0xff);
    for (int i=0; i<cnt; i++) {
        memcpy(bufp, &lines[i*(LPM013M126_COLUMNS/2)], (LPM013M126_COLUMNS/2));
        bufp += (LPM013M126_COLUMNS/2);
        start++;
        *bufp++ = (start >> 8);
        *bufp++ = (start & 0xff);
    }
    /* two trailing dummy bytes per line */
    *bufp++ = 0;
    *bufp++ = 0;

    gpio_set(dev->params->cs_pin);
    spi_acquire(dev->params->spi, SPI_CS_UNDEF, dev->params->spi_mode, dev->params->spi_clk);

    spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, false, transfer_buffer, NULL, (cnt*(2+(LPM013M126_COLUMNS/2))+2) );

    spi_release(dev->params->spi);
    gpio_clear(dev->params->cs_pin);

    return cnt;
}

/* the LVGL input color buffer is 16bpp, RGB 565 */
static int lpm013m126_write_8bpp_lines(const lpm013m126_t *dev, const uint16_t *lines, uint8_t start, uint16_t cnt)
{
    uint8_t *bufp;
    uint8_t *inlines = (uint8_t *)lines;

    if (cnt > TRANSER_BUFFER_LINES) {
        DEBUG("lpm013m126: write clipped to buffer %d>%d\n", cnt, TRANSER_BUFFER_LINES);
        cnt=TRANSER_BUFFER_LINES;
    }

    DEBUG("l8 %d %d\n", start, cnt);

    if (dev->params->spi_mode != SPI_MODE_0)
        DEBUG("w8 mode fail %d\n", dev->params->spi_mode);
    if (dev->params->spi_clk != SPI_CLK_5MHZ)
        DEBUG("w8 clk fail %u != %u\n", dev->params->spi_clk, SPI_CLK_5MHZ);

    bufp = transfer_buffer;
    *bufp++ = LPM013M126_CMD_UPDATE | (start >> 8);
    *bufp++ = (start & 0xff);

    for (int i=0; i<cnt; i++) {
        for (int x=0; x<LPM013M126_COLUMNS; x+=2) {
           #define P4(x)  ((((x & 0xc0) ? 1 : 0) << 3) | (((x & 0x38) ? 1 : 0) << 2) | (((x & 0x07) ? 1 : 0) << 1))
           *(bufp+(x/2))=(P4(inlines[(i*LPM013M126_COLUMNS)+x]) << 4) | P4(inlines[(i*LPM013M126_COLUMNS)+x+1]);
        }
        bufp += (LPM013M126_COLUMNS/2);
        start++;
        *bufp++ = (start >> 8);
        *bufp++ = (start & 0xff);
    }
    /* two trailing dummy bytes per transfer */
    *bufp++ = 0;
    *bufp++ = 0;

    gpio_set(dev->params->cs_pin);
    spi_acquire(dev->params->spi, SPI_CS_UNDEF, dev->params->spi_mode, SPI_CLK_5MHZ);

    spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, false, transfer_buffer, NULL, (cnt*(2+(LPM013M126_COLUMNS/2))+2) );

    spi_release(dev->params->spi);
    gpio_clear(dev->params->cs_pin);

    return cnt;
}

void lpm013m126_pixmap(const lpm013m126_t *dev, uint16_t x1, uint16_t x2, uint16_t y1,
                 uint16_t y2, const uint16_t *color)
{
    (void) x1;
    (void) x2;
    /* DEBUG("lpm013m126_pixmap(%d, %d, %d, %d)\n", x1,x2,y1,y2); */
    assert(dev);
    assert(color);

    _display_on=1;

    lpm013m126_write_8bpp_lines(dev, color, y1, 1+y2-y1);
}

// clear the entire screen (to black)
void lpm013m126_clear(const lpm013m126_t *dev)
{
    uint8_t buf[2];

    buf[0]=LPM013M126_CMD_ALL_CLEAR;
    buf[1]=0x00;
    _lpm013m126_write(dev, buf, 2);
}

/* invert all colors on the whole screen */
void lpm013m126_invert(const lpm013m126_t *dev)
{
    uint8_t buf[2];

    buf[0]=LPM013M126_CMD_INVERSION;
    buf[1]=0x00;
    _lpm013m126_write(dev, buf, 2);
}

/* clear flags like INVERSION */
void lpm013m126_noupdate(const lpm013m126_t *dev)
{
    uint8_t buf[2];

    buf[0]=LPM013M126_CMD_NO_UPDATE;
    buf[1]=0x00;
    _lpm013m126_write(dev, buf, 2);
}

int lpm013m126_init(lpm013m126_t *dev, const lpm013m126_params_t *params)
{
    assert(dev);
    assert(params);
    dev->params = params;

    DEBUG("LPM013M126 init()\n");

    gpio_init(dev->params->cs_pin, GPIO_OUT);
    gpio_clear(dev->params->cs_pin);

    gpio_init(dev->params->disp_pin, GPIO_OUT);
    gpio_set(dev->params->disp_pin);

    lpm013m126_clear(dev);

    return 0;
}
