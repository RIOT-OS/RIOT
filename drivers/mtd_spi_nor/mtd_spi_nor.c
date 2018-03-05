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
#include <errno.h>

#include "mtd.h"
#if MODULE_XTIMER
#include "xtimer.h"
#include "timex.h"
#else
#include "thread.h"
#endif
#include "byteorder.h"
#include "mtd_spi_nor.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"
#define ENABLE_TRACE    (0)

#if ENABLE_TRACE
#define TRACE(...) DEBUG(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#ifndef MTD_SPI_NOR_WRITE_WAIT_US
#define MTD_SPI_NOR_WRITE_WAIT_US (50 * US_PER_MS)
#endif

#define MTD_32K             (32768ul)
#define MTD_32K_ADDR_MASK   (0x7FFF)
#define MTD_4K              (4096ul)
#define MTD_4K_ADDR_MASK    (0xFFF)

static int mtd_spi_nor_init(mtd_dev_t *mtd);
static int mtd_spi_nor_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t size);
static int mtd_spi_nor_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t size);
static int mtd_spi_nor_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t size);
static int mtd_spi_nor_power(mtd_dev_t *mtd, enum mtd_power_state power);

const mtd_desc_t mtd_spi_nor_driver = {
    .init = mtd_spi_nor_init,
    .read = mtd_spi_nor_read,
    .write = mtd_spi_nor_write,
    .erase = mtd_spi_nor_erase,
    .power = mtd_spi_nor_power,
};

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
                                  be_uint32_t addr, void *dest, uint32_t count)
{
    TRACE("mtd_spi_cmd_addr_read: %p, %02x, (%02x %02x %02x %02x), %p, %" PRIu32 "\n",
          (void *)dev, (unsigned int)opcode, addr.u8[0], addr.u8[1], addr.u8[2],
          addr.u8[3], dest, count);

    uint8_t *addr_buf = &addr.u8[4 - dev->addr_width];
    if (ENABLE_TRACE) {
        TRACE("mtd_spi_cmd_addr_read: addr:");
        for (unsigned int i = 0; i < dev->addr_width; ++i) {
            TRACE(" %02x", addr_buf[i]);
        }
        TRACE("\n");
    }

    do {
        /* Send opcode followed by address */
        spi_transfer_byte(dev->spi, dev->cs, true, opcode);
        spi_transfer_bytes(dev->spi, dev->cs, true, (char *)addr_buf, NULL, dev->addr_width);

        /* Read data */
        spi_transfer_bytes(dev->spi, dev->cs, false, NULL, dest, count);
    } while (0);
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
                                   be_uint32_t addr, const void *src, uint32_t count)
{
    TRACE("mtd_spi_cmd_addr_write: %p, %02x, (%02x %02x %02x %02x), %p, %" PRIu32 "\n",
          (void *)dev, (unsigned int)opcode, addr.u8[0], addr.u8[1], addr.u8[2],
          addr.u8[3], src, count);

    uint8_t *addr_buf = &addr.u8[4 - dev->addr_width];
    if (ENABLE_TRACE) {
        TRACE("mtd_spi_cmd_addr_write: addr:");
        for (unsigned int i = 0; i < dev->addr_width; ++i) {
            TRACE(" %02x", addr_buf[i]);
        }
        TRACE("\n");
    }

    do {
        /* Send opcode followed by address */
        spi_transfer_byte(dev->spi, dev->cs, true, opcode);
        bool cont = (count > 0); /* only keep CS asserted when there is data that follows */
        spi_transfer_bytes(dev->spi, dev->cs, cont, (char *)addr_buf, NULL, dev->addr_width);
        /* Write data */
        if (cont) {
            spi_transfer_bytes(dev->spi, dev->cs, false, (void *)src, NULL, count);
        }
    } while (0);
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

    spi_transfer_regs(dev->spi, dev->cs, opcode, NULL, dest, count);
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

    spi_transfer_regs(dev->spi, dev->cs, opcode, (void *)src, NULL, count);
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

    spi_transfer_byte(dev->spi, dev->cs, false, opcode);
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
    /* not using above read functions because of variable length rdid response */
    int status = 0;
    mtd_jedec_id_t jedec;

    DEBUG("mtd_spi_read_jedec_id: rdid=0x%02x\n", (unsigned int)dev->opcode->rdid);

    /* Send opcode */
    spi_transfer_byte(dev->spi, dev->cs, true, dev->opcode->rdid);

    /* Read manufacturer ID */
    jedec.bank = 1;
    while (status == 0) {
        jedec.manuf = spi_transfer_byte(dev->spi, dev->cs, true, 0);
        if (jedec.manuf == JEDEC_NEXT_BANK) {
            /* next bank, see JEP106 */
            DEBUG("mtd_spi_read_jedec_id: manuf bank incr\n");
            ++jedec.bank;
            continue;
        }
        if (parity8(jedec.manuf) == 0) {
            /* saw even parity, we expected odd parity => parity error */
            DEBUG("mtd_spi_read_jedec_id: Parity error (0x%02x)\n", (unsigned int)jedec.manuf);
            status = -2;
            break;
        }
        else {
            /* all OK! */
            break;
        }
    }
    DEBUG("mtd_spi_read_jedec_id: bank=%u manuf=0x%02x\n", (unsigned int)jedec.bank,
          (unsigned int)jedec.manuf);

    /* Read device ID */
    if (status == 0) {
        spi_transfer_bytes(dev->spi, dev->cs, false, NULL, (char *)&jedec.device[0], sizeof(jedec.device));
    }
    DEBUG("mtd_spi_read_jedec_id: device=0x%02x, 0x%02x\n",
          (unsigned int)jedec.device[0], (unsigned int)jedec.device[1]);

    if (status == 0) {
        *out = jedec;
    }

    return status;
}

