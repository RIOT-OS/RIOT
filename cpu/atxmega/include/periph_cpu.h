/*
 * Copyright (C) 2021 Gerson Fernando Budke <nandojve@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_atxmega
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Gerson Fernando Budke <nandojve@gmail.com>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <avr/io.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name   Length of the CPU_ID in octets
 * @{
 */
#define CPUID_LEN               (11U)
/** @} */

/**
 * @name   Interrupt level used to control nested interrupts
 * @{
 */
typedef enum {
    CPU_INT_LVL_OFF,    /**< Interrupt Disabled     */
    CPU_INT_LVL_LOW,    /**< Interrupt Low Level    */
    CPU_INT_LVL_MID,    /**< Interrupt Medium Level */
    CPU_INT_LVL_HIGH,   /**< Interrupt High Level   */
} cpu_int_lvl_t;
/** @} */

/**
 * @brief   Available ports on the ATxmega family
 */
enum {
    PORT_A,     /**< port A - 600 - 0 */
    PORT_B,     /**< port B - 620 - 1 */
    PORT_C,     /**< port C - 640 - 2 */
    PORT_D,     /**< port D - 660 - 3 */
    PORT_E,     /**< port E - 680 - 4 */
    PORT_F,     /**< port F - 6A0 - 5 */
    PORT_G,     /**< port G - 6C0 - 6 */
    PORT_H,     /**< port H - 6E0 - 7 */
    PORT_J,     /**< port J - 700 - 8 */
    PORT_K,     /**< port K - 720 - 9 */
    PORT_L,     /**< port L - 740 - A */
    PORT_M,     /**< port M - 760 - B */
    PORT_N,     /**< port N - 780 - C */
    PORT_P,     /**< port P - 7A0 - D */
    PORT_Q,     /**< port Q - 7C0 - E */
    PORT_R,     /**< port R - 7E0 - F */
    /* ... */
    PORT_MAX,
};

/**
 * @brief   Power Reduction Peripheral Mask
 */
typedef uint16_t pwr_reduction_t;

/**
 * @brief   Define a CPU specific Power Reduction index macro
 */
#define PWR_RED_REG(reg, dev) ((reg << 8) | dev)

/**
 * @brief   Define a CPU specific Power Reduction index macro
 */
enum {
    PWR_GENERAL_POWER,
    PWR_PORT_A,
    PWR_PORT_B,
    PWR_PORT_C,
    PWR_PORT_D,
    PWR_PORT_E,
    PWR_PORT_F,
};

/**
 * @name    Power management configuration
 * @{
 */
#define PM_NUM_MODES            (5)
#define AVR8_PM_SLEEP_MODE_0    SLEEP_MODE_PWR_DOWN    /**< Power Down */
#define AVR8_PM_SLEEP_MODE_1    SLEEP_MODE_PWR_SAVE    /**< Power Save */
#define AVR8_PM_SLEEP_MODE_2    SLEEP_MODE_STANDBY     /**< Standby */
#define AVR8_PM_SLEEP_MODE_3    SLEEP_MODE_EXT_STANDBY /**< Extended Standby*/
/** @} */

/**
 * @brief   Define the number of GPIO interrupts vectors for ATxmega CPU.
 * @{
 */
#define GPIO_EXT_INT_NUMOF      (2 * PORT_MAX)
/** @} */

/**
 * @brief   Override GPIO type
 * @{
 */
#define HAVE_GPIO_T
typedef uint16_t gpio_t;
/** @} */

/**
 * @brief   Definition of a fitting UNDEF value
 */
#define GPIO_UNDEF              (0xffff)

/**
 * @brief   Define a CPU specific GPIO pin generator macro
 *
 * The ATxmega internally uses pin mask to manipulate all gpio functions.
 * This allows simultaneous pin actions at any method call.  ATxmega specific
 * applications can use ATXMEGA_GPIO_PIN macro to define pins and generic
 * RIOT-OS application should continue to use GPIO_PIN API for compatibility.
 *
 * @{
 */
