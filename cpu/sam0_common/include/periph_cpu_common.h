/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_sam0_common
 * @brief           Common CPU specific definitions for all SAMx21 based CPUs
 * @{
 *
 * @file
 * @brief           Common CPU specific definitions for all SAMx21 based CPUs
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author          Dylan Laduranty <dylan.laduranty@mesotic.com>
 */

#ifndef PERIPH_CPU_COMMON_H
#define PERIPH_CPU_COMMON_H

#include "cpu.h"
#include "exti_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of the CPU_ID in octets
 */
#define CPUID_LEN           (16U)

/**
 * @brief   Use shared SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @name   Use shared I2C functions
 * @{
 */
#define PERIPH_I2C_NEED_READ_REG
#define PERIPH_I2C_NEED_READ_REGS
#define PERIPH_I2C_NEED_WRITE_REG
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

/**
 * @brief   Override GPIO type
 * @{
 */
#define HAVE_GPIO_T
typedef uint32_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF          (0xffffffff)

/**
 * @brief   Macro for accessing GPIO pins
 * @{
 */
#ifdef CPU_FAM_SAML11
#define GPIO_PIN(x, y)      (((gpio_t)(&PORT_SEC->Group[x])) | y)
#else
#define GPIO_PIN(x, y)      (((gpio_t)(&PORT->Group[x])) | y)
#endif

/**
 * @brief   Available ports on the SAMD21 & SAML21
 */
enum {
    PA = 0,                 /**< port A */
    PB = 1,                 /**< port B */
    PC = 2,                 /**< port C */
    PD = 3,                 /**< port D */
};

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 3 bit to determine the pin functions:
 * - bit 0: PD(0) or PU(1)
 * - bit 1: input enable
 * - bit 2: pull enable
 */
#define GPIO_MODE(pr, ie, pe)   (pr | (ie << 1) | (pe << 2))

#ifndef DOXYGEN
/**
 * @brief   Override GPIO modes
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN    = GPIO_MODE(0, 1, 0),    /**< IN */
    GPIO_IN_PD = GPIO_MODE(0, 1, 1),    /**< IN with pull-down */
    GPIO_IN_PU = GPIO_MODE(1, 1, 1),    /**< IN with pull-up */
    GPIO_OUT   = GPIO_MODE(0, 0, 0),    /**< OUT (push-pull) */
    GPIO_OD    = 0xfe,                  /**< not supported by HW */
    GPIO_OD_PU = 0xff                   /**< not supported by HW */
} gpio_mode_t;

/**
 * @brief   Override active flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Available MUX values for configuring a pin's alternate function
 */
#ifndef SAM_MUX_T
typedef enum {
    GPIO_MUX_A = 0x0,       /**< select peripheral function A */
    GPIO_MUX_B = 0x1,       /**< select peripheral function B */
    GPIO_MUX_C = 0x2,       /**< select peripheral function C */
    GPIO_MUX_D = 0x3,       /**< select peripheral function D */
    GPIO_MUX_E = 0x4,       /**< select peripheral function E */
    GPIO_MUX_F = 0x5,       /**< select peripheral function F */
    GPIO_MUX_G = 0x6,       /**< select peripheral function G */
    GPIO_MUX_H = 0x7,       /**< select peripheral function H */
} gpio_mux_t;
#endif

/**
 * @brief   Available values for SERCOM UART RX pad selection
 */
typedef enum {
    UART_PAD_RX_0 = 0x0,    /**< use pad 0 for RX line */
    UART_PAD_RX_1 = 0x1,    /**< select pad 1 */
    UART_PAD_RX_2 = 0x2,    /**< select pad 2 */
    UART_PAD_RX_3 = 0x3,    /**< select pad 3 */
} uart_rxpad_t;

/**
 * @brief   Available values for SERCOM UART TX pad selection
 */
