#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

#include "cpu.h"
#include "cpu_conf.h"
#include "kernel_defines.h"
#include "vendor/gpio_psoc6_02_124_bga.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef HAVE_GPIO_T
#define HAVE_GPIO_T
typedef unsigned int gpio_t;
#endif

#ifndef GPIO_UNDEF
#define GPIO_UNDEF          ((gpio_t)(UINT_MAX))
#endif

#ifndef GPIO_PIN
#define GPIO_PIN(port, pin) (((uint8_t)(port) << 3U) | ((uint8_t)(pin) & 0x07U))
#endif

#ifndef UART_DEV
#define UART_DEV(x)         (x)
#endif

#define GPIO_PORT_NUM(pin)  ((uint8_t)((pin) >> 3U))
#define GPIO_PIN_NUM(pin)   ((uint8_t)((pin) & 0x07U))
#define GPIO_PORT(pin)      (&GPIO->PRT[GPIO_PORT_NUM(pin)])
#define _port_num(pin)      GPIO_PORT_NUM(pin)
#define _pin_num(pin)       GPIO_PIN_NUM(pin)

#define PSOC6_DM_BITS       (4U)
#define PSOC6_DM_MASK       (0x0FU)
#define PSOC6_DM_HIGHZ      (0x08UL)
#define PSOC6_DM_PULLUP     (0x0AUL)
#define PSOC6_DM_PULLDOWN   (0x0BUL)
#define PSOC6_DM_OD_SLOW    (0x0CUL)
#define PSOC6_DM_OD_PULLUP  (0x0DUL)
#define PSOC6_DM_STRONG     (0x0EUL)

#define DRIVE_MODE_BITS     PSOC6_DM_BITS
#define DRIVE_MODE_MASK     PSOC6_DM_MASK
#define SCB_FIFO_SIZE       (128UL)
#define PSOC6_HSIOM_GPIO    (0UL)
#define PSOC6_SCB_CTRL_MODE_UART      (2UL)
#define PERIPH_TIMER_PROVIDES_SET

#define CPUSS_CM0_SYSTEM_INT_CTL_CPU_INT_IDX_Pos                 CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_IDX_Pos
#define CPUSS_CM0_SYSTEM_INT_CTL_CPU_INT_IDX_Msk                 CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_IDX_Msk
#define CPUSS_CM0_SYSTEM_INT_CTL_CPU_INT_VALID_Pos               CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_VALID_Pos
#define CPUSS_CM0_SYSTEM_INT_CTL_CPU_INT_VALID_Msk               CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_VALID_Msk

#define CY_SCB_SPI_CTRL_CLK_MODE_Pos    SCB_SPI_CTRL_CPHA_Pos
#define CY_SCB_SPI_CTRL_CLK_MODE_Msk    (SCB_SPI_CTRL_CPHA_Msk | SCB_SPI_CTRL_CPOL_Msk)
#define CY_SCB_SPI_CTRL_SSEL_POLARITY_Pos   SCB_SPI_CTRL_SSEL_POLARITY0_Pos
#define CY_SCB_SPI_CTRL_SSEL_POLARITY_Msk   (SCB_SPI_CTRL_SSEL_POLARITY0_Msk | \
                                             SCB_SPI_CTRL_SSEL_POLARITY1_Msk | \
                                             SCB_SPI_CTRL_SSEL_POLARITY2_Msk | \
                                             SCB_SPI_CTRL_SSEL_POLARITY3_Msk)

#define SCB_I2C_CTRL_MODE_Pos    SCB_I2C_CTRL_SLAVE_MODE_Pos
#define SCB_I2C_CTRL_MODE_Msk    (SCB_I2C_CTRL_SLAVE_MODE_Msk | \
                                  SCB_I2C_CTRL_MASTER_MODE_Msk)
#define CY_SCB_I2C_ADDRESS_Pos   (1UL)
#define CY_SCB_I2C_ADDRESS_Msk   (0xFEUL)
#define CY_SCB_I2C_WRITE_XFER    (0U)
#define CY_SCB_I2C_READ_XFER     (1U)
#define CY_SCB_I2C_MASTER_INTR_ALL (SCB_INTR_M_I2C_ARB_LOST_Msk | \
                                    SCB_INTR_M_I2C_BUS_ERROR_Msk | \
                                    SCB_INTR_M_I2C_NACK_Msk | \
                                    SCB_INTR_M_I2C_STOP_Msk | \
                                    SCB_INTR_M_I2C_ACK_Msk)
