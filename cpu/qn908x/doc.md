/* NXP QN908x specific information for the `periph` drivers */
/**

@defgroup    cpu_qn908x NXP QN908x
@ingroup     cpu
@brief       NXP QN908x BLE-enabled Cortex-M4F MCU specific implementation

The NXP QN908x family of chips such as the QN9080 feature a Cortex-M4F,
Bluetooth Low Energy, USB 2.0 and in some SKUs like the QN9080SIP NFC as well.
The CPU is designed to be ultra-low-power and high-performance, allowing
applications with small battery capacity. It includes an optional DC-DC and LDO,
low power sleep timers, I2C, SPI, ADC, SPIFI and several other peripherals.


@defgroup    cpu_qn908x_cpuid NXP QN908x CPUID
@ingroup     cpu_qn908x
@brief       NXP QN908x CPUID driver

No configuration is necessary. The CPUID value is based on the factory assigned
default Bluetooth address in the read-only flash section which may not be the
Bluetooth address used by the Bluetooth module if a different one was programmed
there.


@defgroup    cpu_qn908x_gpio NXP QN908x GPIO
@ingroup     cpu_qn908x
@brief       NXP QN908x GPIO driver

The GPIO driver uses the @ref GPIO_PIN(port, pin) macro to declare pins.

No configuration is necessary.

@defgroup    cpu_qn908x_adc NXP QN908x ADC - Analog to Digital converter
@ingroup     cpu_qn908x
@brief       NXP QN908x ADC driver

This ADC is a differential sigma-delta ADC. There are 9 external signals
named ADC0 to ADC7 and ADC_VREFI that can be connected to specific external GPIO
pins. There are several combinations of measurements possible with the ADC
module using these external signals as well as some internal signals, but not
every combination is a possible input pair to the ADC.

The ADC block runs at either 4 MHz or 32 KHz from the high speed or low speed
clock sources respective. An additional divisor is available to select some
intermediate clock values. However, this is not the sample rate, since a single
sample from @ref adc_sample() requires multiple internal samples which are then
filtered and decimated by the hardware, giving about 128 slower sample rate than
the selected clock.

Each board-defined ADC line is configured with a single integer value which is
the logic or of the following values:
 - The differential pair of signals among the options in @ref
   qn908x_adc_channel_t,
 - For those channels that use the "Vinn" signal, a value selecting
   the Vinn signal in @ref qn908x_adc_vinn_t,
 - The reference voltage "Vref" that the ADC will use for its full range
   selected with @ref qn908x_adc_vref_t,
 - The ADC input gain as selected by @ref qn908x_adc_gain_t, which will
   multiply the differential input by a factor between 0.5 and 2, and
 - An optional gain flag @ref ADC_VREF_GAIN_X15.
 - An optional PGA enabled flag @ref ADC_PGA_ENABLE.

The hardware resolution of the ADC data is always 23-bits signed, but smaller
resolutions can be requested which will result in a smaller output value.

An internal temperature sensor is available and connected to the ADC when
selecting @ref ADC_CHANNEL_TEMP as the channel. In this case the returned
value is still a number that represents the temperature dependent voltage level
of the internal signal which then needs to be converted to a temperature by the
application using calibration parameters. When using the internal temperature
sensor, the 1.2V bandgap Vref is recommended with a Vinn of 1/2 Vref since the
measured voltage is about 800 mV at room temperature.

A special microvolts (ADC_RES_UV) resolution value for @ref adc_res_t is
supported when using the internal 1.2 V bandgap as the reference voltage, in
which case @ref adc_sample will return the measured value in microvolts as a
signed integer, with a max range of +/- 1.8 V when using the x1.5 Vref
multiplier.
This special resolution mode takes into account the factory calibration of
the internal reference voltage for more accurate readings. In any other case,
the return value is a signed integer with as many bits as resolution
requested not including the sign bit. Note that the return value may be a
negative when measuring a negative differential voltage between the plus and
minus side of the input.

For example, if 8-bit resolution is requested for an ADC line where the channel
connects the - side to Vinn configured as Vss, a maximum value of 255 can be
returned when the + side level is as high as the Vref signal. However, a
negative value of -255 is also possible if Vinn is configured as Vref and
the + side level is as low as Vss.

### ADC configuration example (for periph_conf.h) ###

@code
    static const adc_conf_t adc_config[] = {
        /* Pin A11 to Vss, 1.8v Vref. */
        ADC_CHANNEL_ADC7_VINN | ADC_VREF_GAIN_X15,
        /* Pin A10 to A11, 1.2V Vref. */
        ADC_CHANNEL_ADC6_ADC7,
        /* Temperature (in V) over to 0.6 V, 1.2 V Vref. */
        ADC_CHANNEL_TEMP | ADC_VINN_VREF_2,
        /* Internal "battery monitor", Vcc/4 to Vss, 1.2V Vref. */
        ADC_CHANNEL_VCC4_VINN | ADC_VINN_AVSS,
    };
    #define ADC_NUMOF           ARRAY_SIZE(adc_config)

    #define QN908X_ADC_CLOCK    ADC_CLOCK_500K
