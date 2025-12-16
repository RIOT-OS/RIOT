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

#include "busy_wait.h"
#include "byteorder.h"
#include "kernel_defines.h"
#include "macros/math.h"
#include "macros/utils.h"
#include "mtd.h"
#include "mtd_spi_nor.h"
#include "time_units.h"
#include "thread.h"
#include "xtimer.h"

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

#define SFLASH_CMD_ULBPR       (0x98)   /**< Global Block Protection Unlock */

#define MTD_64K             (65536ul)
#define MTD_64K_ADDR_MASK   (0xFFFF)
#define MTD_32K             (32768ul)
#define MTD_32K_ADDR_MASK   (0x7FFF)
#define MTD_4K              (4096ul)
#define MTD_4K_ADDR_MASK    (0xFFF)

#define MBIT_AS_BYTES       ((1024 * 1024) / 8)

/**
 * @brief   JEDEC memory manufacturer ID codes.
 *
 *          see http://www.softnology.biz/pdf/JEP106AV.pdf
 * @{
 */
#define JEDEC_BANK(n)   ((n) << 8)

typedef enum {
    SPI_NOR_JEDEC_ATMEL = 0x1F | JEDEC_BANK(1),
    SPI_NOR_JEDEC_MICROCHIP = 0xBF | JEDEC_BANK(1),
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
            DEBUG_PUTS("mtd_spi_read_jedec_id: bank out of bounds\n");
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
        /* capacity encoded as power of 32k sectors */
        return (32 * 1024) << (0x1F & id->device[0]);
    }
    if (mtd_spi_manuf_match(id, SPI_NOR_JEDEC_MICROCHIP)) {
        switch (id->device[1]) {
        case 0x12:  /* SST26VF020A */
        case 0x8c:  /* SST25VF020B */
            return 2 * MBIT_AS_BYTES;
        case 0x54:  /* SST26WF040B */
        case 0x8d:  /* SST25VF040B */
            return 4 * MBIT_AS_BYTES;
        case 0x58:  /* SST26WF080B */
        case 0x8e:  /* SST25VF080B */
            return 8 * MBIT_AS_BYTES;
        case 0x1:   /* SST26VF016  */
        case 0x41:  /* SST26VF016B */
            return 16 * MBIT_AS_BYTES;
        case 0x2:   /* SST26VF032  */
        case 0x42:  /* SST26VF032B */
            return 32 * MBIT_AS_BYTES;
        case 0x43:  /* SST26VF064B */
        case 0x53:  /* SST26WF064C */
            return 64 * MBIT_AS_BYTES;
        }
    }

    /* everyone else seems to use device ID 2 for density */
    return 1 << id->device[1];
}

static inline void wait_for_write_complete(const mtd_spi_nor_t *dev, uint32_t us)
{
    unsigned i = 0, j = 0;
    uint32_t div = 1; /* first wait one full interval */
#if IS_ACTIVE(ENABLE_DEBUG)
    uint32_t diff = xtimer_now_usec();
#endif
    do {
        uint8_t status;
        mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status, sizeof(status));

        TRACE("mtd_spi_nor: wait device status = 0x%02x\n", (unsigned int)status);
        if ((status & 1) == 0) { /* TODO magic number */
            break;
        }
        i++;
        if (us) {
            uint32_t wait_us = us / div;
            uint32_t wait_min = 2;

            wait_us = wait_us > wait_min ? wait_us : wait_min;

            xtimer_usleep(wait_us);
            /* reduce the waiting time quickly if the estimate was too short,
             * but still avoid busy (yield) waiting */
            div++;
        }
        else {
            j++;
            thread_yield();
        }
    } while (1);
    DEBUG("wait loop %u times, yield %u times", i, j);
#if IS_ACTIVE(ENABLE_DEBUG)
    diff = xtimer_now_usec() - diff;
    DEBUG(", total wait %"PRIu32"us\n", diff);
#endif
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

static void _enable_32bit_addr(mtd_spi_nor_t *dev)
{
    mtd_spi_cmd(dev, dev->params->opcode->wren);
    mtd_spi_cmd(dev, SFLASH_CMD_4_BYTE_ADDR);
}

