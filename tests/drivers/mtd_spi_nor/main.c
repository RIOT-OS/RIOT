/*
 * Copyright (C) 2024 Technische Universität Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       Test for the MTD SPI NOR Driver
 *
 * @author      Christopher Büchse <christopher.buechse@tuhh.de>
 */

#include <string.h>
#include <errno.h>

#include "busy_wait.h"
#include "embUnit.h"

#include "mtd_spi_nor.h"
#include "../../drivers/mtd_spi_nor/include/mtd_spi_nor_defines.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define ENABLE_TRACE    0
#define TRACE(...)      DEBUG(__VA_ARGS__)

#define TEST_MTD mtd_dev_get(MTD_NUMOF-1) /* Make sure to get the last device */

/* This function was copied from drivers/mtd_spi_nor/mtd_spi_nor.c */
static inline spi_t _get_spi(const mtd_spi_nor_t *dev)
{
    return dev->params->spi;
}

/* This function was copied from drivers/mtd_spi_nor/mtd_spi_nor.c */
static void mtd_spi_cmd(const mtd_spi_nor_t *dev, uint8_t opcode)
{
    if (IS_ACTIVE(ENABLE_TRACE)) {
        TRACE("mtd_spi_cmd: %p, %02x\n",
              (void *)dev, (unsigned int)opcode);
    }
    spi_transfer_byte(_get_spi(dev), dev->params->cs, false, opcode);
}

/* This function was copied from drivers/mtd_spi_nor/mtd_spi_nor.c */
static void mtd_spi_cmd_read(const mtd_spi_nor_t *dev, uint8_t opcode, void *dest, uint32_t count)
{
    if (IS_ACTIVE(ENABLE_TRACE)) {
        TRACE("mtd_spi_cmd_read: %p, %02x, %p, %" PRIu32 "\n",
              (void *)dev, (unsigned int)opcode, dest, count);
    }
    spi_transfer_regs(_get_spi(dev), dev->params->cs, opcode, NULL, dest, count);
}

/* This function was copied from drivers/mtd_spi_nor/mtd_spi_nor.c */
static void mtd_spi_cmd_write(const mtd_spi_nor_t *dev, uint8_t opcode,
                              const void *src, uint32_t count)
{
    if (IS_ACTIVE(ENABLE_TRACE)) {
        TRACE("mtd_spi_cmd_write: %p, %02x, %p, %" PRIu32 "\n",
            (void *)dev, (unsigned int)opcode, src, count);
    }
    spi_transfer_regs(_get_spi(dev), dev->params->cs, opcode,
                      (void *)src, NULL, count);
}

/* This function was copied from drivers/mtd_spi_nor/mtd_spi_nor.c */
static inline void wait_for_write_enable_set(const mtd_spi_nor_t *dev)
{
    do {
        uint8_t status;
        mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status, sizeof(status));

        if (IS_ACTIVE(ENABLE_TRACE)) {
            TRACE("mtd_spi_nor: wait device status = 0x%02x, waiting WEL-Flag\n",
                  (unsigned int)status);
        }
        if (status & SPI_NOR_STATUS_WEL) {
            break;
        }
        thread_yield();
    } while (1);
}

/* This function was copied from drivers/mtd_spi_nor/mtd_spi_nor.c */
static void mtd_spi_acquire(const mtd_spi_nor_t *dev)
{
    spi_acquire(_get_spi(dev), dev->params->cs,
                dev->params->mode, dev->params->clk);
}

/* This function was copied from drivers/mtd_spi_nor/mtd_spi_nor.c */
static void mtd_spi_release(const mtd_spi_nor_t *dev)
{
    spi_release(_get_spi(dev));
}

static void setup(void)
{
}

static void teardown(void)
{
}

static void test_mtd_init(void)
{
    DEBUG("test_mtd_init: Initializing the Device\n");

    int ret = mtd_init(TEST_MTD);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* make sure the Block Protect bits are not set before we start the tests */
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)TEST_MTD;

    uint8_t status_reg;
    const uint8_t bp_flags = SPI_NOR_STATUS_BP0 | SPI_NOR_STATUS_BP1 | \
                             SPI_NOR_STATUS_BP2 | SPI_NOR_STATUS_BP3;

    mtd_init(TEST_MTD);

    /* Revert everything back to the original state */
    mtd_spi_acquire(dev);

    /* check that the Status Register is writable */
    mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status_reg, sizeof(status_reg));
    if (status_reg & SPI_NOR_STATUS_SRWD) {
        status_reg &= !SPI_NOR_STATUS_SRWD;
    }

    /* Send WREN command to write to the Status Register */
    mtd_spi_cmd(dev, dev->params->opcode->wren);
    wait_for_write_enable_set(dev);

    /* Mask the status bits and Block Protection Flags */
    status_reg &= !(SPI_NOR_STATUS_WEL | SPI_NOR_STATUS_WIP);
    status_reg &= !bp_flags;

    mtd_spi_cmd_write(dev, 0x01, &status_reg, sizeof(status_reg)); /* Opcode: WRSR */
    busy_wait_us(15000); /* writing the SR can take up to 15ms */
    mtd_spi_release(dev);
}

