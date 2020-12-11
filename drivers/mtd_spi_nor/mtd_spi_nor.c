/*
 * Copyright (C) 2016 Eistec AB
 *               2017 OTA keys S.A.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     drivers_mtd_spi_nor
 * @{
 *
 * @file
 * @brief       Driver for serial flash memory attached to SPI
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Vincent Dupont <vincent@otakeys.com>
 *
 * @}
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include "macros/units.h"
#include "mtd.h"
#include "xtimer.h"
#include "timex.h"
#include "thread.h"
#include "byteorder.h"
#include "bitarithm.h"
#include "mtd_spi_nor.h"
#include "mtd_spi_nor/sfdp.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define ENABLE_TRACE    0
#define TRACE(...)      DEBUG(__VA_ARGS__)

/* after power up, on an invalid JEDEC ID, wait and read N times */
#ifndef MTD_POWER_UP_WAIT_FOR_ID
#define MTD_POWER_UP_WAIT_FOR_ID    (0x0F)
#endif

#define SFLASH_CMD_4_BYTE_ADDR (0xB7)   /**< enable 32 bit addressing */
#define SFLASH_CMD_3_BYTE_ADDR (0xE9)   /**< enable 24 bit addressing */

#define MTD_64K             (65536ul)
#define MTD_64K_ADDR_MASK   (0xFFFF)
#define MTD_32K             (32768ul)
#define MTD_32K_ADDR_MASK   (0x7FFF)
#define MTD_4K              (4096ul)
#define MTD_4K_ADDR_MASK    (0xFFF)

#define MBIT_AS_BYTES       ((1024 * 1024) / 8)

#define MIN(a, b) ((a) > (b) ? (b) : (a))

/**
 * @brief   JEDEC memory manufacturer ID codes.
 *
 *          see http://www.softnology.biz/pdf/JEP106AV.pdf
 * @{
 */
#define JEDEC_BANK(n)   ((n) << 8)

typedef enum {
    SPI_NOR_JEDEC_ATMEL = 0x1F | JEDEC_BANK(1),
} jedec_manuf_t;
/** @} */

static inline spi_t _get_spi(const mtd_spi_nor_t *dev)
{
    return dev->params->spi;
}

static void mtd_spi_acquire(const mtd_spi_nor_t *dev)
{
    spi_acquire(_get_spi(dev), dev->params->cs,
                dev->params->mode, dev->params->clk);
}

static void mtd_spi_release(const mtd_spi_nor_t *dev)
{
    spi_release(_get_spi(dev));
}

static inline uint8_t* _be_addr(const mtd_spi_nor_t *dev, uint32_t *addr)
{
    *addr = htonl(*addr);
    return &((uint8_t*)addr)[4 - dev->addr_width];
}

/**
 * @internal
 * @brief Send command opcode followed by address, followed by a read to buffer
 *
 * @param[in]  dev    pointer to device descriptor
 * @param[in]  opcode command opcode
 * @param[in]  addr   address (big endian)
 * @param[out] dest   read buffer
 * @param[in]  count  number of bytes to read after the address has been sent
 */
static void mtd_spi_cmd_addr_read(const mtd_spi_nor_t *dev, uint8_t opcode,
                                  uint32_t addr, void *dest, uint32_t count)
{
    TRACE("mtd_spi_cmd_addr_read: %p, %02x, (%06"PRIx32"), %p, %" PRIu32 "\n",
          (void *)dev, (unsigned int)opcode, addr, dest, count);

    uint8_t *addr_buf = _be_addr(dev, &addr);

    if (IS_ACTIVE(ENABLE_TRACE)) {
        TRACE("mtd_spi_cmd_addr_read: addr:");
        for (unsigned int i = 0; i < dev->addr_width; ++i) {
            TRACE(" %02x", addr_buf[i]);
        }
        TRACE("\n");
    }

    /* Send opcode followed by address */
    spi_transfer_byte(_get_spi(dev), dev->params->cs, true, opcode);
    spi_transfer_bytes(_get_spi(dev), dev->params->cs, true,
                       (char *)addr_buf, NULL, dev->addr_width);

    /* Read data */
    spi_transfer_bytes(_get_spi(dev), dev->params->cs, false,
                       NULL, dest, count);
}

