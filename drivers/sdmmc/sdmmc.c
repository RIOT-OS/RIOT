/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_sdmmc
 * @{
 *
 * @file
 * @brief       SDIO/SD/MMC device API (SDMMC)
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "architecture.h"
#include "assert.h"
#include "bitarithm.h"
#include "byteorder.h"
#include "log.h"
#include "periph_cpu.h"
#include "time_units.h"
#include "ztimer.h"

#include "sdmmc/sdmmc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* millisecond timer definitions dependent on active ztimer backend */
#if IS_USED(MODULE_ZTIMER_MSEC)
#define _ZTIMER_ACQUIRE()       ztimer_acquire(ZTIMER_MSEC)
#define _ZTIMER_RELEASE()       ztimer_release(ZTIMER_MSEC)
#define _ZTIMER_NOW()           ztimer_now(ZTIMER_MSEC)
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_MSEC, n)
#elif IS_USED(MODULE_ZTIMER_USEC)
#define _ZTIMER_ACQUIRE()       ztimer_acquire(ZTIMER_USEC)
#define _ZTIMER_RELEASE()       ztimer_release(ZTIMER_USEC)
#define _ZTIMER_NOW()           ztimer_now(ZTIMER_USEC) / US_PER_MS
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_USEC, n * US_PER_MS)
#else
#error "Either module ztimer_msec or ztimer_usec is needed"
#endif

/**
 * @brief   SDIO/SD/MMC Card initialization timeout in msec
 *
 * Card initialization shall be completed within 1 second from the first
 * ACMD41.
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 4.2.3
 *      [[sdcard.org](https://www.sdcard.org)]
 */
#define SDMMC_INIT_TIMEOUT_MS   (1000)

/**
 * @brief   SD/MMC Timeout Conditions
 *
 * @see Physical Layer Simplified Specification Version 9.00,
 *      Section 4.6.2 Read, Write and Erase Timeout Conditions
 *      [[sdcard.org](https://www.sdcard.org)]
 */
#define SDMMC_DATA_R_TIMEOUT_MS (100)   /**< Read timeout should be 100 ms */
#define SDMMC_DATA_W_TIMEOUT_MS (500)   /**< Write timeout should be 250 ms for,
                                             SDSC and 500 ms for SDHC, SDXC */
#define SDMMC_ERASE_TIMEOUT_MS  (250)   /**< Erase timeout per block 250 ms */

/**
 * @brief   Time to wait for a SDIO/SD/MMC card to become ready for a data transfer
 *
 * When writing the data in a block write transfer, the card uses the
 * READY_FOR_DATA bit to indicate when it is ready to receive more data.
 * The host must wait until the card is ready before making the next data
 * transfer. The maximum wait time is the maximum write time.
 *
 * Physical Layer Simplified Specification Version 9.00,
 * 4.6.2 Read, Write and Erase Timeout Conditions
 */
#define SDMMC_WAIT_FOR_CARD_READY_MS 250

/* bus widths as used in ACMD (power of 2) */
#define SDMMC_ACMD6_BUS_WIDTH_1BIT  0
#define SDMMC_ACMD6_BUS_WIDTH_4BIT  2
#define SDMMC_ACMD6_BUS_WIDTH_8BIT  3

#define SDMMC_ACMD41_S18R       (SDMMC_OCR_S18A)
#define SDMMC_ACMD41_HCS        (SDMMC_OCR_CCS)
#define SDMMC_ACMD41_POWER_UP   (SDMMC_OCR_POWER_UP)
#define SDMMC_ACMD41_VOLTAGES   (SDMMC_OCR_ALL_VOLTAGES)

#define SDMMC_R3_S18A           (SDMMC_OCR_S18A)
#define SDMMC_R3_CCS            (SDMMC_OCR_CCS)
#define SDMMC_R3_POWER_UP       (SDMMC_OCR_POWER_UP)

#define SDMMC_CMD6_ACCESS   (24)    /* CMD6 arg bit shift for EXT_CSD access mode */
#define SDMMC_CMD6_INDEX    (16)    /* CMD6 arg bit shift for EXT_CSD byte index */
#define SDMMC_CMD6_VALUE    (8)     /* CMD6 arg bit shift for EXT_CSD value */
#define SDMMC_CMD6_CMD_SET  (0)     /* CMD6 arg bit shift for EXT_CSD command set */

/**
 * @brief   Extended CSD Bus Widths (MMC only)
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.4, Table 55
 *      [[jedec.org](https://www.jedec.org)]
 */
enum {
    SDMMC_EXT_CSD_BUS_WIDTH_1BIT = 0,   /**< Bus with 1-bit */
    SDMMC_EXT_CSD_BUS_WIDTH_4BIT = 1,   /**< Bus with 4-bit */
    SDMMC_EXT_CSD_BUS_WIDTH_8BIT = 2,   /**< Bus with 8-bit */
};

/**
 * @brief   Extended CSD High Speed Interface Timing (MMC only)
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.4, Table 46
 *      [[jedec.org](https://www.jedec.org)]
 */
enum {
    SDMMC_EXT_CSD_HS_TIMING_BACK = 0,   /**< Backward compatible timing */
    SDMMC_EXT_CSD_HS_TIMING_HS = 1,     /**< High speed */
    SDMMC_EXT_CSD_HS_TIMING_HS200 = 2,  /**< HS200 */
    SDMMC_EXT_CSD_HS_TIMING_HS400 = 3,  /**< HS400 */
};

/**
 * @brief   Extended CSD Card Type (MMC only)
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.4, Table 50
 *      [[jedec.org](https://www.jedec.org)]
 */
enum {
    SDMMC_EXT_CSD_CARD_TYPE_HS26 = 0x01,    /**< Highspeed MMC @ 26MHz */
    SDMMC_EXT_CSD_CARD_TYPE_HS52 = 0x02,    /**< Highspeed MMC @ 52MHz */
};

/**
 * @name    Indices in Extended CSD (MMC only)
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 8.4, Table 46
 *      [[jedec.org](https://www.jedec.org)]
 * @{
 */
#define SDMMC_EXT_CSD_BUS_WIDTH     (183)   /**< Index of Bus width mode */
#define SDMMC_EXT_CSD_HS_TIMING     (185)   /**< High Speed Interface Timing */
#define SDMMC_EXT_CSD_CSD_STRUCTURE (194)   /**< Card Type */
#define SDMMC_EXT_CSD_CARD_TYPE     (196)   /**< CSD Structure Version */
#define SDMMC_EXT_CSD_SEC_COUNT     (212)   /**< Card Type */
/** @} */

/**
 * @brief   EXT_CSD access mode (MMC only)
 *
 * @see JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *      Standard, High Capacity (MMCA, 4.2), Section 7.4.1, Table 2
 *      [[jedec.org](https://www.jedec.org)]
 */
typedef enum {
    SDMMC_EXT_CSD_COMMAND_SET = 0b00,   /**< command set changed */
    SDMMC_EXT_CSD_SET_BITS    = 0b01,   /**< bits in pointed byte in EXT_CSD are set */
    SDMMC_EXT_CSD_CLEAR_BITS  = 0b01,   /**< bits in pointed byte in EXT_CSD are cleared */
    SDMMC_EXT_CSD_WRITE_BYTE  = 0b11,   /**< value is written to pointed byte */
} sdmmc_ext_csd_access_t;

#define SDMMC_CCC_SUPPORT_CMD6          (1 << 10)   /** CMD6 support flag in CSD.CCC */

#define SDMMC_CMD6_MODE_CHECK           (0 << 31)
#define SDMMC_CMD6_MODE_SWITCH          (1 << 31)

#define SDMMC_CMD6_FUNC_GROUP1_SHIFT    (0)         /**< Group 1 in argument */
#define SDMMC_CMD6_FUNC_GROUP1_MASK     (0x0f)      /**< No influence */
#define SDMMC_CMD6_FUNC_GROUP1_DS       (0x00)      /**< Default Speed / SDR12 */
#define SDMMC_CMD6_FUNC_GROUP1_SDR12    (0x00)      /**< Default Speed / SDR12 */
#define SDMMC_CMD6_FUNC_GROUP1_HS       (0x01)      /**< High Speed / SDR25 */
#define SDMMC_CMD6_FUNC_GROUP1_SDR25    (0x01)      /**< High Speed / SDR25 */
#define SDMMC_CMD6_FUNC_GROUP1_SDR50    (0x02)      /**< SDR50 */
#define SDMMC_CMD6_FUNC_GROUP1_SDR104   (0x03)      /**< SDR104 */
#define SDMMC_CMD6_FUNC_GROUP1_DDR50    (0x04)      /**< DDR50 */