typedef enum {
    UART_PAD_TX_0             = 0x0,    /**< select pad 0 */
    UART_PAD_TX_2             = 0x1,    /**< select pad 2 */
    UART_PAD_TX_0_RTS_2_CTS_3 = 0x2,    /**< TX is pad 0, on top RTS on pad 2
                                         *   and CTS on pad 3 */
} uart_txpad_t;

/**
 * @brief   Available SERCOM UART flag selections
 */
typedef enum {
    UART_FLAG_NONE            = 0x0,    /**< No flags set */
    UART_FLAG_RUN_STANDBY     = 0x1,    /**< run SERCOM in standby mode */
    UART_FLAG_WAKEUP          = 0x2,    /**< wake from sleep on receive */
} uart_flag_t;

#ifndef DOXYGEN
/**
 * @brief   Available SERCOM UART data size selections
 *
 * 9 bit UART mode is currently unavailable as it is not supported by the common
 * RIOT UART peripheral API.
 * @{
 */
#define HAVE_UART_DATA_BITS_T
typedef enum {
    UART_DATA_BITS_5 = 0x5,   /**< 5 data bits */
    UART_DATA_BITS_6 = 0x6,   /**< 6 data bits */
    UART_DATA_BITS_7 = 0x7,   /**< 7 data bits */
    UART_DATA_BITS_8 = 0x0,   /**< 8 data bits */
} uart_data_bits_t;
/** @} */
#endif /* ndef DOXYGEN */


/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef SAM0_UART_TXBUF_SIZE
#define SAM0_UART_TXBUF_SIZE    (64)
#endif

/**
 * @brief   UART device configuration
 */
typedef struct {
    SercomUsart *dev;       /**< pointer to the used UART device */
    gpio_t rx_pin;          /**< pin used for RX */
    gpio_t tx_pin;          /**< pin used for TX */
#ifdef MODULE_PERIPH_UART_HW_FC
    gpio_t rts_pin;          /**< pin used for RTS */
    gpio_t cts_pin;          /**< pin used for CTS */
#endif
    gpio_mux_t mux;         /**< alternative function for pins */
    uart_rxpad_t rx_pad;    /**< pad selection for RX line */
    uart_txpad_t tx_pad;    /**< pad selection for TX line */
    uart_flag_t flags;      /**< set optional SERCOM flags */
    uint8_t gclk_src;       /**< GCLK source which supplys SERCOM */
} uart_conf_t;

/**
 * @brief   Available values for SERCOM SPI MISO pad selection
 */
typedef enum {
    SPI_PAD_MISO_0 = 0x0,       /**< use pad 0 for MISO line */
    SPI_PAD_MISO_1 = 0x1,       /**< use pad 0 for MISO line */
    SPI_PAD_MISO_2 = 0x2,       /**< use pad 0 for MISO line */
    SPI_PAD_MISO_3 = 0x3,       /**< use pad 0 for MISO line */
} spi_misopad_t;

/**
 * @brief   Available values for SERCOM SPI MOSI and SCK pad selection
 */
typedef enum {
    SPI_PAD_MOSI_0_SCK_1 = 0x0, /**< use pad 0 for MOSI, pad 1 for SCK */
    SPI_PAD_MOSI_2_SCK_3 = 0x1, /**< use pad 2 for MOSI, pad 3 for SCK */
    SPI_PAD_MOSI_3_SCK_1 = 0x2, /**< use pad 3 for MOSI, pad 1 for SCK */
    SPI_PAD_MOSI_0_SCK_3 = 0x3, /**< use pad 0 for MOSI, pad 3 for SCK */
} spi_mosipad_t;

#ifndef DOXYGEN
/**
 * @brief   Override SPI modes
 * @{
 */
#define HAVE_SPI_MODE_T
typedef enum {
    SPI_MODE_0 = 0x0,       /**< CPOL=0, CPHA=0 */
    SPI_MODE_1 = 0x1,       /**< CPOL=0, CPHA=1 */
    SPI_MODE_2 = 0x2,       /**< CPOL=1, CPHA=0 */
    SPI_MODE_3 = 0x3        /**< CPOL=1, CPHA=1 */
} spi_mode_t;
/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ =   100000U, /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ =   400000U, /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   =  1000000U, /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   =  5000000U, /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000U  /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   SPI device configuration
 */