/**
 * @internal
 * @brief Send command opcode followed by address, followed by a write from buffer
 *
 * @param[in]  dev    pointer to device descriptor
 * @param[in]  opcode command opcode
 * @param[in]  addr   address (big endian)
 * @param[out] src    write buffer
 * @param[in]  count  number of bytes to write after the opcode has been sent
 */
static void mtd_spi_cmd_addr_write(const mtd_spi_nor_t *dev, uint8_t opcode,
                                   uint32_t addr, const void *src, uint32_t count)
{
    TRACE("mtd_spi_cmd_addr_write: %p, %02x, (%06"PRIx32"), %p, %" PRIu32 "\n",
          (void *)dev, (unsigned int)opcode, addr, src, count);

    uint8_t *addr_buf = _be_addr(dev, &addr);

    if (IS_ACTIVE(ENABLE_TRACE)) {
        TRACE("mtd_spi_cmd_addr_write: addr:");
        for (unsigned int i = 0; i < dev->addr_width; ++i) {
            TRACE(" %02x", addr_buf[i]);
        }
        TRACE("\n");
    }

    /* Send opcode followed by address */
    spi_transfer_byte(_get_spi(dev), dev->params->cs, true, opcode);

    /* only keep CS asserted when there is data that follows */
    bool cont = (count > 0);
    spi_transfer_bytes(_get_spi(dev), dev->params->cs, cont,
                       (char *)addr_buf, NULL, dev->addr_width);

    /* Write data */
    if (cont) {
        spi_transfer_bytes(_get_spi(dev), dev->params->cs,
                           false, (void *)src, NULL, count);
    }
}

/**
 * @internal
 * @brief Send command opcode followed by a read to buffer
 *
 * @param[in]  dev    pointer to device descriptor
 * @param[in]  opcode command opcode
 * @param[out] dest   read buffer
 * @param[in]  count  number of bytes to write after the opcode has been sent
 */
static void mtd_spi_cmd_read(const mtd_spi_nor_t *dev, uint8_t opcode, void *dest, uint32_t count)
{
    TRACE("mtd_spi_cmd_read: %p, %02x, %p, %" PRIu32 "\n",
          (void *)dev, (unsigned int)opcode, dest, count);

    spi_transfer_regs(_get_spi(dev), dev->params->cs, opcode, NULL, dest, count);
}

/**
 * @internal
 * @brief Send command opcode followed by a write from buffer
 *
 * @param[in]  dev    pointer to device descriptor
 * @param[in]  opcode command opcode
 * @param[out] src    write buffer
 * @param[in]  count  number of bytes to write after the opcode has been sent
 */
static void __attribute__((unused)) mtd_spi_cmd_write(const mtd_spi_nor_t *dev, uint8_t opcode, const void *src, uint32_t count)
{
    TRACE("mtd_spi_cmd_write: %p, %02x, %p, %" PRIu32 "\n",
          (void *)dev, (unsigned int)opcode, src, count);

    spi_transfer_regs(_get_spi(dev), dev->params->cs, opcode,
                      (void *)src, NULL, count);
}

/**
 * @internal
 * @brief Send command opcode
 *
 * @param[in]  dev    pointer to device descriptor
 * @param[in]  opcode command opcode
 */
static void mtd_spi_cmd(const mtd_spi_nor_t *dev, uint8_t opcode)
{
    TRACE("mtd_spi_cmd: %p, %02x\n",
          (void *)dev, (unsigned int)opcode);

    spi_transfer_byte(_get_spi(dev), dev->params->cs, false, opcode);
}

static bool mtd_spi_manuf_match(const mtd_jedec_id_t *id, jedec_manuf_t manuf)
{
    return manuf == ((id->bank << 8) | id->manuf);
}

/**
 * @internal
 * @brief Compute 8 bit parity
 */