#define SDMMC_CMD6_FUNC_GROUP2_SHIFT    (4)         /**< Group 2 in argument */
#define SDMMC_CMD6_FUNC_GROUP2_MASK     (0x0f)      /**< No influence */
#define SDMMC_CMD6_FUNC_GROUP2_DEFAULT  (0x00)      /**< Default */
#define SDMMC_CMD6_FUNC_GROUP2_EC       (0x01)      /**< For eC */
#define SDMMC_CMD6_FUNC_GROUP2_OTP      (0x03)      /**< OTP */
#define SDMMC_CMD6_FUNC_GROUP2_ASSD     (0x04)      /**< ASSD */
#define SDMMC_CMD6_FUNC_GROUP2_VENDOR   (0x0e)      /**< Vendor specific */

#define SDMMC_CMD6_FUNC_GROUP3_SHIFT    (8)         /**< Group 3 in argument */
#define SDMMC_CMD6_FUNC_GROUP3_MASK     (0x0f)      /**< No influence */
#define SDMMC_CMD6_FUNC_GROUP4_SHIFT    (12)        /**< Group 4 in argument */
#define SDMMC_CMD6_FUNC_GROUP4_MASK     (0x0f)      /**< No influence */
#define SDMMC_CMD6_FUNC_GROUP5_SHIFT    (16)        /**< Group 5 in argument */
#define SDMMC_CMD6_FUNC_GROUP5_MASK     (0x0f)      /**< No influence */
#define SDMMC_CMD6_FUNC_GROUP6_SHIFT    (20)        /**< Group 6 in argument */
#define SDMMC_CMD6_FUNC_GROUP6_MASK     (0x0f)      /**< No influence */

#define SDMMC_CMD6_SW_STATUS_SIZE       (64)        /**< Switch Status Size */

/**
 * @brief   Switch Function Status
 *
 * @note Since the structure is only used to directly map its members to the
 *       switch state buffer returned by the card, the members are given in
 *       big-endian order with word-wise host byte order.
 *
 * @see Physical Layer Simplified Specification Version 9.00, Section 4.3.10.4,
 *      Table 4-13 [[sdcard.org](https://www.sdcard.org)]
 */
typedef struct __attribute__((packed)) {
    /* word 15, byte 60...63 */
    uint32_t group6_supported:16;   /**< Supported Functions in Group 6 [495:480] */
    uint32_t max_power:16;          /**< Maximum Current/Power Consumption [511:496] */
    /* word 14, byte 56...59 */
    uint32_t group4_supported:16;   /**< Supported Functions in Group 4 [463:448] */
    uint32_t group5_supported:16;   /**< Supported Functions in Group 5 [479:464] */
    /* word 13, byte 52...55 */
    uint32_t group2_supported:16;   /**< Supported Functions in Group 2 [431:416] */
    uint32_t group3_supported:16;   /**< Supported Functions in Group 3 [447:432] */
    /* word 12, byte 48...51 */
    uint32_t group3_selected:4;     /**< Selected Function in Group 3 [387:384] */
    uint32_t group4_selected:4;     /**< Selected Function in Group 4 [391:388] */
    uint32_t group5_selected:4;     /**< Selected Function in Group 5 [395:392] */
    uint32_t group6_selected:4;     /**< Selected Function in Group 6 [399:396] */
    uint32_t group1_supported:16;   /**< Supported Functions in Group 1 [415:400] */
    /* word 11, byte 44...47 */
    uint32_t group6_busy:16;        /**< Busy Status of Group 6 Functions [367:352] */
    uint32_t data_struct_v:8;       /**< Data Structure Version [375:368] */
    uint32_t group1_selected:4;     /**< Selected Function in Group 1 [379:376] */
    uint32_t group2_selected:4;     /**< Selected Function in Group 2 [383:380] */
    /* word 10, byte 41...43 */
    uint32_t group4_busy:16;        /**< Busy Status of Group 4 Functions [335:320] */
    uint32_t group5_busy:16;        /**< Busy Status of Group 5 Functions [351:336] */
    /* word 9, byte 36...40 */
    uint32_t group2_busy:16;        /**< Busy Status of Group 2 Functions [303:288] */
    uint32_t group3_busy:16;        /**< Busy Status of Group 3 Functions [319:304] */
    /* word 8, byte 32...35 */
    uint32_t reserved1:16;          /**< reserved [271:256] */
    uint32_t group1_busy:16;        /**< Busy Status of Group 1 Functions [287:272] */
    /* word 0...7, byte 0...31 */
    uint8_t reserved[32];           /**< reserved [255:0] */
} sdmmc_sw_status_t;

/* Definition of the XFA for the SDIO/SD/MMC device descriptor references */
XFA_INIT_CONST(sdmmc_dev_t * const, sdmmc_devs);

/* forward declaration of internal functions */
static int _send_cmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                     sdmmc_resp_t resp_type, uint32_t *resp);
static int _send_acmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                      sdmmc_resp_t resp_type, uint32_t *resp);
static inline int _send_xcmd(sdmmc_dev_t *dev,
                             sdmmc_cmd_t cmd_idx, uint32_t arg,
                             sdmmc_resp_t resp_type, uint32_t *resp);
static int _xfer(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                 uint16_t block_size, uint16_t block_num,
                 const void *data_wr, void *data_rd, uint16_t *done);
static int _assert_card(sdmmc_dev_t *dev);
static int _select_deselect(sdmmc_dev_t *dev, bool select);
static int _get_status(sdmmc_dev_t *dev, sdmmc_card_status_t *cs);
static int _wait_for_ready(sdmmc_dev_t *dev, uint32_t timeout_ms);
static int _wait_while_prg(sdmmc_dev_t *dev,
                           uint32_t sleep_ms, uint32_t timeout_ms);
static int _read_cid(sdmmc_dev_t *dev, uint8_t cmd);
static int _read_csd(sdmmc_dev_t *dev);
static int _read_scr(sdmmc_dev_t *dev);
static inline int _enable_clock(sdmmc_dev_t *dev);
static inline int _disable_clock(sdmmc_dev_t *dev);

#if IS_USED(MODULE_SDMMC_MMC)
static int _read_ext_csd(sdmmc_dev_t *dev);
static int _write_ext_csd(sdmmc_dev_t *dev, uint8_t index, uint8_t value);
#endif

#if ENABLE_DEBUG
static uint8_t _crc_7(const uint8_t *data, unsigned n);
static void _print_raw_data_crc(const uint8_t *data, unsigned size, bool crc);
#endif

int sdmmc_send_cmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                   sdmmc_resp_t resp_type, uint32_t *resp)
{
    assert(dev);
    assert(dev->driver);

    int res = _assert_card(dev);
    if (res) {
        return res;
    }

    return _send_cmd(dev, cmd_idx, arg, resp_type, resp);
}

int sdmmc_send_acmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                    sdmmc_resp_t resp_type, uint32_t *resp)
{
    assert(dev);
    assert(dev->driver);
    assert(cmd_idx & SDMMC_ACMD_PREFIX);

    int res;
    uint32_t response;

    if (cmd_idx == SDMMC_ACMD41) {
        res = sdmmc_send_cmd(dev, SDMMC_CMD55,
                             SDMMC_CMD_NO_ARG,
                             SDMMC_R1, &response);
    }
    else {
        assert(dev->rca);
        res = sdmmc_send_cmd(dev, SDMMC_CMD55,
                             SDMMC_CMD_ARG_RCA(dev->rca),
                             SDMMC_R1, &response);
    }

    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        return res;
    }
    return sdmmc_send_cmd(dev, cmd_idx & ~SDMMC_ACMD_PREFIX, arg,
                          resp_type, resp);
}

