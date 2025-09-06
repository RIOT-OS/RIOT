/*
 * SPDX-FileCopyrightText: 2016 Leon George
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_cc26xx_cc13xx_definitions
 * @{
 *
 * @file
 * @brief           CC26xx/CC13xx VIMS register definitions
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   FLASH registers
 */
typedef struct {
    reg32_t __reserved1[7]; /**< Reserved */
    reg32_t STAT; /**< FMC and efuse status */
    reg32_t __reserved2; /**< Reserved */
    reg32_t CFG; /**< Config */
    reg32_t SYSCODE_START; /**< syscode start address offset config */
    reg32_t FLASH_SIZE; /**< flash size config */
    reg32_t __reserved3[3]; /**< Reserved */
    reg32_t FWLOCK; /**< firmware lock */
    reg32_t FWFLAG; /**< firmware flags */
    reg32_t __reserved4[0x3EF]; /**< Reserved */
    reg32_t EFUSE; /**< efuse instruction */
    reg32_t EFUSEADDR; /**< efuse address */
    reg32_t DATAUPPER; /**< efuse data - upper */
    reg32_t DATALOWER; /**< efuse data - lower */
    reg32_t EFUSECFG; /**< OCP sysconf */
    reg32_t EFUSESTAT; /**< system status */
    reg32_t ACC; /**< arbitrary instruction count */
    reg32_t BOUNDARY; /**< boundary test register to drive I/O */
    reg32_t EFUSEFLAG; /**< efuse key loaded flag */
    reg32_t EFUSEKEY; /**< efuse key */
    reg32_t EFUSERELEASE; /**< efuese release */
    reg32_t EFUSEPINS; /**< efuse pins */
    reg32_t EFUSECRA; /**< efuse column repair address */
    reg32_t EFUSEREAD; /**< efuse read */
    reg32_t EFUSEPROGRAM; /**< efuse program */
    reg32_t EFUSEERROR; /**< efuse error */
    reg32_t SINGLEBIT; /**< single-bit error status */
    reg32_t TWOBIT; /**< two-bit error status */
    reg32_t SELFTESTCYC; /**< self-test cycles */
    reg32_t SELFTESTSIGN; /**< self-test signature */
    reg32_t __reserved5[0x3ec]; /**< Reserved */
    reg32_t FRDCTL; /**< FMC read control */
    reg32_t FSPRD; /**< FMC read margin control */
    reg32_t FEDACCTL1; /**< FMC error correction control 1 */
    reg32_t __reserved6[4]; /**< Reserved */
    reg32_t FEDACSTAT; /**< FMC error status */
    reg32_t __reserved7[4]; /**< Reserved */
    reg32_t FBPROT; /**< FMC bank protection */
    reg32_t FBSE; /**< FMC sector enable */
    reg32_t FBBUSY; /**< FMC bank busy */
    reg32_t FBAC; /**< FMC bank access control */
    reg32_t FBFALLBACK; /**< FMC bank fallback power */
    reg32_t FBPRDY; /**< FMC bank/pump ready */
    reg32_t FPAC1; /**< FMC pump access control 1 */
    reg32_t FPAC2; /**< FMC pump access control 2 */
    reg32_t FMAC; /**< FMC module access control */
    reg32_t FMSTAT; /**< FMC module status */
    reg32_t __reserved8[3]; /**< Reserved */
    reg32_t FLOCK; /**< FMC flash lock */
    reg32_t __reserved9[6]; /**< Reserved */
    reg32_t FVREADCT; /**< FMC VREADCT trim */
    reg32_t FVHVCT1; /**< FMC VHVCT1 trim */
    reg32_t FVHVCT2; /**< FMC VHVCT2 trim */
    reg32_t FVHVCT3; /**< FMC VHVCT3 trim */
    reg32_t FVNVCT; /**< FMC VNVCT trim */
    reg32_t FVSLP; /**< FMC VSL_P trim */
    reg32_t FVWLCT; /**< FMC VWLCT trim */
    reg32_t FEFUSECTL; /**< FMC efuse control */
    reg32_t FEFUSESTAT; /**< FMC efuse status */
    reg32_t FEFUSEDATA; /**< FMC efuse data */
    reg32_t FSEQPMP; /**< FMC sequential pump information */
    reg32_t __reserved10[21]; /**< Reserved */
    reg32_t FBSTROBES; /**< FMC bank signal strobe */
    reg32_t FPSTROBES; /**< FMC pump signal strobe */
    reg32_t FBMODE; /**< FMC bank and pump mode */
    reg32_t FTCR; /**< FMC test command control */
    reg32_t FADDR; /**< FMC bank address */
    reg32_t __reserved11[2]; /**< Reserved */
    reg32_t FTCTL; /**< FMC test control */
    reg32_t FWPWRITE0; /**< FMC flash wide programming write data 0 */
    reg32_t FWPWRITE1; /**< FMC flash wide programming write data 1 */
    reg32_t FWPWRITE2; /**< FMC flash wide programming write data 2 */
    reg32_t FWPWRITE3; /**< FMC flash wide programming write data 3 */
    reg32_t FWPWRITE4; /**< FMC flash wide programming write data 4 */
    reg32_t FWPWRITE5; /**< FMC flash wide programming write data 5 */
    reg32_t FWPWRITE6; /**< FMC flash wide programming write data 6 */
    reg32_t FWPWRITE7; /**< FMC flash wide programming write data 7 */
    reg32_t FWPWRITE_ECC; /**< FMC flash wide programming ECC */
    reg32_t FSWSTAT; /**< FMC software interface status */
    reg32_t __reserved12[0x2E]; /**< Reserved */
    reg32_t FSM_GLBCTL; /**< FMC FSM global control */
    reg32_t FSM_STATE; /**< FMC FSM state status */
    reg32_t FSM_STAT; /**< FMC FSM status */
    reg32_t FSM_CMD; /**< FMC FSM command */
    reg32_t FSM_PE_OSU; /**< FMC FSM program/erase operation setup */
    reg32_t FSM_VSTAT; /**< FMC FSM voltage status setup */
    reg32_t FSM_PE_VSU; /**< FMC FSM program/erase verify setup */
    reg32_t FSM_CMP_VSU; /**< FMC FSM compare verify setup */
    reg32_t FSM_EX_VAL; /**< FMC FSM EXECUTEZ to valid data */
    reg32_t FSM_RD_H; /**< FMC FSM read mode hold */
    reg32_t FSM_P_OH; /**< FMC FSM program hold */
    reg32_t FSM_ERA_OH; /**< FMC FSM erase operation hold */
    reg32_t FSM_SAV_PPUL; /**< FMC FSM saved program pulses */
    reg32_t FSM_PE_VH; /**< FMC FSM program/erase verify hold */
    reg32_t __reserved13[2]; /**< Reserved */
    reg32_t FSM_PRG_PW; /**< FMC FSM program pulse width */
    reg32_t FSM_ERA_PW; /**< FMC FSM erase pulse width */
    reg32_t __reserved14[3]; /**< Reserved */
    reg32_t FSM_SAV_ERA_PUL; /**< FMC FSM saved erased pulses */
    reg32_t FSM_TIMER; /**< FMC FSM timer */
    reg32_t FSM_MODE; /**< FMC FSM MODE */
    reg32_t FSM_PGM; /**< FMC FSM program bits */
    reg32_t FSM_ERA; /**< FMC FSM erase bits */
    reg32_t FSM_PRG_PUL; /**< FMC FSM maximum programming pulses */
    reg32_t FSM_ERA_PUL; /**< FMC FSM maximum erase pulses */
    reg32_t FSM_STEP_SIZE; /**< FMC FSM EC step size */
    reg32_t FSM_PUL_CNTR; /**< FMC FSM pulse counter */
    reg32_t FSM_EC_STEP_HEIGHT; /**< FMC FSM EC step height */
    reg32_t FSM_ST_MACHINE; /**< FMC FSM ST MACHINE */
    reg32_t FSM_FLES; /**< FMC FSM FLES memory control bits */
    reg32_t __reserved15; /**< Reserved */
    reg32_t FSM_WR_ENA; /**< FMC FSM register write enable */
    reg32_t FSM_ACC_PP; /**< FMC FSM accumulate program pulses */
    reg32_t FSM_ACC_EP; /**< FMC FSM accumulate erase pulses */
    reg32_t __reserved16[3]; /**< Reserved */
    reg32_t FSM_ADDR; /**< FMC FSM address */
    reg32_t FSM_SECTOR; /**< FMC sectors erased */
    reg32_t FMC_REV_ID; /**< FMC revision identification */
    reg32_t FSM_ERR_ADDR; /**< FSM error address */
    reg32_t FSM_PGM_MAXPUL; /**< FMC FSM maximum program pulse */
    reg32_t FSM_EXECUTE; /**< FMC FSM command execute */
    reg32_t __reserved17[2]; /**< Reserved */
    reg32_t FSM_SECTOR1; /**< FMC FSM sector erased 1 */
    reg32_t FSM_SECTOR2; /**< FMC FSM sector erased 2 */
    reg32_t __reserved18[6]; /**< Reserved */
    reg32_t FSM_BSLE0; /**< FMC FSM bank sector lock erase 0 */
    reg32_t FSM_BSLE1; /**< FMC FSM bank sector lock erase 1 */
    reg32_t __reserved19[2]; /**< Reserved */
    reg32_t FSM_BSLP0; /**< FMC FSM bank sector lock program 0 */
    reg32_t FSM_BSLP1; /**< FMC FSM bank sector lock program 1 */
    reg32_t FSM_PGM_128; /**< Enable 128-bit programming. CC26x2_CC13x2 only */
    reg32_t __reserved20[0x41]; /**< Reserved */
    reg32_t FCFG_BANK; /**< FMC flash configuration bank */
    reg32_t FCFG_WRAPPER; /**< FMC flash wrapper configuration */
    reg32_t FCFG_BNK_TYPE; /**< FMC flash bank type */
    reg32_t __reserved21; /**< Reserved */
    reg32_t FCFG_B0_START; /**< FMC flash bank 0 starting address */
    reg32_t FCFG_B1_START; /**< FMC flash bank 1 starting address */
    reg32_t FCFG_B2_START; /**< FMC flash bank 2 starting address */
    reg32_t FCFG_B3_START; /**< FMC flash bank 3 starting address */
    reg32_t FCFG_B4_START; /**< FMC flash bank 4 starting address */
    reg32_t FCFG_B5_START; /**< FMC flash bank 5 starting address */
    reg32_t FCFG_B6_START; /**< FMC flash bank 6 starting address */
    reg32_t FCFG_B7_START; /**< FMC flash bank 7 starting address */
    reg32_t FCFG_B0_SSIZE0; /**< FMC flash bank 0 sector size */
} flash_regs_t;

