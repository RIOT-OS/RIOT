/*
 * Copyright (C) 2019 Ludwig Maximilian Universität
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup        cpu_cc3200_prcm CC3200 PRCM
 * @ingroup         cpu_cc3200_regs
 * @{
 *
 * @file
 * @brief           Driver for the cc3200 PRCM controller
 *
 * Header file with register and macro declarations for the cc3200 PRCM module
 *
 * @author          Wladislaw Meixner <wladislaw.meixner@campus.lmu.de>
 *
 * @{
 */

#ifndef CC3200_PRCM_H
#define CC3200_PRCM_H

#include "cc3200.h"
#include "vendor/hw_memmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief uSec delay for reading some Power Control Module Values
 *
 */
#define PRCM_OP_USEC_DELAY 200

/* Derived/Copied from TIs driverlib/prcm.h */

/**
 * @brief PRCM peripheral clock modes
 * @{
 */
#define PRCM_RUN_MODE_CLK  0x00000001
#define PRCM_SLP_MODE_CLK  0x00000100
#define PRCM_DSLP_MODE_CLK 0x00010000
#define PRCM_MODE_CLK_MASK = PRCM_DSLP_MODE_CLK & PRCM_SLP_MODE_CLK & PRCM_RUN_MODE_CLK
/** @} */

/**
 * @brief PRCM reset cause values
 * @{
 */
#define PRCM_POWER_ON 0x00000000
#define PRCM_LPDS_EXIT 0x00000001
#define PRCM_CORE_RESET 0x00000003
#define PRCM_MCU_RESET 0x00000004
#define PRCM_WDT_RESET 0x00000005
#define PRCM_SOC_RESET 0x00000006
#define PRCM_HIB_EXIT 0x00000007
/** @} */

/**
 * @brief PRCM hybernation wakeup causes
 * @{
 */
#define PRCM_HIB_WAKEUP_CAUSE_SLOW_CLOCK 0x00000002
#define PRCM_HIB_WAKEUP_CAUSE_GPIO 0x00000004
/** @} */

/**
 * @brief PRCM peripheral reset macro
 * @{
 */
#define PRCM_SOFT_RESET 0x00000001
/** @} */

/**
 * @brief Application Reset-Clock Manager regsiter
 * @{
 */
typedef struct cc3200_arcm_reg_t {
    cc3200_reg_t clk_gating; /**< clock gating register. Used to configure clock
                                mode. */
    cc3200_reg_t soft_reset; /**< software reset register. Used to enable
                                ordisable hardware. */
} cc3200_arcm_reg_t;
/** @} */

/**
 * @brief Application Reset-Clock Manager (ARCM) control register
 * @{
 */