int sdmmc_card_init(sdmmc_dev_t *dev)
{
    assert(dev);
    assert(dev->driver);

    dev->init_done = false;

    /* use driver's card_init function if it defines its own */
    if (dev->driver->card_init) {
        return dev->driver->card_init(dev);
    }

    /* set bus width to 1-bit and the peripheral clock to 400 kHz for init */
    dev->driver->set_bus_width(dev, SDMMC_BUS_WIDTH_1BIT);
    dev->driver->set_clock_rate(dev, SDMMC_CLK_400K);

    _ZTIMER_ACQUIRE();      /* ztimer is needed for different timeouts */

    uint32_t response[4];   /* long response requires four 32-bit words */
    int res;

    /* timeout handling */
    uint32_t t_start = _ZTIMER_NOW();

    _enable_clock(dev);

    DEBUG("[sdmmc] send CMD0 to put all cards into 'idle' state\n");
    /* TODO: SPI mode uses CMD0 with R1 */
    res = _send_cmd(dev, SDMMC_CMD0, SDMMC_CMD_NO_ARG, SDMMC_NO_R, NULL);
    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        goto out;
    }

    /* a short delay is required here */
    _ZTIMER_SLEEP_MS(1);

    /* reset RCA and card type */
    dev->rca = 0;
    dev->type = SDMMC_CARD_TYPE_UNKNOWN;

    /* Card identification process */

    DEBUG("[sdmmc] send CMD8 to test for a SD/SDIO card\n");
    res = _send_cmd(dev, SDMMC_CMD8, SDMMC_CMD8_CHECK, SDMMC_R7, response);

    /* SD Card should now be in `idle` state */

    /* TODO: SPI mode uses CMD59 with CRC_ON_OFF (mandatory) */

    /*
     * If command CMD8 succeeds, the card is either
     * - a SD Card compliant to Version 2.00 or later, or
     * - a Combo card compliant to Version 2.00 or later.
     * Use
     * - CMD5 with Voltage Window 0 to determine whether it is a Combo Card
     * - ACMD41 with HCS=1 to determine the capacity.
     *
     * If the card didn't respond to CMD8 (-ETIMEDOUT), the card is either
     * - a SD Card Version 2.00 or later SD Card but the voltage mismatches, or
     * - a SD Card Version 1.x, or
     * - a MMC, or
     * - a SDIO card
     * Use
     * - CMD5 with Voltage Window 0 to determine whether it is a SDIO Card
     * - ACMD41 with HCS=0 to check
     *
     * If the card answers with invalid response, the card is unusable and
     * the card initialization and identification process is stopped.
     */

    if (res && (res != -ETIMEDOUT)) {
        /* card answers with invalid response, card is unusable */
        LOG_ERROR("[sdmmc] command failed with error %d, card unusable\n", res);
        res = -ENODEV;
        goto out;
    }

    if ((res == 0) && ((response[0] & SDMMC_CMD8_CHECK) != SDMMC_CMD8_CHECK)) {
        /* card answers but received R7 doesn't match, card unusable */
        LOG_ERROR("[sdmmc] R7 mismatch %08"PRIx32"!=%08"PRIx32", "
                  "card unusable\n",
                  (uint32_t)SDMMC_CMD8_CHECK, response[0] & SDMMC_CMD8_CHECK);
        res = -ENODEV;
        goto out;
    }

    bool flag_f8 = (res != -ETIMEDOUT);

    /* check whether it is a SDIO or Combo card using CMD5 with
     * Voltage Window = 0 to test */
    DEBUG("[sdmmc] send CMD5 to test for a SDIO or Combo card\n");

    /* If CMD5 succeeds, it is a SDIO or Combo Card. */
    if (_send_cmd(dev, SDMMC_CMD5, SDMMC_CMD_NO_ARG, SDMMC_R4, response) == 0) {
        dev->type = SDMMC_CARD_TYPE_SDIO;
        /* If CMD8 previously failed (flag_f8 = 0), it is a SDIO only card */
        if (!IS_USED(MODULE_SDMMC_SDIO) && (res == 0) && !flag_f8) {
            /* TODO: SDIO support */
            LOG_ERROR("[sdmmc] card is a SDIO card, not supported yet\n");
            res = -ENOTSUP;
            goto out;
        }
    }

    /* use ACMD41 with Voltage Window = 0 to test for a SD Card/Combo Card */
    res = _send_acmd(dev, SDMMC_ACMD41, SDMMC_CMD_NO_ARG,
                     SDMMC_R3, response);

    uint8_t cmd;
    uint32_t arg;

    /* Note: for use in SPI mode
     * CMD58 has to be used here in SPI mode to determine the CCS flag */

    if (res == 0) {
        /* if SD Card/Combo Card, then use ACMD41 with Voltage Window and
         * HCS=1 if flag_f8 is true or HCS=0 otherwise */
        cmd = SDMMC_ACMD41;
        arg = SDMMC_ACMD41_VOLTAGES | (flag_f8 ? SDMMC_ACMD41_HCS : 0);
    }
    else {
        /* otherwise try CMD1 with Voltage Window for MMC */
        cmd = SDMMC_CMD1;
        arg = SDMMC_OCR_ALL_VOLTAGES | SDMMC_OCR_18V | SDMMC_OCR_CCS;
    }

    /* the card is either SD card, Combo card or MMC */
    do {
        /* According to the Physical Layer Simplified Specification
         * Version 9.00 [[sdcard.org](https://www.sdcard.org)] section 4.4
         * "Clock Control", the polling interval for ACMD41 must be less than
         * 50 ms, if the clock is stopped by the host and not continuous
         * during ACMD41 polling until the card becomes ready. */
        _ZTIMER_SLEEP_MS(10);
        DEBUG("[sdmmc] send %s %s to test whether card is SDSC/SDXC or MMC card\n",
              cmd == SDMMC_ACMD41 ? "ACMD41" : "CMD1",
              arg & SDMMC_ACMD41_HCS ? "HCS=1" : "HCS=0");
        res = _send_xcmd(dev, cmd, arg, SDMMC_R3, response);
        if (res == -ETIMEDOUT) {
            LOG_ERROR("[sdmmc] no response, card not present or not a SD Card\n");
            goto out;
        }
        else if (res) {
            DEBUG("[sdmmc] command failed with error %d\n", res);
            res = -EIO;
            goto out;
        }
    } while (((response[0] & SDMMC_R3_POWER_UP) == 0) &&
             ((_ZTIMER_NOW() - t_start) < SDMMC_INIT_TIMEOUT_MS));

    if ((_ZTIMER_NOW() - t_start) >= SDMMC_INIT_TIMEOUT_MS) {
        /* if timed out, it is neither SD Card/Combo Card nor MMC card */
        LOG_ERROR("[sdmmc] Card could not be detected\n");
        res = -ENODEV;
        goto out;
    }

    /* On success the card can operate at 2.7-3.6V VDD here */

    if (cmd == SDMMC_ACMD41) {
        /* card is either SD Card or Combo card
         * TODO: since SDIO is not yet supported, the type is simply
         * overwritten, otherwise the type would have to be ORed here  */
        if (!flag_f8) {
            /* SDSC Card Version 1.x if it did not respond to CMD8 */
            dev->type = SDMMC_CARD_TYPE_SDSC_V1;
        }
        else if (response[0] & SDMMC_R3_CCS) {
            /* SDHC/SDXC Card card if did react on CMD8 and set CCS */
            dev->type = SDMMC_CARD_TYPE_SDHC_SDXC;
        }
        else {
            /* SDSC Card Version 2.x or later if it did respond to CMD8 but
             * didn't set CCS */
            dev->type = SDMMC_CARD_TYPE_SDSC_V2_V3;
        }
    }
    else {
        /* card is a MultimediaCard */
        dev->type = SDMMC_CARD_TYPE_MMC;
        /* TODO: MMC support */
        if (!IS_USED(MODULE_SDMMC_MMC)) {
            LOG_ERROR("[sdmmc] MultimediaCard detected, not supported\n");
            res = -ENOTSUP;
            goto out;
        }
    }

    /* SD Card or all MMC cards are now in `ready` state */

    /* Card identification using CMD2 and CMD3 is done once per peripheral */
    DEBUG("[sdmmc] read CID to put the card in 'ident' state\n");

    /* read CID and bring the card into the `ident` state */
    res = _read_cid(dev, SDMMC_CMD2);
    if (res) {
        /* no card found */
        goto out;
    }

    /* SD Card or MMC card is now in `ident` state */

    if (IS_USED(MODULE_SDMMC_MMC) && (dev->type == SDMMC_CARD_TYPE_MMC)) {
        DEBUG("[sdmmc] send CMD3 to set RCA\n");
        /* for MMCs, RCA is selected and assigned by host using the device address */
        dev->rca = ((uint32_t)dev & 0x0000ffff);
        res = _send_cmd(dev, SDMMC_CMD3, SDMMC_CMD_ARG_RCA(dev->rca),
                        SDMMC_R1, response);
    }
    else {
        DEBUG("[sdmmc] send CMD3 to get RCA\n");
        /* for SD cards, the card selects RCA and sends it back in R6 */
        res = _send_cmd(dev, SDMMC_CMD3, SDMMC_CMD_NO_ARG, SDMMC_R6, response);
        dev->rca = response[0] >> 16;
    }
    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        goto out;
    }

    /* SD Card or MMC is now in `stby` state */

    DEBUG("[sdmmc] read CSD\n");
    res = _read_csd(dev);
    if (res) {
        goto out;
    }

    /* select the card to put it in the `tran` state for further programming */
    res = _select_deselect(dev, true);
    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        goto out;
    }

    /* SD Memory Card or MMC card is now selected and in `tran` state */

    if (dev->type != SDMMC_CARD_TYPE_MMC) {
        res = _read_scr(dev);
        if (res) {
            goto out;
        }

        DEBUG("[sdmmc] send CMD16 to set the block length to %d byte\n",
              SDMMC_SDHC_BLOCK_SIZE);
        res = _send_cmd(dev, SDMMC_CMD16, SDMMC_SDHC_BLOCK_SIZE,
                        SDMMC_R1, response);
        if (res) {
            DEBUG("[sdmmc] command failed with error %d\n", res);
            goto out;
        }

        /* change the bus width if the configured bus width is not 1-bit */
        if (dev->bus_width != SDMMC_BUS_WIDTH_1BIT) {
            /* ensure that the sdmmc_bus_width_t is not changed accientially */
            static_assert(SDMMC_BUS_WIDTH_1BIT == 1, "SDMMC_BUS_WIDTH_1BIT != 1");
            static_assert(SDMMC_BUS_WIDTH_4BIT == 4, "SDMMC_BUS_WIDTH_4BIT != 4");
            static_assert(SDMMC_BUS_WIDTH_8BIT == 8, "SDMMC_BUS_WIDTH_8BIT != 8");

            /* use the minimum of configured and supported bus width,
             * supported bus widths are given as ORed integer numbers. */
            sdmmc_bus_width_t width = (dev->bus_width < dev->scr.SD_BUS_WIDTHS)
                                    ? dev->bus_width
                                    : bitarithm_msb(dev->scr.SD_BUS_WIDTHS);
            DEBUG("[sdmmc] send ACMD6 to set %d-bit bus width\n", width);
            res = _send_acmd(dev, SDMMC_ACMD6,
                             width == SDMMC_BUS_WIDTH_4BIT ? SDMMC_ACMD6_BUS_WIDTH_4BIT
                                                           : SDMMC_ACMD6_BUS_WIDTH_8BIT,
                             SDMMC_R1, response);
            if (res) {
                DEBUG("[sdmmc] command failed with error %d\n", res);
                goto out;
            }

            dev->driver->set_bus_width(dev, width);
        }

        /* finally change the bus speed used by the peripheral to default speed */
        dev->driver->set_clock_rate(dev, SDMMC_CLK_25M);

        /* if the peripheral supports High Speed mode, try to switch into */
        if (IS_USED(MODULE_PERIPH_SDMMC_HS) && dev->scr.SD_SPEC) {
            static_assert(sizeof(sdmmc_sw_status_t) == SDMMC_CMD6_SW_STATUS_SIZE,
                          "sizeof(sdmmc_sw_status_t) != SDMMC_CMD6_SW_STATUS_SIZE");
            /* try to switch to high speed for SD Memory Cards v1.10 or higher */
            uint32_t arg = (SDMMC_CMD6_FUNC_GROUP1_HS << SDMMC_CMD6_FUNC_GROUP1_SHIFT) |
                           (SDMMC_CMD6_FUNC_GROUP2_DEFAULT << SDMMC_CMD6_FUNC_GROUP2_SHIFT) |
                           (SDMMC_CMD6_FUNC_GROUP3_MASK << SDMMC_CMD6_FUNC_GROUP3_SHIFT) |
                           (SDMMC_CMD6_FUNC_GROUP4_MASK << SDMMC_CMD6_FUNC_GROUP4_SHIFT) |
                           (SDMMC_CMD6_FUNC_GROUP5_MASK << SDMMC_CMD6_FUNC_GROUP5_SHIFT) |
                           (SDMMC_CMD6_FUNC_GROUP6_MASK << SDMMC_CMD6_FUNC_GROUP6_SHIFT);
            sdmmc_buf_t status[SDMMC_CMD6_SW_STATUS_SIZE];
            res = _xfer(dev, SDMMC_CMD6, SDMMC_CMD6_MODE_SWITCH | arg,
                        SDMMC_CMD6_SW_STATUS_SIZE, 1, NULL, status, NULL);
#if ENABLE_DEBUG
            _print_raw_data_crc(status, SDMMC_CMD6_SW_STATUS_SIZE, false);
#endif
            if (res) {
                goto out;
            }

            /* convert big endian to host byte order */
            uint32_t *raw_data = (void *)status;
            for (unsigned i = 0; i < (SDMMC_CMD6_SW_STATUS_SIZE >> 2); i++) {
                raw_data[i] = ntohl(raw_data[i]);
            }

            sdmmc_sw_status_t *sw_status = (void *)status;
            if (sw_status->group1_selected == SDMMC_CMD6_FUNC_GROUP1_HS) {
                /* switching the card to high speed was successful */
                dev->driver->set_clock_rate(dev, SDMMC_CLK_50M);
            }
        }
    }
    else {
        /* handle MMCs only if MMC support is enabled */
#if IS_USED(MODULE_SDMMC_MMC)
        /* read Extended CSD */
        res = _read_ext_csd(dev);
        if (res) {
            goto out;
        }

        /* change the bus width if the configured bus width is not 1-bit */
        if (dev->bus_width != SDMMC_BUS_WIDTH_1BIT) {
            /* TODO: the bus width for MMC would require testing the
             * functional pins with CMD19, for simplicity the configured
             * device bus width is used for the moment */
            sdmmc_bus_width_t width = dev->bus_width;
            uint8_t value = (width == SDMMC_BUS_WIDTH_4BIT ? SDMMC_EXT_CSD_BUS_WIDTH_4BIT
                                                           : SDMMC_EXT_CSD_BUS_WIDTH_8BIT);
            DEBUG("[sdmmc] set %d-bit bus width\n", dev->bus_width);
            res = _write_ext_csd(dev, SDMMC_EXT_CSD_BUS_WIDTH, value);
            if (res) {
                goto out;
            }
            dev->driver->set_bus_width(dev, dev->bus_width);
        }

        /* change HS_TIMING if the card supports high speed interface */
        if (IS_USED(MODULE_PERIPH_SDMMC_HS) &&
            (dev->csd.mmc.SPEC_VERS >= 4) &&
            (dev->ext_csd.CARD_TYPE & SDMMC_EXT_CSD_CARD_TYPE_HS52)) {
            DEBUG("[sdmmc] set high speed interface timing\n");
            res = _write_ext_csd(dev, SDMMC_EXT_CSD_HS_TIMING, SDMMC_EXT_CSD_HS_TIMING_HS);
            if (res) {
                goto out;
            }
            dev->driver->set_clock_rate(dev, SDMMC_CLK_52M);
        }
        else {
            dev->driver->set_clock_rate(dev, SDMMC_CLK_26M);
        }
#endif /* IS_USED(MODULE_SDMMC_MMC) */
    }

    dev->init_done = true;

