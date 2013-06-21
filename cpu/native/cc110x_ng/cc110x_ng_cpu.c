#include <stdio.h>
#include <err.h>

#include <debug.h>

#include <cc110x-arch.h>
#include <cc110x_ng.h>
#include <cc110x_spi.h>
#include <cc110x-internal.h> /* CC1100_READ_BURST etc. */

static int native_cc110x_enabled;

static int native_cc110x_gd0;
static int native_cc110x_gd1;
static int native_cc110x_gd2;

static int native_cc110x_gd0_enabled;
static int native_cc110x_gd2_enabled;

static uint8_t native_cc110x_ssp0dr;

/* arch */

/**
 * writes to SSP0 data register and reads from it once it is ready
 */
uint8_t cc110x_txrx(uint8_t c)
{
    native_cc110x_ssp0dr = c;

    switch(c) {
        case CC1100_READ_BURST:
        case CC1100_WRITE_BURST:
        case CC1100_READ_SINGLE:
        case CC1100_NOBYTE:
        default:
            warnx("cc110x_txrx (%i): not implemented", c);
    }

    DEBUG("cc110x_txrx\n");
    return native_cc110x_ssp0dr;
}

/**
 * disables GDO0 interrupt
 */
void cc110x_gdo0_enable(void)
{
    /* this would be for rising/high edge if this was proper hardware */
    native_cc110x_gd0_enabled = 1;
    DEBUG("cc110x_gdo0_enable\n");
    return;
}


/**
 * enables GDO0 interrupt
 */
void cc110x_gdo0_disable(void)
{
    native_cc110x_gd0_enabled = 0;
    DEBUG("cc110x_gdo0_disable\n");
    return;
}

/**
 * enables GDO2 interrupt
 */
void cc110x_gdo2_enable(void)
{
    /* this would be for falling/low edge if this was proper hardware */
    native_cc110x_gd2_enabled = 1;
    DEBUG("cc110x_gdo2_enable\n");
    return;
}

/**
 * disables GDO2 interrupt
 */
void cc110x_gdo2_disable(void)
{
    native_cc110x_gd2_enabled = 0;
    DEBUG("cc110x_gdo2_disable\n");
    return;
}

/**
 * enable interrupts for GDO0 and GDO2
 */
void cc110x_init_interrupts(void)
{
    /* this would be for low edge in both cases if this was proper hardware */
    cc110x_gdo2_enable();
    cc110x_gdo0_enable();
    DEBUG("cc110x_init_interrupts\n");
    return;
}

void cc110x_before_send(void)
{
    cc110x_gdo2_disable();
    DEBUG("cc110x_before_send\n");
    return;
}
void cc110x_after_send(void)
{
    cc110x_gdo2_enable();
    DEBUG("cc110x_after_send\n");
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
    return native_cc110x_gd2;
}

void cc110x_spi_init(void)
{
    native_cc110x_enabled = 1; /* power on */
    DEBUG("cc110x_spi_init\n");
    return;
}

void cc110x_spi_cs(void)
{
    DEBUG("cc110x_spi_cs\n");
    return;
}
void cc110x_spi_select(void)
{
    DEBUG("cc110x_spi_select\n");
    return;
}
void cc110x_spi_unselect(void)
{
    DEBUG("cc110x_spi_unselect\n");
    return;
}

/* ng */