typedef struct {
    SercomSpi *dev;         /**< pointer to the used SPI device */
    gpio_t miso_pin;        /**< used MISO pin */
    gpio_t mosi_pin;        /**< used MOSI pin */
    gpio_t clk_pin;         /**< used CLK pin */
    gpio_mux_t miso_mux;    /**< alternate function for MISO pin (mux) */
    gpio_mux_t mosi_mux;    /**< alternate function for MOSI pin (mux) */
    gpio_mux_t clk_mux;     /**< alternate function for CLK pin (mux) */
    spi_misopad_t miso_pad; /**< pad to use for MISO line */
    spi_mosipad_t mosi_pad; /**< pad to use for MOSI and CLK line */
    uint8_t gclk_src;       /**< GCLK source which supplys SERCOM */
} spi_conf_t;
/** @} */

/**
 * @brief   Available SERCOM I2C flag selections
 */
typedef enum {
    I2C_FLAG_NONE            = 0x0,    /**< No flags set */
    I2C_FLAG_RUN_STANDBY     = 0x1,    /**< run SERCOM in standby mode */
} i2c_flag_t;

#ifndef DOXYGEN
/**
 * @name    Override I2C clock speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = 10000U,      /**< low speed mode:    ~10kbit/s */
    I2C_SPEED_NORMAL    = 100000U,     /**< normal mode:       ~100kbit/s */
    I2C_SPEED_FAST      = 400000U,     /**< fast mode:         ~400kbit/s */
    I2C_SPEED_FAST_PLUS = 1000000U,    /**< fast plus mode:    ~1Mbit/s */
    I2C_SPEED_HIGH      = 3400000U,    /**< high speed mode:   ~3.4Mbit/s */
} i2c_speed_t;
/** @} */

/**
 * @name    I2C pin getter functions
 * @{
 */
#define i2c_pin_sda(dev) i2c_config[dev].sda_pin
#define i2c_pin_scl(dev) i2c_config[dev].scl_pin
/** @} */

#endif /* ndef DOXYGEN */

/**
 * @brief   I2C device configuration
 */
typedef struct {
    SercomI2cm *dev;        /**< pointer to the used I2C device */
    i2c_speed_t speed;      /**< baudrate used for the bus */
    gpio_t scl_pin;         /**< used SCL pin */
    gpio_t sda_pin;         /**< used MOSI pin */
    gpio_mux_t mux;         /**< alternate function (mux) */
    uint8_t gclk_src;       /**< GCLK source which supplys SERCOM */
    uint8_t flags;          /**< allow SERCOM to run in standby mode */
} i2c_conf_t;

/**
 * @brief   Timer device configuration
 */
typedef struct {
    Tc *dev;                /**< pointer to the used Timer device */
    IRQn_Type irq;          /**< IRQ# of Timer Interrupt */
#ifdef MCLK
    volatile uint32_t *mclk;/**< Pointer to MCLK->APBxMASK.reg */
    uint32_t mclk_mask;     /**< MCLK_APBxMASK bits to enable Timer */
    uint16_t gclk_id;       /**< TCn_GCLK_ID */
#else
    uint32_t pm_mask;       /**< PM_APBCMASK bits to enable Timer */
    uint16_t gclk_ctrl;     /**< GCLK_CLKCTRL_ID for the Timer */
#endif
    uint8_t gclk_src;       /**< GCLK source which supplys Timer */
    uint16_t flags;         /**< flags for CTRA, e.g. TC_CTRLA_MODE_COUNT32 */
} tc32_conf_t;

/**
 * @brief   Number of available timer channels
 */
#define TIMER_CHANNELS      (2)

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

/**
 * @brief   Called before the power management enters a power mode
 *
 * @param[in] deep
 */
void gpio_pm_cb_enter(int deep);

/**
 * @brief   Called after the power management left a power mode
 *
 * @param[in] deep
 */