out:
    /* release ztimer */
    _ZTIMER_RELEASE();
    _disable_clock(dev);

    return res;
}

int sdmmc_xfer(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
               uint16_t block_size, uint16_t block_num,
               const void *data_wr, void *data_rd, uint16_t *done)
{
    assert(dev);
    assert(dev->driver);

    if (done) {
        *done = 0;
    }

    int res = _assert_card(dev);
    if (res) {
        return res;
    }

    return _xfer(dev, cmd_idx, arg, block_size, block_num,
                 data_wr, data_rd, done);
}

int sdmmc_read_blocks(sdmmc_dev_t *dev,
                      uint32_t block_addr, uint16_t block_size,
                      uint16_t block_num, void *data, uint16_t *done)
{
    DEBUG("[sdmmc] %s dev=%p block_addr=%"PRIu32" block_size=%u block_num=%u done=%p\n",
          __func__, dev, block_addr, block_size, block_num, done);

    return sdmmc_xfer(dev, (block_num == 1) ? SDMMC_CMD17 : SDMMC_CMD18,
                      block_addr, block_size, block_num, NULL, data, done);
}

int sdmmc_write_blocks(sdmmc_dev_t *dev,
                       uint32_t block_addr, uint16_t block_size,
                       uint16_t block_num, const void *data, uint16_t *done)
{
    DEBUG("[sdmmc] %s dev=%p block_addr=%"PRIu32" block_size=%u block_num=%u done=%p\n",
          __func__, dev, block_addr, block_size, block_num, done);

    return sdmmc_xfer(dev, (block_num == 1) ? SDMMC_CMD24 : SDMMC_CMD25,
                      block_addr, block_size, block_num, data, NULL, done);
}