#define SCB_I2C_ENABLE_ANALOG_FILTER  (_VAL2FLD(SCB_I2C_CFG_SDA_IN_FILT_TRIM, 3UL)   | \
                                       _VAL2FLD(SCB_I2C_CFG_SDA_IN_FILT_SEL, 1UL)    | \
                                       _VAL2FLD(SCB_I2C_CFG_SCL_IN_FILT_SEL, 1UL)    | \
                                       _VAL2FLD(SCB_I2C_CFG_SDA_OUT_FILT0_TRIM, 2UL) | \
                                       _VAL2FLD(SCB_I2C_CFG_SDA_OUT_FILT1_TRIM, 2UL) | \
                                       _VAL2FLD(SCB_I2C_CFG_SDA_OUT_FILT2_TRIM, 2UL))
#define SCB_I2C_RX_CTRL         (_VAL2FLD(SCB_RX_CTRL_DATA_WIDTH, 7UL) | \
                                  SCB_RX_CTRL_MSB_FIRST_Msk)

#if defined(SCB_INTR_M_I2C_MASTER_TX_DONE_Msk)
#define PSOC6_I2C_TX_DONE_MASK   SCB_INTR_M_I2C_MASTER_TX_DONE_Msk
#elif defined(SCB_INTR_M_I2C_MASTER_TX_BYTE_DONE_Msk)
#define PSOC6_I2C_TX_DONE_MASK   SCB_INTR_M_I2C_MASTER_TX_BYTE_DONE_Msk
#elif defined(SCB_INTR_M_I2C_TX_DONE_Msk)
#define PSOC6_I2C_TX_DONE_MASK   SCB_INTR_M_I2C_TX_DONE_Msk
#else
#define PSOC6_I2C_TX_DONE_MASK   (SCB_INTR_M_I2C_ACK_Msk | SCB_INTR_M_I2C_NACK_Msk)
#endif

#if defined(SCB_INTR_M_I2C_MASTER_STOP_Msk)
#define PSOC6_I2C_STOP_DONE_MASK SCB_INTR_M_I2C_MASTER_STOP_Msk
#elif defined(SCB_INTR_M_I2C_STOP_Msk)
#define PSOC6_I2C_STOP_DONE_MASK SCB_INTR_M_I2C_STOP_Msk
#else
#define PSOC6_I2C_STOP_DONE_MASK SCB_INTR_M_I2C_STOP_Msk
#endif

#define PSOC6_I2C_ERROR_MASK    (SCB_INTR_M_I2C_ARB_LOST_Msk | \
                                 SCB_INTR_M_I2C_NACK_Msk | \
                                 SCB_INTR_M_I2C_BUS_ERROR_Msk)