static int mtd_spi_nor_power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;

    mtd_spi_acquire(dev);
    switch (power) {
        case MTD_POWER_UP:
            mtd_spi_cmd(dev, dev->params->opcode->wake);

            unsigned retries = MTD_POWER_UP_WAIT_FOR_ID;
            int res = 0;
            do {
                xtimer_usleep(dev->params->wait_chip_wake_up);
                res = mtd_spi_read_jedec_id(dev, &dev->jedec_id);
            } while (res < 0 && --retries);
            if (res < 0) {
                mtd_spi_release(dev);
                return -EIO;
            }
            /* enable 32 bit address mode */
            if (dev->addr_width == 4) {
                _enable_32bit_addr(dev);
            }

            break;
        case MTD_POWER_DOWN:
            mtd_spi_cmd(dev, dev->params->opcode->sleep);
            break;
    }
    mtd_spi_release(dev);

    return 0;
}

static void _set_addr_width(mtd_dev_t *mtd)
{
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;

    uint32_t flash_size = mtd->pages_per_sector * mtd->page_size
                        * mtd->sector_count;

    if (flash_size > (0x1UL << 24)) {
        dev->addr_width = 4;
    } else {
        dev->addr_width = 3;
    }
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
    DEBUG_PUTS("mtd_spi_nor_init: power up MTD device");
    if (mtd_spi_nor_power(mtd, MTD_POWER_UP)) {
        DEBUG_PUTS("mtd_spi_nor_init: failed to power up MTD device");
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

    /* derive density from JEDEC ID  */
    if (mtd->sector_count == 0) {
        mtd->sector_count = mtd_spi_nor_get_size(&dev->jedec_id)
                          / (mtd->pages_per_sector * mtd->page_size);
    }
    /* SPI NOR is byte addressable; instances don't need to configure that */
    assert(mtd->write_size <= 1);
    mtd->write_size = 1;
    _set_addr_width(mtd);

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
    DEBUG("mtd_spi_nor_init: device status = 0x%02x\n", (unsigned int)status);

    /* enable 32 bit address mode */
    if (dev->addr_width == 4) {
        _enable_32bit_addr(dev);
    }

    /* Global Block-Protection Unlock */
    mtd_spi_cmd(dev, dev->params->opcode->wren);
    mtd_spi_cmd(dev, SFLASH_CMD_ULBPR);

    mtd_spi_release(dev);

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
            us = dev->params->wait_chip_erase;
        }
        else if ((dev->params->flag & SPI_NOR_F_SECT_64K) && (size >= MTD_64K) &&
                 ((addr & MTD_64K_ADDR_MASK) == 0)) {
            /* 64 KiB blocks can be erased with block erase command */
            mtd_spi_cmd_addr_write(dev, dev->params->opcode->block_erase_64k, addr, NULL, 0);
            addr += MTD_64K;
            size -= MTD_64K;
            us = dev->params->wait_64k_erase;
        }
        else if ((dev->params->flag & SPI_NOR_F_SECT_32K) && (size >= MTD_32K) &&
                 ((addr & MTD_32K_ADDR_MASK) == 0)) {
            /* 32 KiB blocks can be erased with block erase command */
            mtd_spi_cmd_addr_write(dev, dev->params->opcode->block_erase_32k, addr, NULL, 0);
            addr += MTD_32K;
            size -= MTD_32K;
            us = dev->params->wait_32k_erase;
        }
        else if ((dev->params->flag & SPI_NOR_F_SECT_4K) && (size >= MTD_4K) &&
                 ((addr & MTD_4K_ADDR_MASK) == 0)) {
            /* 4 KiB sectors can be erased with sector erase command */
            mtd_spi_cmd_addr_write(dev, dev->params->opcode->sector_erase, addr, NULL, 0);
            addr += MTD_4K;
            size -= MTD_4K;
            us = dev->params->wait_sector_erase;
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
    .write_page = mtd_spi_nor_write_page,
    .erase = mtd_spi_nor_erase,
    .power = mtd_spi_nor_power,
};