int sdmmc_erase_blocks(sdmmc_dev_t *dev,
                       uint32_t block_addr, uint16_t block_num)
{
    DEBUG("[sdmmc] %s dev=%p addr=%"PRIu32" num=%u\n",
          __func__, dev, block_addr, block_num);

    assert(dev);
    assert(dev->driver);
    assert(block_num);

    int res = _assert_card(dev);
    if (res) {
        return res;
    }

    if (IS_USED(MODULE_SDMMC_MMC) && (dev->type == SDMMC_CARD_TYPE_MMC)) {
        /* MMCs don't support the erase of a single block but only the erase of
         * a group of blocks. Blocks are erased implicitly on write. */
        DEBUG("[sdmmc] MMCs don't support the erase of single blocks\n");
        return -ENOTSUP;
    }

    uint32_t block_end = block_addr + block_num - 1;

    /* SDSC Cards use byte unit address */
    if ((dev->type == SDMMC_CARD_TYPE_SDSC_V1) ||
        (dev->type == SDMMC_CARD_TYPE_SDSC_V2_V3)) {
        block_addr *= SDMMC_SDHC_BLOCK_SIZE;
        block_end *= SDMMC_SDHC_BLOCK_SIZE;
    }

    if (_enable_clock(dev)) {
        return -EIO;
    }

    /* wait until the card is ready */
    if ((res = _wait_for_ready(dev, SDMMC_WAIT_FOR_CARD_READY_MS))) {
        /* reset the Card */
        dev->init_done = false;
        goto out;
    }

    uint32_t response;

    DEBUG("[sdmmc] send CMD32 and CMD33 to set erase parameters\n");
    if (((res = _send_cmd(dev, SDMMC_CMD32, block_addr, SDMMC_R1, &response)) != 0) ||
        ((res = _send_cmd(dev, SDMMC_CMD33, block_end, SDMMC_R1, &response)) != 0)) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        goto out;
    }

    DEBUG("[sdmmc] send CMD38 to execute erase\n");
    if ((res = _send_cmd(dev, SDMMC_CMD38, 0, SDMMC_R1B, &response))) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        goto out;
    }

    /* wait until the card left the programming state */
    if ((res = _wait_while_prg(dev, 1, block_num * SDMMC_ERASE_TIMEOUT_MS))) {
        goto out;
    }

out:
    if (_disable_clock(dev)) {
        return -EIO;
    }

    return 0;
}

uint64_t sdmmc_get_capacity(sdmmc_dev_t *dev)
{
    DEBUG("[sdmmc] %s dev=%p\n", __func__, dev);

    assert(dev);
    assert(dev->driver);

    int res = _assert_card(dev);
    if (res) {
        return res;
    }

    uint32_t block_len = 0;
    uint32_t block_nr = 0;

    if (dev->type == SDMMC_CARD_TYPE_MMC) {
#if IS_USED(MODULE_SDMMC_MMC)
        if (dev->csd.mmc.C_SIZE == 0xfff) {
            /* memory capacity = SEC_COUNT * 512 Byte */
            block_nr = dev->ext_csd.SEC_COUNT;
            block_len = SDMMC_SDHC_BLOCK_SIZE;
        }
        else {
            /* memory capacity = BLOCKNR * BLOCK_LEN
             *      MULT = 2^(C_SIZE_MULT+2)
             *      BLOCKNR = (C_SIZE+1) * MULT
             *      BLOCK_LEN = 2^READ_BL_LEN */
            block_nr = (dev->csd.mmc.C_SIZE + 1) *
                       (1 << (dev->csd.mmc.C_SIZE_MULT + 2));
            block_len = 1 << dev->csd.mmc.READ_BL_LEN;
        }
#endif
    }
    else if (dev->csd.v1.CSD_STRUCTURE == SDMMC_CSD_V1) {
        /* memory capacity = BLOCKNR * BLOCK_LEN
         *      MULT = 2^(C_SIZE_MULT+2)
         *      BLOCKNR = (C_SIZE+1) * MULT
         *      BLOCK_LEN = 2^READ_BL_LEN */
        block_nr = (dev->csd.v1.C_SIZE + 1) *
                   (1 << (dev->csd.v1.C_SIZE_MULT + 2));
        block_len = 1 << dev->csd.v1.READ_BL_LEN;
    }
    else if (dev->csd.v2.CSD_STRUCTURE == SDMMC_CSD_V2) {
        /* memory capacity = (C_SIZE+1) * 512 KByte */
        block_nr = dev->csd.v2.C_SIZE + 1;
        block_len = SDMMC_SDHC_BLOCK_SIZE << 10;
    }
    return (uint64_t)block_nr * block_len;
}

int sdmmc_read_sds(sdmmc_dev_t *dev, sdmmc_sd_status_t *sds)
{
    DEBUG("[sdmmc] %s dev=%p sds=%p\n", __func__, dev, sds);

    assert(dev);
    assert(sds);

    /* card must have a valid RCA */
    assert(dev->rca);

    int res = _assert_card(dev);
    if (res) {
        return res;
    }

    sdmmc_buf_t raw_data[SDMMC_SD_STATUS_SIZE];

    if (IS_USED(MODULE_SDMMC_MMC) && (dev->type == SDMMC_CARD_TYPE_MMC)) {
        LOG_ERROR("[sdmmc] MMC cards don't have a SD Status register\n");
        return -ENOTSUP;
    }

    DEBUG("[sdmmc] read SD status raw data");
    res = _xfer(dev, SDMMC_ACMD13, SDMMC_CMD_NO_ARG,
                SDMMC_SD_STATUS_SIZE, 1, NULL, raw_data, NULL);
#if ENABLE_DEBUG
    _print_raw_data_crc(raw_data, SDMMC_SD_STATUS_SIZE, false);
#endif

    if (res) {
        DEBUG("[sdmmc] reading SD status raw data failed");
        return res;
    }

    sds->DAT_BUS_WIDTH =  raw_data[0] >> 6;
    sds->SECURED_MODE = (raw_data[0] & (1 << 5)) >> 5;
    sds->SD_CARD_TYPE = (raw_data[2] << 8) | raw_data[3];
    sds->SIZE_OF_PROTECTED_AREA = (raw_data[4] << 24) |
                                  (raw_data[5] << 16) |
                                  (raw_data[6] << 8) | raw_data[7];
    sds->SPEED_CLASS = raw_data[8];
    sds->PERFORMANCE_MOVE = raw_data[9];
    sds->AU_SIZE = raw_data[10] >> 4;
    sds->ERASE_SIZE = (raw_data[11] << 8) | raw_data[12];
    sds->ERASE_TIMEOUT = raw_data[13] >> 2;
    sds->ERASE_OFFSET = raw_data[13] & 0x03;
    sds->UHS_SPEED_GRADE = raw_data[14] >> 4;
    sds->UHS_AU_SIZE = raw_data[14] & 0x0F;
    sds->VIDEO_SPEED_CLASS = raw_data[15];
    sds->VSC_AU_SIZE = ((raw_data[16] & 0x03) << 8) | raw_data[17];
    sds->SUS_ADDR = (raw_data[18] << 14) | (raw_data[19] << 6) | (raw_data[20] >> 2);

    return 0;
}

/* Internal functions */

static int _send_cmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                     sdmmc_resp_t resp_type, uint32_t *resp)
{
    DEBUG("[sdmmc] %s dev=%p cmd_idx=%u arg=0x%"PRIx32" resp_type=0x%x resp=%p\n",
          __func__, dev, cmd_idx, arg, resp_type, resp);

#if !IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK)
    /* enable the SD CLK signal if the SDIO/SD/MMC peripheral driver does
     * not support the Auto-CLK feature (periph_sdmmc_auto_clk) */
    if (dev->driver->enable_clock && dev->driver->enable_clock(dev, true)) {
        return -EIO;
    }
#endif

    int res = dev->driver->send_cmd(dev, cmd_idx, arg, resp_type, resp);

#if ENABLE_DEBUG
    if (!resp) {
        return res;
    }

    if (resp_type == SDMMC_R2) {
        DEBUG("[sdmmc] %s dev=%p cmd_idx=%u "
              "r[0]=0x%08"PRIx32" r[2]=0x%08"PRIx32" r[1]=0x%08"PRIx32" r[3]=0x%08"PRIx32"\n",
              __func__, dev, cmd_idx, resp[0], resp[1], resp[2], resp[3]);
    }
    else if (resp_type != SDMMC_NO_R) {
        DEBUG("[sdmmc] %s dev=%p cmd_idx=%u r=0x%08"PRIx32"\n",
              __func__, dev, cmd_idx, *resp);
    }
#endif

    return res;
}

