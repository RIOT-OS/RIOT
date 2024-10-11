/*
 * Copyright (C) 2020 Koen Zandberg <koen@bergzand.net>
 *               2023 Gunar Schorcht <gunar@schorcht.net>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup         cpu_gd32v
 * @{
 *
 * @file
 * @brief           CPU specific definitions for internal peripheral handling
 *
 * @author          Koen Zandberg <koen@bergzand.net>
 * @author          Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef PERIPH_CPU_H
#define PERIPH_CPU_H

#include <inttypes.h>

#include "cpu.h"
#include "clic.h"
#include "kernel_defines.h"
#include "macros/units.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Power management configuration
 * @{
 */
/**
 * @brief   Number of usable low power modes
 */
#define PM_NUM_MODES    (3U)    /**< Number of usable low power modes */

/**
 * @brief   Power modes
 *
 * The GD32V has three power modes (terminology as defined by GigaDevice).
 * - Sleep:      Only the clock of the RISC-V core is switched off.
 * - Deep sleep: The RISC-V core including all AHB and APB peripheralsa and all
 *               high speed clocks are off. The LDO is in operation and the
 *               SRAM is retained.
 *               The MCU can be woken up by external interrupts or events
 *               without restart.
 * - Standby:    The RISC-V core including all AHB and APB peripherals, all
 *               high-speed clocks, and the LDO are off. The SRAM is not
 *               retained.
 *               The MCU can be woken up by WKUP or the NRST pin, watchdog
 *               reset and RTC alarms with restart.
 */
enum {
    GD32V_PM_STANDBY = 0,       /**< STANDBY mode,  */
    GD32V_PM_DEEPSLEEP = 1,     /**< DEEPSLEEP mode, corresponds to STOP mode of STM32 */
    GD32V_PM_IDLE = 2           /**< IDLE mode */
};

/**
 * @brief   Wake-up pin used
 */
#ifndef CONFIG_PM_EWUP_USED
#define CONFIG_PM_EWUP_USED     (0U)
#endif
/** @} */

/**
 * @brief   On-Chip buses
 */
typedef enum {
    AHB,    /**< Advanced High-performance Bus */
    APB1,   /**< Advanced Peripheral Bus 1     */
    APB2,   /**< Advanced Peripheral Bus 2     */
} bus_t;

/**
 * @brief   Available GPIO ports
 */
enum {
#ifdef GPIOA
    PORT_A  = 0,            /**< port A */
#endif
#ifdef GPIOB
    PORT_B  = 1,            /**< port B */
#endif
#ifdef GPIOC
    PORT_C  = 2,            /**< port C */
#endif
#ifdef GPIOD
    PORT_D  = 3,            /**< port D */
#endif
#ifdef GPIOE
    PORT_E  = 4,            /**< port E */
#endif
};

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default gpio_t type definition
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
 * @brief   Define a CPU specific GPIO pin generator macro
 */
#define GPIO_PIN(x, y)      ((GPIOA_BASE + (x << 10)) | y)

/**
 * @brief   Generate GPIO mode bitfields
 *
 * We use 5 bit to encode the mode:
 * - bit 0+1: pin mode (input / output)
 * - bit 2+3: pull resistor configuration
 * - bit   4: output type (0: push-pull, 1: open-drain)
 */
#define GPIO_MODE(io, pr, ot)   ((io << 0) | (pr << 2) | (ot << 4))

/**
 * @brief   Override GPIO mode options
 *
 * We use 4 bit to encode CNF and MODE.
 * @{
 */
#define HAVE_GPIO_MODE_T
typedef enum {
    GPIO_IN     = GPIO_MODE(0, 1, 0),   /**< input w/o pull R */
    GPIO_IN_PD  = GPIO_MODE(0, 2, 0),   /**< input with pull-down */
    GPIO_IN_PU  = GPIO_MODE(0, 2, 1),   /**< input with pull-up */
    GPIO_OUT    = GPIO_MODE(3, 0, 0),   /**< push-pull output */
    GPIO_OD     = GPIO_MODE(3, 1, 0),   /**< open-drain w/o pull R */
    GPIO_OD_PU  = (0xff)                /**< not supported by HW */
} gpio_mode_t;
/** @} */

/**
 * @brief   Override flank configuration values
 * @{
 */
#define HAVE_GPIO_FLANK_T
typedef enum {
    GPIO_RISING = 1,        /**< emit interrupt on rising flank */
    GPIO_FALLING = 2,       /**< emit interrupt on falling flank */
    GPIO_BOTH = 3           /**< emit interrupt on both flanks */
} gpio_flank_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   Override alternative GPIO mode options
 */