#define ATXMEGA_GPIO_PIN(x, y)  (((x & 0x0f) << 8) | (y & 0xff))
#define GPIO_PIN(x, y)          ATXMEGA_GPIO_PIN(x, (1U << (y & 0x07)))
/** @} */

/**
 * @brief   Available pin modes
 *
 * Generally, a pin can be configured to be input or output. In output mode, a
 * pin can further be put into push-pull or open drain configuration. Though
 * this is supported by most platforms, this is not always the case, so driver
 * implementations may return an error code if a mode is not supported.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum GPIO_MODE {
    GPIO_SLEW_RATE            = (1 << 7), /**< enable slew rate */
    GPIO_INVERTED             = (1 << 6), /**< enable inverted signal */

    GPIO_OPC_TOTEN            = (0 << 3), /**< select no pull resistor (TOTEM) */
    GPIO_OPC_BSKPR            = (1 << 3), /**< push-pull mode (BUSKEEPER) */
    GPIO_OPC_PD               = (2 << 3), /**< pull-down resistor */
    GPIO_OPC_PU               = (3 << 3), /**< pull-up resistor */
    GPIO_OPC_WRD_OR           = (4 << 3), /**< enable wired OR  */
    GPIO_OPC_WRD_AND          = (5 << 3), /**< enable wired AND */
    GPIO_OPC_WRD_OR_PULL      = (6 << 3), /**< enable wired OR and pull-down resistor */
    GPIO_OPC_WRD_AND_PULL     = (7 << 3), /**< enable wired AND and pull-up resistor */

    GPIO_ANALOG               = (1 << 1), /**< select GPIO for analog function */

    GPIO_IN                   = (0 << 0), /**< select GPIO MASK as input */
    GPIO_OUT                  = (1 << 0), /**< select GPIO MASK as output */

    /* Compatibility Mode */
    GPIO_IN_PU                = GPIO_IN  | GPIO_OPC_PU,
    GPIO_IN_PD                = GPIO_IN  | GPIO_OPC_PD,
    GPIO_OD                   = GPIO_OUT | GPIO_OPC_WRD_OR,
    GPIO_OD_PU                = GPIO_OUT | GPIO_OPC_WRD_OR_PULL,
} gpio_mode_t;
/** @} */

/**
 * @brief   Definition of possible active flanks for external interrupt mode
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_ISC_BOTH             = (0 << 4), /**< emit interrupt on both flanks (default) */
    GPIO_ISC_RISING           = (1 << 4), /**< emit interrupt on rising flank */
    GPIO_ISC_FALLING          = (2 << 4), /**< emit interrupt on falling flank */
    GPIO_ISC_LOW_LEVEL        = (3 << 4), /**< emit interrupt on low level */

    GPIO_INT_DISABLED_ALL     = (1 << 3), /**< disable all interrupts */

    GPIO_INT0_VCT             = (0 << 2), /**< enable interrupt on Vector 0 (default) */
    GPIO_INT1_VCT             = (1 << 2), /**< enable interrupt on Vector 1 */

    GPIO_LVL_OFF              = (0 << 0), /**< interrupt disabled (default) */
    GPIO_LVL_LOW              = (1 << 0), /**< interrupt low level */
    GPIO_LVL_MID              = (2 << 0), /**< interrupt medium level */
    GPIO_LVL_HIGH             = (3 << 0), /**< interrupt higher */

    /* Compatibility Mode */
    GPIO_FALLING              = GPIO_ISC_FALLING | GPIO_LVL_LOW,
    GPIO_RISING               = GPIO_ISC_RISING  | GPIO_LVL_LOW,
    GPIO_BOTH                 = GPIO_ISC_BOTH    | GPIO_LVL_LOW,
} gpio_flank_t;
/** @} */

/**
 * @brief   Max number of available UARTs
 */
#define UART_MAX_NUMOF          (7)

/**
 * @brief   Size of the UART TX buffer for non-blocking mode.
 */
#ifndef UART_TXBUF_SIZE
#define UART_TXBUF_SIZE         (64)
#endif

/**
 * @brief   UART device configuration
 */