static inline uint8_t parity8(uint8_t x)
{
    /* Taken from http://stackoverflow.com/a/21618038/1805713 */
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return (x & 1);
}

/**
 * @internal
 * @brief Read JEDEC ID
 */
static int mtd_spi_read_jedec_id(const mtd_spi_nor_t *dev, mtd_jedec_id_t *out)
{
    uint8_t buffer[JEDEC_BANK_MAX + sizeof(mtd_jedec_id_t) - 1];

    DEBUG("mtd_spi_read_jedec_id: rdid=0x%02x\n",
          (unsigned int)dev->params->opcode->rdid);

    /* Send opcode */
    mtd_spi_cmd_read(dev, dev->params->opcode->rdid, buffer, sizeof(buffer));

    /* Manufacturer IDs are organized in 'banks'.
     * If we read the 'next bank' instead of manufacturer ID, skip
     * the byte and increment the bank counter.
     */
    uint8_t bank = 0;
    while (buffer[bank] == JEDEC_NEXT_BANK) {
        if (++bank == JEDEC_BANK_MAX) {
            DEBUG_PUTS("mtd_spi_read_jedec_id: bank out of bounds\n")
            return -1;
        }
    }

    if (parity8(buffer[bank]) == 0) {
        /* saw even parity, we expected odd parity => parity error */
        DEBUG("mtd_spi_read_jedec_id: Parity error (0x%02x)\n", buffer[bank]);
        return -2;
    }

    if (buffer[bank] == 0xFF || buffer[bank] == 0x00) {
        DEBUG_PUTS("mtd_spi_read_jedec_id: failed to read manufacturer ID");
        return -3;
    }

    /* Copy manufacturer ID */
    out->bank = bank + 1;
    memcpy((uint8_t*)out + 1, &buffer[bank], 3);

    DEBUG("mtd_spi_read_jedec_id: bank=%u manuf=0x%02x\n", (unsigned int)out->bank,
          (unsigned int)out->manuf);

    DEBUG("mtd_spi_read_jedec_id: device=0x%02x, 0x%02x\n",
          (unsigned int)out->device[0], (unsigned int)out->device[1]);

    return 0;
}

static void _read_sfdp_data(mtd_spi_nor_t *dev, uint32_t addr, void *data,
                            size_t len)
{
    static const uint8_t opcode = 0x5a;
    uint32_t s_addr = htonl(addr << 8); /* Shift to include the dummy byte */
    /* Send opcode followed by address */
    spi_transfer_byte(_get_spi(dev), dev->params->cs, true, opcode);
    /* Includes dummy byte */
    spi_transfer_bytes(_get_spi(dev), dev->params->cs, true,
                       (char *)&s_addr, NULL, 4);

    /* Read data */
    spi_transfer_bytes(_get_spi(dev), dev->params->cs, false,
                       NULL, data, len);
}

/* See the SFDP spec for these timing multipliers */
static uint32_t _sfdp_sector_erase_timing(uint8_t erase)
{
    uint8_t count = (erase & 0x1F) + 1;
    switch (erase & 0x60) {
        case 0x00:
            return count;
        case 0x20:
            return count * 16; /* Multiplied with 16 us */
        case 0x40:
            return count * 128; /* Multiplied with 128 us */
        case 0x60:
            return count * 1000; /* Multiplied with 1ms */
    }
    return 0;
}

static uint32_t _sfdp_chip_erase_timing(uint8_t erase)
{
    uint8_t count = (erase & 0x1F) + 1;
    switch (erase & 0x60) {
        case 0x00:
            return count * 16 * US_PER_MS; /* Multiplied with 16 ms */
        case 0x20:
            return count * 256 * US_PER_MS; /* Multiplied with 256 ms */
        case 0x40:
            return count * 4 * US_PER_SEC; /* Multiplied with 4 seconds */
        case 0x60:
            return count * 64 * US_PER_SEC; /* Multiplied with 64 seconds */
    }
    return 0;
}