@endcode


@defgroup    cpu_qn908x_i2c NXP QN908x I2C
@ingroup     cpu_qn908x
@brief       NXP QN908x I2C driver

There are several FLEXCOMM interfaces in this chip, but only two of these
support I2C (FLEXCOMM1 and FLEXCOMM2) which are mapped as I2C0 and I2C1,
respectively. A single FLEXCOMM interface can only be used for one of the I2C,
UART or SPI interfaces, so for example USART1 and I2C0 can't be used at the
same time since they are both the same FLEXCOMM1 interface.

### I2C configuration example (for periph_conf.h) ###

    static const i2c_conf_t i2c_config[] = {
        {
            .dev          = I2C0,
            .pin_scl      = GPIO_PIN(PORT_A, 6),  // or A8, A12, A20
            .pin_sda      = GPIO_PIN(PORT_A, 7),  // or A9, A13, A21
            .speed        = I2C_SPEED_FAST,
        },
        {
            .dev          = I2C1,
            .pin_scl      = GPIO_PIN(PORT_A, 2),  // or A5, A23, A27
            .pin_sda      = GPIO_PIN(PORT_A, 3),  // or A4, A22, A26
            .speed        = I2C_SPEED_FAST,
        },
    };
    #define I2C_NUMOF           ARRAY_SIZE(i2c_config)


@defgroup    cpu_qn908x_spi NXP QN908x Serial Peripheral Interface (SPI)
@ingroup     cpu_qn908x
@brief       NXP QN908x timer driver

Two of the FLEXCOMM interfaces in this chip can be used as SPI interfaces named
SPI0 and SPI1, which correspond to FLEXCOMM2 and FLEXCOMM3. Note that FLEXCOMM2
(SPI0) is also shared with the I2C peripheral I2C1 and both can't be used at
the same time.

The SPI flexcomm clock is directly driven from the AHB bus, so its clock is
limited by the core CPU clock and the AHB divisor on the higher side with an
optional frequency divider of up to 65536 to generate lower clock frequencies.

Multiple peripherals can be connected to the same SPI bus, using different CS
pins, with a maximum of 4 hardware CS peripherals per bus and any number of
software CS peripherals.

