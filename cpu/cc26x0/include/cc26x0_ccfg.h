/*
 * Copyright (C) 2016 Leon George
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */
/**
 * @ingroup         cpu_cc26x0_definitions
 * @{
 *
 * @file
 * @brief           CC26x0 CCFG register definitions
 */

#ifndef CC26x0_CCFG_H
#define CC26x0_CCFG_H

#include <cc26x0.h>

#ifdef __cplusplus
extern "C" {
#endif

/** @ingroup cpu_specific_peripheral_memory_map
 * @{
 */
#define CCFG_BASE                           0x50003000 /**<  base address of CCFG memory */
/*@}*/

/* TODO not present in datasheet but explained in cc26x0ware-lds */
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM   0x00000004 /**< CCFG_SIZE_AND_DIS_FLAGS_DIS_GPRAM */

/**
 * CCFG registers
 */
typedef struct {
    reg32_t EXT_LF_CLK; /**< extern LF clock config */
    reg32_t MODE_CONF_1; /**< mode config 1 */
    reg32_t SIZE_AND_DIS_FLAGS; /**< CCFG size and disable flags */
    reg32_t MODE_CONF; /**< mode config 0 */
    reg32_t VOLT_LOAD_0; /**< voltage load 0 */
    reg32_t VOLT_LOAD_1; /**< voltage load 1 */
    reg32_t RTC_OFFSET; /**< RTC offset */
    reg32_t FREQ_OFFSET; /**< frequency offset */
    reg32_t IEEE_MAC_0; /**< IEEE MAC address 0 */
    reg32_t IEEE_MAC_1; /**< IEEE MAC address 1 */
    reg32_t IEEE_BLE_0; /**< IEEE BLE address 0 */
    reg32_t IEEE_BLE_1; /**< IEEE BLE address 1 */
    reg32_t BL_CONFIG; /**< bootloader config */
    reg32_t ERASE_CONF; /**< erase config */
    reg32_t CCFG_TI_OPTIONS; /**< TI options */
    reg32_t CCFG_TAP_DAP_0; /**< test access points enable 0 */
    reg32_t CCFG_TAP_DAP_1; /**< test access points enable 1 */
    reg32_t IMAGE_VALID_CONF; /**< image valid */
    reg32_t CCFG_PROT_31_0; /**< protect sectors 0-31 */
    reg32_t CCFG_PROT_63_32; /**< protect sectors 32-63 */
    reg32_t CCFG_PROT_95_64; /**< protect sectors 64-95 */
    reg32_t CCFG_PROT_127_96; /**< protect sectors 96-127 */
} ccfg_regs_t;

#define CCFG ((ccfg_regs_t *) (CCFG_BASE + 0xFA8)) /**< CCFG register bank */

/**
 * @brief   CCFG register values
 * @{
 */
#define CCFG_EXT_LF_CLK_RTC_INCREMENT_MASK              (0x00FFFFFF)
#define CCFG_EXT_LF_CLK_DIO_MASK                        (0xFF000000)

#define CCFG_MODE_CONF_1_XOSC_MAX_START_MASK            (0x000000FF)
#define CCFG_MODE_CONF_1_DELTA_IBIAS_OFFSET_MASK        (0x00000F00)
#define CCFG_MODE_CONF_1_DELTA_IBIAS_INIT_MASK          (0x0000F000)
#define CCFG_MODE_CONF_1_ALT_DCDC_IPEAK_MASK            (0x00070000)
#define CCFG_MODE_CONF_1_ALT_DCDC_DITHER_EN             (0x00080000)
#define CCFG_MODE_CONF_1_ALT_DCDC_VMIN_MASK             (0x00F00000)

#define CCFG_SIZE_AND_DIS_FLAGS_DIS_XOSC_OVR            (0x00000001)
#define CCFG_SIZE_AND_DIS_FLAGS_DIS_ALT_DCDC_SETTING    (0x00000002)
#define CCFG_SIZE_AND_DIS_FLAGS_DISABLE_FLAGS_MASK      (0x0000FFFC)
#define CCFG_SIZE_AND_DIS_FLAGS_SIZE_OF_CCFG_MASK       (0xFFFF0000)

#define CCFG_MODE_CONF_VDDR_CAP_MASK                    (0x000000FF)
#define CCFG_MODE_CONF_XOSC_CAPARRAY_DELTA_MASK         (0x0000FF00)
#define CCFG_MODE_CONF_HF_COMP                          (0x00010000)
#define CCFG_MODE_CONF_XOSC_CAP_MOD                     (0x00020000)
#define CCFG_MODE_CONF_XOSC_FREQ_MASK                   (0x000C0000)
#define CCFG_MODE_CONF_RTC_COMP                         (0x00100000)
#define CCFG_MODE_CONF_SCLK_LF_OPTION_MASK              (0x00C00000)
#define CCFG_MODE_CONF_VDDS_BOD_LEVEL                   (0x01000000)
#define CCFG_MODE_CONF_VDDR_EXT_LOAD                    (0x02000000)
#define CCFG_MODE_CONF_DCDC_ACTIVE                      (0x04000000)
#define CCFG_MODE_CONF_DCDC_RECHARGE                    (0x08000000)
#define CCFG_MODE_CONF_VDDR_TRIM_SLEEP_DELTA_MASK       (0xF0000000)

#define CCFG_BL_CONFIG_BL_ENABLE_MASK                   (0x000000FF)
#define CCFG_BL_CONFIG_BL_PIN_NUMBER_MASK               (0x0000FF00)
#define CCFG_BL_CONFIG_BL_LEVEL                         (0x00010000)
#define CCFG_BL_CONFIG_BOOTLOADER_ENABLE_MASK           (0xFF000000)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC26x0_CCFG_H */

/*@}*/
