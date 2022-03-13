#include <assert.h>
#include <string.h>
#include "byteorder.h"
#include "periph/spi.h"
#include "ztimer.h"
#include "kernel_defines.h"

#include "lpm013m126.h"
#include "lpm013m126_internal.h"

#define ENABLE_DEBUG 1
#include "debug.h"

//
// since the display always expects full lines and
// each line must be preceeded with a line address and trailed by dummy bytes
// we can not just transfer a bitmap buffer but need to chop the lines up
// and copy everything into a transfer buffer to speed up things,
// the TRANSER_BUFFER_LINES defines how many lines will be copied in one transfer,
// the more lines the faster it will be
//
#define TRANSER_BUFFER_LINES 16
static uint8_t transfer_buffer[(TRANSER_BUFFER_LINES * ((LPM013M126_COLUMNS / 2) + 2)) +2];

static int _lpm013m126_write(const lpm013m126_t *dev, uint8_t *buf, int len)
{
    // at least two bytes for commands
    if (len<2)
        return -1;
    // all writes must be 16bit aligned
    if (len%2)
        return -1;

    gpio_set(dev->params->cs_pin);

    spi_acquire(dev->params->spi, SPI_CS_UNDEF, SPI_MODE_0, SPI_CLK_1MHZ);
    spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, false, buf, NULL, len);
    spi_release(dev->params->spi);

    gpio_clear(dev->params->cs_pin);

    return len;
}


//
// write cnt number of lines to the LCD,
// starting from line start
// Note: only full lines can be written
// line buffer oly conntains pixel data, one nibble per pixel RGBx
//
int lpm013m126_write_lines(const lpm013m126_t *dev, uint8_t *lines, uint8_t start, uint16_t cnt)
{
    uint8_t *bufp;

    if (cnt > TRANSER_BUFFER_LINES)
        cnt=TRANSER_BUFFER_LINES;

    gpio_set(dev->params->cs_pin);

    spi_acquire(dev->params->spi, SPI_CS_UNDEF, SPI_MODE_0, SPI_CLK_1MHZ);

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
    // two trailing dummy bytes per line
    *bufp++ = 0;
    *bufp++ = 0;

    gpio_set(dev->params->cs_pin);
    spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, false, transfer_buffer, NULL, (cnt*(2+(LPM013M126_COLUMNS/2))+2) );
    gpio_clear(dev->params->cs_pin);

    spi_release(dev->params->spi);

    return cnt;
}

// clear the entire screen (to black)
void lpm013m126_clear(const lpm013m126_t *dev)
{
    uint8_t buf[2];

    buf[0]=LPM013M126_CMD_ALL_CLEAR;
    buf[1]=0x00;
    _lpm013m126_write(dev, buf, 2);
}

// invert all colors on the whole screen
void lpm013m126_invert(const lpm013m126_t *dev)
{
    uint8_t buf[2];

    buf[0]=LPM013M126_CMD_INVERSION;
    buf[1]=0x00;
    _lpm013m126_write(dev, buf, 2);
}

// clear flags like INVERSION
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
    // XXX_pwm_poweron(PWM_DEV(1));

    gpio_init(dev->params->cs_pin, GPIO_OUT);
    gpio_clear(dev->params->cs_pin);
    gpio_init(dev->params->disp_pin, GPIO_OUT);
    gpio_set(dev->params->disp_pin);
    spi_init(dev->params->spi);

    lpm013m126_clear(dev);

    return 0;
}