static uint32_t _sfdp_powerdown_delay_timing(uint8_t erase)
{
    uint8_t count = (erase & 0x1F) + 1;
    switch (erase & 0x60) {
        case 0x00:
            /* Count as units of 128ns */
            return (count * 128 + NS_PER_US) / NS_PER_US;
        case 0x20:
            return count; /* Multiplied with 1 us */
        case 0x40:
            return count * 8; /* Multiplied with 8 us */
        case 0x60:
            return count * 64; /* Multiplied with 64 us */
    }
    return 0;
}

static void _read_sfdp_access(mtd_spi_nor_t *dev,
                              mtd_jedec_param_header_t *param)
{
    size_t offset = param->table_ptr;
    uint32_t access = 0;
    mtd_jedec_basic_spi_access_info_t access_info;

    _read_sfdp_data(dev, offset + JEDEC_SFDP_BASIC_SPI_FLAGS, &access, sizeof(access));

    if (access & JEDEC_SFDP_PARAM_SUP_ADDRESS_4_ONLY) {
        dev->addr_width = 4;
    }
    else {
        dev->addr_width = 3;
    }

    _read_sfdp_data(dev, offset + JEDEC_SFDP_BASIC_SPI_ERASE_12_INFO,
                    &access_info, sizeof(access_info));

    /* Supported Sector erase timings */
    for (size_t i = 0; i < 4; i++) {
        DEBUG("Erase type %u: %"PRIu32"B, inst: 0x%.2x, erase time: %"PRIu32"ms\n",
              i + 1, 1LU << (access_info.erase[i].size), access_info.erase[i].instruction, _sfdp_sector_erase_timing(access_info.erase_time >> (4 + 7 * i)));
        size_t size_exp = access_info.erase[i].size;

        if (!size_exp) {
            DEBUG("Empty erase time record, skipping\n");
            continue;
        }

        size_t size = 1LU << size_exp;
        uint32_t timing = _sfdp_sector_erase_timing(access_info.erase_time >> (4 + 7 * i));
        switch (size) {
            case KiB(4):
                dev->wait_sector_erase = timing;
                dev->flag |= SPI_NOR_F_SECT_4K;
                break;
            case KiB(32):
                dev->wait_32k_erase = timing;
                dev->flag |= SPI_NOR_F_SECT_32K;
                break;
            case KiB(64):
                dev->wait_64k_erase = timing;
                dev->flag |= SPI_NOR_F_SECT_64K;
                break;
        }
    }

    /* Chip erase timings */
    dev->wait_chip_erase = _sfdp_chip_erase_timing(access_info.chip_erase_time);

    /* Chip wake up timing */
    mtd_jedec_basic_spi_powerdown_info_t powerdown_info = { 0 };
    _read_sfdp_data(dev, offset + JEDEC_SFDP_BASIC_SPI_POWERDOWN_INFO,
                    &powerdown_info, sizeof(powerdown_info));

    dev->wait_chip_wake_up = _sfdp_powerdown_delay_timing(powerdown_info.delay);
}

static void _read_sfdp_size(mtd_spi_nor_t *dev,
                             mtd_jedec_param_header_t *param)
{
    size_t offset = param->table_ptr;
    uint32_t size = 0;

    _read_sfdp_data(dev, offset + JEDEC_SFDP_BASIC_SPI_MEM_DENSITY, &size, sizeof(size));
    /* For sizes greater than 4 gigabits */
    if (size & BIT31) {
        size = 1 << ((size & ~BIT31) - 3);
    }

    size = (size + 1) / 8; /* Another 0-based number */
    mtd_dev_t *mtd = &dev->base;
    mtd->sector_count = size / (mtd->pages_per_sector * mtd->page_size);

    DEBUG("Full size: %"PRIu32", sector_count = %"PRIu32"\n", size, mtd->sector_count);
}

static void _read_sfdp_params(mtd_spi_nor_t *dev,
                              mtd_jedec_param_header_t *param)
{
    /* Read the useful parameters from the basic SPI flash param table */

    _read_sfdp_access(dev, param);

    /* Size of the flash */
    _read_sfdp_size(dev, param);

    /* Erase timings */


}