void gpio_pm_cb_leave(int deep);

/**
 * @brief   Called before the power management enters a power mode
 *
 * @param[in] deep
 */
void cpu_pm_cb_enter(int deep);

/**
 * @brief   Called after the power management left a power mode
 *
 * @param[in] deep
 */
void cpu_pm_cb_leave(int deep);

/**
 * @brief   Wrapper for cortexm_sleep calling power management callbacks
 *
 * @param[in] deep
 */
static inline void sam0_cortexm_sleep(int deep)
{
#ifdef MODULE_PERIPH_GPIO
    gpio_pm_cb_enter(deep);
#endif

    cpu_pm_cb_enter(deep);

    cortexm_sleep(deep);

    cpu_pm_cb_leave(deep);

#ifdef MODULE_PERIPH_GPIO
    gpio_pm_cb_leave(deep);
#endif
}

/**
 * @brief   Disable alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to reset the multiplexing for
 */
void gpio_disable_mux(gpio_t pin);

/**
 * @brief   Available voltage regulators on the supply controller.
 */
typedef enum {
    SAM0_VREG_LDO,  /*< LDO, always available but not very power efficient */
    SAM0_VREG_BUCK  /*< Buck converter, efficient but may clash with internal
                        fast clock generators (see errata sheets) */
} sam0_supc_t;

/**
 * @brief       Switch the internal voltage regulator used for generating the
 *              internal MCU voltages.
 *              Available options are:
 *
 *               - LDO: not very efficient, but will always work
 *               - BUCK converter: Most efficient, but incompatible with the
 *                 use of DFLL or DPLL.
 *                 Please refer to the errata sheet, further restrictions may
 *                 apply depending on the MCU.
 *
 * @param[in]   src
 */
static inline void sam0_set_voltage_regulator(sam0_supc_t src)
{
#ifdef REG_SUPC_VREG
    SUPC->VREG.bit.SEL = src;
    while (!SUPC->STATUS.bit.VREGRDY) {}
#else
    (void) src;
    assert(0);
#endif
}

/**
 * @brief   Returns the frequency of a GCLK provider.
 *
 * @param[in] id    The ID of the GCLK
 *
 * @return          The frequency of the GCLK with the given ID.
 */
uint32_t sam0_gclk_freq(uint8_t id);

/**
 * @brief   Enables an on-demand GCLK that has been configured in cpu.c
 *
 * @param[in] id    The ID of the GCLK
 */
void sam0_gclk_enable(uint8_t id);

/**
 * @brief   Return the numeric id of a SERCOM device derived from its address
 *
 * @param[in] sercom    SERCOM device
 *
 * @return              numeric id of the given SERCOM device
 */
static inline uint8_t sercom_id(const void *sercom)
{
#ifdef SERCOM0
    if (sercom == SERCOM0) {
        return 0;
    }
#endif
#ifdef SERCOM1
    if (sercom == SERCOM1) {
        return 1;
    }
#endif
#ifdef SERCOM2
    if (sercom == SERCOM2) {
        return 2;
    }
#endif
#ifdef SERCOM3
    if (sercom == SERCOM3) {
        return 3;
    }
#endif
#ifdef SERCOM4
    if (sercom == SERCOM4) {
        return 4;
    }
#endif
#ifdef SERCOM5
    if (sercom == SERCOM5) {
        return 5;
    }
#endif
#ifdef SERCOM6
    if (sercom == SERCOM6) {
        return 6;
    }
#endif
#ifdef SERCOM7
    if (sercom == SERCOM7) {
        return 7;
    }
#endif

    /* should not be reached, so fail with assert */
    assert(false);

    return SERCOM_INST_NUM;
}

/**
 * @brief   Enable peripheral clock for given SERCOM device
 *
 * @param[in] sercom    SERCOM device
 */
