/*
 * Copyright (C) 2016 Eistec AB
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
 *
 * @}
 */

#include <stdint.h>
#include <errno.h>

#include "mtd.h"
#if MODULE_XTIMER
#include "xtimer.h"
#else
#include "thread.h"
#endif
#include "byteorder.h"
#include "mtd_spi_nor.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"
#define ENABLE_TRACE    (0)

#if ENABLE_TRACE
#define TRACE(...) DEBUG(__VA_ARGS__)
#else
#define TRACE(...)
#endif

static mtd_sta_t mtd_spi_nor_init(mtd_dev_t *mtd);
static int mtd_spi_nor_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t size);
static int mtd_spi_nor_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t size);
static int mtd_spi_nor_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t size);
static int mtd_spi_nor_ioctl(mtd_dev_t *mtd, unsigned char ctrl, void *buff);

const mtd_desc_t mtd_spi_nor_driver = {
    .init = mtd_spi_nor_init,
    .read = mtd_spi_nor_read,
    .write = mtd_spi_nor_write,
    .erase = mtd_spi_nor_erase,
    .ioctl = mtd_spi_nor_ioctl,
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
static int mtd_spi_cmd_addr_read(mtd_spi_nor_t *dev, uint8_t opcode,
    be_uint32_t addr, void* dest, uint32_t count)
{
    TRACE("mtd_spi_cmd_addr_read: %p, %02x, (%02x %02x %02x %02x), %p, %" PRIu32 "\n",
        (void *)dev, (unsigned int)opcode, addr.u8[0], addr.u8[1], addr.u8[2],
        addr.u8[3], dest, count);
    int status = 0;

    uint8_t *addr_buf = &addr.u8[4 - dev->addr_width];
    if (ENABLE_TRACE) {
        TRACE("mtd_spi_cmd_addr_read: addr:");
        for (unsigned int i = 0; i < dev->addr_width; ++i)
        {
            TRACE(" %02x", addr_buf[i]);
        }
        TRACE("\n");
    }
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);
    do {
        int res;
        /* Send opcode followed by address */
        res = spi_transfer_regs(dev->spi, opcode, (char *)addr_buf, NULL, dev->addr_width);
        if (res < dev->addr_width) {
            DEBUG("mtd_spi_cmd_addr_read: SPI error %d\n", res);
            status = -1;
            break;
        }

        /* Read data */
        res = spi_transfer_bytes(dev->spi, NULL, dest, count);
        if (res != count) {
            DEBUG("mtd_spi_cmd_addr_read: SPI error %d\n", res);
            status = -2;
            break;
        }
    } while(0);

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    return status;
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
static int mtd_spi_cmd_addr_write(mtd_spi_nor_t *dev, uint8_t opcode,
    be_uint32_t addr, const void* src, uint32_t count)
{
    TRACE("mtd_spi_cmd_addr_write: %p, %02x, (%02x %02x %02x %02x), %p, %" PRIu32 "\n",
        (void *)dev, (unsigned int)opcode, addr.u8[0], addr.u8[1], addr.u8[2],
        addr.u8[3], src, count);
    int status = 0;

    uint8_t *addr_buf = &addr.u8[4 - dev->addr_width];
    if (ENABLE_TRACE) {
        TRACE("mtd_spi_cmd_addr_write: addr:");
        for (unsigned int i = 0; i < dev->addr_width; ++i)
        {
            TRACE(" %02x", addr_buf[i]);
        }
        TRACE("\n");
    }
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    do {
        int res;
        /* Send opcode followed by address */
        res = spi_transfer_regs(dev->spi, opcode, (char *)addr_buf, NULL, dev->addr_width);
        if (res < dev->addr_width) {
            DEBUG("mtd_spi_cmd_addr_write: SPI error %d\n", res);
            status = -1;
            break;
        }

        /* Write data */
        if (count > 0) {
            res = spi_transfer_bytes(dev->spi, (void *)src, NULL, count);
            if (res != count) {
                DEBUG("mtd_spi_cmd_addr_write: SPI error %d\n", res);
                status = -2;
                break;
            }
        }
    } while(0);

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);

    return status;
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
static int mtd_spi_cmd_read(mtd_spi_nor_t *dev, uint8_t opcode, void* dest, uint32_t count)
{
    TRACE("mtd_spi_cmd_read: %p, %02x, %p, %" PRIu32 "\n",
        (void *)dev, (unsigned int)opcode, dest, count);
    int status = 0;
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    int res = spi_transfer_regs(dev->spi, opcode, NULL, dest, count);
    if (res < count) {
        DEBUG("mtd_spi_cmd_read: SPI error %d\n", res);
        status = -1;
    }

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
    return status;
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
static int __attribute__((unused)) mtd_spi_cmd_write(mtd_spi_nor_t *dev, uint8_t opcode, const void* src, uint32_t count)
{
    TRACE("mtd_spi_cmd_write: %p, %02x, %p, %" PRIu32 "\n",
        (void *)dev, (unsigned int)opcode, src, count);
    int status = 0;
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    int res = spi_transfer_regs(dev->spi, opcode, (void *)src, NULL, count);
    if (res < count) {
        DEBUG("mtd_spi_cmd_write: SPI error %d\n", res);
        status = -1;
    }

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
    return status;
}

/**
 * @internal
 * @brief Send command opcode
 *
 * @param[in]  dev    pointer to device descriptor
 * @param[in]  opcode command opcode
 */
static int mtd_spi_cmd(mtd_spi_nor_t *dev, uint8_t opcode)
{
    TRACE("mtd_spi_cmd: %p, %02x\n",
        (void *)dev, (unsigned int)opcode);
    int status = 0;
    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    int res = spi_transfer_byte(dev->spi, opcode, NULL);
    if (res < 1) {
        DEBUG("mtd_spi_cmd_write: SPI error %d\n", res);
        status = -1;
    }

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
    return status;
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
static int mtd_spi_read_jedec_id(mtd_spi_nor_t *dev, mtd_jedec_id_t *out)
{
    /* not using above read functions because of variable length rdid response */
    int status = 0;
    int res;
    mtd_jedec_id_t jedec;

    DEBUG("mtd_spi_read_jedec_id: rdid=0x%02x\n", (unsigned int)dev->opcode->rdid);

    /* Acquire exclusive access to the bus. */
    spi_acquire(dev->spi);
    /* Perform the transaction */
    gpio_clear(dev->cs);

    /* Send opcode */
    res = spi_transfer_byte(dev->spi, dev->opcode->rdid, NULL);
    if (res < 1) {
            DEBUG("mtd_spi_read_jedec_id: SPI error %d\n", res);
        status = -1;
    }

    /* Read manufacturer ID */
    jedec.bank = 1;
    while (status == 0) {
        res = spi_transfer_byte(dev->spi, 0, (char *)&jedec.manuf);
        if (res < 1) {
            DEBUG("mtd_spi_read_jedec_id: SPI error %d\n", res);
            status = -1;
            break;
        }
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
        res = spi_transfer_bytes(dev->spi, NULL, (char *)&jedec.device[0], sizeof(jedec.device));
        if (res < 1) {
            DEBUG("mtd_spi_read_jedec_id: SPI error %d\n", res);
            status = -1;
        }
    }
    DEBUG("mtd_spi_read_jedec_id: device=0x%02x, 0x%02x\n",
        (unsigned int)jedec.device[0], (unsigned int)jedec.device[1]);

    if (status == 0) {
        *out = jedec;
    }

    gpio_set(dev->cs);
    /* Release the bus for other threads. */
    spi_release(dev->spi);
    return status;
}

static inline int wait_for_write_complete(mtd_spi_nor_t *dev)
{
    do {
        uint8_t status;
        int res = mtd_spi_cmd_read(dev, dev->opcode->rdsr, &status, sizeof(status));
        if (res < 0) {
            DEBUG("mtd_spi_nor: SPI error %d\n", res);
            return -1;
        }
        TRACE("mtd_spi_nor: wait device status = 0x%02x\n", (unsigned int)status);
        if ((status & 1) == 0) { /* TODO magic number */
            break;
        }
        #if MODULE_XTIMER
        xtimer_usleep(50 * MS_IN_USEC); /* TODO magic number */
        #else
        thread_yield();
        #endif
    } while (1);
    return 0;
}

static mtd_sta_t mtd_spi_nor_init(mtd_dev_t *mtd)
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
        return MTD_STA_NOINIT;
    }

    /* CS */
    DEBUG("mtd_spi_nor_init: CS init\n");
    gpio_init(dev->cs, GPIO_OUT);
    gpio_set(dev->cs);

    int res = mtd_spi_read_jedec_id(dev, &dev->jedec_id);
    if (res < 0) {
        return MTD_STA_NOINIT;
    }
    DEBUG("mtd_spi_nor_init: Found chip with ID: (%d, 0x%02x, 0x%02x, 0x%02x)\n",
        dev->jedec_id.bank, dev->jedec_id.manuf, dev->jedec_id.device[0], dev->jedec_id.device[1]);

    uint8_t status;
    res = mtd_spi_cmd_read(dev, dev->opcode->rdsr, &status, sizeof(status));
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

    return MTD_STA_INIT;
}

static int mtd_spi_nor_read(mtd_dev_t *mtd, void *dest, uint32_t addr, uint32_t size)
{
    DEBUG("mtd_spi_nor_read: %p, %p, 0x%" PRIx32 ", 0x%" PRIx32 "\n",
        (void *)mtd, dest, addr, size);
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;
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
    int res = mtd_spi_cmd_addr_read(dev, dev->opcode->read, addr_be, dest, size);
    if (res < 0) {
        DEBUG("mtd_spi_nor_read: SPI error %d\n", res);
        return -EIO;
    }
    return size;
}

static int mtd_spi_nor_write(mtd_dev_t *mtd, const void *src, uint32_t addr, uint32_t size)
{
    DEBUG("mtd_spi_nor_write: %p, %p, 0x%" PRIx32 ", 0x%" PRIx32 "\n",
        (void *)mtd, src, addr, size);
    if (size == 0) {
        return 0;
    }
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;
    if (size > mtd->page_size) {
        DEBUG("mtd_spi_nor_write: ERR: page program >1 page (%" PRIu32 ")!\n", mtd->page_size);
        return -EOVERFLOW;
    }
    if (dev->page_addr_mask &&
        ((addr & dev->page_addr_mask) != ((addr + size - 1) & dev->page_addr_mask))) {
        DEBUG("mtd_spi_nor_write: ERR: page program spans page boundary!\n");
        return -EOVERFLOW;
    }
    be_uint32_t addr_be = byteorder_htonl(addr);

    int res;
    /* write enable */
    res = mtd_spi_cmd(dev, dev->opcode->wren);
    if (res < 0) {
        DEBUG("mtd_spi_nor_write: wren SPI error %d\n", res);
        return -EIO;
    }
    /* Page program */
    res = mtd_spi_cmd_addr_write(dev, dev->opcode->page_program, addr_be, src, size);
    if (res < 0) {
        DEBUG("mtd_spi_nor_write: SPI error %d\n", res);
        return -EIO;
    }

    /* waiting for the command to complete before returning */
    wait_for_write_complete(dev);
    return size;
}

static int mtd_spi_nor_erase(mtd_dev_t *mtd, uint32_t addr, uint32_t size)
{
    DEBUG("mtd_spi_nor_erase: %p, 0x%" PRIx32 ", 0x%" PRIx32 "\n",
        (void *)mtd, addr, size);
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)mtd;
    uint32_t sector_size = mtd->page_size * mtd->pages_per_sector;
    if (size != sector_size) {
        DEBUG("mtd_spi_nor_erase: ERR: erase size != sector size (%" PRIu32 " != %" PRIu32 ")!\n",
            size, sector_size);
        return -EINVAL;
    }
    if (dev->sec_addr_mask &&
        ((addr & ~dev->sec_addr_mask) != 0)) {
        /* This is not a requirement in hardware, but it helps in catching
         * software bugs (the erase-all-your-files kind) */
        DEBUG("addr = %" PRIx32 " ~dev->sec_addr_mask = %" PRIx32 "", addr, ~dev->sec_addr_mask);
        DEBUG("mtd_spi_nor_erase: ERR: erase addr not aligned on %" PRIu32 " byte boundary.\n",
            sector_size);
        return -EINVAL;
    }
    be_uint32_t addr_be = byteorder_htonl(addr);

    int res;
    /* write enable */
    res = mtd_spi_cmd(dev, dev->opcode->wren);
    if (res < 0) {
        DEBUG("mtd_spi_nor_erase: wren SPI error %d\n", res);
        return -EIO;
    }
    res = mtd_spi_cmd_addr_write(dev, dev->opcode->sector_erase, addr_be, NULL, 0);
    if (res < 0) {
        DEBUG("mtd_spi_nor_erase: SPI error %d\n", res);
        return -EIO;
    }

    /* waiting for the command to complete before returning */
    wait_for_write_complete(dev);
    return 0;
}

static int mtd_spi_nor_ioctl(mtd_dev_t *mtd, unsigned char ctrl, void *buf)
{
    int ret = -EINVAL;
    switch (ctrl) {
        case MTD_CTRL_POWER:
            /* TODO: Do something */
            break;
        default:
            break;
    }
    return ret;
}