typedef struct cc3200_arcm_t {
    cc3200_reg_t CAM_CLK_CFG;         /**< 0x00 Camera Clock Configuration */
    cc3200_reg_t CAM_CLK_EN;          /**< 0x04 Camera Clock Enable register */
    cc3200_reg_t CAM_SOFT_RST;        /**< 0x08 Camera Soft Reset register */
    cc3200_reg_t RESERVED1[2];        /**< 0x0C - 0x14 RESERVED */    
    cc3200_reg_t MCASP_CLK_GATING;    /**< 0x14 Multichannel Audio Serial Port (MCASP) Clock Gating  */
    cc3200_reg_t MCASP_SOFT_RST;      /**< 0x18 MCASP Soft Reset  */
    cc3200_reg_t RESERVED2[1];        /**< 0x1C - 0x20 RESERVED */   
    cc3200_reg_t MMCHS_CLK_GEN;       /**< 0x20 MMCHS/SDIOM Configuration */
    cc3200_arcm_reg_t MMCHS;          /**< 0x24 MMCHS/SDIOM ARCM register */
    cc3200_reg_t MCSPI_A1_CLK_GEN;    /**< 0x2C SPI 1 Clock Configuration */
    cc3200_arcm_reg_t MCSPI_A1;       /**< SPI 1 ARCM register */
    cc3200_reg_t MCSPI_A2_CLK_GEN;    /**< SPI 2 Clock Configuration */
    cc3200_reg_t RESERVED3[1];        /**< RESERVED */
    cc3200_arcm_reg_t MCSPI_A2;       /**< SPI 2 ARCM register */
    cc3200_arcm_reg_t UDMA_A;         /**< UDMA_A ARCM register */
    cc3200_arcm_reg_t GPIO_A;         /**< GPIO_A ARCM register */
    cc3200_arcm_reg_t GPIO_B;         /**< GPIO_B ARCM register */
    cc3200_arcm_reg_t GPIO_C;         /**< GPIO_C ARCM register */
    cc3200_arcm_reg_t GPIO_D;         /**< GPIO_D ARCM register */
    cc3200_arcm_reg_t GPIO_E;         /**< GPIO_E ARCM register */
    cc3200_arcm_reg_t WDOG_A;         /**< WDOG_A ARCM register */
    cc3200_arcm_reg_t UART_A0;        /**< UART_A0 ARCM register */
    cc3200_arcm_reg_t UART_A1;        /**< UART_A1 ARCM register */
    cc3200_arcm_reg_t GPT_A0;         /**< GPT_A0 ARCM register */
    cc3200_arcm_reg_t GPT_A1;         /**< GPT_A1 ARCM register */
    cc3200_arcm_reg_t GPT_A2;         /**< GPT_A2 ARCM register */
    cc3200_arcm_reg_t GPT_A3;         /**< GPT_A3 ARCM register */
    cc3200_reg_t MCASP_FRAC_CLK_CFG0; /**< MCASP_FRAC_CLK_CFG0 Configuration */
    cc3200_reg_t MCASP_FRAC_CLK_CFG1; /**< MCASP_FRAC_CLK_CFG1 Configuration */
    cc3200_arcm_reg_t CRYPTO;         /**< Crypto Engine ARCM register */
    cc3200_reg_t RESERVED4[2];        /**< RESERVED */
    cc3200_arcm_reg_t MCSPI_S0; /**< Integrated CC3100 SPI ARMC register */
    cc3200_reg_t MCSPI_S0_CLKDIV_CFG;   /**< Integrated CC3100 SPI Configuration
                                         */
    cc3200_reg_t RESERVED5[1];          /**< RESERVED */
    cc3200_arcm_reg_t I2C;              /**< I2C Clock ARCM register */
    cc3200_reg_t RESERVED6[1];          /**< RESERVED */
    cc3200_reg_t APPS_LPDS_REQ;         /**< APPS_LPDS_REQ */
    cc3200_reg_t RESERVED7[1];          /**< RESERVED */
    cc3200_reg_t APPS_TURBO_REQ;        /**< APPS_TURBO_REQ */
    cc3200_reg_t RESERVED8[6];          /**< RESERVED */
    cc3200_reg_t APPS_DSLP_WAKE_CONFIG; /**< APPS_DSLP_WAKE_CONFIG */
    cc3200_reg_t APPS_DSLP_WAKE_TIMER_CFG;  /**< APPS_DSLP_WAKE_TIMER_CFG
                                             */
    cc3200_reg_t APPS_RCM_SLP_WAKE_ENABLE;  /**< APPS_RCM_SLP_WAKE_ENABLE */
    cc3200_reg_t APPS_SLP_WAKETIMER_CFG;    /**< APPS_SLP_WAKETIMER_CFG */
    cc3200_reg_t APPS_TO_NWP_WAKE_REQUEST;  /**< APPS_TO_NWP_WAKE_REQUEST */
    cc3200_reg_t APPS_RCM_INTERRUPT_STATUS; /**< APPS_RCM_INTERRUPT_STATUS */
    cc3200_reg_t APPS_RCM_INTERRUPT_ENABLE; /**< APPS_RCM_INTERRUPT_ENABLE */
} cc3200_arcm_t;

/**
 * @brief One and only instance of the System Control module
 *
 */
#define ARCM ((cc3200_arcm_t *)ARCM_BASE)
/** @} */

/**
 * @brief Global Power-Reset-Clock Manager (GPRCM) control register
 * @{
 */