static inline void sercom_clk_en(void *sercom)
{
    const uint8_t id = sercom_id(sercom);
#if defined(CPU_FAM_SAMD21)
    PM->APBCMASK.reg |= (PM_APBCMASK_SERCOM0 << id);
#elif defined (CPU_FAM_SAMD5X)
    if (id < 2) {
        MCLK->APBAMASK.reg |= (1 << (id + 12));
    } else if (id < 4) {
        MCLK->APBBMASK.reg |= (1 << (id + 7));
    } else {
        MCLK->APBDMASK.reg |= (1 << (id - 4));
    }
#else
    if (id < 5) {
        MCLK->APBCMASK.reg |= (MCLK_APBCMASK_SERCOM0 << id);
    }
#if defined(CPU_FAM_SAML21)
    else {
        MCLK->APBDMASK.reg |= (MCLK_APBDMASK_SERCOM5);
    }
#endif /* CPU_FAM_SAML21 */
#endif
}

/**
 * @brief   Disable peripheral clock for given SERCOM device
 *
 * @param[in] sercom    SERCOM device
 */
static inline void sercom_clk_dis(void *sercom)
{
    const uint8_t id = sercom_id(sercom);
#if defined(CPU_FAM_SAMD21)
    PM->APBCMASK.reg &= ~(PM_APBCMASK_SERCOM0 << id);
#elif defined (CPU_FAM_SAMD5X)
    if (id < 2) {
        MCLK->APBAMASK.reg &= ~(1 << (id + 12));
    } else if (id < 4) {
        MCLK->APBBMASK.reg &= ~(1 << (id + 7));
    } else {
        MCLK->APBDMASK.reg &= ~(1 << (id - 4));
    }
#else
    if (id < 5) {
        MCLK->APBCMASK.reg &= ~(MCLK_APBCMASK_SERCOM0 << id);
    }
#if defined (CPU_FAM_SAML21)
    else {
        MCLK->APBDMASK.reg &= ~(MCLK_APBDMASK_SERCOM5);
    }
#endif /* CPU_FAM_SAML21 */
#endif
}

#ifdef CPU_FAM_SAMD5X
static inline uint8_t _sercom_gclk_id_core(uint8_t sercom_id) {
    if (sercom_id < 2)
        return sercom_id + 7;
    if (sercom_id < 4)
        return sercom_id + 21;
    else
        return sercom_id + 30;
}
#endif

/**
 * @brief   Configure generator clock for given SERCOM device
 *
 * @param[in] sercom    SERCOM device
 * @param[in] gclk      Generator clock
 */
static inline void sercom_set_gen(void *sercom, uint8_t gclk)
{
    const uint8_t id = sercom_id(sercom);
    sam0_gclk_enable(gclk);
#if defined(CPU_FAM_SAMD21)
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN(gclk) |
                         (SERCOM0_GCLK_ID_CORE + id));
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
#elif defined(CPU_FAM_SAMD5X)
    GCLK->PCHCTRL[_sercom_gclk_id_core(id)].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk));
#else
    if (id < 5) {
        GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + id].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk));
    }
#if defined(CPU_FAM_SAML21)
    else {
        GCLK->PCHCTRL[SERCOM5_GCLK_ID_CORE].reg = (GCLK_PCHCTRL_CHEN | GCLK_PCHCTRL_GEN(gclk));
    }
#endif /* CPU_FAM_SAML21 */
#endif
}

/**
 * @brief   Returns true if the CPU woke deep sleep (backup/standby)
 */
static inline bool cpu_woke_from_backup(void)
{
#ifdef RSTC_RCAUSE_BACKUP
    return RSTC->RCAUSE.bit.BACKUP;
#else
    return false;
#endif
}

/**
 * @brief ADC Channel Configuration
 */
typedef struct {
    gpio_t pin;            /**< ADC channel pin */
    uint32_t muxpos;       /**< ADC channel pin multiplexer value */
} adc_conf_chan_t;

/**
 * @brief USB peripheral parameters
 */