This driver uses the [OSHA SPI Signal Names](
https://www.oshwa.org/a-resolution-to-redefine-spi-signal-names/) and while it
only implements the Controller mode, the hardware is capable of operating in
Peripheral mode as well so we use the COPI/CIPO names.

### SPI configuration example (for periph_conf.h) ###

The following example uses only one hardware CS (number 0) and leaves the rest
unused. Check the user manual for the full list of CS pins available.

When configuring the CS line on a driver, you should pass a @ref SPI_HWCS to use
the hardware CS mode defined in this configuration. To use any other GPIO as a
CS line selected by software it is also possible to pass a @ref GPIO_PIN pin.

@code
    static const spi_conf_t spi_config[] = {
        {
            .dev            = SPI0,
            .cipo_pin       = GPIO_PIN(PORT_A, 5),
            .copi_pin       = GPIO_PIN(PORT_A, 4),
            .clk_pin        = GPIO_PIN(PORT_A, 30),
            .cs_pin         = {
                GPIO_PIN(PORT_A, 3), /* Use as SPI_HWCS(0) */
                GPIO_UNDEF,
                GPIO_UNDEF,
                GPIO_UNDEF
            },
        },
    };
@endcode


@defgroup    cpu_qn908x_timer NXP QN908x Standard counter/timers (CTIMER)
@ingroup     cpu_qn908x
@brief       NXP QN908x timer driver

The QN908x have 4 standard counter/timers (CTIMER). These timers allow to count
clock cycles from the APB clock with a 32-bit prescaler, effectively dividing
the APB clock frequency by a configurable number up to 2^32, allowing a great
range of timer frequencies selected at runtime. Each timer has 4 independent
channels to match against which can generate an interrupt.

TODO: These CTIMERs and the SCT timers can both be used as PWM as well, with
different set of capabilities. Boards should be able to split these CTIMER and
SCT blocks between pwm and timer functions.

### Timer configuration example (for periph_conf.h) ###

    #define TIMER_NUMOF           4


@defgroup    cpu_qn908x_rtc NXP QN908x Real-Time-Clock (RTC)
@ingroup     cpu_qn908x
@brief       NXP QN908x RTC driver

The RTC block in the QN908x can be driven by the external 32.768 kHz crystal or
by the internal 32 kHz RCO oscillator clock, whichever is selected as the
`CLK_32K` clock source. The RTC has an internal "second counter" calibrated
depending on the frequency of the clock source selected at the time the RTC
clock is initialized by calling @ref rtc_init.

The RTC function in this cpu doesn't have a match against a target value to
generate an interrupt like the timer peripheral, instead, the alarm function in
the rtc.h interface is implemented by an interrupt generated every second which
checks the target value in software. Keep in mind that while the RTC can operate
while the cpu is the power-down 0 mode, using the alarm functionality during
that time means that the cpu will wake up every second for a brief moment,
potentially impacting the power consumption.

No RTC-specific configuration is necessary.


@defgroup    cpu_qn908x_uart NXP QN908x UART
@ingroup     cpu_qn908x
@brief       NXP QN908x UART driver

There are several FLEXCOMM interfaces in this chip, but only two of these
support UART (FLEXCOMM0 and FLEXCOMM1). The default UART mode is 8n1 and can
be changed with the uart_mode() function. If only RX or only TX is desired, the
other pin can be set to GPIO_UNDEF.

### UART configuration example (for periph_conf.h) ###

    static const uart_conf_t uart_config[] = {
        {
            .dev          = USART0,
            .rx_pin       = GPIO_PIN(PORT_A, 17),  // or A5
            .tx_pin       = GPIO_PIN(PORT_A, 16),  // or A4
        },
        {
            .dev          = USART1,
            .rx_pin       = GPIO_PIN(PORT_A, 9),  // or A13
            .tx_pin       = GPIO_PIN(PORT_A, 8),  // or A12
        },
    };
    #define UART_NUMOF          ARRAY_SIZE(uart_config)


@defgroup    cpu_qn908x_wdt NXP QN908x Watchdog timer (WDT)
@ingroup     cpu_qn908x
@brief       NXP QN908x Watchdog timer (WDT)

The Watchdog timer in the NXP QN908x starts disabled on reset: the clock bit
`CLK_WDT_EN` is enabled in the `CLK_EN` register on reset so the timer is
running but the interrupt and reset functions are disabled. However, after the
read-only bootloader ROM in the QN908x transfer the control flow to the user
application (the RIOT kernel) the Watchdog is enabled with a timeout of 10
seconds.

If your board does not include the `periph_wdt` module, the Watchdog will be
disabled at `cpu_init()` time and there's no configuration necessary. However,
if your board or application does include it, the Watchdog will be left
configured with the 10 second timeout set by the Bootloader and you need to
call `wdt_setup_reboot()` or `wdt_setup_reboot_with_callback()` within the first
10 seconds.

The WDT block supports different clock sources which would be configured by the
board since they depend on whether the optional crystals are populated in your
board. Nevertheless, the millisecond values passed to `wdt_setup_reboot*` are
internally converted to clock ticks using the clock configured at the time the
function was called. `wdt_setup_reboot*()` can be called multiple times to
change the WDT parameters or after changing the WDT clock source, but in any
case `wdt_start()` must be called after it to start the WDT operation.

Once the WDT triggers, it is not possible to avoid the device reboot and calling
wdt_kick() from the WDT callback (if any) or after the callback was called will
not have any effect. Note that, however, if the WDT callback returns before the
configured CONFIG_WDT_WARNING_PERIOD the CPU will continue executing the code
before the WDT interrupt occurred. If this is not desired, an infinite loop at
the end of the WDT callback, after the safety operations have been performed is
advisable.

*/