typedef enum {
    GPIO_AF_OUT_PP  = 0xb,  /**< alternate function output - push-pull */
    GPIO_AF_OUT_OD  = 0xf,  /**< alternate function output - open-drain */
} gpio_af_t;

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

/**
 * @brief   Configure the given pin to be used as ADC input
 *
 * @param[in] pin       pin to configure
 */
void gpio_init_analog(gpio_t pin);

/* Hide this from Doxygen to avoid merging implementation details into
 * public view on type */
#ifndef DOXYGEN

#define HAVE_GPIO_STATE_T
typedef enum {
    GPIO_OUTPUT_PUSH_PULL,
    GPIO_OUTPUT_OPEN_DRAIN,
    GPIO_OUTPUT_OPEN_SOURCE,
    GPIO_INPUT,
    GPIO_USED_BY_PERIPHERAL,
    GPIO_DISCONNECT,
} gpio_state_t;

#define HAVE_GPIO_PULL_T
typedef enum {
    GPIO_FLOATING,
    GPIO_PULL_UP,
    GPIO_PULL_DOWN,
    GPIO_PULL_KEEP,
} gpio_pull_t;

#define HAVE_GPIO_PULL_STRENGTH_T
typedef enum {
    GPIO_PULL_WEAKEST = 0,
    GPIO_PULL_WEAK = 0,
    GPIO_PULL_STRONG = 0,
    GPIO_PULL_STRONGEST = 0
} gpio_pull_strength_t;

#define HAVE_GPIO_DRIVE_STRENGTH_T
typedef enum {
    GPIO_DRIVE_WEAKEST = 0,
    GPIO_DRIVE_WEAK = 0,
    GPIO_DRIVE_STRONG = 0,
    GPIO_DRIVE_STRONGEST = 0
} gpio_drive_strength_t;

#define HAVE_GPIO_SLEW_T
typedef enum {
    GPIO_SLEW_SLOWEST = 0,
    GPIO_SLEW_SLOW = 1,
    GPIO_SLEW_FAST = 2,
    GPIO_SLEW_FASTEST = 2,
} gpio_slew_t;

#define HAVE_GPIO_CONF_T
typedef union gpio_conf_gd32v gpio_conf_t;

#endif /* !DOXYGEN */

/**
 * @brief       GPIO pin configuration for GD32V MCUs.
 * @ingroup     drivers_periph_gpio_ll
 */
union gpio_conf_gd32v {
    uint8_t bits;  /**< the raw bits */
    struct {
        /**
         * @brief   State of the pin
         */
        gpio_state_t state              : 3;
        /**
         * @brief   Pull resistor configuration
         */
        gpio_pull_t pull                : 2;
        /**
         * @brief   Configure the slew rate of outputs
         *
         * @warning If the requested slew rate is not available, the closest fit
         *          supported will be configured instead.
         *
         * This value is ignored *unless* @ref gpio_conf_stm32::state is
         * configured to @ref GPIO_OUTPUT_PUSH_PULL or @ref GPIO_OUTPUT_OPEN_DRAIN.
         */
        gpio_slew_t slew_rate           : 2;
        /**
         * @brief   Initial value of the output
         *
         * Ignored if @ref gpio_conf_stm32::state is set to @ref GPIO_INPUT or
         * @ref GPIO_DISCONNECT. If the pin was previously in a high impedance
         * state, it is guaranteed to directly transition to the given initial
         * value.
         *
         * @ref gpio_ll_query_conf will write the current value of the specified
         * pin here, which is read from the input register when the state is
         * @ref GPIO_INPUT, otherwise the state from the output register is
         * consulted.
         */
        bool initial_value          : 1;
    };
};

/**
 * @brief   Available number of ADC devices
 */
#define ADC_DEVS            (2U)

/**
 * @brief   ADC channel configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the channel */
    uint8_t dev;            /**< ADCx - 1 device used for the channel */
    uint8_t chan;           /**< CPU ADC channel connected to the pin */
} adc_conf_t;

/**
 * @brief   GD32V DAC has 2 channels
 */
#define DAC_CHANNEL_NUMOF (2)

/**
 * @brief   DAC line configuration data
 */
typedef struct {
    gpio_t pin;             /**< pin connected to the line */
    uint8_t chan;           /**< DAC device used for this line */
} dac_conf_t;

/**
 * @brief   GD32V timers have 4 capture-compare channels
 */
#define TIMER_CHANNEL_NUMOF (4U)

