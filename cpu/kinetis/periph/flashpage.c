/*
 * Copyright (C) 2019 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_kinetis
 * @ingroup     drivers_periph_flashpage
 * @{
 *
 * @file
 * @brief       Low-level flashpage driver implementation
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "cpu.h"
#include "assert.h"

#include "periph/flashpage.h"

#define ENABLE_DEBUG                0
#include "debug.h"

/**
 * @brief   Flash controller commands
 */
#define FTFx_VERIFY_SECTION         (0x01U)     /* RD1SEC */
#define FTFx_ERASE_SECTOR           (0x09U)     /* ERSSCR */
#define FTFx_PROGRAM_LONGWORD       (0x06U)     /* PGM4 */
#define FTFx_PROGRAM_PHRASE         (0x07U)     /* PGM8 */

/**
 * @brief   Kinetis Flash Memory Module common registers
 */
#if defined(FTFA)
#define FTFx                        FTFA
#define FTFx_BASE                   FTFA_BASE
#define FTFx_FSTAT_CCIF_MASK        FTFA_FSTAT_CCIF_MASK
#define FTFx_FSTAT_RDCOLERR_MASK    FTFA_FSTAT_RDCOLERR_MASK
#define FTFx_FSTAT_ACCERR_MASK      FTFA_FSTAT_ACCERR_MASK
#define FTFx_FSTAT_FPVIOL_MASK      FTFA_FSTAT_FPVIOL_MASK
#define FTFx_FSTAT_MGSTAT0_MASK     FTFA_FSTAT_MGSTAT0_MASK
#define FTFx_FSEC_SEC_MASK          FTFA_FSEC_SEC_MASK
#define FTFx_FSEC_KEYEN_MASK        FTFA_FSEC_KEYEN_MASK
#elif defined(FTFE)
#define FTFx                        FTFE
#define FTFx_BASE                   FTFE_BASE
#define FTFx_FSTAT_CCIF_MASK        FTFE_FSTAT_CCIF_MASK
#define FTFx_FSTAT_RDCOLERR_MASK    FTFE_FSTAT_RDCOLERR_MASK
#define FTFx_FSTAT_ACCERR_MASK      FTFE_FSTAT_ACCERR_MASK
#define FTFx_FSTAT_FPVIOL_MASK      FTFE_FSTAT_FPVIOL_MASK
#define FTFx_FSTAT_MGSTAT0_MASK     FTFE_FSTAT_MGSTAT0_MASK
#define FTFx_FSEC_SEC_MASK          FTFE_FSEC_SEC_MASK
#define FTFx_FSEC_KEYEN_MASK        FTFE_FSEC_KEYEN_MASK
#elif defined(FTFL)
#define FTFx                        FTFL
#define FTFx_BASE                   FTFL_BASE
#define FTFx_FSTAT_CCIF_MASK        FTFL_FSTAT_CCIF_MASK
#define FTFx_FSTAT_RDCOLERR_MASK    FTFL_FSTAT_RDCOLERR_MASK
#define FTFx_FSTAT_ACCERR_MASK      FTFL_FSTAT_ACCERR_MASK
#define FTFx_FSTAT_FPVIOL_MASK      FTFL_FSTAT_FPVIOL_MASK
#define FTFx_FSTAT_MGSTAT0_MASK     FTFL_FSTAT_MGSTAT0_MASK
#define FTFx_FSEC_SEC_MASK          FTFL_FSEC_SEC_MASK
#define FTFx_FSEC_KEYEN_MASK        FTFL_FSEC_KEYEN_MASK
#endif

#define BYTES_JOIN_TO_WORD_1_3(x, y) ((((uint32_t)(x) & 0xFFU) << 24) | \
                                      ((uint32_t)(y) & 0xFFFFFFU))
#define BYTES_JOIN_TO_WORD_2_1_1(x, y, z) \
    ((((uint32_t)(x) & 0xFFFFU) << 16) | (((uint32_t)(y) & 0xFFU) << 8) | \
     ((uint32_t)(z) & 0xFFU))

/**
 * @brief   Flash errors
 */
enum {
    NO_ERROR            =  0,
    PROTECTION_ERROR    = -1,
    ALIGN_ERROR         = -2,
    ACCESS_ERROR        = -3,
    COLLISION_ERROR     = -4,
    RUNTIME_ERROR       = -5,
    ERASE_ERROR         = -6,
};

/* Pointer to FCCOB register */
volatile uint32_t *const FCCOBx = (volatile uint32_t *)&FTFx->FCCOB3;

/* Erasing/Programming flash is not allowed inside the same flash block
   where the program is being read, for _run_command to be executed in
   RAM to avoid this, more details in:
   https://www.nxp.com/docs/en/application-note/AN4695.pdf */