typedef struct {
    USART_t *dev;                   /**< pointer to the used UART device */
    pwr_reduction_t pwr;            /**< Power Management */
    gpio_t rx_pin;                  /**< pin used for RX */
    gpio_t tx_pin;                  /**< pin used for TX */
#ifdef MODULE_PERIPH_UART_HW_FC
    gpio_t rts_pin;                 /**< RTS pin */
    gpio_t cts_pin;                 /**< CTS pin */
#endif
    cpu_int_lvl_t rx_int_lvl;       /**< RX Complete Interrupt Level */
    cpu_int_lvl_t tx_int_lvl;       /**< TX Complete Interrupt Level */
    cpu_int_lvl_t dre_int_lvl;      /**< Data Registry Empty Interrupt Level */
} uart_conf_t;

/**
 * @brief   Max number of available timer channels
 */
#define TIMER_CH_MAX_NUMOF      (4)

/**
 * @brief   A low-level timer_set() implementation is provided
 */
#define PERIPH_TIMER_PROVIDES_SET

/**
 * @brief   Timer Type
 *
 * Timer Type 1 is equal to Type 0 (two channels instead four)
 * Timer Type 2 is Type 0 configured as two 8 bit timers instead one 16 bit
 * Timer Type 2 won't be available as a standard timer
 * Timer Type 5 is equal to Type 4 (two channels instead four)
 */
typedef enum {
    TC_TYPE_0                 = 0,
    TC_TYPE_1                 = 1,
    TC_TYPE_2                 = 2,
    TC_TYPE_4                 = 4,
    TC_TYPE_5                 = 5,
} timer_type_t;

/**
 * @brief   Timer device configuration
 *
 * All timers can be derived from TC0_t struct. Need check at runtime the
 * type and number of channels to perform all operations.
 */
typedef struct {
    TC0_t *dev;                                 /**< Pointer to the used as Timer device */
    pwr_reduction_t pwr;                        /**< Power Management */
    timer_type_t type;                          /**< Timer Type */
    cpu_int_lvl_t int_lvl[TIMER_CH_MAX_NUMOF];  /**< Interrupt channels level */
} timer_conf_t;

/**
 * @name    Override I2C clock speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       =   10000ul, /**< low speed mode:     ~10 kbit/s */
    I2C_SPEED_NORMAL    =  100000ul, /**< normal mode:       ~100 kbit/s */
    I2C_SPEED_FAST      =  400000ul, /**< fast mode:         ~400 kbit/s */
    I2C_SPEED_FAST_PLUS = 1000000ul, /**< fast plus mode:   ~1000 kbit/s */
    /* High speed is not supported without external hardware hacks */
    I2C_SPEED_HIGH      = 3400000ul, /**< high speed mode:  ~3400 kbit/s */
} i2c_speed_t;
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
 * @brief   I2C configuration structure
 */
typedef struct {
    TWI_t *dev;             /**< Pointer to hardware module registers */
    pwr_reduction_t pwr;    /**< Power Management */
    gpio_t sda_pin;         /**< SDA GPIO pin */
    gpio_t scl_pin;         /**< SCL GPIO pin */
    i2c_speed_t speed;      /**< Configured bus speed, actual speed may be lower but never higher */
    cpu_int_lvl_t int_lvl;  /**< Serial Interrupt Level */
} i2c_conf_t;

/**
 * @brief   Enable common SPI functions
 * @{
 */
