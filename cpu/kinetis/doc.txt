/* Kinetis specific information for the periph drivers */
/**

@defgroup    cpu_kinetis NXP Kinetis
@ingroup     cpu
@brief       NXP Kinetis Cortex-M MCU specific implementation


@defgroup    cpu_kinetis_adc Kinetis ADC
@ingroup     cpu_kinetis
@brief       Kinetis ADC driver

### ADC configuration example (for periph_conf.h) ###

    static const adc_conf_t adc_config[] = {
        { .dev = ADC0, .pin = GPIO_UNDEF          , .chan =  0 }, // ADC0_DP0
        { .dev = ADC0, .pin = GPIO_UNDEF          , .chan = 19 }, // ADC0_DM0
        { .dev = ADC1, .pin = GPIO_UNDEF          , .chan =  0 }, // ADC1_DP0
        { .dev = ADC1, .pin = GPIO_UNDEF          , .chan = 19 }, // ADC1_DM0
        { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  0), .chan =  8 }, // PTB0
        { .dev = ADC0, .pin = GPIO_PIN(PORT_B,  1), .chan =  9 }, // PTB1
        { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  1), .chan = 15 }, // PTC1
        { .dev = ADC0, .pin = GPIO_PIN(PORT_C,  2), .chan =  4 }, // PTC2
    };
    #define ADC_NUMOF           ARRAYSIZE(adc_config)


@defgroup    cpu_kinetis_cpuid Kinetis CPUID
@ingroup     cpu_kinetis
@brief       Kinetis CPUID driver

No configuration is necessary.


@defgroup    cpu_kinetis_gpio Kinetis GPIO
@ingroup     cpu_kinetis
@brief       Kinetis GPIO driver

The GPIO driver uses the @ref GPIO_PIN(port, pin) macro to declare pins.

No configuration is necessary.


@defgroup    cpu_kinetis_i2c Kinetis I2C
@ingroup     cpu_kinetis
@brief       Kinetis I2C driver

### I2C configuration example (for periph_conf.h) ###

    static const i2c_conf_t i2c_config[] = {
        {
            .i2c = I2C0,
            .scl_pin = GPIO_PIN(PORT_B, 2),
            .sda_pin = GPIO_PIN(PORT_B, 3),
            .freq = CLOCK_BUSCLOCK,
            .speed = I2C_SPEED_FAST,
            .irqn = I2C0_IRQn,
            .scl_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
            .sda_pcr = (PORT_PCR_MUX(2) | PORT_PCR_ODE_MASK),
        },
    };
    #define I2C_NUMOF           ARRAYSIZE(i2c_config)
    #define I2C_0_ISR           (isr_i2c0)


@defgroup    cpu_kinetis_pwm Kinetis PWM
@ingroup     cpu_kinetis
@brief       Kinetis PWM driver

### PWM configuration example (for periph_conf.h) ###

    #define PWM_NUMOF           (1U)
    #define PWM_0_EN            1
    #define PWM_MAX_CHANNELS    2

    #define PWM_0_DEV           FTM0
    #define PWM_0_CHANNELS      2
    #define PWM_0_CLK           (48e6)
    #define PWM_0_CLKEN()       (SIM->SCGC6 |= (SIM_SCGC6_FTM0_MASK))
    #define PWM_0_CLKDIS()      (SIM->SCGC6 &= ~(SIM_SCGC6_FTM0_MASK))
    #define PWM_0_PORT_CLKEN()  (SIM->SCGC5 |= (SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTA_MASK))

    #define PWM_0_PIN_CH0       4
    #define PWM_0_FTMCHAN_CH0   1
    #define PWM_0_PORT_CH0      PORTA
    #define PWM_0_PIN_AF_CH0    3

    #define PWM_0_PIN_CH1       4
    #define PWM_0_FTMCHAN_CH1   4
    #define PWM_0_PORT_CH1      PORTD
    #define PWM_0_PIN_AF_CH1    4


@defgroup    cpu_kinetis_rnga Kinetis RNGA
@ingroup     cpu_kinetis
@brief       Kinetis RNGA driver

RNGA generates data that looks random. Reference Manual recommends to use the
RNGA as an entropy source for PRNGs.

### RNGA configuration example (for periph_conf.h) ###

    #define RANDOM_NUMOF         (1U)
    #define KINETIS_RNGA         RNG
    #define RANDOM_CLKEN()       (SIM->SCGC6 |= (1 << 9))
    #define RANDOM_CLKDIS()      (SIM->SCGC6 &= ~(1 << 9))


@defgroup    cpu_kinetis_rtc Kinetis RTC
@ingroup     cpu_kinetis
@brief       Kinetis RTC driver

RTC is clocked by a 32.768 kHz clock.

Please note the manual of your MCU or SiP for the clock setting for the RTC
module. After initialization Time Seconds Register (TSR) increments once a
second. The TSR (also TAR) value will be converted to the struct tm and back
with the help of stdlib functions that are defined in time.h. The driver
supports alarm, it is stored in the Time Alarm Registers (TAR) and the unit is
seconds.

### RTC configuration example (for periph_conf.h) ###

    #define RTC_DEV             RTC
    #define RTC_UNLOCK()        (SIM->SCGC6 |= (SIM_SCGC6_RTC_MASK))

Optional settings to configure internal load capacitors (see reference manual):

    #define RTC_LOAD_CAP_BITS   0


@defgroup    cpu_kinetis_spi Kinetis SPI
@ingroup     cpu_kinetis
@brief       Kinetis SPI driver

The SPI baud rate and other timings are generated from the bus clock via
prescalers, the hardware module allows for very detailed timing configuration,
but a tool exists to generate a standard timing configuration for any given
module clock frequency. The timing configuration tool is found in
cpu/kinetis/dist/calc_spi_scalers

Finer tuning of timings than the RIOT SPI API is capable of is supported by
modifying the generated configuration. See the reference manual for your
Kinetis CPU (Chapter: "SPI module, Functional description, Module baud rate and
clock delay generation") for a description of each delay.

The SPI driver supports using GPIO pins for chip select, as an alternative to
using hardware chip select. The pins specified in spi_config[x].pin_cs[y] are
the hardware chip select pins, designated SPIx_PCSy in the hardware reference
documentation. These pins can not be chosen arbitrarily but must be selected
from the pins which support the SPIx_PCSy function in the pin multiplexing
table found in the reference manual.

To use a hardware controlled chip select pin, use the SPI_HWCS macro instead of
GPIO_PIN when calling spi_acquire() in the device driver. Software managed CS
signals can use any GPIO pin, at the cost of extra delays in the transfer
because of the additional overhead of calling gpio_set/clear at every transfer.

### SPI configuration example (for periph_conf.h): ###

    static const uint32_t spi_clk_config[] = {
    - Use cpu/kinetis/dist/calc_spi_scalers to generate the timing configuration -
    };

    static const spi_conf_t spi_config[] = {
        {
            .dev      = SPI0,
            .pin_miso = GPIO_PIN(PORT_D, 3),
            .pin_mosi = GPIO_PIN(PORT_D, 2),
            .pin_clk  = GPIO_PIN(PORT_D, 1),
            .pin_cs   = {
                GPIO_PIN(PORT_D, 0),
                GPIO_PIN(PORT_D, 4),
                GPIO_PIN(PORT_D, 5),
                GPIO_PIN(PORT_D, 6),
                GPIO_UNDEF
            },
            .pcr      = GPIO_AF_2,
            .simmask  = SIM_SCGC6_SPI0_MASK
        },
        {
            .dev      = SPI1,
            .pin_miso = GPIO_PIN(PORT_E, 3),
            .pin_mosi = GPIO_PIN(PORT_E, 1),
            .pin_clk  = GPIO_PIN(PORT_E, 2),
            .pin_cs   = {
                GPIO_PIN(PORT_E, 4),
                GPIO_UNDEF,
                GPIO_UNDEF,
                GPIO_UNDEF,
                GPIO_UNDEF
            },
            .pcr      = GPIO_AF_2,
            .simmask  = SIM_SCGC6_SPI1_MASK
        }
    };
    #define SPI_NUMOF           ARRAYSIZE(spi_config)


@defgroup    cpu_kinetis_timer Kinetis Timer
@ingroup     cpu_kinetis
@brief       Periodic Interrupt Timer (PIT) and Low-Power Timer (LPTMR) driver.

The PIT is a count down timer, in order to use it with riot-os
a count up timer will be simulated. The PIT has four channels,
each two channels are cascaded. The n-1 channel is a prescaler
and the n channel a down counter. In standard configuration
with four channels, two simulated count up timer are possible.

To counteract the effects of the asynchronous operation of the
LPTMR, this driver uses the RTT as a time base which the LPTMR
is referenced against. This method reduces the timing jitter
caused by mixing the clock domains of the bus clock used by the
CPU and the 32kHz reference clock for the LPTMR counter.

### Timer configuration example (for periph_conf.h) ###


    #define PIT_NUMOF               (2U)
    #define PIT_CONFIG {                 \
            {                            \
                .prescaler_ch = 0,       \
                .count_ch = 1,           \
            },                           \
            {                            \
                .prescaler_ch = 2,       \
                .count_ch = 3,           \
            },                           \
        }
    #define LPTMR_NUMOF             (1U)
    #define LPTMR_CONFIG { \
            { \
                .dev = LPTMR0, \
                .irqn = LPTMR0_IRQn, \
            } \
        }
    #define TIMER_NUMOF             ((PIT_NUMOF) + (LPTMR_NUMOF))

    #define PIT_BASECLOCK           (CLOCK_BUSCLOCK)
    #define PIT_ISR_0               isr_pit1
    #define PIT_ISR_1               isr_pit3
    #define LPTMR_ISR_0             isr_lptmr0


@defgroup    cpu_kinetis_uart Kinetis UART
@ingroup     cpu_kinetis
@brief       Kinetis UART driver

There are different implementations of the UART interface. The treatment of
interrupts is also slightly different. The register UARTx_BDH to UARTx_C4 look
almost the same. We distinguish the type of the UART using the BRFA field in
the UART C4 register. Currently, only the base TX/RX functionality is
available.

### UART configuration example (for periph_conf.h) ###

    static const uart_conf_t uart_config[] = {
        {
            .dev    = UART0,
            .freq   = CLOCK_CORECLOCK,
            .pin_rx = GPIO_PIN(PORT_A, 14),
            .pin_tx = GPIO_PIN(PORT_A, 15),
            .pcr_rx = PORT_PCR_MUX(3),
            .pcr_tx = PORT_PCR_MUX(3),
            .irqn   = UART0_RX_TX_IRQn,
            .scgc_addr = &SIM->SCGC4,
            .scgc_bit = SIM_SCGC4_UART0_SHIFT,
            .mode   = UART_MODE_8N1,
        },
        {
            .dev    = UART1,
            .freq   = CLOCK_CORECLOCK,
            .pin_rx = GPIO_PIN(PORT_C, 3),
            .pin_tx = GPIO_PIN(PORT_C, 4),
            .pcr_rx = PORT_PCR_MUX(3),
            .pcr_tx = PORT_PCR_MUX(3),
            .irqn   = UART1_RX_TX_IRQn,
            .scgc_addr = &SIM->SCGC4,
            .scgc_bit = SIM_SCGC4_UART1_SHIFT,
            .mode   = UART_MODE_8N1,
        },
    };
    #define UART_NUMOF          ARRAYSIZE(uart_config)

*/
