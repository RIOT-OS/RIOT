/*
 * Copyright(C) 2017 Francois Berder <fberder@outlook.fr>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

#include "assert.h"
#include "board.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#define SPIxCON(U)          ((U).regs[0x00 / 4])
#define SPIxCONCLR(U)       ((U).regs[0x04 / 4])
#define SPIxCONSET(U)       ((U).regs[0x08 / 4])
#define SPIxSTAT(U)         ((U).regs[0x10 / 4])
#define SPIxSTATCLR(U)      ((U).regs[0x14 / 4])
#define SPIxBUF(U)          ((U).regs[0x20 / 4])
#define SPIxBRG(U)          ((U).regs[0x30 / 4])
#define SPIxCON2(U)         ((U).regs[0x40 / 4])
#define SPI_REGS_SPACING    (_SPI2_BASE_ADDRESS - _SPI1_BASE_ADDRESS)

/* PERIPHERAL_CLOCK must be defined in board file */


typedef struct PIC32_SPI_tag {
    volatile uint32_t   *regs;
} PIC32_SPI_T;

static PIC32_SPI_T pic_spi[SPI_NUMOF + 1];
static mutex_t locks[SPI_NUMOF + 1];

void spi_init(spi_t bus)
{
    /*
     * PIC32 SPI Bus numbering starts at 1 but generic periph init will
     * initialise from 0 to SPI_NUMOF-1
     *
     * Ignore during initialisation but assert on other calls
     */
    if (bus == 0) return;

    assert(bus <= SPI_NUMOF);

    mutex_init(&locks[bus]);

    PMD5SET = _PMD5_SPI1MD_MASK << (bus - 1);
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    assert((bus != 0 )&& (bus <= SPI_NUMOF));

    gpio_init(spi_config[bus].mosi_pin, GPIO_OUT);
    gpio_init(spi_config[bus].miso_pin, GPIO_IN);
    *(spi_config[bus].mosi_reg) = spi_config[bus].mosi_af;
    *(spi_config[bus].miso_reg) = spi_config[bus].miso_af;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    (void)cs;
    volatile int rdata __attribute__((unused));

    assert((bus != 0) && (bus <= SPI_NUMOF));

    pic_spi[bus].regs =
        (volatile uint32_t *)(_SPI1_BASE_ADDRESS + (bus - 1) * SPI_REGS_SPACING);

    mutex_lock(&locks[bus]);

    PMD5CLR = _PMD5_SPI1MD_MASK << (bus - 1);

    SPIxCON(pic_spi[bus]) = 0;
    SPIxCON2(pic_spi[bus]) = 0;

    /* Clear receive FIFO */
    rdata = SPIxBUF(pic_spi[bus]);

    switch (mode) {
        case SPI_MODE_0:
            SPIxCONCLR(pic_spi[bus]) = _SPI1CON_CKP_MASK;
            SPIxCONSET(pic_spi[bus]) = _SPI1CON_CKE_MASK;
            break;
        case SPI_MODE_1:
            SPIxCONCLR(pic_spi[bus]) = (_SPI1CON_CKP_MASK | _SPI1CON_CKE_MASK);
            break;
        case SPI_MODE_2:
            SPIxCONSET(pic_spi[bus]) = (_SPI1CON_CKP_MASK | _SPI1CON_CKE_MASK);
            break;
        case SPI_MODE_3:
            SPIxCONCLR(pic_spi[bus]) = _SPI1CON_CKE_MASK;
            SPIxCONSET(pic_spi[bus]) = _SPI1CON_CKP_MASK;
            break;
        default:
            return SPI_NOMODE;
    }

    SPIxBRG(pic_spi[bus]) = (PERIPHERAL_CLOCK / (2 * clk)) - 1;
    SPIxSTATCLR(pic_spi[bus]) = _SPI1STAT_SPIROV_MASK;
    SPIxCONSET(pic_spi[bus]) = (_SPI1CON_ON_MASK | _SPI1CON_MSTEN_MASK);

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    assert((bus != 0) && (bus <= SPI_NUMOF));

    /* SPI module must be turned off before powering it down */
    SPIxCON(pic_spi[bus]) = 0;

    PMD5SET = _PMD5_SPI1MD_MASK << (bus - 1);

    mutex_unlock(&locks[bus]);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    const uint8_t *out_buffer = (const uint8_t*)out;
    uint8_t *in_buffer = (uint8_t*)in;

    assert((bus != 0) && (bus <= SPI_NUMOF));

    if (cs != SPI_CS_UNDEF)
        gpio_clear((gpio_t)cs);

    while (len--) {
        uint8_t rdata;

        if (out_buffer) {
            SPIxBUF(pic_spi[bus]) = *out_buffer++;

            /* Wait until TX FIFO is empty */
            while (!(SPIxSTAT(pic_spi[bus]) & _SPI1STAT_SPITBE_MASK)) {}
        }

        /* Wait until RX FIFO is not empty */
        while (!(SPIxSTAT(pic_spi[bus]) & _SPI1STAT_SPIRBF_MASK)) {}

        rdata = SPIxBUF(pic_spi[bus]);

        if (in_buffer)
            *in_buffer++ = rdata;
    }

    if (!cont && cs != SPI_CS_UNDEF)
        gpio_set((gpio_t)cs);
}