static int _send_acmd(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                      sdmmc_resp_t resp_type, uint32_t *resp)
{
    uint32_t response;
    int res;

    DEBUG("[sdmmc] %s dev=%p cmd_idx=%u arg=0x%"PRIx32" resp_type=0x%x resp=%p\n",
          __func__, dev, cmd_idx, arg, resp_type, resp);

    assert(cmd_idx & SDMMC_ACMD_PREFIX);

    if (cmd_idx == SDMMC_ACMD41) {
        res = _send_cmd(dev, SDMMC_CMD55, SDMMC_CMD_NO_ARG,
                        SDMMC_R1, &response);
    }
    else {
        assert(dev->rca);
        res = _send_cmd(dev, SDMMC_CMD55, SDMMC_CMD_ARG_RCA(dev->rca),
                             SDMMC_R1, &response);
    }

    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        return res;
    }

    return _send_cmd(dev, cmd_idx & ~SDMMC_ACMD_PREFIX, arg, resp_type, resp);
}

static inline int _send_xcmd(sdmmc_dev_t *dev,
                             sdmmc_cmd_t cmd_idx, uint32_t arg,
                             sdmmc_resp_t resp_type, uint32_t *resp)
{
    assert(dev);
    assert(dev->driver);

    if (cmd_idx & SDMMC_ACMD_PREFIX) {
        return _send_acmd(dev, cmd_idx, arg, resp_type, resp);
    }
    else {
        return _send_cmd(dev, cmd_idx, arg, resp_type, resp);
    }
}

static int _xfer(sdmmc_dev_t *dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                 uint16_t block_size, uint16_t block_num,
                 const void *data_wr, void *data_rd, uint16_t *done)
{
    DEBUG("[sdmmc] %s dev=%p cmd_idx=%u arg=%"PRIu32" block_size=%u "
          "block_num=%u data_wr=%p data_rd=%p done=%p\n",
          __func__, dev, cmd_idx, arg, block_size, block_num,
          data_wr, data_rd, done);

    /* TODO: in SDIO multi-byte mode, the block_size must be between 1 and 512
     *       and block_num must be 1 */
    /* TODO: in stream mode, the block_size and block_num must be 0 */
    assert(block_num);

    /* check for valid transfer commands */
    assert((cmd_idx == SDMMC_CMD6) ||
           (cmd_idx == SDMMC_CMD8) || (cmd_idx == SDMMC_CMD9) ||
           (cmd_idx == SDMMC_CMD17) || (cmd_idx == SDMMC_CMD18) ||
           (cmd_idx == SDMMC_CMD24) || (cmd_idx == SDMMC_CMD25) ||
           (cmd_idx == SDMMC_ACMD13) || (cmd_idx == SDMMC_ACMD51));

    /* only CMD18 and CMD25 are multiple block transfers */
    if ((cmd_idx == SDMMC_CMD18) || (cmd_idx == SDMMC_CMD25)) {
        assert(block_num > 1);
    }
    else {
        assert(block_num == 1);
    }

    uint32_t response;
    int res;

    /* TODO: all transfer types (MMC Stream, SDIO Multibyte) and Auto-CDM23 feature */
    sdmmc_xfer_desc_t xfer = {
        .type = SDMMC_BLOCK,    /* at the moment only block transfer supported */
        .cmd_idx = cmd_idx,
        .arg = arg,
        .resp_type = SDMMC_R1,  /* all supported transfer commands use R1 */
        .write = (cmd_idx == SDMMC_CMD24) || (cmd_idx == SDMMC_CMD25),
        .block_size = block_size,
        .block_num = block_num,
    };

    /* for write transfers `data_wr` must not be NULL, otherwise `data_rd` */
    assert((xfer.write && data_wr) || data_rd);

    /* enable the SD CLK signal */
    if (_enable_clock(dev)) {
        return -EIO;
    }

    /* wait until the card is ready */
    if ((res = _wait_for_ready(dev, SDMMC_WAIT_FOR_CARD_READY_MS))) {
        /* reset the Card */
        dev->init_done = false;
        goto out;
    }

    /* send CMD55 for application specific commands before preparing the transfer */
    if (cmd_idx & SDMMC_ACMD_PREFIX) {
        res = _send_cmd(dev, SDMMC_CMD55, SDMMC_CMD_ARG_RCA(dev->rca),
                        SDMMC_R1, &response);
        if (res) {
            DEBUG("[sdmmc] command failed with error %d\n", res);
            goto out;
        }
    }

    /* prepare the transfer in device driver */
    DEBUG("[sdmmc] prepare transfer @%08"PRIx32" for %u block(s) with %u bytes\n",
          arg, block_num, block_size);
    res = dev->driver->xfer_prepare(dev, &xfer);
    if (res) {
        res = -EINVAL;
        goto out;
    }

    /* send the command to trigger the transfer */
    DEBUG("[sdmmc] send %s%d\n", cmd_idx & SDMMC_ACMD_PREFIX ? "ACMD" : "CMD",
          cmd_idx & ~SDMMC_ACMD_PREFIX);
    res = _send_cmd(dev, cmd_idx & ~SDMMC_ACMD_PREFIX, arg,
                    SDMMC_R1, &response);
    if (res) {
        goto out;
    }

    /* execute the transfer in device driver */
    DEBUG("[sdmmc] execute transfer\n");
    res = dev->driver->xfer_execute(dev, &xfer, xfer.write ? data_wr : NULL,
                                    !xfer.write ? data_rd : NULL, done);
    if (res) {
        DEBUG("[sdmmc] transfer failed with error %d\n", res);
        /* Don't return by intention to call _xfer_finish in any case */
    }

    /* finish the transfer in device driver */
    DEBUG("[sdmmc] stop transfer\n");
    dev->driver->xfer_finish(dev, &xfer);

    /* TODO: use CMD23 (SET_BLOCK_COUNT) instead of CMD12 (STOP_TRANSMISSION)
     * in case of multiple block transfers if supported by card, requires
     * that SCR.CMD_SUPPORT[1] (SCR bit 33) is set */
    if (block_num > 1) {
        if (IS_USED(MODULE_PERIPH_SDMMC_AUTO_CMD12)) {
            DEBUG("[sdmmc] Auto CMD12 used\n");
        }
        else if (IS_USED(MODULE_SDMMC_MMC) && (dev->type == SDMMC_CARD_TYPE_MMC)) {
            /* MMC cards use R1 in case of read and R1B in case of write */
            _send_cmd(dev, SDMMC_CMD12, SDMMC_CMD_NO_ARG,
                      xfer.write ? SDMMC_R1B : SDMMC_R1, &response);
        }
        else {
            /* SD Cards always use R1B */
            _send_cmd(dev, SDMMC_CMD12, SDMMC_CMD_NO_ARG, SDMMC_R1B, &response);
        }
    }

    /* wait until the card left the programming state */
    if (xfer.write && (res = _wait_while_prg(dev, 1, SDMMC_DATA_W_TIMEOUT_MS))) {
        /* reset the Card */
        dev->init_done = false;
        goto out;
    }

out:
    if (_disable_clock(dev)) {
        return -EIO;
    }

    return res;
}

/**
 * @brief   Ensure that the card is present and initialized
 */
static int _assert_card(sdmmc_dev_t *dev)
{
    if (!dev->present) {
        return -ENODEV;
    }

    /* return if card is already initialized */
    if (dev->init_done) {
        return 0;
    }

    /* otherwise initialize the card */
    return sdmmc_card_init(dev);
}

/**
 * @brief   Select or deselect a card (Internal Function)
 *
 * The function selects or deselects the given card. The card to be selected
 * is addressed by its RCA and must be in the `stby` state. When the card is
 * selected, it changes to the `tran` state. All other not addressed cards
 * are deselected by this and go into the state `stby`.
 *
 * @param[in]   dev     SDIO/SD/MMC device to be used
 * @param[in]   select  If `true`, card will be selected, otherwise deselected
 *
 * @return 0 on success or negative error code on error
 */
static int _select_deselect(sdmmc_dev_t *dev, bool select)
{
    /* card must have a valid RCA */
    assert(dev->rca);

    uint32_t response;
    int res;

    DEBUG("[sdmmc] send CMD7 to %s the card\n", select ? "select" : "deselect");
    if (select) {
        res = _send_cmd(dev, SDMMC_CMD7, SDMMC_CMD_ARG_RCA(dev->rca),
                        SDMMC_R1B, &response);
    }
    else {
        res = _send_cmd(dev, SDMMC_CMD7, SDMMC_CMD_NO_ARG, SDMMC_NO_R, NULL);
    }

    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        return res;
    }

    return 0;
}