#if defined(USB_INST_NUM) || defined(DOXYGEN)
typedef struct {
    gpio_t dm;              /**< D- line gpio                           */
    gpio_t dp;              /**< D+ line gpio                           */
    gpio_mux_t d_mux;       /**< alternate function (mux) for data pins */
    UsbDevice *device;      /**< ptr to the device registers            */
    uint8_t gclk_src;       /**< GCLK source which supplys 48 MHz       */
} sam0_common_usb_config_t;
#endif /* USB_INST_NUM */

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
/* Limits are in clock cycles according to data sheet.
   As the WDT is clocked by a 1024 Hz clock, 1 cycle ≈ 1 ms */
#define NWDT_TIME_LOWER_LIMIT          (8U)
#define NWDT_TIME_UPPER_LIMIT          (16384U)
/** @} */


/**
 * @brief Watchdog can be stopped.
 */
#define WDT_HAS_STOP                   (1)
/**
 * @brief Watchdog has to be initialized.
 */
#define WDT_HAS_INIT                   (1)

/**
 * The sam0 DMA peripheral has a number of channels. Each channel is a separate
 * data stream, triggered by a configurable trigger when enabled, or triggered
 * by software (not yet supported). In theory each DMA channel is equal and can
 * have a configurable priority and can be triggered by the full set of triggers
 * available.
 *
 * DMA descriptors, specifying a single transfer with size, source and
 * destination, are kept in RAM and are read when the channel is enabled and
 * triggered. On the SAML21 platform, these descriptors must reside in the LP
 * SRAM.
 */

/**
 * @brief Indicates that the peripheral doesn't utilize the DMA controller.
 *        Matches with the register configuration for software based triggers.
 */
#define DMA_TRIGGER_DISABLED           0

/**
 * @brief Move the DMA descriptors to the LP SRAM. Required on the SAML21
 */
#if defined(CPU_FAM_SAML21)
#define DMA_DESCRIPTOR_IN_LPSRAM
#endif

#ifdef DMA_DESCRIPTOR_IN_LPSRAM
#define DMA_DESCRIPTOR_ATTRS    __attribute__((section(".backup.bss")))
#else
#define DMA_DESCRIPTOR_ATTRS
#endif

/**
 * @brief DMA channel type
 */
typedef unsigned dma_t;

/**
 * @brief Available DMA address increment modes
 */
typedef enum {
    DMA_INCR_NONE   = 0,    /**< Don't increment any addresses after a beat */
    DMA_INCR_SRC    = 1,    /**< Increment the source address after a beat */
    DMA_INCR_DEST   = 2,    /**< Increment destination address after a beat */
    DMA_INCR_BOTH   = 3,    /**< Increment both addresses after a beat */
} dma_incr_t;

/**
 * @brief   Initialize DMA
 */
void dma_init(void);

/**
 * @brief Acquire a DMA channel.
 *
 * A free DMA channel is marked as allocated and a reference is returned.
 * DMA channels can be acquired for long periods of time, e.g. from the start to
 * end of a number of transfers or directly at boot and never released.
 *
 * @returns     A reference to the DMA channel
 * @returns     UINT8_MAX when no DMA channel is available
 */
dma_t dma_acquire_channel(void);

/**
 * @brief   Release a previously acquired DMA channel
 *
 * @param   dma     DMA channel to release
 */
void dma_release_channel(dma_t dma);

/**
 * @brief   Initialize a previously allocated DMA channel with one-time settings
 *
 * @param   dma     DMA channel reference
 * @param   trigger Trigger to use for this DMA channel
 * @param   prio    Channel priority
 * @param   irq     Whether to enable the interrupt handler for this channel
 */
void dma_setup(dma_t dma, unsigned trigger, uint8_t prio, bool irq);

/**
 * @brief   Prepare the DMA channel for an individual transfer.
 *
 * @param   dma     DMA channel reference
 * @param   width   Transfer beat size to use
 * @param   src     Source address for the transfer
 * @param   dst     Destination address for the transfer
 * @param   len     Number of beats to transfer
 * @param   incr    Which of the addresses to increment after a beat
 */
void dma_prepare(dma_t dma, uint8_t width, void *src, void *dst, size_t len,
                 dma_incr_t incr);