#define PERIPH_SPI_NEEDS_INIT_CS
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
#define PERIPH_SPI_NEEDS_TRANSFER_REG
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Define global value for undefined SPI device
 * @{
 */
#define SPI_UNDEF               (UCHAR_MAX)
/** @} */

/**
 * @brief Define spi_t data type to save data
 * @{
 */
#define HAVE_SPI_T
typedef uint8_t spi_t;
/** @} */

/**
 * @brief  SPI device configuration
 * @{
 */
typedef struct {
    SPI_t *dev;                     /**< pointer to the used SPI device */
    pwr_reduction_t pwr;            /**< Power Management */
    gpio_t sck_pin;                 /**< pin used for SCK */
    gpio_t miso_pin;                /**< pin used for MISO */
    gpio_t mosi_pin;                /**< pin used for MOSI */
    gpio_t ss_pin;                  /**< pin used for SS line */
} spi_conf_t;
/** @} */

/**
 * @brief  Available SPI clock speeds
 * @{
 */
#define HAVE_SPI_CLK_T
typedef enum {
    SPI_CLK_100KHZ = 100000U,       /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = 400000U,       /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = 1000000U,      /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = 5000000U,      /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = 10000000U,     /**< drive the SPI bus with 10MHz */
} spi_clk_t;
/** @} */

#if defined(__AVR_ATxmega64A1__)   || \
    defined(__AVR_ATxmega128A1__)  || \
    defined(__AVR_ATxmega64A1U__)  || \
    defined(__AVR_ATxmega128A1U__) || \
    defined(Doxygen)
/**
 * @brief   EBI (External Bus Interface)
 * {@
 */

/**
 * @brief   EBI Low Pin Count (LPC) Mode Address Latch Enable (ALE) config
 */
typedef enum {
    EBI_LPC_MODE_ALE1       = 0x01,     /**< Data multiplexed with Address byte 0 */
    EBI_LPC_MODE_ALE12      = 0x03,     /**< Data multiplexed with Address byte 0 and 1 */
} ebi_lpc_mode_t;

/**
 * @brief   EBI Port Access Flags
 *
 * Indicate what should be configured
 */
typedef enum {
    EBI_PORT_3PORT          = 0x01,     /**< Three Port Config */
    EBI_PORT_SDRAM          = 0x02,     /**< SDRAM Port Config */
    EBI_PORT_SRAM           = 0x04,     /**< SRAM Port Config */
    EBI_PORT_LPC            = 0x08,     /**< Low Pin Count Port Config */
    EBI_PORT_CS0            = 0x10,     /**< Chip Select 0 Config */
    EBI_PORT_CS1            = 0x20,     /**< Chip Select 1 Config */
    EBI_PORT_CS2            = 0x40,     /**< Chip Select 2 Config */
    EBI_PORT_CS3            = 0x80,     /**< Chip Select 3 Config */
    EBI_PORT_CS_ALL         = 0xF0,     /**< Chip Select 0-3 Config */
} ebi_port_mask_t;

/**
 * @brief   SDRAM Column Address Strobe latency
 */
typedef enum {
    EBI_SDRAM_CAS_LAT_2CLK  = 0x00,     /**< 2 Clk PER2 cycles delay */
    EBI_SDRAM_CAS_LAT_3CLK  = 0x01,     /**< 3 Clk PER2 cycles delay */
} ebi_sdram_cas_latency_t;

/**
 * @brief   SDRAM number of Row Bits
 */
typedef enum {
    EBI_SDRAM_ROW_BITS_11  = 0x00,     /**< 11 row bits */
    EBI_SDRAM_ROW_BITS_12  = 0x01,     /**< 12 row bits */
} ebi_sdram_row_bits_t;

/**
 * @brief   EBI maximum Chip Select
 */
#define PERIPH_EBI_MAX_CS   (4)

/**
 * @brief   EBI SDRAM Chip Select
 */
#define PERIPH_EBI_SDRAM_CS (3)

/* for compatibility between different versions of AVR libc: Legacy versions
 * occasionally use ASPACE instead of ASIZE for some MCUs, while new AVR libc
 * uses ASIZE as name consistently */
#ifndef EBI_CS_ASIZE_gm
typedef EBI_CS_ASPACE_t EBI_CS_ASIZE_t;
#define EBI_CS_ASIZE_256B_gc EBI_CS_ASPACE_256B_gc
#define EBI_CS_ASIZE_512B_gc EBI_CS_ASPACE_512B_gc
#define EBI_CS_ASIZE_1KB_gc EBI_CS_ASPACE_1KB_gc
#define EBI_CS_ASIZE_2KB_gc EBI_CS_ASPACE_2KB_gc
#define EBI_CS_ASIZE_4KB_gc EBI_CS_ASPACE_4KB_gc
#define EBI_CS_ASIZE_8KB_gc EBI_CS_ASPACE_8KB_gc
#define EBI_CS_ASIZE_16KB_gc EBI_CS_ASPACE_16KB_gc
#define EBI_CS_ASIZE_32KB_gc EBI_CS_ASPACE_32KB_gc
#define EBI_CS_ASIZE_64KB_gc EBI_CS_ASPACE_64KB_gc
#define EBI_CS_ASIZE_128KB_gc EBI_CS_ASPACE_128KB_gc
#define EBI_CS_ASIZE_256KB_gc EBI_CS_ASPACE_256KB_gc
#define EBI_CS_ASIZE_512KB_gc EBI_CS_ASPACE_512KB_gc
#define EBI_CS_ASIZE_1MB_gc EBI_CS_ASPACE_1MB_gc
#define EBI_CS_ASIZE_2MB_gc EBI_CS_ASPACE_2MB_gc
#define EBI_CS_ASIZE_4MB_gc EBI_CS_ASPACE_4MB_gc
#define EBI_CS_ASIZE_8MB_gc EBI_CS_ASPACE_8MB_gc
#define EBI_CS_ASIZE_16MB_gc EBI_CS_ASPACE_16MB_gc
#endif

/**
 * @brief   EBI Chip Select configuration structure
 */
typedef struct {
    EBI_CS_MODE_t   mode;               /**< Chip Select address mode */
    EBI_CS_ASIZE_t  space;              /**< Chip Select address space */
    EBI_CS_SRWS_t   wait;               /**< SRAM Wait State Selection */
    uint32_t        address;            /**< Chip Select Base Address */
} ebi_cs_t;

/**
 * @brief   EBI SDRAM configuration structure
 */
typedef struct {
    uint8_t refresh;                     /**< Self-Refresh Enabled */
    uint16_t refresh_period;             /**< microseconds */
    uint16_t init_dly;                   /**< microseconds */
    EBI_CS_SDMODE_t mode;                /**< Access Mode */
    ebi_sdram_cas_latency_t cas_latency; /**< CAS Latency */
    ebi_sdram_row_bits_t row_bits;       /**< ROW bits */
    EBI_SDCOL_t column_bits;             /**< COLUMN bits */
    EBI_MRDLY_t ld_mode_dly;             /**< Number of Clk PER2 cycles */
    EBI_ROWCYCDLY_t row_cycle_dly;       /**< Number of Clk PER2 cycles */
    EBI_RPDLY_t row_prechage_dly;        /**< Number of Clk PER2 cycles */
    EBI_WRDLY_t write_recovery_dly;      /**< Number of Clk PER2 cycles */
    EBI_ESRDLY_t exit_self_rfsh_dly;     /**< Number of Clk PER2 cycles */
    EBI_ROWCOLDLY_t row_to_column_dly;   /**< Number of Clk PER2 cycles */
} ebi_sd_t;

/**
 * @brief   EBI configuration structure
 *
 * The ebi_conf_t define the whole external memory that ATxmega A1 can address.
 * It is recommended read all EBI chapter from XMEGA-AU manual.
 *
 * The external address space can be used to address external peripherals and
 * expand SRAM.  The ebi driver provide methods to read/write in external
 * address space.  The SRAM can be expanded up to 64k when one chip select
 * address have value equal to zero. To allow expand external RAM both
 * @b RAM_LEN and @b EXP_RAM variables should be override at board
 * makefile.include file.
 *
 * When expanding RAM for use with RIOT-OS, the memory address must be aligned
 * at external memory size boundary, respecting power of 2.  In this case, to
 * add 32K memory on the system, the chip select address should be set to 0,
 * or 32k, or 64 etc.  This means that when the board have external memory and
 * user wants to map part of that memory to expand RAM, both internal and
 * external memories must have same start address @b A, inclusive, see image
 * for details. This is necessary to make sure RIOT-OS have a contiguous address
 * space.  The drawback is that first @b Xk external memory will be lost.
 * Assuming internal top address memory is @b B and external top address memory
 * is @b C.  The XMEGA will automatically access internal memory <b><= B</b>
 * when address collide with external memory.  At address <b>>= B + 1</b>, XMEGA
 * will access memory thru EBI.  Also, note that @b RAM_LEN must be a power
 * of 2, so it can't e.g. be 48k.
 *
 *   C      ------
 *          |    |
 *          |    |
 *          |    |
 *   B -----|    |
 *     |    |    |
 *     |    |    |
 *   A -----------
 *
 * @note To avoid parser problems, @b RAM_LEN must be defined as decimal value.
 *
 * Example: Add 256K of external RAM
 *
 * The max addressable RAM by SP is 64K due to limit of 16 bits.  In this case,
 * RAM will be 64K.  The remaining RAM can be addressed only by ebi_mem methods
 * and GCC doesn't see it.
 *
 * At board/periph_conf.h:
 *
 * static const ebi_conf_t ebi_config = {
 *   ...
 *   .cs                     = {
 *                              { EBI_CS_MODE_DISABLED_gc,
 *                                0,
 *                                EBI_CS_SRWS_0CLK_gc,
 *                                0x0UL,
 *                              },
 *                              { EBI_CS_MODE_DISABLED_gc,
 *                                0,
 *                                EBI_CS_SRWS_0CLK_gc,
 *                                0x0UL,
 *                              },
 *                              { EBI_CS_MODE_LPC_gc,
 *                                EBI_CS_ASIZE_256KB_gc,
 *                                EBI_CS_SRWS_1CLK_gc,
 *                                0x0UL,
 *                              },
 *                              { EBI_CS_MODE_DISABLED_gc,
 *                                0,
 *                                EBI_CS_SRWS_0CLK_gc,
 *                                0x0UL,
 *                              },
 *                            },
 * };
 *
 * At board/Makefile.include:
 * override RAM_LEN = 65536
 * override EXP_RAM = 1
 *
 * Example: Add 32K of external RAM and a LCD
 *
 * At board/periph_conf.h:
 *
 * static const ebi_conf_t ebi_config = {
 * ...
 *   .cs                     = {
 *                              { EBI_CS_MODE_DISABLED_gc,
 *                                0,
 *                                EBI_CS_SRWS_0CLK_gc,
 *                                0x0UL,
 *                              },
 *                              { EBI_CS_MODE_DISABLED_gc,
 *                                0,
 *                                EBI_CS_SRWS_0CLK_gc,
 *                                0x0UL,
 *                              },
 *                              { EBI_CS_MODE_LPC_gc,
 *                                EBI_CS_ASIZE_32KB_gc,
 *                                EBI_CS_SRWS_1CLK_gc,
 *                                0x0UL,
 *                              },
 *                              { EBI_CS_MODE_LPC_gc,
 *                                EBI_CS_ASIZE_256B_gc,
 *                                EBI_CS_SRWS_5CLK_gc,
 *                                0x100000UL,
 *                              },
 *                            },
 * };
 *
 * At board/Makefile.include:
 * override RAM_LEN = 32768
 * override EXP_RAM = 1
 *
 * This data structure a mandatory configuration for A1 variation. If no
 * external memory is used the module can be disabled defining data struct
 * with all zeros, as below:
 *
 * static const ebi_conf_t ebi_config = { 0 };
 *
 * or, for a temporary disable, set addr_bits to 0 at periph_conf.h:
 *
 * static const ebi_conf_t ebi_config = {
 *     .addr_bits = 0,
 *     ...
 * };
 *
 */
typedef struct {
    uint8_t addr_bits;                  /**< EBI port address lines */
    ebi_port_mask_t flags;              /**< EBI port flags */
    uint8_t sram_ale;                   /**< Number of ALE for SRAM mode */
    uint8_t lpc_ale;                    /**< Number of ALE for LPC mode */
    ebi_sd_t sdram;                     /**< SDRAM configuration */
    ebi_cs_t cs[PERIPH_EBI_MAX_CS];     /**< Chip Select configuration */
} ebi_conf_t;
/** @} */

#endif /* __AVR_ATxmegaxxxA1x__ */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_H */