static inline uint16_t _sfdp_id(mtd_jedec_param_header_t *header)
{
    return header->id_msb << 8 | header->id_lsb;
}

static int mtd_spi_nor_read_sfdp(mtd_spi_nor_t *dev)
{
    mtd_jedec_sfdp_header_t header;

    _read_sfdp_data(dev, 0, &header, sizeof(header));

    uint8_t num_headers = header.num_headers + 1; /* 0-based, so increment for actual number */

    uint32_t signature = header.signature;

    if (signature != 0x50444653) {
        DEBUG("mtd_spi_sfdp: signature not found, no SFDP table\n");
        return -1;
    }

    DEBUG("mtd_spi_sfdp: signature: %.8"PRIx32"\n", header.signature);
    DEBUG("mtd_spi_sfdp: revision %u.%u, %u headers \n", header.major, header.minor, num_headers);


    for (unsigned i = 0; i < num_headers; i++) {
        mtd_jedec_param_header_t param;
        /* includes dummy byte */
        uint32_t addr = (8 * i + 8);

        _read_sfdp_data(dev, addr, &param, sizeof(param));

        uint16_t id = _sfdp_id(&param);

        DEBUG("mtd_spi_sfdp: param ID: %.4x revision %u.%u, %u bytes length, offset %.6"PRIx32"\n",
              id,
              param.major, param.minor, 4*param.length, (uint32_t)param.table_ptr);

        switch (id) {
            case JEDEC_SFDP_PARAM_TABLE_BASIC_SPI:
                _read_sfdp_params(dev, &param);
                break;
        }
    }

    return 0;
}

/**
 * @internal
 * @brief Get Flash capacity based on JEDEC ID
 *
 * @note The way the capacity is encoded differs between vendors.
 *       This formula has been tested with flash chips from Adesto,
 *       ISSI, Micron and Spansion, but it might not cover all cases.
 *       Please extend the function if necessary.
 */
static uint32_t mtd_spi_nor_get_size(const mtd_jedec_id_t *id)
{
    /* old Atmel (now Adesto) parts use 5 lower bits of device ID 1 for density */
    if (mtd_spi_manuf_match(id, SPI_NOR_JEDEC_ATMEL) &&
        /* ID 2 is used to encode the product version, usually 1 or 2 */
        (id->device[1] & ~0x3) == 0) {
        return (0x1F & id->device[0]) * MBIT_AS_BYTES;
    }

    /* everyone else seems to use device ID 2 for density */
    return 1 << id->device[1];
}

static inline void wait_for_write_complete(const mtd_spi_nor_t *dev, uint32_t us)
{
    unsigned i = 0, j = 0;
    uint32_t div = 2;
    uint32_t diff = 0;
    if (IS_ACTIVE(ENABLE_DEBUG) && IS_USED(MODULE_XTIMER)) {
        diff = xtimer_now_usec();
    }
    do {
        uint8_t status;
        mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status, sizeof(status));

        TRACE("mtd_spi_nor: wait device status = 0x%02x\n", (unsigned int)status);
        if ((status & 1) == 0) { /* TODO magic number */
            break;
        }
        i++;
#if MODULE_XTIMER
        if (us) {
            xtimer_usleep(us);
            /* reduce the waiting time quickly if the estimate was too short,
             * but still avoid busy (yield) waiting */
            if (us > 2 * XTIMER_BACKOFF) {
                us -= (us / div);
                div++;
            }
            else {
                us = 2 * XTIMER_BACKOFF;
            }
        }
        else {
            j++;
            thread_yield();
        }
#else
        (void)div;
        (void) us;
        thread_yield();
#endif
    } while (1);
    DEBUG("wait loop %u times, yield %u times", i, j);
    if (IS_ACTIVE(ENABLE_DEBUG) && IS_ACTIVE(MODULE_XTIMER)) {
        diff = xtimer_now_usec() - diff;
        DEBUG(", total wait %"PRIu32"us", diff);
    }
    DEBUG("\n");
}