/**
 * @brief    Get card status word as returned in responses R1 (Internal Function)
 *
 * The function returns the status of the given card as defined in
 * - Physical Layer Simplified Specification Version 9.00, Section 4.10.1,
 *   Table 4-42 [[sdcard.org](https://www.sdcard.org)](sdcard.org)
 * - SDIO Simplified Specification Version 3.00, 4.10.8, Table 4-7,
 *   [[sdcard.org](https://www.sdcard.org)]
 * - JEDEC Standard No. JESD84-B42, MultiMediaCard (MMC) Electrical
 *   Standard, High Capacity (MMCA, 4.2), Section 7.11, Table 25
 *   [[jedec.org](https://www.jedec.org)]
 * and returned in R1 response.
 *
 * CMD13 can be used for the addressed card in most states except `idle`,
 * `ready`, `ident` and `ina`.
 *
 * @param[in]   dev     SDIO/SD/MMC device to be used
 * @param[out]  cs      Card status
 *
 * @retval  0       on success
 * @retval  -ENODEV on error
 */
static int _get_status(sdmmc_dev_t *dev, sdmmc_card_status_t *cs)
{
    static_assert(sizeof(sdmmc_card_status_t) == sizeof(uint32_t),
                  "sizeof(sdmmc_card_status_t) != sizeof(uint32_t)");

    /* card must have a valid RCA */
    assert(dev->rca);

    if (_send_cmd(dev, SDMMC_CMD13, SDMMC_CMD_ARG_RCA(dev->rca),
                  SDMMC_R1, &cs->value)) {
        /* we suppose that the device is not present on error */
        return -ENODEV;
    }
    return 0;
}

/**
 * @brief   Wait until the given SD Memory Card or MMC is ready (Internal Function)
 *
 * The function waits until the given SD Memory Card or MMC is ready
 * which is indicated by the `READY_FOR_DATA` bit in the card status word.
 *
 * @param[in]   dev         SD/MMC device to be used
 * @param[in]   timeout_ms  Timeout in ms, must not be 0
 *
 * @retval  0           on success
 * @retval  -ENODEV     if _get_status returned with any error
 * @retval  -ETIMEDOUT  if card was available but waiting for ready timed out
 */
static inline int _wait_for_ready(sdmmc_dev_t *dev, uint32_t timeout_ms)
{
    /* card must be any SD Memory Card or MMC */
    assert(dev->type & (SDMMC_CARD_TYPE_SD | SDMMC_CARD_TYPE_MMC));
    assert(timeout_ms);

    uint32_t t_start = _ZTIMER_NOW();
    sdmmc_card_status_t cs;

    do {
        if (_get_status(dev, &cs)) {
            LOG_ERROR("[sdmmc] Card not present\n");
            return -ENODEV;
        }
        if (cs.value & SDMMC_CARD_STATUS_READY_FOR_DATA) {
            return 0;
        }
        /* check the state every 10 ms */
        _ZTIMER_SLEEP_MS(10);
    } while ((_ZTIMER_NOW() - t_start) < timeout_ms);

    LOG_ERROR("[sdmmc] Card is busy\n");
    return -ETIMEDOUT;
}

/**
 * @brief   Wait with sleep as long as the card is programming
 *
 * @param[in]   dev         SD/MMC device to be used
 * @param[in]   sleep_ms    Sleep time in ms, busy wait if 0
 * @param[in]   timeout_ms  Timeout in ms, no timeout if 0
 *
 * @return  0 on success or negative error code on error
 */
static int _wait_while_prg(sdmmc_dev_t *dev,
                           uint32_t sleep_ms, uint32_t timeout_ms)
{
    uint32_t t_start = _ZTIMER_NOW();

    int res;
    sdmmc_card_status_t cs;

    _ZTIMER_ACQUIRE();
    do {
        res = _get_status(dev, &cs);
        if (res || (cs.CURRENT_STATE != SDMMC_CARD_STATE_PRG)) {
            /* return on error with error code in res or with res=0 on success */
            goto out;
        }
        if (sleep_ms) {
            /* check the status every ms */
            _ZTIMER_SLEEP_MS(1);
        }
    } while (!timeout_ms || ((_ZTIMER_NOW() - t_start) < timeout_ms));

    LOG_ERROR("[sdmmc] Card did not leave the programming state \n");
    res = -ETIMEDOUT;

out:
   _ZTIMER_RELEASE();
   return 0;
}

/**
 * @brief   Read the CID register (Internal Function)
 *
 * The function reads the CID register of a SD Memory Card or MMC and stores
 * it in the card descriptor.
 *
 * @pre The card has to be either in the `ready` state when CMD2 is used or in
 *      the `stby` state (deselected) when CMD10 is used to read the CID.
 *
 * @param[in]   dev     SD/MMC device to be used
 * @param[in]   cmd     Command to be used
 *                      (CMD2 in state `ready` or CMD10 in state `stdby')
 *
 * @return 0 on success or negative error code on error
 */
static int _read_cid(sdmmc_dev_t *dev, uint8_t cmd)
{
    /* sanity checks for CID structure versions */
    static_assert(sizeof(sdmmc_cid_sd_t) == sizeof(sdmmc_cid_mmc_t),
                  "sizeof(sdmmc_cid_sd_t) != sizeof(sdmmc_cid_mmc_t)");
    static_assert(sizeof(sdmmc_cid_sd_t) == SDMMC_CID_REG_SIZE,
                  "sizeof(sdmmc_cid_sd_t) != SDMMC_CID_REG_SIZE");

    uint32_t response[4];   /* long response requires four 32-bit words */
    int res;

    assert((cmd == SDMMC_CMD2) || (cmd == SDMMC_CMD10));

    /* card must have a valid RCA in case of CMD10 */
    assert((cmd != SDMMC_CMD10) || dev->rca);

    DEBUG("[sdmmc] send CMD%d to read CID\n", cmd);
    res = _send_cmd(dev, cmd,
                    SDMMC_CMD10 ? SDMMC_CMD_ARG_RCA(dev->rca) : SDMMC_CMD_NO_ARG,
                    SDMMC_R2, response);

    /* raw data that are used to fill CID struct have to be in big-endian order */
    uint8_t cid_raw[SDMMC_CID_REG_SIZE];
    byteorder_htobebufl(cid_raw, response[0]);
    byteorder_htobebufl(cid_raw + 4, response[1]);
    byteorder_htobebufl(cid_raw + 8, response[2]);
    byteorder_htobebufl(cid_raw + 12, response[3]);

#if ENABLE_DEBUG
    _print_raw_data_crc(cid_raw, SDMMC_CID_REG_SIZE, true);
#endif

    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        return res;
    }

    memcpy(&dev->cid, cid_raw, sizeof(sdmmc_cid_t));

    return 0;
}

#define SDMMC_CSD_STRUCTURE(raw_data) ((raw_data)[0] >> 6)

/**
 * @brief   Read the CSD register (Internal Function)
 *
 * The function reads the CSD of a SD Memory Card or MMC and stores it in
 * the card descriptor because it contains information that are required
 * when handling the card.
 *
 * @pre The card has to be addressable by its RCA and has to be in the `stby`
 *      state to read the CSD (deselected).
 *
 * @param[in]   dev     SD/MMC device to be used
 *
 * @return 0 on success or negative error code on error
 */