typedef struct cc3200_gprcm_t {
    cc3200_reg_t APPS_SOFT_RESET;             /**< APPS_SOFT_RESET */
    cc3200_reg_t APPS_LPDS_WAKEUP_CFG;        /**< APPS_LPDS_WAKEUP_CFG */
    cc3200_reg_t APPS_LPDS_WAKEUP_SRC;        /**< APPS_LPDS_WAKEUP_SRC */
    cc3200_reg_t APPS_RESET_CAUSE;            /**< APPS_RESET_CAUSE */
    cc3200_reg_t APPS_LPDS_WAKETIME_OPP_CFG;  /**< APPS_LPDS_WAKETIME_OPP_CFG */
    cc3200_reg_t RESERVER1[1];                /**< RESERVER1 */
    cc3200_reg_t APPS_SRAM_DSLP_CFG;          /**< APPS_SRAM_DSLP_CFG */
    cc3200_reg_t APPS_SRAM_LPDS_CFG;          /**< APPS_SRAM_LPDS_CFG */
    cc3200_reg_t APPS_LPDS_WAKETIME_WAKE_CFG; /**< APPS_LPDS_WAKETIME_WAKE_CFG
                                               */
    cc3200_reg_t RESERVER2[55];               /**< RESERVER2 */
    cc3200_reg_t TOP_DIE_ENABLE;              /**< TOP_DIE_ENABLE */
    cc3200_reg_t TOP_DIE_ENABLE_PARAMETERS;   /**< TOP_DIE_ENABLE_PARAMETERS */
    cc3200_reg_t MCU_GLOBAL_SOFT_RESET;       /**< MCU_GLOBAL_SOFT_RESET */
    cc3200_reg_t ADC_CLK_CONFIG;              /**< ADC_CLK_CONFIG */
    cc3200_reg_t APPS_GPIO_WAKE_CONF;         /**< APPS_GPIO_WAKE_CONF */
    cc3200_reg_t EN_NWP_BOOT_WO_DEVINIT;      /**< EN_NWP_BOOT_WO_DEVINIT */
    cc3200_reg_t MEM_HCLK_DIV_CFG;            /**< MEM_HCLK_DIV_CFG */
    cc3200_reg_t MEM_SYSCLK_DIV_CFG;          /**< MEM_SYSCLK_DIV_CFG */
    cc3200_reg_t APLLMCS_LOCK_TIME_CONF;      /**< APLLMCS_LOCK_TIME_CONF */
    cc3200_reg_t RESERVER3[183];              /**< RESERVER3 */
    cc3200_reg_t NWP_SOFT_RESET;              /**< NWP_SOFT_RESET */
    cc3200_reg_t NWP_LPDS_WAKEUP_CFG;         /**< NWP_LPDS_WAKEUP_CFG */
    cc3200_reg_t NWP_LPDS_WAKEUP_SRC;         /**< NWP_LPDS_WAKEUP_SRC */
    cc3200_reg_t NWP_RESET_CAUSE;             /**< NWP_RESET_CAUSE */
    cc3200_reg_t NWP_LPDS_WAKETIME_OPP_CFG;   /**< NWP_LPDS_WAKETIME_OPP_CFG */
    cc3200_reg_t RESERVER4[1];                /**< RESERVER4 */
    cc3200_reg_t NWP_SRAM_DSLP_CFG;           /**< NWP_SRAM_DSLP_CFG */
    cc3200_reg_t NWP_SRAM_LPDS_CFG;           /**< NWP_SRAM_LPDS_CFG */
    cc3200_reg_t NWP_LPDS_WAKETIME_WAKE_CFG;  /**< NWP_LPDS_WAKETIME_WAKE_CFG */
    cc3200_reg_t NWP_AUTONMS_SPI_MASTER_SEL;  /**< NWP_AUTONMS_SPI_MASTER_SEL */
    cc3200_reg_t NWP_AUTONMS_SPI_IDLE_REQ;    /**< NWP_AUTONMS_SPI_IDLE_REQ */
    cc3200_reg_t WLAN_TO_NWP_WAKE_REQUEST;    /**< WLAN_TO_NWP_WAKE_REQUEST */
    cc3200_reg_t NWP_TO_WLAN_WAKE_REQUEST;    /**< NWP_TO_WLAN_WAKE_REQUEST */
    cc3200_reg_t NWP_GPIO_WAKE_CONF;          /**< NWP_GPIO_WAKE_CONF */
    cc3200_reg_t GPRCM_EFUSE_READ_REG12;      /**< GPRCM_EFUSE_READ_REG12 */
    cc3200_reg_t RESERVER5[3];                /**< RESERVER5 */
    cc3200_reg_t GPRCM_DIEID_READ_REG5;       /**< GPRCM_DIEID_READ_REG5 */
    cc3200_reg_t GPRCM_DIEID_READ_REG6;       /**< GPRCM_DIEID_READ_REG6 */
    cc3200_reg_t RESERVER6[236];              /**< RESERVER6 */
    cc3200_reg_t REF_FSM_CFG0;                /**< REF_FSM_CFG0 */
    cc3200_reg_t REF_FSM_CFG1;                /**< REF_FSM_CFG1 */
    cc3200_reg_t APLLMCS_WLAN_CONFIG0_40;     /**< APLLMCS_WLAN_CONFIG0_40 */
    cc3200_reg_t APLLMCS_WLAN_CONFIG1_40;     /**< APLLMCS_WLAN_CONFIG1_40 */
    cc3200_reg_t APLLMCS_WLAN_CONFIG0_26;     /**< APLLMCS_WLAN_CONFIG0_26 */
    cc3200_reg_t APLLMCS_WLAN_CONFIG1_26;     /**< APLLMCS_WLAN_CONFIG1_26 */
    cc3200_reg_t APLLMCS_WLAN_OVERRIDES;      /**< APLLMCS_WLAN_OVERRIDES */
    cc3200_reg_t APLLMCS_MCU_RUN_CONFIG0_38P4; /**< APLLMCS_MCU_RUN_CONFIG0_38P4
                                                */
    cc3200_reg_t APLLMCS_MCU_RUN_CONFIG1_38P4; /**< APLLMCS_MCU_RUN_CONFIG1_38P4
                                                */
    cc3200_reg_t APLLMCS_MCU_RUN_CONFIG0_26; /**< APLLMCS_MCU_RUN_CONFIG0_26 */
    cc3200_reg_t APLLMCS_MCU_RUN_CONFIG1_26; /**< APLLMCS_MCU_RUN_CONFIG1_26 */
    cc3200_reg_t SPARE_RW0;                  /**< SPARE_RW0 */
    cc3200_reg_t SPARE_RW1;                  /**< SPARE_RW1 */
    cc3200_reg_t APLLMCS_MCU_OVERRIDES;      /**< APLLMCS_MCU_OVERRIDES */
    cc3200_reg_t SYSCLK_SWITCH_STATUS;       /**< SYSCLK_SWITCH_STATUS */
    cc3200_reg_t REF_LDO_CONTROLS;           /**< REF_LDO_CONTROLS */
    cc3200_reg_t REF_RTRIM_CONTROL;          /**< REF_RTRIM_CONTROL */
    cc3200_reg_t REF_SLICER_CONTROLS0;       /**< REF_SLICER_CONTROLS0 */
    cc3200_reg_t REF_SLICER_CONTROLS1;       /**< REF_SLICER_CONTROLS1 */
    cc3200_reg_t REF_ANA_BGAP_CONTROLS0;     /**< REF_ANA_BGAP_CONTROLS0 */
    cc3200_reg_t REF_ANA_BGAP_CONTROLS1;     /**< REF_ANA_BGAP_CONTROLS1 */
    cc3200_reg_t REF_ANA_SPARE_CONTROLS0;    /**< REF_ANA_SPARE_CONTROLS0 */
    cc3200_reg_t REF_ANA_SPARE_CONTROLS1;    /**< REF_ANA_SPARE_CONTROLS1 */
    cc3200_reg_t MEMSS_PSCON_OVERRIDES0;     /**< MEMSS_PSCON_OVERRIDES0 */
    cc3200_reg_t MEMSS_PSCON_OVERRIDES1;     /**< MEMSS_PSCON_OVERRIDES1 */
    cc3200_reg_t PLL_REF_LOCK_OVERRIDES;     /**< PLL_REF_LOCK_OVERRIDES */
    cc3200_reg_t MCU_PSCON_DEBUG;            /**< MCU_PSCON_DEBUG */
    cc3200_reg_t MEMSS_PWR_PS;               /**< MEMSS_PWR_PS */
    cc3200_reg_t REF_FSM_DEBUG;              /**< REF_FSM_DEBUG */
    cc3200_reg_t MEM_SYS_OPP_REQ_OVERRIDE;   /**< MEM_SYS_OPP_REQ_OVERRIDE */
    cc3200_reg_t MEM_TESTCTRL_PD_OPP_CONFIG; /**< MEM_TESTCTRL_PD_OPP_CONFIG */
    cc3200_reg_t MEM_WL_FAST_CLK_REQ_OVERRIDES;    /**<
                                                      MEM_WL_FAST_CLK_REQ_OVERRIDES
                                                    */
    cc3200_reg_t MEM_MCU_PD_MODE_REQ_OVERRIDES;    /**<
                                                      MEM_MCU_PD_MODE_REQ_OVERRIDES
                                                    */
    cc3200_reg_t MEM_MCSPI_SRAM_OFF_REQ_OVERRIDES; /**<
                                                 MEM_MCSPI_SRAM_OFF_REQ_OVERRIDES
                                               */
    cc3200_reg_t MEM_WLAN_APLLMCS_OVERRIDES;  /**< MEM_WLAN_APLLMCS_OVERRIDES */
    cc3200_reg_t MEM_REF_FSM_CFG2;            /**< MEM_REF_FSM_CFG2 */
    cc3200_reg_t RESERVER7[224];              /**< RESERVER7 */
    cc3200_reg_t TESTCTRL_POWER_CTRL;         /**< TESTCTRL_POWER_CTRL */
    cc3200_reg_t SSDIO_POWER_CTRL;            /**< SSDIO_POWER_CTRL */
    cc3200_reg_t MCSPI_N1_POWER_CTRL;         /**< MCSPI_N1_POWER_CTRL */
    cc3200_reg_t WELP_POWER_CTRL;             /**< WELP_POWER_CTRL */
    cc3200_reg_t WL_SDIO_POWER_CTRL;          /**< WL_SDIO_POWER_CTRL */
    cc3200_reg_t WLAN_SRAM_ACTIVE_PWR_CFG;    /**< WLAN_SRAM_ACTIVE_PWR_CFG */
    cc3200_reg_t RESERVER8[1];                /**< RESERVER8 */
    cc3200_reg_t WLAN_SRAM_SLEEP_PWR_CFG;     /**< WLAN_SRAM_SLEEP_PWR_CFG */
    cc3200_reg_t APPS_SECURE_INIT_DONE;       /**< APPS_SECURE_INIT_DONE */
    cc3200_reg_t APPS_DEV_MODE_INIT_DONE;     /**< APPS_DEV_MODE_INIT_DONE */
    cc3200_reg_t EN_APPS_REBOOT;              /**< EN_APPS_REBOOT */
    cc3200_reg_t MEM_APPS_PERIPH_PRESENT;     /**< MEM_APPS_PERIPH_PRESENT */
    cc3200_reg_t MEM_NWP_PERIPH_PRESENT;      /**< MEM_NWP_PERIPH_PRESENT */
    cc3200_reg_t MEM_SHARED_PERIPH_PRESENT;   /**< MEM_SHARED_PERIPH_PRESENT */
    cc3200_reg_t NWP_PWR_STATE;               /**< NWP_PWR_STATE */
    cc3200_reg_t APPS_PWR_STATE;              /**< APPS_PWR_STATE */
    cc3200_reg_t MCU_PWR_STATE;               /**< MCU_PWR_STATE */
    cc3200_reg_t WTOP_PM_PS;                  /**< WTOP_PM_PS */
    cc3200_reg_t WTOP_PD_RESETZ_OVERRIDE_REG; /**< WTOP_PD_RESETZ_OVERRIDE_REG
                                               */
    cc3200_reg_t WELP_PD_RESETZ_OVERRIDE_REG; /**< WELP_PD_RESETZ_OVERRIDE_REG
                                               */
    cc3200_reg_t WL_SDIO_PD_RESETZ_OVERRIDE_REG; /**<
                                                    WL_SDIO_PD_RESETZ_OVERRIDE_REG
                                                  */
    cc3200_reg_t SSDIO_PD_RESETZ_OVERRIDE_REG; /**< SSDIO_PD_RESETZ_OVERRIDE_REG
                                                */
    cc3200_reg_t
            MCSPI_N1_PD_RESETZ_OVERRIDE_REG; /**<
                                                MCSPI_N1_PD_RESETZ_OVERRIDE_REG
                                              */
    cc3200_reg_t
            TESTCTRL_PD_RESETZ_OVERRIDE_REG; /**<
                                                TESTCTRL_PD_RESETZ_OVERRIDE_REG
                                              */
    cc3200_reg_t MCU_PD_RESETZ_OVERRIDE_REG; /**< MCU_PD_RESETZ_OVERRIDE_REG */
    cc3200_reg_t RESERVER9[1];               /**< RESERVER9 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG0;      /**< GPRCM_EFUSE_READ_REG0 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG1;      /**< GPRCM_EFUSE_READ_REG1 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG2;      /**< GPRCM_EFUSE_READ_REG2 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG3;      /**< GPRCM_EFUSE_READ_REG3 */
    cc3200_reg_t WTOP_MEM_RET_CFG;           /**< WTOP_MEM_RET_CFG */
    cc3200_reg_t COEX_CLK_SWALLOW_CFG0;      /**< COEX_CLK_SWALLOW_CFG0 */
    cc3200_reg_t COEX_CLK_SWALLOW_CFG1;      /**< COEX_CLK_SWALLOW_CFG1 */
    cc3200_reg_t COEX_CLK_SWALLOW_CFG2;      /**< COEX_CLK_SWALLOW_CFG2 */
    cc3200_reg_t COEX_CLK_SWALLOW_ENABLE;    /**< COEX_CLK_SWALLOW_ENABLE */
    cc3200_reg_t DCDC_CLK_GEN_CONFIG;        /**< DCDC_CLK_GEN_CONFIG */
    cc3200_reg_t GPRCM_EFUSE_READ_REG4;      /**< GPRCM_EFUSE_READ_REG4 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG5;      /**< GPRCM_EFUSE_READ_REG5 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG6;      /**< GPRCM_EFUSE_READ_REG6 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG7;      /**< GPRCM_EFUSE_READ_REG7 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG8;      /**< GPRCM_EFUSE_READ_REG8 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG9;      /**< GPRCM_EFUSE_READ_REG9 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG10;     /**< GPRCM_EFUSE_READ_REG10 */
    cc3200_reg_t GPRCM_EFUSE_READ_REG11;     /**< GPRCM_EFUSE_READ_REG11 */
    cc3200_reg_t GPRCM_DIEID_READ_REG0;      /**< GPRCM_DIEID_READ_REG0 */
    cc3200_reg_t GPRCM_DIEID_READ_REG1;      /**< GPRCM_DIEID_READ_REG1 */
    cc3200_reg_t GPRCM_DIEID_READ_REG2;      /**< GPRCM_DIEID_READ_REG2 */
    cc3200_reg_t GPRCM_DIEID_READ_REG3;      /**< GPRCM_DIEID_READ_REG3 */
    cc3200_reg_t GPRCM_DIEID_READ_REG4;      /**< GPRCM_DIEID_READ_REG4 */
    cc3200_reg_t APPS_SS_OVERRIDES;          /**< APPS_SS_OVERRIDES */
    cc3200_reg_t NWP_SS_OVERRIDES;           /**< NWP_SS_OVERRIDES */
    cc3200_reg_t SHARED_SS_OVERRIDES;        /**< SHARED_SS_OVERRIDES */
    cc3200_reg_t IDMEM_CORE_RST_OVERRIDES;   /**< IDMEM_CORE_RST_OVERRIDES */
    cc3200_reg_t TOP_DIE_FSM_OVERRIDES;      /**< TOP_DIE_FSM_OVERRIDES */
    cc3200_reg_t MCU_PSCON_OVERRIDES;        /**< MCU_PSCON_OVERRIDES */
    cc3200_reg_t WTOP_PSCON_OVERRIDES;       /**< WTOP_PSCON_OVERRIDES */
    cc3200_reg_t WELP_PSCON_OVERRIDES;       /**< WELP_PSCON_OVERRIDES */
    cc3200_reg_t WL_SDIO_PSCON_OVERRIDES;    /**< WL_SDIO_PSCON_OVERRIDES */
    cc3200_reg_t MCSPI_PSCON_OVERRIDES;      /**< MCSPI_PSCON_OVERRIDES */
    cc3200_reg_t SSDIO_PSCON_OVERRIDES;      /**< SSDIO_PSCON_OVERRIDES */
} cc3200_gprcm_t;

/**
 * @brief One and only instance of the General Power Control * Module
 */
#define GPRCM ((cc3200_gprcm_t *)GPRCM_BASE)

/** @} */
#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* CC3200_PRCM_H */

/** @} */