static void _init_pins(mtd_spi_nor_t *dev)
{
    DEBUG("mtd_spi_nor_init: init pins\n");

    /* CS */
    spi_init_cs(_get_spi(dev), dev->params->cs);

    /* Write Protect - not used by the driver */
    if (gpio_is_valid(dev->params->wp)) {
        gpio_init(dev->params->wp, GPIO_OUT);
        gpio_set(dev->params->wp);
    }

    /* Hold - not used by the driver */
    if (gpio_is_valid(dev->params->hold)) {
        gpio_init(dev->params->hold, GPIO_OUT);
        gpio_set(dev->params->hold);
    }
}

static int mtd_spi_nor_power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;

    mtd_spi_acquire(dev);
    switch (power) {
        case MTD_POWER_UP:
            mtd_spi_cmd(dev, dev->params->opcode->wake);
#if defined(MODULE_XTIMER)
            /* No sense in trying multiple times if no xtimer to wait between
               reads */
            uint8_t retries = 0;
            int res = 0;
            do {
                xtimer_usleep(dev->wait_chip_wake_up);
                res = mtd_spi_read_jedec_id(dev, &dev->jedec_id);
                retries++;
            } while (res < 0 || retries < MTD_POWER_UP_WAIT_FOR_ID);
            if (res < 0) {
                return -EIO;
            }
#endif
            /* enable 32 bit address mode */
            if (dev->addr_width == 4) {
                mtd_spi_cmd(dev, SFLASH_CMD_4_BYTE_ADDR);
            }

            break;
        case MTD_POWER_DOWN:
            mtd_spi_cmd(dev, dev->params->opcode->sleep);
            break;
    }
    mtd_spi_release(dev);

    return 0;
}

static int mtd_spi_nor_init(mtd_dev_t *mtd)
{
    DEBUG("mtd_spi_nor_init: %p\n", (void *)mtd);
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;

    DEBUG("mtd_spi_nor_init: -> spi: %lx, cs: %lx, opcodes: %p\n",
          (unsigned long)_get_spi(dev), (unsigned long)dev->params->cs, (void *)dev->params->opcode);

    /* CS, WP, Hold */
    _init_pins(dev);

    /* power up the MTD device*/
    DEBUG("mtd_spi_nor_init: power up MTD device");
    if (mtd_spi_nor_power(mtd, MTD_POWER_UP)) {
        DEBUG("mtd_spi_nor_init: failed to power up MTD device");
        return -EIO;
    }

    mtd_spi_acquire(dev);
    int res = mtd_spi_read_jedec_id(dev, &dev->jedec_id);
    if (res < 0) {
        mtd_spi_release(dev);
        return -EIO;
    }
    DEBUG("mtd_spi_nor_init: Found chip with ID: (%d, 0x%02x, 0x%02x, 0x%02x)\n",
          dev->jedec_id.bank, dev->jedec_id.manuf, dev->jedec_id.device[0], dev->jedec_id.device[1]);

    if (!(dev->flag & SPI_NOR_F_NO_SFDP)) {
        mtd_spi_nor_read_sfdp(dev);
    }
    /* Should be configured by sfdp if present, derive from JEDEC ID otherwise */
    if (mtd->sector_count == 0) {
        mtd->sector_count = mtd_spi_nor_get_size(&dev->jedec_id)
                          / (mtd->pages_per_sector * mtd->page_size);
    }

    /* verify configuration */
    assert(dev->addr_width > 0);
    assert(dev->addr_width <= 4);

    DEBUG("mtd_spi_nor_init: %" PRIu32 " bytes "
          "(%" PRIu32 " sectors, %" PRIu32 " bytes/sector, "
          "%" PRIu32 " pages, "
          "%" PRIu32 " pages/sector, %" PRIu32 " bytes/page)\n",
          mtd->pages_per_sector * mtd->sector_count * mtd->page_size,
          mtd->sector_count, mtd->pages_per_sector * mtd->page_size,
          mtd->pages_per_sector * mtd->sector_count,
          mtd->pages_per_sector, mtd->page_size);
    DEBUG("mtd_spi_nor_init: Using %u byte addresses\n", dev->addr_width);

    uint8_t status;
    mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status, sizeof(status));
    mtd_spi_release(dev);

    DEBUG("mtd_spi_nor_init: device status = 0x%02x\n", (unsigned int)status);

    /* check whether page size and sector size are powers of two (most chips' are)
     * and compute the number of shifts needed to get the page and sector addresses
     * from a byte address */
    uint8_t shift = 0;
    uint32_t page_size = mtd->page_size;
    uint32_t mask = 0;

    if ((page_size & (page_size - 1)) == 0) {
        while ((page_size >> shift) > 1) {
            ++shift;
        }
        mask = (UINT32_MAX << shift);
    }

    dev->page_addr_mask = mask;
    dev->page_addr_shift = shift;
    DEBUG("mtd_spi_nor_init: page_addr_mask = 0x%08" PRIx32 ", page_addr_shift = %u\n",
          mask, (unsigned int)shift);

    mask = 0;
    shift = 0;
    uint32_t sector_size = mtd->page_size * mtd->pages_per_sector;
    if ((sector_size & (sector_size - 1)) == 0) {
        while ((sector_size >> shift) > 1) {
            ++shift;
        }
        mask = (UINT32_MAX << shift);
    }
    dev->sec_addr_mask = mask;
    dev->sec_addr_shift = shift;

    DEBUG("mtd_spi_nor_init: sec_addr_mask = 0x%08" PRIx32 ", sec_addr_shift = %u\n",
          mask, (unsigned int)shift);

    return 0;
}

