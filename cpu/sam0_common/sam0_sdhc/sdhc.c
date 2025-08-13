/*
 * Copyright (C) 2018 Alkgrove
 *
 *  C file for SD card host controller driver
 *  This is a complete rewrite of the SD Card code from ASF4
 *  Copyright © Alkgrove 04/29/2018
 *
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this list of
 *    conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its contributors may be
 *    used to endorse or promote products derived from this software without specific prior written
 *    permission.
 *
 * @par THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @ingroup     cpu_sam0_common_sdhc
 * @{
 *
 * @file
 * @brief       Driver for the SD Host Controller
 *
 * @author      alkgrove <bob@alkgrove.com>
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 *
 * @}
 */

#include <errno.h>
#include <limits.h>
#include <stdalign.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "busy_wait.h"
#include "periph_cpu.h"
#include "periph/pm.h"
#include "macros/math.h"
#include "vendor/sd_mmc_protocol.h"
#include "sdhc.h"
#include "time_units.h"
#include "ztimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#ifndef SDHC_CLOCK
#define SDHC_CLOCK          SAM0_GCLK_MAIN
#endif

#ifndef SDHC_CLOCK_SLOW
#define SDHC_CLOCK_SLOW     SAM0_GCLK_TIMER
#endif

#ifndef SDHC_ENABLE_HS
#define SDHC_ENABLE_HS      1
#endif

/**
 * @brief   The board can overwrite this if only a single SDHC instance is used
 *          to save 80 Bytes of ROM.
 */
#ifndef SDHC_DEV
#ifdef SDHC1
#define SDHC_DEV            state->dev
#else
#define SDHC_DEV            SDHC0
#endif
#endif

/**
 * @brief   Monitor card insertion and removal
 */
#define NISTR_CARD_DETECT   (SDHC_NISTR_CREM | SDHC_NISTR_CINS)

static sdhc_state_t *isr_ctx_0;
static sdhc_state_t *isr_ctx_1;

static void _set_speed(sdhc_state_t *state, uint32_t fsdhc);
static void _set_hc(sdhc_state_t *state);
static bool _test_voltage(sdhc_state_t *state, bool f8);
static bool _test_capacity(sdhc_state_t *state);
static bool _test_version(sdhc_state_t *state);
static bool _wait_not_busy(sdhc_state_t *state);
static bool _test_bus_width(sdhc_state_t *state);
static bool _test_high_speed(sdhc_state_t *state);
static bool _init_transfer(sdhc_state_t *state, uint32_t cmd, uint32_t arg, uint16_t block_size,
                          uint16_t num_blocks);
static bool sdio_test_type(sdhc_state_t *state);

static bool _card_detect(sdhc_state_t *state)
{
    return state->dev->PSR.reg & SDHC_PSR_CARDINS;
}

static inline void _clock_sdcard(sdhc_state_t *state, bool on)
{
    (void)state;

    if (on) {
        SDHC_DEV->CCR.reg |= SDHC_CCR_SDCLKEN;
    }
    else {
        SDHC_DEV->CCR.reg &= ~SDHC_CCR_SDCLKEN;
    }
}

static bool _check_mask(uint32_t val, uint32_t mask)
{
    return (val & mask) == mask;
}

static void _delay(unsigned us)
{
    if (IS_USED(MODULE_ZTIMER_USEC)) {
        ztimer_sleep(ZTIMER_USEC, us);
    } else if (IS_USED(MODULE_ZTIMER_MSEC)) {
        ztimer_sleep(ZTIMER_MSEC, 1);
    } else {
        busy_wait_us(us);
    }
}

/**
 * @brief   Reset the entire SDHC peripheral or a part of it
 *
 * @param   state   SDHC device context
 * @param   type    Reset type
 *                  [SDHC_SRR_SWRSTALL | SDHC_SRR_SWRSTCMD | SDHC_SRR_SWRSTDAT]
 */
static void _reset_sdhc(sdhc_state_t *state, uint8_t type)
{
    SDHC_DEV->SRR.reg = type;
    while (SDHC_DEV->SRR.reg & type) {}

    if (type == SDHC_SRR_SWRSTALL) {
        /* trigger card_init */
        state->need_init = true;
        state->error = 0;
    }
}

/**
 * @brief   Wait for a given event while checking for errors
 *
 * @param   state       SDHC device context
 * @param   event       Event to wait for [SDHC_NISTR_*]
 * @param   error_mask  Mask of errors to be checked [SDHC_EISTR_*]
 * @param   reset       Reset type in case of errors
 *                      [SDHC_SRR_SWRSTALL | SDHC_SRR_SWRSTCMD | SDHC_SRR_SWRSTDAT]
 *
 * @return true if event occurred or false on error
 */