static inline void wait_for_write_complete(const mtd_spi_nor_t *dev)
{
    do {
        uint8_t status;
        mtd_spi_cmd_read(dev, dev->opcode->rdsr, &status, sizeof(status));

        TRACE("mtd_spi_nor: wait device status = 0x%02x\n", (unsigned int)status);
        if ((status & 1) == 0) { /* TODO magic number */
            break;
        }
#if MODULE_XTIMER
        xtimer_usleep(MTD_SPI_NOR_WRITE_WAIT_US);
#else
        thread_yield();
#endif
    } while (1);
}

static int mtd_spi_nor_init(mtd_dev_t *mtd)
{
    DEBUG("mtd_spi_nor_init: %p\n", (void *)mtd);
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;

    DEBUG("mtd_spi_nor_init: -> spi: %lx, cs: %lx, opcodes: %p\n",
          (unsigned long)dev->spi, (unsigned long)dev->cs, (void *)dev->opcode);

    DEBUG("mtd_spi_nor_init: %" PRIu32 " bytes "
          "(%" PRIu32 " sectors, %" PRIu32 " bytes/sector, "
          "%" PRIu32 " pages, "
          "%" PRIu32 " pages/sector, %" PRIu32 " bytes/page)\n",
          mtd->pages_per_sector * mtd->sector_count * mtd->page_size,
          mtd->sector_count, mtd->pages_per_sector * mtd->page_size,
          mtd->pages_per_sector * mtd->sector_count,
          mtd->pages_per_sector, mtd->page_size);
    DEBUG("mtd_spi_nor_init: Using %u byte addresses\n", dev->addr_width);

    if (dev->addr_width == 0) {
        return -EINVAL;
    }

    /* CS */
    DEBUG("mtd_spi_nor_init: CS init\n");
    spi_init_cs(dev->spi, dev->cs);

    spi_acquire(dev->spi, dev->cs, dev->mode, dev->clk);
    int res = mtd_spi_read_jedec_id(dev, &dev->jedec_id);
    if (res < 0) {
        spi_release(dev->spi);
        return -EIO;
    }
    DEBUG("mtd_spi_nor_init: Found chip with ID: (%d, 0x%02x, 0x%02x, 0x%02x)\n",
          dev->jedec_id.bank, dev->jedec_id.manuf, dev->jedec_id.device[0], dev->jedec_id.device[1]);

    uint8_t status;
    mtd_spi_cmd_read(dev, dev->opcode->rdsr, &status, sizeof(status));
    spi_release(dev->spi);

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
    size_t chipsize = mtd->page_size * mtd->pages_per_sector * mtd->sector_count;
    if (addr > chipsize) {
        return -EOVERFLOW;
    }
    if (size > mtd->page_size) {
        size = mtd->page_size;
    }
    if ((addr + size) > chipsize) {
        size = chipsize - addr;
    }
    uint32_t page_addr_mask = dev->page_addr_mask;
    if ((addr & page_addr_mask) != ((addr + size - 1) & page_addr_mask)) {
        /* Reads across page boundaries must be split */
        size = mtd->page_size - (addr & ~(page_addr_mask));
    }
    if (size == 0) {
        return 0;
    }
    be_uint32_t addr_be = byteorder_htonl(addr);

    spi_acquire(dev->spi, dev->cs, dev->mode, dev->clk);
    mtd_spi_cmd_addr_read(dev, dev->opcode->read, addr_be, dest, size);
    spi_release(dev->spi);

    return size;
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
    be_uint32_t addr_be = byteorder_htonl(addr);

    spi_acquire(dev->spi, dev->cs, dev->mode, dev->clk);
    /* write enable */
    mtd_spi_cmd(dev, dev->opcode->wren);

    /* Page program */
    mtd_spi_cmd_addr_write(dev, dev->opcode->page_program, addr_be, src, size);

    /* waiting for the command to complete before returning */
    wait_for_write_complete(dev);

    spi_release(dev->spi);
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

    spi_acquire(dev->spi, dev->cs, dev->mode, dev->clk);
    while (size) {
        be_uint32_t addr_be = byteorder_htonl(addr);
        /* write enable */
        mtd_spi_cmd(dev, dev->opcode->wren);

        if (size == total_size) {
            mtd_spi_cmd(dev, dev->opcode->chip_erase);
            size -= total_size;
        }
        else if ((dev->flag & SPI_NOR_F_SECT_32K) && (size >= MTD_32K) &&
                 ((addr & MTD_32K_ADDR_MASK) == 0)) {
            /* 32 KiB blocks can be erased with block erase command */
            mtd_spi_cmd_addr_write(dev, dev->opcode->block_erase_32k, addr_be, NULL, 0);
            addr += MTD_32K;
            size -= MTD_32K;
        }
        else if ((dev->flag & SPI_NOR_F_SECT_4K) && (size >= MTD_4K) &&
                 ((addr & MTD_4K_ADDR_MASK) == 0)) {
            /* 4 KiB sectors can be erased with sector erase command */
            mtd_spi_cmd_addr_write(dev, dev->opcode->sector_erase, addr_be, NULL, 0);
            addr += MTD_4K;
            size -= MTD_4K;
        }
        else {
            mtd_spi_cmd_addr_write(dev, dev->opcode->block_erase, addr_be, NULL, 0);
            addr += sector_size;
            size -= sector_size;
        }

        /* waiting for the command to complete before continuing */
        wait_for_write_complete(dev);
    }
    spi_release(dev->spi);

    return 0;
}

static int mtd_spi_nor_power(mtd_dev_t *mtd, enum mtd_power_state power)
{
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;

    spi_acquire(dev->spi, dev->cs, dev->mode, dev->clk);
    switch (power) {
        case MTD_POWER_UP:
            mtd_spi_cmd(dev, dev->opcode->wake);
            break;
        case MTD_POWER_DOWN:
            mtd_spi_cmd(dev, dev->opcode->sleep);
            break;
    }
    spi_release(dev->spi);

    return 0;
}