static int mtd_spi_nor_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t size)
{
    DEBUG("mtd_spi_nor_read: %p, %p, 0x%" PRIx32 ", 0x%" PRIx32 "\n",
          (void *)mtd, dest, addr, size);
    const mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;
    uint32_t chipsize = mtd->page_size * mtd->pages_per_sector * mtd->sector_count;

    if (addr > chipsize) {
        return -EOVERFLOW;
    }
    if ((addr + size) > chipsize) {
        size = chipsize - addr;
    }
    if (size == 0) {
        return 0;
    }

    mtd_spi_acquire(dev);
    mtd_spi_cmd_addr_read(dev, dev->params->opcode->read, addr, dest, size);
    mtd_spi_release(dev);

    return 0;
}

static int mtd_spi_nor_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t size)
{
    uint32_t total_size = mtd->page_size * mtd->pages_per_sector * mtd->sector_count;

    DEBUG("mtd_spi_nor_write: %p, %p, 0x%" PRIx32 ", 0x%" PRIx32 "\n",
          (void *)mtd, src, addr, size);
    if (size == 0) {
        return 0;
    }
    const mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;
    if (size > mtd->page_size) {
        DEBUG("mtd_spi_nor_write: ERR: page program >1 page (%" PRIu32 ")!\n", mtd->page_size);
        return -EOVERFLOW;
    }
    if (dev->page_addr_mask &&
        ((addr & dev->page_addr_mask) != ((addr + size - 1) & dev->page_addr_mask))) {
        DEBUG("mtd_spi_nor_write: ERR: page program spans page boundary!\n");
        return -EOVERFLOW;
    }
    if (addr + size > total_size) {
        return -EOVERFLOW;
    }

    mtd_spi_acquire(dev);

    /* write enable */
    mtd_spi_cmd(dev, dev->params->opcode->wren);

    /* Page program */
    mtd_spi_cmd_addr_write(dev, dev->params->opcode->page_program, addr, src, size);

    /* waiting for the command to complete before returning */
    wait_for_write_complete(dev, 0);

    mtd_spi_release(dev);

    return 0;
}