/**
 * @brief   Macro for accessing the capture/compare register of a timer channel
 */
#define TIMER_CHANNEL(tim, chan) *(&dev(tim)->CH0CV + (chan * 2))

/**
 * @brief   Timer configuration
 */
typedef struct {
    TIMER_Type *dev;        /**< timer device */
    uint32_t max;           /**< maximum value to count to (16/32 bit) */
    uint32_t rcu_mask;      /**< corresponding bit in the RCC register */
    uint8_t bus;            /**< APBx bus the timer is clock from */
    uint8_t irqn;           /**< global IRQ channel */
} timer_conf_t;

/**
 * @brief   Structure for UART configuration data
 */
typedef struct {
    USART_Type *dev;        /**< UART device base register address */
    uint32_t rcu_mask;      /**< bit in clock enable register */
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    uint8_t bus;            /**< APB bus */
    uint8_t irqn;           /**< IRQ channel */
} uart_conf_t;

/**
 * @brief   UART interrupt priority
 */
#define UART_ISR_PRIO       (2)

/**
 * @brief   Define a magic number that tells us to use hardware chip select
 *
 * We use a random value here, that does clearly differentiate from any possible
 * GPIO_PIN(x) value.
 */
#define SPI_HWCS_MASK       (0xffffff00)

/**
 * @brief   Override the default SPI hardware chip select access macro
 *
 * Since the CPU does only support one single hardware chip select line, we can
 * detect the usage of non-valid lines by comparing to SPI_HWCS_VALID.
 */
#define SPI_HWCS(x)         (SPI_HWCS_MASK | x)

/**
 * @brief   Define value for unused CS line
 */
#define SPI_CS_UNDEF        (GPIO_UNDEF)

#ifndef DOXYGEN
/**
 * @brief   Overwrite the default spi_cs_t type definition
 * @{
 */
#define HAVE_SPI_CS_T
typedef uint32_t spi_cs_t;
/** @} */
#endif

/**
 * @brief   Use the shared SPI functions
 * @{
 */
/** Use transfer byte function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_BYTE
/** Use transfer reg function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REG
/** Use transfer regs function from periph common */
#define PERIPH_SPI_NEEDS_TRANSFER_REGS
/** @} */

/**
 * @brief   Override SPI clock speed values
 * @{
 */
#define HAVE_SPI_CLK_T
enum {
    SPI_CLK_100KHZ = KHZ(100), /**< drive the SPI bus with 100KHz */
    SPI_CLK_400KHZ = KHZ(400), /**< drive the SPI bus with 400KHz */
    SPI_CLK_1MHZ   = MHZ(1),   /**< drive the SPI bus with 1MHz */
    SPI_CLK_5MHZ   = MHZ(5),   /**< drive the SPI bus with 5MHz */
    SPI_CLK_10MHZ  = MHZ(10),  /**< drive the SPI bus with 10MHz */
};

/**
 * @brief   SPI clock type
 */
typedef uint32_t spi_clk_t;
/** @} */

/**
 * @brief   Structure for SPI configuration data
 */
typedef struct {
    SPI_Type *dev;          /**< SPI device base register address */
    gpio_t mosi_pin;        /**< MOSI pin */
    gpio_t miso_pin;        /**< MISO pin */
    gpio_t sclk_pin;        /**< SCLK pin */
    spi_cs_t cs_pin;        /**< HWCS pin, set to SPI_CS_UNDEF if not mapped */
    uint32_t rcumask;       /**< bit in the RCC peripheral enable register */
    uint8_t apbbus;         /**< APBx bus the device is connected to */
#ifdef MODULE_PERIPH_DMA
    dma_t tx_dma;           /**< Logical DMA stream used for TX */
    uint8_t tx_dma_chan;    /**< DMA channel used for TX */
    dma_t rx_dma;           /**< Logical DMA stream used for RX */
    uint8_t rx_dma_chan;    /**< DMA channel used for RX */
#endif
} spi_conf_t;

/**
 * @name    Use the shared I2C functions
 * @{
 */
/** Use read reg function from periph common */
#define PERIPH_I2C_NEED_READ_REG
/** Use write reg function from periph common */
#define PERIPH_I2C_NEED_WRITE_REG
/** Use read regs function from periph common */
#define PERIPH_I2C_NEED_READ_REGS
/** Use write regs function from periph common */
#define PERIPH_I2C_NEED_WRITE_REGS
/** @} */

#ifndef DOXYGEN
/**
 * @brief   Default mapping of I2C bus speed values
 * @{
 */