/**
 * @brief   Prepare a transfer without modifying the destination address
 *          settings.
 *
 * Can be used when repeatedly using a dma channel to transfer to the same
 * peripheral address, leaving the destination address and related settings
 * untouched
 *
 * @note This only touches the source address, length and source increment
 * settings. Be sure to initialize the full descriptor beforehand with
 * @ref dma_prepare
 *
 * @param   dma     DMA channel reference
 * @param   src     Source address for the transfer
 * @param   len     Number of beats to transfer
 * @param   incr    Whether to increment the source address after a beat
 */
void dma_prepare_src(dma_t dma, void *src, size_t len, bool incr);

/**
 * @brief   Prepare a transfer without modifying the source address
 *          settings.
 *
 * Can be used when repeatedly using a dma channel to transfer from the same
 * peripheral address, leaving the source address and related settings
 * untouched
 *
 * @note This only touches the destination address, length and destination
 * increment settings. Be sure to initialize the full descriptor beforehand with
 * @ref dma_prepare
 *
 * @param   dma     DMA channel reference
 * @param   dst     Destination address for the transfer
 * @param   len     Number of beats to transfer
 * @param   incr    Whether to increment the destination address after a beat
 */
void dma_prepare_dst(dma_t dma, void *dst, size_t len, bool incr);

/**
 * @brief   Append a second transfer descriptor after the default channel
 *          descriptor.
 *
 * @note Only a single extra transfer descriptor is supported for now.
 * @note @p descriptor must remain valid throughout the full transfer duration
 *
 * @param   dma         DMA channel reference to add the descriptor to
 * @param   descriptor  Extra transfer descriptor to append
 * @param   width       Transfer beat size to use
 * @param   src         Source address for the transfer
 * @param   dst         Destination address for the transfer
 * @param   len         Number of beats to transfer
 * @param   incr        Which of the addresses to increment after a beat
 */
void dma_append(dma_t dma, DmacDescriptor *descriptor, uint8_t width,
                void *src, void *dst, size_t len, dma_incr_t incr);

/**
 * @brief   Append a second transfer descriptor after the default channel
 *          descriptor, copying destination and block size from the initial
 *          descriptor.
 *
 * @note Only a single extra transfer descriptor is supported for now.
 * @note @p descriptor must remain valid throughout the full transfer duration
 *
 * @param   dma     DMA channel reference to add the descriptor to
 * @param   next    Extra transfer descriptor to append
 * @param   src     Source address for the transfer
 * @param   len     Number of beats to transfer
 * @param   incr    Whether to increment the source address after a beat
 */
void dma_append_src(dma_t dma, DmacDescriptor *next, void *src, size_t len,
                    bool incr);

/**
 * @brief   Append a second transfer descriptor after the default channel
 *          descriptor, copying source and block size from the initial
 *          descriptor.
 *
 * @note Only a single extra transfer descriptor is supported for now.
 * @note @p descriptor must remain valid throughout the full transfer duration
 *
 * @param   dma     DMA channel reference to add the descriptor to
 * @param   next    Extra transfer descriptor to append
 * @param   dst     Destination address for the transfer
 * @param   len     Number of beats to transfer
 * @param   incr    Whether to increment the source address after a beat
 */
void dma_append_dst(dma_t dma, DmacDescriptor *next, void *dst, size_t len,
                    bool incr);

/**
 * @brief   Start a DMA transfer.
 *
 * @param   dma     DMA channel reference
 */
void dma_start(dma_t dma);

/**
 * @brief   Wait for a DMA channel to finish the transfer.
 *
 * This function uses a blocking mutex to wait for the transfer to finish
 *
 * @note Use only with DMA channels of which the interrupt is enabled
 *
 * @param   dma     DMA channel reference
 */
void dma_wait(dma_t dma);

/**
 * @brief   Cancel an active DMA transfer
 *
 * It is not harmful to call this on an inactive channel, but it will waste some
 * processing time
 *
 * @param   dma     DMA channel reference
 */
void dma_cancel(dma_t dma);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
