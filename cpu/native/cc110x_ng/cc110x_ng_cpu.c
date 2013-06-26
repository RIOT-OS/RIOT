#include <stdio.h>
#include <err.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <arpa/inet.h>

#include "debug.h"

#include "tap.h"
#include "cc1100sim.h"
#include "cpu.h"

#include "cc110x-arch.h"
#include "cc110x_ng.h"
#include "cc110x_spi.h"
#include "cc110x-internal.h" /* CC1100 constants */


static int _native_cc110x_enabled;

/* arch */

/**
 * writes to SSP0 data register and reads from it once it is ready
 * TODO: move content to simulator
 */
uint8_t cc110x_txrx(uint8_t c)
{
    DEBUG("cc110x_txrx\n");
    return do_txrx(c);
}

/**
 * disables GDO0 interrupt
 */
void cc110x_gdo0_enable(void)
{
    /* this would be for rising/high edge if this was proper hardware */
    DEBUG("cc110x_gdo0_enable\n");
    native_cc110x_gd0_enabled = 1;
    return;
}

/**
 * enables GDO0 interrupt
 */
void cc110x_gdo0_disable(void)
{
    DEBUG("cc110x_gdo0_disable\n");
    native_cc110x_gd0_enabled = 0;
    return;
}

/**
 * enables GDO2 interrupt
 */
void cc110x_gdo2_enable(void)
{
    /* this would be for falling/low edge if this was proper hardware */
    DEBUG("cc110x_gdo2_enable\n");
    native_cc110x_gd2_enabled = 1;
    return;
}

/**
 * disables GDO2 interrupt
 */
void cc110x_gdo2_disable(void)
{
    DEBUG("cc110x_gdo2_disable\n");
    native_cc110x_gd2_enabled = 0;
    return;
}

/**
 * enable interrupts for GDO0 and GDO2
 */
void cc110x_init_interrupts(void)
{
    /* this would be for low edge in both cases if this was proper hardware */
    DEBUG("cc110x_init_interrupts\n");
    cc110x_gdo2_enable();
    cc110x_gdo0_enable();
    return;
}

/* Disables RX interrupt etc. */
void cc110x_before_send(void)
{
    DEBUG("cc110x_before_send\n");
    cc110x_gdo2_disable();
    return;
}
void cc110x_after_send(void)
{
    DEBUG("cc110x_after_send\n");
    cc110x_gdo2_enable();
    return;
}

/* spi */

int cc110x_get_gdo0(void)
{
    DEBUG("cc110x_get_gdo0\n");
    return native_cc110x_gd0;
}
int cc110x_get_gdo1(void)
{
    DEBUG("cc110x_get_gdo1\n");
    return native_cc110x_gd1;
}
int cc110x_get_gdo2(void)
{
    DEBUG("cc110x_get_gdo2\n");
    return native_cc110x_gd2--;
}

void cc110x_spi_init(void)
{
    DEBUG("cc110x_spi_init\n");
    _native_cc110x_enabled = 1; /* power on */
    return;
}

void cc110x_spi_cs(void)
{
    DEBUG("cc110x_spi_cs\n");
    return;
}
void cc110x_spi_select(void)
{
    DEBUG("___cc110x_spi_select\n");
    _native_cc110x_state = STATE_SEL;
    return;
}
void cc110x_spi_unselect(void)
{
    DEBUG("cc110x_spi_unselect\n");
    _native_cc110x_state = STATE_NULL;
    return;
}

/* ng */
