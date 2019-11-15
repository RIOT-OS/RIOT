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

/**
 * @name    Power mode configuration
 * @{
 */
#ifdef CPU_SAML1X
#define PM_NUM_MODES        (2)
#else
#define PM_NUM_MODES        (3)
#endif
/** @} */

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
 * @brief   UART device configuration
 */
typedef struct {
    SercomUsart *dev;       /**< pointer to the used UART device */
    gpio_t rx_pin;          /**< pin used for RX */
    gpio_t tx_pin;          /**< pin used for TX */
    gpio_mux_t mux;         /**< alternative function for pins */
    uart_rxpad_t rx_pad;    /**< pad selection for RX line */
    uart_txpad_t tx_pad;    /**< pad selection for TX line */
    uart_flag_t flags;      /**< set optional SERCOM flags */
    uint32_t gclk_src;      /**< GCLK source which supplys SERCOM */
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
    uint16_t gclk_src;      /**< GCLK source which supplys Timer */
    uint16_t prescaler;     /**< prescaler used by the Timer */
    uint16_t flags;         /**< flags for CTRA, e.g. TC_CTRLA_MODE_COUNT32 */
} tc32_conf_t;

/**
 * @brief   Set up alternate function (PMUX setting) for a PORT pin
 *
 * @param[in] pin   Pin to set the multiplexing for
 * @param[in] mux   Mux value
 */
void gpio_init_mux(gpio_t pin, gpio_mux_t mux);

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
static inline void sercom_set_gen(void *sercom, uint32_t gclk)
{
    const uint8_t id = sercom_id(sercom);
#if defined(CPU_FAM_SAMD21)
    GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN | gclk |
                         (SERCOM0_GCLK_ID_CORE + id));
    while (GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY) {}
#elif defined(CPU_FAM_SAMD5X)
    GCLK->PCHCTRL[_sercom_gclk_id_core(id)].reg = (GCLK_PCHCTRL_CHEN | gclk);
#else
    if (id < 5) {
        GCLK->PCHCTRL[SERCOM0_GCLK_ID_CORE + id].reg = (GCLK_PCHCTRL_CHEN | gclk);
    }
#if defined(CPU_FAM_SAML21)
    else {
        GCLK->PCHCTRL[SERCOM5_GCLK_ID_CORE].reg = (GCLK_PCHCTRL_CHEN | gclk);
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

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_CPU_COMMON_H */
/** @} */