static int _read_csd(sdmmc_dev_t *dev)
{
    /* sanity checks for CSD structure versions */
    static_assert(sizeof(sdmmc_csd_v1_t) == sizeof(sdmmc_csd_v2_t),
                  "sizeof(sdmmc_csd_v1_t) != sizeof(sdmmc_csd_v2_t)");
    static_assert(sizeof(sdmmc_csd_v1_t) == sizeof(sdmmc_csd_mmc_t),
                  "sizeof(sdmmc_csd_v1_t) != sizeof(sdmmc_csd_mmc_t)");
    static_assert(sizeof(sdmmc_csd_v1_t) == sizeof(sdmmc_csd_t),
                  "sizeof(sdmmc_csd_v1_t) != sizeof(sdmmc_csd_t)");
#ifndef NDEBUG
    /* ensure to be able to use csd.mmc.* members for all CSD versions */
    sdmmc_csd_t csd = {};
    csd.mmc.CSD_STRUCTURE = 0x3;
    csd.mmc.TAAC = 0xaa;
    csd.mmc.NSAC = 0x55;
    csd.mmc.R2W_FACTOR = 0x5;
    assert(csd.v1.CSD_STRUCTURE == csd.mmc.CSD_STRUCTURE);
    assert(csd.v2.CSD_STRUCTURE == csd.mmc.CSD_STRUCTURE);
    assert(csd.v1.TAAC == csd.mmc.TAAC);
    assert(csd.v2.TAAC == csd.mmc.TAAC);
    assert(csd.v1.NSAC == csd.mmc.NSAC);
    assert(csd.v2.NSAC == csd.mmc.NSAC);
    assert(csd.v1.R2W_FACTOR == csd.mmc.R2W_FACTOR);
    assert(csd.v2.R2W_FACTOR == csd.mmc.R2W_FACTOR);
#endif

    /* card must have a valid RCA */
    assert(dev->rca);

    uint32_t response[4];   /* long response requires four 32-bit words */
    int res;

    DEBUG("[sdmmc] send CMD9 to read CSD\n");
    res = _send_cmd(dev, SDMMC_CMD9, SDMMC_CMD_ARG_RCA(dev->rca),
                    SDMMC_R2, response);
#if ENABLE_DEBUG
    /* raw data that are used to fill CSD struct have to be big endian */
    uint8_t csd_data[SDMMC_CSD_REG_SIZE];

    byteorder_htobebufl(csd_data, response[0]);
    byteorder_htobebufl(csd_data + 4, response[1]);
    byteorder_htobebufl(csd_data + 8, response[2]);
    byteorder_htobebufl(csd_data + 12, response[3]);

    _print_raw_data_crc(csd_data, SDMMC_CSD_REG_SIZE, true);
#endif

    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        return res;
    }

    uint32_t csd_raw[SDMMC_CSD_REG_SIZE >> 2];

    /* read data are word-wise in host byte order */
    csd_raw[0] = response[3];   /* response[3] contains CSD[31:0] */
    csd_raw[1] = response[2];   /* response[2] contains CSD[63:32] */
    csd_raw[2] = response[1];   /* response[1] contains CSD[95:64] */
    csd_raw[3] = response[0];   /* response[0] contains CSD[127:96] */

    memcpy(&dev->csd, csd_raw, sizeof(sdmmc_csd_t));

    return 0;
}

/**
 * @brief   Read the SCR register of the selected SD Memory Card (Internal Function)
 *
 * The function reads the SCR register of the selected SD Memory Card and stores
 * it in the card descriptor because it contains information that are required
 * when handling the card.
 *
 * The SCR register is only availably on SD Memory cards
 *
 * @pre The card has to be selected to read the SCR, that is, the card is in
 *      the `tran` state.
 *
 * @param[in]   dev     SD device to be used
 *
 * @return 0 on success or negative error code on error
 */
static int _read_scr(sdmmc_dev_t *dev)
{
    int res = 0;

    /* selected card has to be any SD Memory Card */
    assert(dev->type & SDMMC_CARD_TYPE_SD);

    sdmmc_buf_t scr_raw[SDMMC_SCR_REG_SIZE];

    DEBUG("[sdmmc] send ACMD51 to read SCR\n");

    res = _xfer(dev, SDMMC_ACMD51, SDMMC_CMD_NO_ARG,
                SDMMC_SCR_REG_SIZE, 1, NULL, scr_raw, NULL);
#if ENABLE_DEBUG
    _print_raw_data_crc(scr_raw, SDMMC_SCR_REG_SIZE, false);
#endif

    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        return res;
    }

    dev->scr.value = byteorder_bebuftohl(scr_raw);
    dev->scr.reserved0 = byteorder_bebuftohl(scr_raw + 4);

    return 0;
}

static inline int _enable_clock(sdmmc_dev_t *dev)
{
    DEBUG("[sdmmc] %s dev=%p\n", __func__, dev);

#if !IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK)
    return (dev->driver->enable_clock) ? dev->driver->enable_clock(dev, true)
                                       : 0;
#else
    (void)dev;
    return 0;
#endif
}

static inline int _disable_clock(sdmmc_dev_t *dev)
{
    DEBUG("[sdmmc] %s dev=%p\n", __func__, dev);

#if !IS_USED(MODULE_PERIPH_SDMMC_AUTO_CLK)
    return (dev->driver->enable_clock) ? dev->driver->enable_clock(dev, false)
                                       : 0;
#else
    (void)dev;
    return 0;
#endif
}

#if IS_USED(MODULE_SDMMC_MMC)

/* To avoid a 512 byte buffer on the stack, a static variable is used here.
 * The exclusive access to this buffer must be guaranteed. */
static sdmmc_buf_t _ext_csd_raw[SDMMC_EXT_CSD_REG_SIZE];

static mutex_t _ext_csd_access = MUTEX_INIT;

/**
 * @brief   Read the EXT_CSD register of the selected MMC (Internal Function)
 *
 * The function reads the EXT_CSD register of the selected MMC and stores
 * it in the card descriptor because it contains information that are required
 * when handling the card.
 *
 * @pre The card has to be selected to read the EXT_CSD, that is, the card is in
 *      the `tran` state.
 *
 * @param[in]   dev     MMC device to be used
 *
 * @return 0 on success or negative error code on error
 */
static int _read_ext_csd(sdmmc_dev_t *dev)
{
    /* card must be a MMC */
    assert(dev->type == SDMMC_CARD_TYPE_MMC);

    DEBUG("[sdmmc] send CMD8 to read Extended CSD\n");

    mutex_lock(&_ext_csd_access);

    int res = _xfer(dev, SDMMC_CMD8, SDMMC_CMD_NO_ARG,
                    SDMMC_EXT_CSD_REG_SIZE, 1, NULL, _ext_csd_raw, NULL);
#if ENABLE_DEBUG
    _print_raw_data_crc(_ext_csd_raw, SDMMC_EXT_CSD_REG_SIZE, false);
#endif

    if (res) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        mutex_unlock(&_ext_csd_access);
        return res;
    }

    dev->ext_csd.SEC_COUNT = byteorder_lebuftohl(_ext_csd_raw + SDMMC_EXT_CSD_SEC_COUNT);
    dev->ext_csd.CSD_STRUCTURE = _ext_csd_raw[SDMMC_EXT_CSD_CSD_STRUCTURE];
    dev->ext_csd.BUS_WIDTH = _ext_csd_raw[SDMMC_EXT_CSD_BUS_WIDTH];
    dev->ext_csd.CARD_TYPE = _ext_csd_raw[SDMMC_EXT_CSD_CARD_TYPE];

    mutex_unlock(&_ext_csd_access);
    return 0;
}

/**
 * @brief   Write the EXT_CSD register of the selected MMC (Internal Function)
 *
 * The function writes @p value to the byte @p index of the EXT_CSD register
 * of the selected MMC.
 *
 * @pre The card has to be selected to write the EXT_CSD, that is, the card
 *      is in the `tran` state.
 *
 * @param[in]   dev     MMC device to be used
 *
 * @return 0 on success or negative error code on error
 */
static int _write_ext_csd(sdmmc_dev_t *dev, uint8_t index, uint8_t value)
{
    /* card must be a MMC */
    assert(dev->type == SDMMC_CARD_TYPE_MMC);

    uint32_t response;
    int res;
    uint32_t arg = (SDMMC_EXT_CSD_WRITE_BYTE << SDMMC_CMD6_ACCESS) |
                   (index << SDMMC_CMD6_INDEX) |
                   (value << SDMMC_CMD6_VALUE);

    DEBUG("[sdmmc] send CMD6 to set EXT_CSD[%u]=%02x\n", index, value);

    if ((res = _send_cmd(dev, SDMMC_CMD6, arg, SDMMC_R1B, &response))) {
        DEBUG("[sdmmc] command failed with error %d\n", res);
        return res;
    }

    if ((res = _wait_while_prg(dev, 1, SDMMC_INIT_TIMEOUT_MS)))  {
        /* reset the Card */
        dev->init_done = false;
        return res;
    }

    return 0;
}
#endif /* IS_USED(MODULE_SDMMC_MMC) */

#if ENABLE_DEBUG
static uint8_t _crc_7(const uint8_t *data, unsigned n)
{
    uint8_t crc = 0;

    for (unsigned i = 0; i < n; i++) {
        uint8_t d = data[i];
        for (unsigned j = 0; j < 8; j++) {
            crc <<= 1;
            if ((d & 0x80) ^ (crc & 0x80)) {
                crc ^= 0x09;
            }
            d <<= 1;
        }
    }
    return crc & 0x7f;
}

static void _print_raw_data_crc(const uint8_t *data, unsigned size, bool crc)
{
    DEBUG("[sdmmc] raw data: ");
    for (unsigned i = 0; i < size; i++) {
        if ((size > 16) && ((i % 16) == 0)) {
            DEBUG("\n%04x: ", i);
        }
        DEBUG("0x%02x ", data[i]);
    }
    DEBUG("\n");
    if (crc) {
        /* print CRC if data include the CRC in last byte */
        DEBUG("[sdmmc] CRC: 0x%02x (received 0x%02x)\n",
              _crc_7(data, size - 1), data[size - 1] >> 1);
    }
}
#endif