#define _BOOL2FLD(field, value) (((value) != false) ? (field ## _Msk) : 0UL)
#define _VAL2FLD(field, value)  (((uint32_t)(value) << field ## _Pos) & field ## _Msk)
#define _FLD2VAL(field, value)  (((uint32_t)(value) & field ## _Msk) >> field ## _Pos)
#define _FLD2BOOL(field, value) (((value) & (field ## _Msk)) != 0UL)
#define _CLR_SET_REG32(reg, field, value) \
    ((reg) = (((reg) & ((uint32_t)(~(field ## _Msk)))) | _VAL2FLD(field, value)))

#define CY_SCB_UART_TX_CTRL_SET_PARITY_Msk      (SCB_UART_TX_CTRL_PARITY_ENABLED_Msk | \
                                                 SCB_UART_TX_CTRL_PARITY_Msk)
#define CY_SCB_UART_TX_CTRL_SET_PARITY_Pos      SCB_UART_TX_CTRL_PARITY_Pos
#define CY_SCB_UART_RX_CTRL_SET_PARITY_Msk      (SCB_UART_RX_CTRL_PARITY_ENABLED_Msk | \
                                                 SCB_UART_RX_CTRL_PARITY_Msk)
#define CY_SCB_UART_RX_CTRL_SET_PARITY_Pos      SCB_UART_RX_CTRL_PARITY_Pos
#define CY_SCB_UART_TX_INTR_MASK    (SCB_INTR_TX_TRIGGER_Msk  | SCB_INTR_TX_NOT_FULL_Msk  | \
                                     SCB_INTR_TX_EMPTY_Msk    | SCB_INTR_TX_OVERFLOW_Msk   | \
                                     SCB_INTR_TX_UNDERFLOW_Msk | SCB_INTR_TX_UART_DONE_Msk | \
                                     SCB_INTR_TX_UART_NACK_Msk | SCB_INTR_TX_UART_ARB_LOST_Msk)
#define CY_SCB_UART_RX_INTR_MASK    (SCB_INTR_RX_TRIGGER_Msk  | SCB_INTR_RX_NOT_EMPTY_Msk | \
                                     SCB_INTR_RX_FULL_Msk     | SCB_INTR_RX_OVERFLOW_Msk  | \
                                     SCB_INTR_RX_UNDERFLOW_Msk | SCB_INTR_RX_FRAME_ERROR_Msk | \
                                     SCB_INTR_RX_PARITY_ERROR_Msk | SCB_INTR_RX_BREAK_DETECT_Msk)

typedef struct {
    CySCB_Type *dev;
    gpio_t tx_pin;
    gpio_t rx_pin;
    gpio_t rts_pin;
    gpio_t cts_pin;
    uint32_t hsiom_tx;
    uint32_t hsiom_rx;
    uint32_t hsiom_rts;
    uint32_t hsiom_cts;
    uint32_t pclk_ctl_index;
    uint8_t div8_index;
    uint8_t div8_int_div;
    uint32_t oversample;
    bool flow_control;
    uint32_t sys_int_src;
    uint8_t cm0_cpu_int_idx;
    IRQn_Type irqn;
} uart_conf_t;

typedef struct {
    CySCB_Type *dev;
    gpio_t miso_pin;
    gpio_t mosi_pin;
    gpio_t sclk_pin;
    gpio_t hw_cs_pin;
    uint32_t hsiom_miso;
    uint32_t hsiom_mosi;
    uint32_t hsiom_sclk;
    uint32_t hsiom_cs;
    uint32_t pclk_ctl_index;
    uint8_t div8_index;
    uint32_t oversample;
} spi_conf_t;

typedef struct {
    CySCB_Type *dev;
    gpio_t scl_pin;
    gpio_t sda_pin;
    uint32_t hsiom_scl;
    uint32_t hsiom_sda;
    uint32_t pclk_ctl_index;
    uint8_t div8_index;
    uint8_t div8_int_div;
    uint8_t low_phase_ovs;
    uint8_t high_phase_ovs;
} i2c_conf_t;

typedef struct {
    TCPWM_Type *dev;
    uint8_t timebase_chan;
    uint8_t alarm_chan;
    uint32_t pclk_timebase;
    uint32_t pclk_alarm;
    uint8_t div8_index;
    IRQn_Type irqn;
    uint32_t sys_int_src;
    uint8_t cm0_cpu_int_idx;
} timer_conf_t;

static const uart_conf_t uart_config[] = {
    {
        .dev = SCB5,
        .tx_pin = GPIO_PIN(5, 1),
        .rx_pin = GPIO_PIN(5, 0),
        .rts_pin = GPIO_UNDEF,
        .cts_pin = GPIO_UNDEF,
        .hsiom_tx = P5_1_SCB5_UART_TX,
        .hsiom_rx = P5_0_SCB5_UART_RX,
        .hsiom_rts = 0U,
        .hsiom_cts = 0U,
        .pclk_ctl_index = PCLK_SCB5_CLOCK,
        .div8_index = 0U,
        .div8_int_div = 35U,
        .oversample = 12UL,
        .flow_control = false,
        .sys_int_src = 0U,
        .cm0_cpu_int_idx = 0U,
#if defined(CPU_CORE_CORTEX_M0PLUS)
        .irqn = (IRQn_Type)NvicMux5_IRQn,
#else
        .irqn = (IRQn_Type)scb_5_interrupt_IRQn,
#endif
    },
    {
        .dev = SCB2,
        .tx_pin = GPIO_PIN(3, 1),
        .rx_pin = GPIO_PIN(3, 0),
        .rts_pin = GPIO_PIN(3, 2),
        .cts_pin = GPIO_PIN(3, 3),
        .hsiom_tx = P3_1_SCB2_UART_TX,
        .hsiom_rx = P3_0_SCB2_UART_RX,
        .hsiom_rts = P3_2_SCB2_UART_RTS,
        .hsiom_cts = P3_3_SCB2_UART_CTS,
        .pclk_ctl_index = PCLK_SCB2_CLOCK,
        .div8_index = 1U,
        .div8_int_div = 35U,
        .oversample = 12UL,
        .flow_control = true,
        .sys_int_src = (uint32_t)scb_2_interrupt_IRQn,
        .cm0_cpu_int_idx = 1U,
#if defined(CPU_CORE_CORTEX_M0PLUS)
        .irqn = (IRQn_Type)NvicMux1_IRQn,
#else
        .irqn = (IRQn_Type)scb_2_interrupt_IRQn,
#endif
    },
};

static const spi_conf_t spi_config[] = {
    {
        .dev = SCB2,
        .miso_pin = GPIO_PIN(9, 1),
        .mosi_pin = GPIO_PIN(9, 0),
        .sclk_pin = GPIO_PIN(9, 2),
        .hw_cs_pin = GPIO_PIN(9, 3),
        .hsiom_miso = P9_1_SCB2_SPI_MISO,
        .hsiom_mosi = P9_0_SCB2_SPI_MOSI,
        .hsiom_sclk = P9_2_SCB2_SPI_CLK,
        .hsiom_cs = P9_3_SCB2_SPI_SELECT0,
        .pclk_ctl_index = PCLK_SCB2_CLOCK,
        .div8_index = 3U,
        .oversample = 10UL,
    },
};

static const i2c_conf_t i2c_config[] = {
    {
        .dev = SCB0,
        .scl_pin = GPIO_PIN(0, 2),
        .sda_pin = GPIO_PIN(0, 3),
        .hsiom_scl = P0_2_SCB0_I2C_SCL,
        .hsiom_sda = P0_3_SCB0_I2C_SDA,
        .pclk_ctl_index = PCLK_SCB0_CLOCK,
        .div8_index = 2U,
        .div8_int_div = 124U,
        .low_phase_ovs = 8U,
        .high_phase_ovs = 8U,
    },
};

static const timer_conf_t timer_config[] = {
    {
        .dev = TCPWM0,
        .timebase_chan = 0U,
        .alarm_chan = 1U,
        .pclk_timebase = PCLK_TCPWM0_CLOCKS0,
        .pclk_alarm = PCLK_TCPWM0_CLOCKS1,
        .div8_index = 1U,
#if defined(CPU_CORE_CORTEX_M0PLUS)
        .irqn = (IRQn_Type)NvicMux0_IRQn,
#else
        .irqn = (IRQn_Type)tcpwm_0_interrupts_1_IRQn,
#endif
        .sys_int_src = (uint32_t)tcpwm_0_interrupts_1_IRQn,
        .cm0_cpu_int_idx = 0U,
    },
};

#ifndef CONFIG_PSOC6_BT_UART
#define CONFIG_PSOC6_BT_UART UART_DEV(1)
#endif

#ifndef PIN_BT_REG_ON
#define PIN_BT_REG_ON       GPIO_PIN(3, 4)
#endif

typedef enum {
    I2C_STATE_UNKNOWN = 0,
    I2C_STATE_IDLE,
    I2C_STATE_MASTER_ACTIVE,
    I2C_STATE_MASTER_WAIT,
    I2C_STATE_MASTER_RX0,
    I2C_STATE_MASTER_RX1,
    I2C_STATE_MASTER_ADDR,
    I2C_STATE_MASTER_TX,
    I2C_STATE_MASTER_TX_DONE,
    I2C_STATE_MASTER_STOP,
    I2C_STATE_MASTER_WAIT_STOP,
    I2C_STATE_MASTER_CMPLT
} i2c_state_t;

static inline void psoc6_set_drive_mode(gpio_t pin, uint32_t mode)
{
    GPIO_PRT_Type *port = GPIO_PORT(pin);
    uint32_t pos = (uint32_t)GPIO_PIN_NUM(pin) * PSOC6_DM_BITS;
    uint32_t cfg = port->CFG;

    cfg &= ~(PSOC6_DM_MASK << pos);
    cfg |= ((mode & PSOC6_DM_MASK) << pos);
    port->CFG = cfg;
}

static inline void psoc6_set_hsiom(gpio_t pin, uint32_t hsiom_sel)
{
    uint8_t port_num = GPIO_PORT_NUM(pin);
    uint8_t pin_num = GPIO_PIN_NUM(pin);
    uint32_t byte = (hsiom_sel & 0x1FU);
    uint32_t shift = (uint32_t)(pin_num & 0x03U) * 8U;
    uint32_t mask = 0xFFU << shift;

    if (pin_num < 4U) {
        uint32_t reg = HSIOM->PRT[port_num].PORT_SEL0;
        reg = (reg & ~mask) | ((byte << shift) & mask);
        HSIOM->PRT[port_num].PORT_SEL0 = reg;
    }
    else {
        uint32_t reg = HSIOM->PRT[port_num].PORT_SEL1;
        reg = (reg & ~mask) | ((byte << shift) & mask);
        HSIOM->PRT[port_num].PORT_SEL1 = reg;
    }
}

static inline void psoc6_route_gpio(gpio_t pin)
{
    psoc6_set_hsiom(pin, PSOC6_HSIOM_GPIO);
}

void cpu_patch_irq_vector(IRQn_Type irqn, void (*handler)(void));

#ifdef __cplusplus
}
#endif