#define HAVE_I2C_SPEED_T
typedef enum {
    I2C_SPEED_LOW       = KHZ(10),      /**< low speed mode: ~10kit/s */
    I2C_SPEED_NORMAL    = KHZ(100),     /**< normal mode:    ~100kbit/s */
    I2C_SPEED_FAST      = KHZ(400),     /**< fast mode:      ~400kbit/s */
    I2C_SPEED_FAST_PLUS = MHZ(1),       /**< fast plus mode: ~1Mbit/s */
} i2c_speed_t;
/** @} */
#endif /* ndef DOXYGEN */

/**
 * @brief   I2C configuration options
 */
typedef struct {
    I2C_Type *dev;              /**< i2c device */
    i2c_speed_t speed;          /**< i2c bus speed */
    gpio_t scl_pin;             /**< scl pin number */
    gpio_t sda_pin;             /**< sda pin number */
    uint32_t rcu_mask;          /**< bit in clock enable register */
    IRQn_Type irqn;             /**< I2C event interrupt number */
} i2c_conf_t;

/**
 * @brief   PWM channel
 */
typedef struct {
    gpio_t pin;             /**< GPIO pin mapped to this channel */
    uint8_t cc_chan;        /**< capture compare channel used */
} pwm_chan_t;

/**
 * @brief   PWM configuration
 */
typedef struct {
    TIMER_Type *dev;                        /**< Timer used */
    uint32_t rcu_mask;                      /**< bit in clock enable register */
    uint32_t remap;                         /**< AFIO remap mask to route periph
                                                 to other pins (or zero, if not
                                                 needed) */
    pwm_chan_t chan[TIMER_CHANNEL_NUMOF];   /**< channel mapping set to
                                                 {GPIO_UNDEF, 0} if not used */
    gpio_af_t af;                           /**< alternate function used */
    uint8_t bus;                            /**< APB bus */
} pwm_conf_t;

/**
 * @name    WDT upper and lower bound times in ms
 * @{
 */
#define NWDT_TIME_LOWER_LIMIT           (1)
/* Ensure the internal "count" variable stays within the uint32 bounds.
   This variable corresponds to max_time * RTC_FREQ / MS_PER_SEC. On fe310,
   RTC_FREQ is 32768Hz. The 15 right shift is equivalent to a division by RTC_FREQ.
 */
#define NWDT_TIME_UPPER_LIMIT           ((UINT32_MAX >> 15)  * MS_PER_SEC + 1)
/** @} */

/**
 * @brief   WDT interrupt priority: use highest priority
 */
#define WDT_INTR_PRIORITY               (PLIC_NUM_PRIORITIES)

/**
 * @brief   No brakes on the WDT train
 */
#define WDT_HAS_STOP                    (0)

/**
 * @name    RTT/RTC configuration
 * @{
 */
#define RTT_DEV             RTC             /**< RTC is used as RTT device */

#define RTT_IRQ             RTC_ALARM_IRQn  /**< RTC_ALARM_IRQn is used as IRQ number */
#define RTT_IRQ_PRIORITY    (2)             /**< RTT interrupt priority */
#define RTT_MAX_VALUE       (0xffffffff)    /**< maximum RTT value */

#define RTT_MIN_FREQUENCY   (1U)            /**< minimum RTT frequency in Hz */
/** @} */

/**
 * @name    USB device definitions
 * @{
 */
#define USBDEV_SET_ADDR_AFTER_STATUS    0   /**< Set device address after SETUP stage */
#define USBDEV_NUM_ENDPOINTS            4   /**< Number of USB OTG FS endpoints including EP0 */
/** @} */

/**
 * @brief   Enable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCU enable register
 */
void periph_clk_en(bus_t bus, uint32_t mask);

/**
 * @brief   Disable the given peripheral clock
 *
 * @param[in] bus       bus the peripheral is connected to
 * @param[in] mask      bit in the RCU enable register
 */
void periph_clk_dis(bus_t bus, uint32_t mask);

/**
 * @brief   Get the actual bus clock frequency for the APB buses
 *
 * @param[in] bus       target APBx bus
 *
 * @return              bus clock frequency in Hz
 */
uint32_t periph_apb_clk(bus_t bus);

/**
 * @brief   Configure the alternate function for the given pin
 *
 * @param[in] pin       pin to configure
 * @param[in] af        alternate function to use
 */
void gpio_init_af(gpio_t pin, gpio_af_t af);

void gd32vf103_clock_init(void);
void gd32v_enable_irc8(void);
void gd32v_disable_irc8(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_H */