__attribute__ ((section (".ramfunc")))
static void _run_command(void)
{
    /* Clear previous errors */
    FTFx->FSTAT = FTFx_FSTAT_FPVIOL_MASK | FTFx_FSTAT_ACCERR_MASK |
                  FTFx_FSTAT_RDCOLERR_MASK;
    /* Start Operation */
    FTFx->FSTAT = FTFx_FSTAT_CCIF_MASK;
    /* Wait for CCIF to be set (operation finished) */
    while (!(FTFx->FSTAT & FTFx_FSTAT_CCIF_MASK)) {}
}

static int _check_errors(void)
{
    if (FTFx->FSTAT & FTFx_FSTAT_FPVIOL_MASK) {
        DEBUG("[flash_write]: flash protection violation\n");
        return PROTECTION_ERROR;
    }
    if (FTFx->FSTAT & FTFx_FSTAT_ACCERR_MASK) {
        DEBUG("[flash_write]: flash access error\n");
        return ACCESS_ERROR;
    }
    if (FTFx->FSTAT & FTFx_FSTAT_RDCOLERR_MASK) {
        DEBUG("[flash_write]: collision error\n");
        return COLLISION_ERROR;
    }
    if (FTFx->FSTAT & FTFx_FSTAT_MGSTAT0_MASK) {
        DEBUG("[flash_write]: runtime error\\n");
        return RUNTIME_ERROR;
    }
    DEBUG("[flash_write]: no error reported\n");
    return 0;
}

static int _verify_erased(uint32_t address, size_t len)
{
    /* Ensures verifies are aligned */
    assert(!(address % FLASHPAGE_RAW_SECTION_ALIGNMENT));

    /* We verify FLASHPAGE_RAW_SECTION_ALIGNMENT Bytes at a time, we
       round up before adjusting to FLASHPAGE_RAW_SECTION_ALIGNMENT */
    uint32_t num = (len + (FLASHPAGE_RAW_SECTION_ALIGNMENT - 1)) /
                   FLASHPAGE_RAW_SECTION_ALIGNMENT;

    /* Setup command and run */
    FCCOBx[0] = BYTES_JOIN_TO_WORD_1_3(FTFx_VERIFY_SECTION, address);
    FCCOBx[1] = BYTES_JOIN_TO_WORD_2_1_1(num, 0x00, 0xFF);
    _run_command();

    return _check_errors();
}

static int _sector_erase(uint32_t address)
{
    /* Ensures erases are aligned */
    assert(!(address % FLASHPAGE_RAW_SECTION_ALIGNMENT));

    /* Setup command and run */
    FCCOBx[0] = BYTES_JOIN_TO_WORD_1_3(FTFx_ERASE_SECTOR, address);
    _run_command();

    return _check_errors();
}

static int _flash_write(uint32_t address, uint32_t *data)
{
    /* Setup command and run */
    FCCOBx[1] = *data++;
    if (FLASHPAGE_RAW_BLOCKSIZE == 8U) {
        FCCOBx[2] = *data;
        FCCOBx[0] = BYTES_JOIN_TO_WORD_1_3(FTFx_PROGRAM_PHRASE, address);
    }
    else {
        FCCOBx[0] = BYTES_JOIN_TO_WORD_1_3(FTFx_PROGRAM_LONGWORD, address);
    }
    _run_command();

    return _check_errors();
}

void flashpage_write_raw(void *target_addr, const void *data, size_t len)
{
    /* Assert multiples of FLASHPAGE_RAW_BLOCKSIZE are written and no less of
       that length. */
    assert(!(len % FLASHPAGE_RAW_BLOCKSIZE));

    /* Ensure writes are aligned */
    assert(!(((unsigned)target_addr % FLASHPAGE_RAW_ALIGNMENT) ||
             ((unsigned)data % FLASHPAGE_RAW_ALIGNMENT)));

    /* Ensure the length doesn't exceed the actual flash size */
    assert(((unsigned)target_addr + len) <
           (CPU_FLASH_BASE + (FLASHPAGE_SIZE * FLASHPAGE_NUMOF)) + 1);

#ifdef FLASHPAGE_RAW_PHRASE
    const uint64_t *data_addr = data;
    uint64_t *dst = target_addr;
#else
    const uint32_t *data_addr = data;
    uint32_t *dst = target_addr;
#endif

    /* Verify sector was previously erased */
    if (_verify_erased((uint32_t)dst, len) != NO_ERROR) {
        DEBUG("[flash-write]: ERROR sector not in erased state\n");
        return;
    }

    /* Write to flash FLASHPAGE_RAW_BLOCKSIZE bytes at a time */
    for (size_t i = 0; i < (len / FLASHPAGE_RAW_BLOCKSIZE); i++) {
        _flash_write((uint32_t) dst++, (uint32_t *) data_addr++);
    }

}

void flashpage_write(int page, const void *data)
{
    assert(page < (int)FLASHPAGE_NUMOF);

    uint32_t *page_addr = flashpage_addr(page);

    /* ERASE sector */
    _sector_erase((uint32_t)page_addr);

    /* WRITE sector */
    if (data != NULL) {
        flashpage_write_raw(page_addr, data, FLASHPAGE_SIZE);
    }
}