static bool _wait_for_event(sdhc_state_t *state,
                            uint16_t event, uint16_t error_mask,
                            uint8_t reset)
{
    /* wait for the event given by event mask */
    do {
        /* check for any error in error mask */
        if (SDHC_DEV->EISTR.reg & error_mask) {
            state->error = SDHC_DEV->EISTR.reg;
            SDHC_DEV->EISTR.reg = SDHC_EISTR_MASK;
            if (IS_USED(ENABLE_DEBUG)) {
                DEBUG("sdhc error: %x, ", state->error);
                switch (reset) {
                case SDHC_SRR_SWRSTCMD:
                    DEBUG("reset CMD\n");
                    break;
                case SDHC_SRR_SWRSTDAT:
                    DEBUG("reset DAT\n");
                    break;
                case SDHC_SRR_SWRSTALL:
                    DEBUG("reset ALL\n");
                    break;
                default:
                    assert(false);
                    break;
                }
            }
            _reset_sdhc(state, reset);
            return false;
        }
    } while (!(SDHC_DEV->NISTR.reg & event));

    /* clear the event */
    SDHC_DEV->NISTR.reg = event;

    return true;
}

static void _init_clocks(sdhc_state_t *state)
{
    assert((SDHC_DEV == SDHC0)
#ifdef SDHC1
        || (SDHC_DEV == SDHC1)
#endif
          );

    sam0_gclk_enable(SDHC_CLOCK);
    sam0_gclk_enable(SDHC_CLOCK_SLOW);

    gpio_init_mux(state->cd, SAM0_SDHC_MUX);
    if (gpio_is_valid(state->wp)) {
        gpio_init_mux(state->wp, SAM0_SDHC_MUX);
    }

    if (SDHC_DEV == SDHC0) {
        /* data pins are fixed */
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT0, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT1, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT2, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDDAT3, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDCMD, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC0_PIN_SDCK, SAM0_SDHC_MUX);

        GCLK->PCHCTRL[SDHC0_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                                         | GCLK_PCHCTRL_GEN(SDHC_CLOCK);
        GCLK->PCHCTRL[SDHC0_GCLK_ID_SLOW].reg = GCLK_PCHCTRL_CHEN
                                              | GCLK_PCHCTRL_GEN(SDHC_CLOCK_SLOW);
        MCLK->AHBMASK.reg |= MCLK_AHBMASK_SDHC0;
        isr_ctx_0 = state;
        NVIC_EnableIRQ(SDHC0_IRQn);
    }

#ifdef SDHC1
    if (SDHC_DEV == SDHC1) {
        /* data pins are fixed */
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT0, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT1, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT2, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDDAT3, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDCMD, SAM0_SDHC_MUX);
        gpio_init_mux(SAM0_SDHC1_PIN_SDCK, SAM0_SDHC_MUX);

        GCLK->PCHCTRL[SDHC1_GCLK_ID].reg = GCLK_PCHCTRL_CHEN
                                         | GCLK_PCHCTRL_GEN(SDHC_CLOCK);
        GCLK->PCHCTRL[SDHC1_GCLK_ID_SLOW].reg = GCLK_PCHCTRL_CHEN
                                              | GCLK_PCHCTRL_GEN(SDHC_CLOCK_SLOW);
        MCLK->AHBMASK.reg |= MCLK_AHBMASK_SDHC1;
        isr_ctx_1 = state;
        NVIC_EnableIRQ(SDHC1_IRQn);
    }
#endif
}