static void test_mtd_erase(void)
{
    DEBUG("test_mtd_erase: Erasing the first sector\n");

    const uint32_t sector_size = TEST_MTD->page_size*TEST_MTD->pages_per_sector;
    uint8_t buffer[TEST_MTD->page_size];

    int ret = mtd_erase(TEST_MTD, 0, sector_size);
    TEST_ASSERT_EQUAL_INT(0, ret);

    /* read back the sector and check that it is blank */
    for (uint32_t page = 0; page < TEST_MTD->pages_per_sector; page++) {
        ret = mtd_read_page(TEST_MTD, buffer, page, 0, TEST_MTD->page_size);
        TEST_ASSERT_EQUAL_INT(0, ret);

        uint8_t sum = 0xFF;
        for (uint32_t i = 0; i < TEST_MTD->page_size; i++) {
            sum &= buffer[i];
        }
        TEST_ASSERT_EQUAL_INT(0xFF, sum);
    }
}

static void test_mtd_block_protect(void)
{
    int ret;
    const char test_data[16] = "BLOCK TEST";
    const uint8_t bp_flags = SPI_NOR_STATUS_BP0 | SPI_NOR_STATUS_BP1 | \
                             SPI_NOR_STATUS_BP2 | SPI_NOR_STATUS_BP3;

    uint8_t status_reg;
    mtd_spi_nor_t *dev = (mtd_spi_nor_t *)TEST_MTD;

    if (!(IS_USED(MODULE_MTD_SPI_NOR_MX_SECURITY) || IS_USED(MODULE_MTD_SPI_NOR_ISSI_SECURITY))) {
        DEBUG("test_mtd_block_protect: No security features enabled, skip test.\n");
        return;
    }

    DEBUG("test_mtd_block_protect: Checking the Block Protect Functions\n");

    mtd_spi_acquire(dev);

    /* check that the Status Register is writable */
    mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status_reg, sizeof(status_reg));
    if (status_reg & SPI_NOR_STATUS_SRWD) {
        status_reg &= !SPI_NOR_STATUS_SRWD;
    }

    /* Mask the status bits */
    status_reg &= !(SPI_NOR_STATUS_WEL | SPI_NOR_STATUS_WIP);

    /* Send WREN command to write to the Status Register */
    mtd_spi_cmd(dev, dev->params->opcode->wren);
    wait_for_write_enable_set(dev);

    /* read Status Register again */
    mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status_reg, sizeof(status_reg));
    TEST_ASSERT_EQUAL_INT(0, status_reg & SPI_NOR_STATUS_SRWD);
    TEST_ASSERT_EQUAL_INT(SPI_NOR_STATUS_WEL, status_reg & SPI_NOR_STATUS_WEL);

    /* protect all blocks */
    status_reg |= bp_flags;
    mtd_spi_cmd_write(dev, 0x01, &status_reg, sizeof(status_reg)); /* Opcode WRSR */
    busy_wait_us(15000); /* writing the SR can take up to 15ms */

    /* confirm that the Block Protection was actually set */
    mtd_spi_cmd_read(dev, dev->params->opcode->rdsr, &status_reg, sizeof(status_reg));
    TEST_ASSERT_EQUAL_INT(bp_flags, status_reg & bp_flags);

    mtd_spi_release(dev);

    /* Perform a write test to check if the P_FAIL flag is correctly handled */
    ret = mtd_write_page_raw(TEST_MTD, test_data, 0x00000000, 0x00, sizeof(test_data));
    TEST_ASSERT_EQUAL_INT(-EIO, ret);

    /* Perform an erase test to check if the E_FAIL flag is correctly handled */
    ret = mtd_erase(TEST_MTD, 0x00000000, dev->base.page_size * dev->base.pages_per_sector);
    TEST_ASSERT_EQUAL_INT(-EIO, ret);

    /* Revert everything back to the original state */
    mtd_spi_acquire(dev);

    /* Send WREN command to write to the Status Register */
    mtd_spi_cmd(dev, dev->params->opcode->wren);
    wait_for_write_enable_set(dev);

    /* Mask the status bits and Block Protection Flags */
    status_reg &= !(SPI_NOR_STATUS_WEL | SPI_NOR_STATUS_WIP);
    status_reg &= !bp_flags;

    mtd_spi_cmd_write(dev, 0x01, &status_reg, sizeof(status_reg)); /* Opcode: WRSR */
    busy_wait_us(15000); /* writing the SR can take up to 15ms */
    mtd_spi_release(dev);
}

Test *tests_mtd_spi_nor_tests(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_mtd_init),
        new_TestFixture(test_mtd_erase),

        new_TestFixture(test_mtd_block_protect),
    };

    EMB_UNIT_TESTCALLER(mtd_mtd_spi_nor_tests, setup, teardown, fixtures);

    return (Test *)&mtd_mtd_spi_nor_tests;
}

int main(void)
{
    DEBUG("tests/mtd_spi_nor: %u MTD devices present, selected device: %u\n",
           (uint8_t)MTD_NUMOF, (uint8_t)(MTD_NUMOF-1));

    TESTS_START();
    TESTS_RUN(tests_mtd_spi_nor_tests());
    TESTS_END();
    return 0;
}
/** @} */