/**
 * @brief   FLASH register values
 * @{
 */
#define FLASH_CFG_DIS_STANDBY    0x00000002
#define FLASH_CFG_DIS_EFUSECLK   0x00000020
#define FLASH_FPAC1_PSLEEPTDIS_m 0x0FFF0000
#define FLASH_FPAC1_PSLEEPTDIS_s 16
/** @} */

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   FLASH base address
 */
#define FLASH_BASEADDR       (PERIPH_BASE + 0x30000)
/** @} */

/**
 * @brief   FLASH register bank
 */
#define FLASH                ((flash_regs_t *) (FLASH_BASEADDR))

/**
 * @brief   VIMS registers
 */
typedef struct {
    reg32_t STAT; /**< Status */
    reg32_t CTL; /**< Control */
} vims_regs_t;

/**
 * @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
/**
 * @brief   VIMS base address
 */
#define VIMS_BASE            (PERIPH_BASE + 0x34000)
/** @} */

/**
 * @brief   VIMS register bank
 */
#define VIMS                 ((vims_regs_t *) (VIMS_BASE))

/**
 * @brief   VIMS register values
 * @{
 */
#define VIMS_CTL_STATS_CLR       0x80000000
#define VIMS_CTL_STATS_CLR_m     0x80000000

#define VIMS_CTL_STATS_EN        0x40000000
#define VIMS_CTL_STATS_EN_m      0x40000000

#define VIMS_CTL_DYN_CG_EN       0x20000000
#define VIMS_CTL_DYN_CG_EN_m     0x20000000

#define VIMS_CTL_IDCODE_LB_DIS   0x00000020
#define VIMS_CTL_IDCODE_LB_DIS_m 0x00000020

#define VIMS_CTL_SYSBUS_LB_DIS   0x00000010
#define VIMS_CTL_SYSBUS_LB_DIS_m 0x00000010

#define VIMS_CTL_ARB_CFG         0x00000008
#define VIMS_CTL_ARB_CFG_m       0x00000008

#define VIMS_CTL_PREF_EN         0x00000004
#define VIMS_CTL_PREF_EN_m       0x00000004

#define VIMS_CTL_MODE_GPRAM      0x00000000
#define VIMS_CTL_MODE_CACHE      0x00000001
#define VIMS_CTL_MODE_SPLIT      0x00000002
#define VIMS_CTL_MODE_OFF        0x00000003
#define VIMS_CTL_MODE_m          0x00000003

#define VIMS_STAT_MODE_CHANGING  0x00000008
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