static int mtd_spi_nor_write_page(mtd_dev_t *mtd, const void *src, uint32_t page, uint32_t offset,
                                  uint32_t size)
{
    const mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;

    DEBUG("mtd_spi_nor_write_page: %p, %p, 0x%" PRIx32 ", 0x%" PRIx32 ", 0x%" PRIx32 "\n",
          (void *)mtd, src, page, offset, size);

    uint32_t remaining = mtd->page_size - offset;
    size = MIN(remaining, size);

    uint32_t addr = page * mtd->page_size + offset;

    mtd_spi_acquire(dev);

    /* write enable */
    mtd_spi_cmd(dev, dev->params->opcode->wren);

    /* Page program */
    mtd_spi_cmd_addr_write(dev, dev->params->opcode->page_program, addr, src, size);

    /* waiting for the command to complete before returning */
    wait_for_write_complete(dev, 0);

    mtd_spi_release(dev);

    return size;
}

static int mtd_spi_nor_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t size)
{
    DEBUG("mtd_spi_nor_erase: %p, 0x%" PRIx32 ", 0x%" PRIx32 "\n",
          (void *)mtd, addr, size);
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;
    uint32_t sector_size = mtd->page_size * mtd->pages_per_sector;
    uint32_t total_size = sector_size * mtd->sector_count;

    if (dev->sec_addr_mask &&
        ((addr & ~dev->sec_addr_mask) != 0)) {
        /* This is not a requirement in hardware, but it helps in catching
         * software bugs (the erase-all-your-files kind) */
        DEBUG("addr = %" PRIx32 " ~dev->erase_addr_mask = %" PRIx32 "", addr, ~dev->sec_addr_mask);
        DEBUG("mtd_spi_nor_erase: ERR: erase addr not aligned on %" PRIu32 " byte boundary.\n",
              sector_size);
        return -EOVERFLOW;
    }
    if (addr + size > total_size) {
        return -EOVERFLOW;
    }
    if (size % sector_size != 0) {
        return -EOVERFLOW;
    }

    mtd_spi_acquire(dev);
    while (size) {
        uint32_t us;

        /* write enable */
        mtd_spi_cmd(dev, dev->params->opcode->wren);

        if (size == total_size) {
            mtd_spi_cmd(dev, dev->params->opcode->chip_erase);
            size -= total_size;
            us = dev->wait_chip_erase;
        }
        else if ((dev->flag & SPI_NOR_F_SECT_64K) && (size >= MTD_64K) &&
                 ((addr & MTD_64K_ADDR_MASK) == 0)) {
            /* 64 KiB blocks can be erased with block erase command */
            mtd_spi_cmd_addr_write(dev, dev->params->opcode->block_erase_64k, addr, NULL, 0);
            addr += MTD_64K;
            size -= MTD_64K;
            us = dev->wait_64k_erase;
        }
        else if ((dev->flag & SPI_NOR_F_SECT_32K) && (size >= MTD_32K) &&
                 ((addr & MTD_32K_ADDR_MASK) == 0)) {
            /* 32 KiB blocks can be erased with block erase command */
            mtd_spi_cmd_addr_write(dev, dev->params->opcode->block_erase_32k, addr, NULL, 0);
            addr += MTD_32K;
            size -= MTD_32K;
            us = dev->wait_32k_erase;
        }
        else if ((dev->flag & SPI_NOR_F_SECT_4K) && (size >= MTD_4K) &&
                 ((addr & MTD_4K_ADDR_MASK) == 0)) {
            /* 4 KiB sectors can be erased with sector erase command */
            mtd_spi_cmd_addr_write(dev, dev->params->opcode->sector_erase, addr, NULL, 0);
            addr += MTD_4K;
            size -= MTD_4K;
            us = dev->wait_sector_erase;
        }
        else {
            /* no suitable erase block found */
            assert(0);

            mtd_spi_release(dev);
            return -EINVAL;
        }

        /* waiting for the command to complete before continuing */
        wait_for_write_complete(dev, us);
    }
    mtd_spi_release(dev);

    return 0;
}

const mtd_desc_t mtd_spi_nor_driver = {
    .init = mtd_spi_nor_init,
    .read = mtd_spi_nor_read,
    .write = mtd_spi_nor_write,
    .write_page = mtd_spi_nor_write_page,
    .erase = mtd_spi_nor_erase,
    .power = mtd_spi_nor_power,
};