int sdhc_init(sdhc_state_t *state)
{
    bool f8;
    uint32_t response;
    int res = 0;

    /* set the initial clock slow, single bit and normal speed */
    state->type = CARD_TYPE_SD;
    state->version = CARD_VER_UNKNOWN;
    state->rca = 0;
    state->bus_width = 1;
    state->high_speed = false;
    state->clock = SDHC_SLOW_CLOCK_HZ;
    mutex_init(&state->lock);
    const mutex_t _init_locked = MUTEX_INIT_LOCKED;
    state->sync = _init_locked;

    _init_clocks(state);
    _reset_sdhc(state, SDHC_SRR_SWRSTALL);

    SDHC_DEV->NISIER.reg = NISTR_CARD_DETECT;

    SDHC_DEV->TCR.reg = 14;                     /* max timeout is 14 or about 1sec */
    SDHC_DEV->PCR.reg = SDHC_PCR_SDBPWR | SDHC_PCR_SDBVSEL_3V3;
    SDHC_DEV->NISTER.reg = SDHC_NISTER_MASK;    /* clear all normal interrupt bits */
    SDHC_DEV->EISTER.reg = SDHC_EISTER_MASK;    /* clear all error interrupt bits  */

    _set_hc(state);
    /* 74 startup clocks (190us) */
    _delay(190);

    /* reset the SD card to idle state CMD0 */
    f8 = false;
    for (int i = 0; i < 2; i++) { /* we do this step twice before failing */
        if (!sdhc_send_cmd(state, SDMMC_MCI_CMD0_GO_IDLE_STATE, 0)) {
            if (i == 1) {
                res = -EIO;
                goto out;
            }
        }
        /* Test for SD version 2 */
        if (!sdhc_send_cmd(state, SD_CMD8_SEND_IF_COND, SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
            if (i == 1) {
                /* bad card */
                res = -EIO;
                goto out;
            }
        }
        else {
            response = SDHC_DEV->RR[0].reg;
            /* good response but no compliance R7 Value, legacy card */
            if (response == 0xFFFFFFFF) {
                f8 = false;
                break;
            }
            if (_check_mask(response, SD_CMD8_PATTERN | SD_CMD8_HIGH_VOLTAGE)) {
                f8 = true;
                break;
            }
        }
    }
    if (!sdio_test_type(state)) {
        res = -EIO;
        goto out;
    }
    if (state->type & CARD_TYPE_SDIO) {
        res = -ENOTSUP;
        goto out;
    }
    /* Try to get the SD card's operating condition */
    if (!_test_voltage(state, f8)) {
        state->type = CARD_TYPE_UNKNOWN;
        res = -EIO;
        goto out;
    }
    /* SD MEMORY, Put the Card in Identify Mode
     * Note: The CID is not used in this stack */
    if (!sdhc_send_cmd(state, SDMMC_CMD2_ALL_SEND_CID, 0)) {
        res = -EIO;
        goto out;
    }
    /* Ask the card to publish a new relative address (RCA).*/
    if (!sdhc_send_cmd(state, SD_CMD3_SEND_RELATIVE_ADDR, 0)) {
        res = -EIO;
        goto out;
    }
    state->rca = (uint16_t)(SDHC_DEV->RR[0].reg >> 16);
    /* SD MEMORY, Get the Card-Specific Data */
    if (!_test_capacity(state)) {
        res = -EIO;
        goto out;
    }
    /* Put it into Transfer Mode */
    if (!sdhc_send_cmd(state, SDMMC_CMD7_SELECT_CARD_CMD, (uint32_t)state->rca << 16)) {
        res = -EIO;
        goto out;
    }
    /* SD MEMORY, Read the SCR to get card version */
    if (!_test_version(state)) {
        res = -EIO;
        goto out;
    }
    if (!_test_bus_width(state)) {
        res = -EIO;
        goto out;
    }

    /* all SD Cards should support this clock at that point */
    state->clock = SDHC_FAST_CLOCK_HZ;

    /* update the host controller to the detected changes in bus_width and clock */
    _set_hc(state);

    /* if it is high speed capable, (well it is) */
    if (IS_USED(SDHC_ENABLE_HS) && (SDHC_DEV->CA0R.reg & SDHC_CA0R_HSSUP)) {
        if (!_test_high_speed(state)) {
            res = -EIO;
            goto out;
        }
    }

    /* update host controller */
    _set_hc(state);

    if (!sdhc_send_cmd(state, SDMMC_CMD16_SET_BLOCKLEN, SD_MMC_BLOCK_SIZE)) {
        res = -EIO;
        goto out;
    }

    state->need_init = false;

out:
    _clock_sdcard(state, 0);
    return res;
}

bool sdhc_send_cmd(sdhc_state_t *state, uint32_t cmd, uint32_t arg)
{
    uint32_t timeout = 0xFFFFFFFF;
    uint32_t command;
    uint32_t eis;

    /* wait if card is busy */
    while (SDHC_DEV->PSR.reg & (SDHC_PSR_CMDINHC | SDHC_PSR_CMDINHD)) {}

    SDHC_DEV->TMR.reg = 0;  /* only single transfer */
    SDHC_DEV->BCR.reg = 0;  /* block count is zero */

    command = SDHC_CR_CMDIDX(cmd) | SDHC_CR_CMDTYP_NORMAL;
    if (cmd & MCI_RESP_PRESENT) {
        if (cmd & MCI_RESP_136) {
            command |= SDHC_CR_RESPTYP_136_BIT;
        }
        else if (cmd & MCI_RESP_BUSY) {
            command |= SDHC_CR_RESPTYP_48_BIT_BUSY;
        }
        else {
            command |= SDHC_CR_RESPTYP_48_BIT;
        }
    }

    if (cmd & MCI_CMD_OPENDRAIN) {
        SDHC_DEV->MC1R.reg |= SDHC_MC1R_OPD;
    }
    else {
        SDHC_DEV->MC1R.reg &= ~SDHC_MC1R_OPD;
    }

    eis = (cmd & MCI_RESP_CRC)
        ? SDHC_EISTR_CMDTEO | SDHC_EISTR_CMDEND | SDHC_EISTR_CMDIDX | SDHC_EISTR_DATTEO |
          SDHC_EISTR_DATEND | SDHC_EISTR_ADMA
        : SDHC_EISTR_CMDTEO | SDHC_EISTR_CMDEND | SDHC_EISTR_CMDIDX | SDHC_EISTR_DATTEO |
          SDHC_EISTR_DATEND | SDHC_EISTR_ADMA | SDHC_EISTR_CMDCRC | SDHC_EISTR_DATCRC;

    SDHC_DEV->ARG1R.reg = arg;    /* setup the argument register */
    SDHC_DEV->CR.reg = command;   /* send command */

    if (!_wait_for_event(state, SDHC_NISTR_CMDC, eis, SDHC_SRR_SWRSTCMD)) {
        return false;
    }

    if (cmd & MCI_RESP_BUSY) {
        do {
            if (--timeout == 0) {
                SDHC_DEV->SRR.reg = SDHC_SRR_SWRSTCMD; /* reset command */
                while (SDHC_DEV->SRR.reg & SDHC_SRR_SWRSTCMD) {}
                return false;
            }
        } while (!(SDHC_DEV->PSR.reg & SDHC_PSR_DATLL(1))); /* DAT[0] is busy bit */
    }

    return true;
}

static void _set_speed(sdhc_state_t *state, uint32_t fsdhc)
{
    (void)state;

    if (SDHC_DEV->CCR.reg & SDHC_CCR_SDCLKEN) {
        /* wait for command/data to go inactive */
        while (SDHC_DEV->PSR.reg & (SDHC_PSR_CMDINHC | SDHC_PSR_CMDINHD)) {}
        /* disable the clock */
        SDHC_DEV->CCR.reg = 0;
    }

    uint32_t div = DIV_ROUND_UP(sam0_gclk_freq(SDHC_CLOCK), fsdhc) - 1;

    DEBUG("sdhc: switch to %lu Hz (div %lu) -> %lu Hz\n",
          fsdhc, div, sam0_gclk_freq(SDHC_CLOCK) / (div + 1));

    /* write the 10 bit clock divider */
    SDHC_DEV->CCR.reg = SDHC_CCR_SDCLKFSEL(div) | SDHC_CCR_USDCLKFSEL(div >> 8)
                      | SDHC_CCR_CLKGSEL | SDHC_CCR_INTCLKEN;
    while (!(SDHC_DEV->CCR.reg & SDHC_CCR_INTCLKS)) {}  /* wait for clock to be stable */
    SDHC_DEV->CCR.reg |= SDHC_CCR_SDCLKEN;   /* enable clock to card        */
}

/**
 * _set_hc selects either one or four bit mode, low/high speed and clock
 *
 * bitwidth is SDHC_HC1R_DW_1BIT_Val or SDHC_HC1R_DW_4BIT_Val
 * speed is SDHC_HC1R_HSEN_NORMAL_Val or SDHC_HC1R_HSEN_HIGH_Val
 */
static void _set_hc(sdhc_state_t *state)
{
    if (state->high_speed) {
        SDHC_DEV->HC1R.reg |= SDHC_HC1R_HSEN;
    }
    else {
        SDHC_DEV->HC1R.reg &= ~SDHC_HC1R_HSEN;
    }
    if (!(SDHC_DEV->HC2R.reg & SDHC_HC2R_PVALEN)) {  /* PVALEN is probably always low */
        _set_speed(state, state->clock);
    }
    if (state->bus_width == 4) {
        /* set four bit mode */
        SDHC_DEV->HC1R.reg |= SDHC_HC1R_DW;
    }
    else {
        /* set one bit mode */
        SDHC_DEV->HC1R.reg &= ~SDHC_HC1R_DW;
    }
}

/**
 * @brief Ask to all cards to send their operations conditions (MCI only).
 * - ACMD41 sends operation condition command.
 * - ACMD41 reads OCR
 *
 * @param state pointer to sdhc
 *
 * @return true if success, otherwise false
 */
static bool _test_voltage(sdhc_state_t *state, bool f8)
{
    uint32_t arg;
    uint32_t retry = 2100;
    uint32_t response;

    /*
     * Timeout 1s = 400KHz / ((6+6+6+6)*8) cycles = 2100 retry
     * 6 = cmd byte size
     * 6 = response byte size
     * 6 = cmd byte size
     * 6 = response byte size
     */
    retry = 2100;
    do {
        /* CMD55 - Indicate to the card that the next command is an
         * application specific command rather than a standard command.*/
        if (!sdhc_send_cmd(state, SDMMC_CMD55_APP_CMD, 0)) {
            return false;
        }

        /* (ACMD41) Sends host OCR register */
        arg = OCR_VDD_27_28 | OCR_VDD_28_29 | OCR_VDD_29_30 | OCR_VDD_30_31 |
              OCR_VDD_31_32 | OCR_VDD_32_33;
        if (f8) { /* if not legacy card */
            arg |= SD_ACMD41_HCS;
        }

        /* Check response */
        if (!sdhc_send_cmd(state, SD_MCI_ACMD41_SD_SEND_OP_COND, arg)) {
            return false;
        }
        response = SDHC_DEV->RR[0].reg;
        if (response & OCR_POWER_UP_BUSY) {
            /* Card is ready */
            if ((response & OCR_CCS) != 0) {
                state->type |= CARD_TYPE_HC;
            }
            break;
        }
    } while (retry--);

    return retry;
}

/**
 * \brief CMD9: Addressed card sends its card-specific
 * data (CSD) on the CMD line mci.
 *
 * \return true if success, otherwise false
 */
static bool _test_capacity(sdhc_state_t *state)
{
    alignas(uint32_t)
    uint8_t csd[CSD_REG_BSIZE];

    if (!sdhc_send_cmd(state, SDMMC_MCI_CMD9_SEND_CSD, (uint32_t)state->rca << 16)) {
        return false;
    }
    for (int i = 0; i < 4; i++) {
        uint32_t *csd32 = (void *)csd;
        csd32[i] = __builtin_bswap32(SDHC_DEV->RR[3 - i].reg);
    }

    /*
     * Card Capacity.
     * ----------------------------------------------------
     * For normal SD/MMC card:
     * sector size is ((device size + 1) * (1 << (device size multiplier + 2)) * (1 << max_read_data_block_length))/512
     * we can rearrange this like this
     * sector size is ((device size + 1) * (1 << (device size multiplier +  max_read_data_block_length - 7))
     * device_size = SD_CSD_1_0_C_SIZE(&csd[1])
     * device_size_multiplier = SD_CSD_1_0_C_SIZE_MULT(&csd[1])
     * max_read_data_block_length = SD_CSD_1_0_READ_BL_LEN(&csd[1])
     * Number of sectors is bytes/512
     * ----------------------------------------------------
     * For high capacity SD card:
     * memory capacity = (C_SIZE+1) * 512K byte
     * So number of sectors is ((C_SIZE+1) * 512 * 1024) / 512
     * or 1024 * (C_SIZE+1)
     */
    if (CSD_STRUCTURE_VERSION(&csd[1]) >= SD_CSD_VER_2_0) {
        state->sectors = (SD_CSD_2_0_C_SIZE(&csd[1]) + 1) * 1024;
    }
    else {
        state->sectors = (SD_CSD_1_0_C_SIZE(&csd[1]) + 1)
                       * (1 << (SD_CSD_1_0_C_SIZE_MULT(&csd[1]) + SD_CSD_1_0_READ_BL_LEN(&csd[1]) - 7));
    }

    return true;
}

/**
 * @brief ACMD51 - Read the SD Configuration Register.
 *
 * @note
 * SD Card Configuration Register (SCR) provides information on the SD Memory
 * Card's special features that were configured into the given card. The size
 * of SCR register is 64 bits.
 *
 *
 * @return true if success, otherwise false
 */
static bool _test_version(sdhc_state_t *state)
{
    uint8_t scr[SD_SCR_REG_BSIZE];
    uint32_t *p = (void *)scr;

    /* CMD55 - Indicate to the card that the next command is an
     * application specific command rather than a standard command.*/
    if (!sdhc_send_cmd(state, SDMMC_CMD55_APP_CMD, (uint32_t)state->rca << 16)) {
        return false;
    }

    if (!_init_transfer(state, SD_ACMD51_SEND_SCR, 0, SD_SCR_REG_BSIZE, 1)) {
        return false;
    }

    /* wait until buffer read ready */
    if (!_wait_for_event(state, SDHC_NISTR_BRDRDY,
                         SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                         SDHC_SRR_SWRSTALL)) {
        return false;
    }

    for (int words = 0; words < (SD_SCR_REG_BSIZE / 4); words++) {
        *p++ = SDHC_DEV->BDPR.reg;
    }

    /* wait until transfer is complete */
    if (!_wait_for_event(state, SDHC_NISTR_TRFC,
                         SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                         SDHC_SRR_SWRSTALL)) {
        return false;
    }

    /* Get SD Memory Card - Spec. Version */
    switch (SD_SCR_SD_SPEC(scr)) {
    case SD_SCR_SD_SPEC_1_0_01:
        state->version = CARD_VER_SD_1_0;
        break;

    case SD_SCR_SD_SPEC_1_10:
        state->version = CARD_VER_SD_1_10;
        break;

    case SD_SCR_SD_SPEC_2_00:
        if (SD_SCR_SD_SPEC3(scr) == SD_SCR_SD_SPEC_3_00) {
            state->version = CARD_VER_SD_3_0;
        }
        else {
            state->version = CARD_VER_SD_2_0;
        }
        break;

    default:
        state->version = CARD_VER_SD_1_0;
        break;
    }

    return true;
}

static bool _init_transfer(sdhc_state_t *state, uint32_t cmd, uint32_t arg, uint16_t block_size,
                           uint16_t num_blocks)
{
    uint32_t tmr;
    uint32_t command;
    uint32_t eis;
    uint32_t timeout = 0xFFFFFFFF;

    /* wait if card is busy */
    while (SDHC_DEV->PSR.reg & (SDHC_PSR_CMDINHC | SDHC_PSR_CMDINHD)) {}

    if (cmd & MCI_CMD_WRITE) {
        tmr = SDHC_TMR_DTDSEL_WRITE;
    }
    else {
        tmr = SDHC_TMR_DTDSEL_READ;
    }

    if (cmd & MCI_CMD_SDIO_BYTE) {
        tmr |= SDHC_TMR_MSBSEL_SINGLE;
    }
    else if (cmd & MCI_CMD_SDIO_BLOCK) {
        tmr |= SDHC_TMR_BCEN | SDHC_TMR_MSBSEL_MULTIPLE;
    }
    else if (cmd & MCI_CMD_SINGLE_BLOCK) {
        tmr |= SDHC_TMR_MSBSEL_SINGLE;
    }
    else if (cmd & MCI_CMD_MULTI_BLOCK) {
        tmr |= SDHC_TMR_BCEN | SDHC_TMR_MSBSEL_MULTIPLE;
        if (tmr & SDHC_TMR_DTDSEL_WRITE) {
            tmr |= SDHC_TMR_ACMDEN_CMD23;
        } else {
            tmr |= SDHC_TMR_ACMDEN_CMD12;
        }
    }
    else {
        return false;
    }

    SDHC_DEV->TMR.reg = tmr;
    SDHC_DEV->BSR.reg = SDHC_BSR_BLOCKSIZE(block_size) | SDHC_BSR_BOUNDARY_4K;
    SDHC_DEV->BCR.reg = SDHC_BCR_BCNT(num_blocks);

    command = SDHC_CR_DPSEL_DATA;
    command |= SDHC_CR_CMDIDX(cmd) | SDHC_CR_CMDTYP_NORMAL;
    if (cmd & MCI_RESP_PRESENT) {
        if (cmd & MCI_RESP_136) {
            command |= SDHC_CR_RESPTYP_136_BIT;
        }
        else if (cmd & MCI_RESP_BUSY) {
            command |= SDHC_CR_RESPTYP_48_BIT_BUSY;
        }
        else {
            command |= SDHC_CR_RESPTYP_48_BIT;
        }
    }
    if (cmd & MCI_CMD_OPENDRAIN) {
        SDHC_DEV->MC1R.reg |= SDHC_MC1R_OPD;
    }
    else {
        SDHC_DEV->MC1R.reg &= ~SDHC_MC1R_OPD;
    }

    eis = SDHC_EISTR_CMDTEO | SDHC_EISTR_CMDEND | SDHC_EISTR_CMDIDX | SDHC_EISTR_DATTEO
        | SDHC_EISTR_DATEND | SDHC_EISTR_ADMA | SDHC_EISTR_ACMD;
    if ((cmd & MCI_RESP_CRC) == 0) {
        eis |= (SDHC_EISTR_CMDCRC | SDHC_EISTR_DATCRC);
    }

    DEBUG("sdhc: send cmd %lx\n", command);

    SDHC_DEV->SSAR.reg = num_blocks;  /* Setup block size for Auto CMD23 */
    SDHC_DEV->ARG1R.reg = arg;        /* setup the argument register */
    SDHC_DEV->CR.reg = command;       /* send command */

    if (!_wait_for_event(state, SDHC_NISTR_CMDC, eis, SDHC_SRR_SWRSTCMD)) {
        return false;
    }

    if (cmd & MCI_RESP_BUSY) {
        do {
            if (--timeout == 0) {
                SDHC_DEV->SRR.reg = SDHC_SRR_SWRSTCMD; /* reset command */
                while (SDHC_DEV->SRR.reg & SDHC_SRR_SWRSTCMD) {}
                return false;
            }
        } while (!(SDHC_DEV->PSR.reg & SDHC_PSR_DATLL(1))); /* DAT[0] is busy bit */
    }

    return true;
}

/**
 *  @brief Start a read blocks transfer on the line
 *
 *  dst must be on 4 byte boundary
 */
int sdhc_read_blocks(sdhc_state_t *state, uint32_t address, void *dst, uint16_t num_blocks)
{
    uint32_t cmd;
    uint32_t arg;
    uint32_t *p = dst;
    int res = 0;

    /* card detect should be done differently
     * card detect with interrupt and if removed and reinstalled
     * set need_init, sdhc_init clears need_init
     */
    if (!_card_detect(state)) {
        return -ENODEV;
    }

    mutex_lock(&state->lock);
    _clock_sdcard(state, 1);

    if (state->need_init) {
        res = sdhc_init(state);
        if (res != 0) {
            goto out;
        }
    }

    if (state->type & CARD_TYPE_HC) {
        arg = address;
    }
    else {
        arg = address * SD_MMC_BLOCK_SIZE;
    }

    cmd = (1 == num_blocks)
        ? SDMMC_CMD17_READ_SINGLE_BLOCK
        : SDMMC_CMD18_READ_MULTIPLE_BLOCK;

    if (!_wait_not_busy(state)) {
        res = -EBUSY;
        goto out;
    }

    if (!_init_transfer(state, cmd, arg, SD_MMC_BLOCK_SIZE, num_blocks)) {
        res = -EIO;
        goto out;
    }

    if (SDHC_DEV->RR[0].reg & CARD_STATUS_ERR_RD_WR) {
        res = -EIO;
        goto out;
    }

    /* wait until buffer read ready */
    if (!_wait_for_event(state, SDHC_NISTR_BRDRDY,
                         SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                         SDHC_SRR_SWRSTALL)) {
        res = -EIO;
        goto out;
    }

    int num_words = (num_blocks * SD_MMC_BLOCK_SIZE) / 4;
    for (int words = 0; words < num_words; words++) {
        while (!(SDHC_DEV->PSR.reg & SDHC_PSR_BUFRDEN)) {}
        *p++ = SDHC_DEV->BDPR.reg;
    }

    /* wait until transfer is complete */
    if (!_wait_for_event(state, SDHC_NISTR_TRFC,
                         SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                         SDHC_SRR_SWRSTALL)) {
        res = -EIO;
        goto out;
    }

out:
    _clock_sdcard(state, 0);
    mutex_unlock(&state->lock);
    return res;
}

/**
 * @brief   Start a write blocks transfer on the line
 * @note    The driver will use the DMA available to speed up the transfer.
 *          (There is no evidence of that.)
 * @pre     @p src must be on 4 byte boundary
 */
int sdhc_write_blocks(sdhc_state_t *state, uint32_t address, const void *src,
                      uint16_t num_blocks)
{
    assert(((uintptr_t)src & 3) == 0);
    uint32_t cmd;
    uint32_t arg;
    const uint32_t *p = src;
    int res = 0;

    if (!_card_detect(state)) {
        return -ENODEV;
    }

    mutex_lock(&state->lock);
    _clock_sdcard(state, 1);

    if (state->need_init) {
        res = sdhc_init(state);
        if (res != 0) {
            goto out;
        }
    }

    /*
     * SDSC Card (CCS=0) uses byte unit address,
     * SDHC and SDXC Cards (CCS=1) use block unit address (512 Bytes unit).
     */
    if (state->type & CARD_TYPE_HC) {
        arg = address;
    }
    else {
        arg = address * SD_MMC_BLOCK_SIZE;
    }

    cmd = (1 == num_blocks) ? SDMMC_CMD24_WRITE_BLOCK : SDMMC_CMD25_WRITE_MULTIPLE_BLOCK;

    if (!_wait_not_busy(state)) {
        res = -EBUSY;
        goto out;
    }

    if (!_init_transfer(state, cmd, arg, SD_MMC_BLOCK_SIZE, num_blocks)) {
        res = -EIO;
        goto out;
    }

    if (SDHC_DEV->RR[0].reg & CARD_STATUS_ERR_RD_WR) {
        res = -EIO;
        goto out;
    }

    /* wait until buffer write ready */
    if (!_wait_for_event(state, SDHC_NISTR_BWRRDY,
                         SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                         SDHC_SRR_SWRSTALL)) {
        res = -EIO;
        goto out;
    }

    /* Write data */
    int num_words = (num_blocks * SD_MMC_BLOCK_SIZE) / 4;
    for (int words = 0; words < num_words; words++) {
        while (!(SDHC_DEV->PSR.reg & SDHC_PSR_BUFWREN)) {}
        SDHC_DEV->BDPR.reg = *p++;
    }

    /* wait until transfer is complete */
    if (!_wait_for_event(state, SDHC_NISTR_TRFC,
                         SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                         SDHC_SRR_SWRSTALL)) {
        res = -EIO;
        goto out;
    }

out:
    _wait_not_busy(state);
    _clock_sdcard(state, 0);
    mutex_unlock(&state->lock);
    return res;
}

int sdhc_erase_blocks(sdhc_state_t *state, uint32_t start, uint16_t num_blocks)
{
    uint32_t end = start + num_blocks - 1;
    int res = 0;

    if (!_card_detect(state)) {
        return -ENODEV;
    }

    mutex_lock(&state->lock);
    _clock_sdcard(state, 1);

    if (state->need_init) {
        res = sdhc_init(state);
        if (res != 0) {
            goto out;
        }
    }

    /*
     * SDSC Card (CCS=0) uses byte unit address,
     * SDHC and SDXC Cards (CCS=1) use block unit address (512 Bytes unit).
     */
    if (!(state->type & CARD_TYPE_HC)) {
        start *= SD_MMC_BLOCK_SIZE;
        end   *= SD_MMC_BLOCK_SIZE;
    }

    if (!_wait_not_busy(state)) {
        res = -EBUSY;
        goto out;
    }

    sdhc_send_cmd(state, SD_CMD32_ERASE_WR_BLK_START, start);
    sdhc_send_cmd(state, SD_CMD33_ERASE_WR_BLK_END, end);
    if (!sdhc_send_cmd(state, SDMMC_CMD38_ERASE, 0)) {
        res = -EIO;
    }

out:
    _wait_not_busy(state);
    _clock_sdcard(state, 0);
    mutex_unlock(&state->lock);
    return res;
}

/**
 * @brief CMD13 - Addressed card sends its status register.
 * This function waits the clear of the busy flag
 *
 * @return true if success, otherwise false
 */
static bool _wait_not_busy(sdhc_state_t *state)
{
    uint32_t timeout;

    /* Wait for data ready status.
     * Nec timing: 0 to unlimited
     * However a timeout is used.
     * 200 000 * 8 cycles
     */
    timeout = 200000;
    do {
        if (!sdhc_send_cmd(state, SDMMC_MCI_CMD13_SEND_STATUS, (uint32_t)state->rca << 16)) {
            return false;
        }
        /* Check busy flag */
        if (SDHC_DEV->RR[0].reg & CARD_STATUS_READY_FOR_DATA) {
            break;
        }
        if (timeout-- == 0) {
            return false;
        }
    } while (1);

    return true;
}

/**
 * @brief Try to get the SDIO card's operating condition
 * - CMD5 to read OCR NF field
 * - CMD5 to wait OCR power up busy
 * - CMD5 to read OCR MP field
 *   sd_mmc_card->type is updated
 *
 * @return true if success, otherwise false
 */
static bool sdio_test_type(sdhc_state_t *state)
{
    uint32_t response;
    uint32_t cmd5_retry = 5000;

    /* CMD5 - SDIO send operation condition (OCR) command. */
    if (!sdhc_send_cmd(state, SDIO_CMD5_SEND_OP_COND, 0)) {
        return true; /* No error but card type not updated */
    }
    response = SDHC_DEV->RR[0].reg;
    if ((response & OCR_SDIO_NF) == 0) {
        return true; /* No error but card type not updated */
    }

    /*
     * Wait card ready
     * Timeout 1s = 400KHz / ((6+4)*8) cycles = 5000 retry
     * 6 = cmd byte size
     * 4(SPI) 6(MCI) = response byte size
     */
    while (1) {
        response &= OCR_VDD_27_28 | OCR_VDD_28_29 | OCR_VDD_29_30
                  | OCR_VDD_30_31 | OCR_VDD_31_32 | OCR_VDD_32_33;
        /* CMD5 - SDIO send operation condition (OCR) command.*/
        if (!sdhc_send_cmd(state, SDIO_CMD5_SEND_OP_COND, response)) {
            return false;
        }
        response = SDHC_DEV->RR[0].reg;
        if ((response & OCR_POWER_UP_BUSY) == OCR_POWER_UP_BUSY) {
            break;
        }
        if (cmd5_retry-- == 0) {
            return false;
        }
    }
    /* Update card type at the end of busy */
    if ((response & OCR_SDIO_MP) > 0) {
        state->type = CARD_TYPE_SD_COMBO;
    }
    else {
        state->type = CARD_TYPE_SDIO;
    }
    return true;
}

static bool _test_high_speed(sdhc_state_t *state)
{
    alignas(uint32_t)
    uint8_t switch_status[SD_SW_STATUS_BSIZE] = { 0 };
    uint32_t *p = (void *)switch_status;

    if ((state->type & CARD_TYPE_SD) && (state->version > CARD_VER_SD_1_0)) {

        if (!_init_transfer(state, SD_CMD6_SWITCH_FUNC,
                            SD_CMD6_MODE_SWITCH       | SD_CMD6_GRP6_NO_INFLUENCE |
                            SD_CMD6_GRP5_NO_INFLUENCE | SD_CMD6_GRP4_NO_INFLUENCE |
                            SD_CMD6_GRP3_NO_INFLUENCE | SD_CMD6_GRP2_DEFAULT      |
                            SD_CMD6_GRP1_HIGH_SPEED, SD_SW_STATUS_BSIZE, 1)) {

            return false;
        }

        /* wait until buffer read ready */
        if (!_wait_for_event(state, SDHC_NISTR_BRDRDY,
                             SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                             SDHC_SRR_SWRSTALL)) {
            return false;
        }

        for (int words = 0; words < (SD_SW_STATUS_BSIZE / 4); words++) {
            *p++ = SDHC_DEV->BDPR.reg;
        }

        /* wait until transfer is complete */
        if (!_wait_for_event(state, SDHC_NISTR_TRFC,
                             SDHC_EISTR_DATTEO | SDHC_EISTR_DATCRC | SDHC_EISTR_DATEND,
                             SDHC_SRR_SWRSTALL)) {
            return false;
        }

        if (SDHC_DEV->RR[0].reg & CARD_STATUS_SWITCH_ERROR) {
            return false;
        }

        if (SD_SW_STATUS_FUN_GRP1_RC(switch_status) == SD_SW_STATUS_FUN_GRP_RC_ERROR) {
            /* No supported, it is not a protocol error */
            return true;
        }

        if (SD_SW_STATUS_FUN_GRP1_BUSY(switch_status)) {
            return false;
        }

        /* CMD6 function switching period is within 8 clocks
         * after the end bit of status data.*/
        _delay(100);
    }
    state->high_speed = true;
    state->clock *= 2; /* turbo clock */
    return true;
}

static bool _test_bus_width(sdhc_state_t *state)
{
    /**
     * A SD memory card always supports bus 4bit
     * A SD COMBO card always supports bus 4bit
     * A SDIO Full-Speed alone always supports 4bit
     * A SDIO Low-Speed alone can supports 4bit (Optional)
     */
    if (state->type & CARD_TYPE_SD) {
        /* CMD55 - Indicate to the card that the next command is an
         * application specific command rather than a standard command.*/
        if (!sdhc_send_cmd(state, SDMMC_CMD55_APP_CMD, (uint32_t)state->rca << 16)) {
            return false;
        }
        if (!sdhc_send_cmd(state, SD_ACMD6_SET_BUS_WIDTH, SD_ACMD6_4B)) {
            return false;
        }
    }
    state->bus_width = 4;
    return true;
}

static void _isr(sdhc_state_t *state)
{
    /* if card got inserted we need to re-init */
    if (SDHC_DEV->NISTR.reg & NISTR_CARD_DETECT) {
        SDHC_DEV->NISTR.reg = NISTR_CARD_DETECT;
        DEBUG_PUTS("card presence changed");
        state->need_init = true;
    }

    cortexm_isr_end();
}

#ifdef SDHC_DEV_ISR
void SDHC_DEV_ISR(void)
{
    if (SDHC_DEV == SDHC0) {
        _isr(isr_ctx_0);
    }
    if (SDHC_DEV == SDHC1) {
        _isr(isr_ctx_1);
    }
}
#else
void isr_sdhc0(void)
{
    _isr(isr_ctx_0);
}

void isr_sdhc1(void)
{
    _isr(isr_ctx_1);
}
#endif
