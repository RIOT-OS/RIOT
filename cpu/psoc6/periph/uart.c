#include <stdbool.h>

#include "assert.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "ztimer.h"

static uart_rx_cb_t _rx_cb[UART_NUMOF];
static void *_rx_arg[UART_NUMOF];

// drain rx fifo, forward bytes
static void _uart_irq_handler(uart_t uart)
{
    const uart_conf_t *cfg = &uart_config[uart];
    CySCB_Type *dev = cfg->dev;
    uint32_t intr = dev->INTR_RX_MASKED;

    while (_FLD2VAL(SCB_RX_FIFO_STATUS_USED, dev->RX_FIFO_STATUS) != 0UL) {
        uint8_t data = (uint8_t)dev->RX_FIFO_RD;
        if (_rx_cb[uart]) {
            _rx_cb[uart](_rx_arg[uart], data);
        }
    }

    dev->INTR_RX = intr;
    dev->INTR_RX = SCB_INTR_RX_NOT_EMPTY_Msk;
}

void scb_2_interrupt_IRQHandler(void)
{
    _uart_irq_handler(UART_DEV(1));
}

void NvicMux1_IRQHandler(void)
{
    _uart_irq_handler(UART_DEV(1));
}

// init uart + pin mux
int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    if (uart >= UART_NUMOF) {
        return -ENODEV;
    }

    const uart_conf_t *cfg = &uart_config[uart];
    CySCB_Type *dev = cfg->dev;
    bool fast_hci_uart = (baudrate > 115200U);

    _rx_cb[uart] = rx_cb;
    _rx_arg[uart] = arg;

    dev->CTRL &= ~SCB_CTRL_ENABLED_Msk;
    dev->CTRL = _BOOL2FLD(SCB_CTRL_ADDR_ACCEPT, 0UL) |
                _BOOL2FLD(SCB_CTRL_BYTE_MODE, 1UL) |
                _VAL2FLD(SCB_CTRL_OVS, cfg->oversample - 1UL) |
                _VAL2FLD(SCB_CTRL_MODE, PSOC6_SCB_CTRL_MODE_UART);

    dev->UART_CTRL = _VAL2FLD(SCB_UART_CTRL_MODE, 0U);

    dev->UART_RX_CTRL = _BOOL2FLD(SCB_UART_RX_CTRL_POLARITY, 0UL) |
                        _BOOL2FLD(SCB_UART_RX_CTRL_MP_MODE, 0UL) |
                        _BOOL2FLD(SCB_UART_RX_CTRL_DROP_ON_PARITY_ERROR, 0UL) |
                        _BOOL2FLD(SCB_UART_RX_CTRL_DROP_ON_FRAME_ERROR, 0UL) |
                        _VAL2FLD(SCB_UART_RX_CTRL_BREAK_WIDTH, 10UL) |
                        _VAL2FLD(SCB_UART_RX_CTRL_STOP_BITS, 1UL) |
                        _VAL2FLD(CY_SCB_UART_RX_CTRL_SET_PARITY, 0U);

    dev->RX_CTRL = _BOOL2FLD(SCB_RX_CTRL_MSB_FIRST, 0UL) |
                   _BOOL2FLD(SCB_RX_CTRL_MEDIAN, 0UL) |
                   _VAL2FLD(SCB_RX_CTRL_DATA_WIDTH, 7UL);

    dev->RX_MATCH = _VAL2FLD(SCB_RX_MATCH_ADDR, 0UL) |
                    _VAL2FLD(SCB_RX_MATCH_MASK, 0UL);

    dev->UART_TX_CTRL = _BOOL2FLD(SCB_UART_TX_CTRL_RETRY_ON_NACK, 0UL) |
                        _VAL2FLD(SCB_UART_TX_CTRL_STOP_BITS, 1UL) |
                        _VAL2FLD(CY_SCB_UART_TX_CTRL_SET_PARITY, 0U);

    dev->TX_CTRL = _BOOL2FLD(SCB_TX_CTRL_MSB_FIRST, 0UL) |
                   _VAL2FLD(SCB_TX_CTRL_DATA_WIDTH, 7UL) |
                   _BOOL2FLD(SCB_TX_CTRL_OPEN_DRAIN, 0UL);

    dev->RX_FIFO_CTRL = _VAL2FLD(SCB_RX_FIFO_CTRL_TRIGGER_LEVEL, 8UL);
    dev->TX_FIFO_CTRL = _VAL2FLD(SCB_TX_FIFO_CTRL_TRIGGER_LEVEL, 1UL);

    dev->UART_FLOW_CTRL = _BOOL2FLD(SCB_UART_FLOW_CTRL_CTS_ENABLED, cfg->flow_control) |
                          _BOOL2FLD(SCB_UART_FLOW_CTRL_CTS_POLARITY, 0UL) |
                          _BOOL2FLD(SCB_UART_FLOW_CTRL_RTS_POLARITY, 0UL) |
                          _VAL2FLD(SCB_UART_FLOW_CTRL_TRIGGER_LEVEL, 1UL);

    dev->INTR_RX_MASK = 0UL;
    dev->INTR_TX_MASK = 0UL;

    // route pins to scb uart
    psoc6_set_hsiom(cfg->rx_pin, cfg->hsiom_rx);
    psoc6_set_hsiom(cfg->tx_pin, cfg->hsiom_tx);
    psoc6_set_drive_mode(cfg->rx_pin, PSOC6_DM_HIGHZ);
    psoc6_set_drive_mode(cfg->tx_pin, PSOC6_DM_STRONG);

    if (cfg->flow_control) {
        // cfg rts/cts pins if enabled
        psoc6_set_hsiom(cfg->cts_pin, cfg->hsiom_cts);
        psoc6_set_hsiom(cfg->rts_pin, cfg->hsiom_rts);
        psoc6_set_drive_mode(cfg->cts_pin, PSOC6_DM_HIGHZ);
        psoc6_set_drive_mode(cfg->rts_pin, PSOC6_DM_STRONG);
    }

    PERI->CLOCK_CTL[cfg->pclk_ctl_index] =
        _VAL2FLD(CY_PERI_CLOCK_CTL_TYPE_SEL, 0U) |
        _VAL2FLD(CY_PERI_CLOCK_CTL_DIV_SEL, cfg->div8_index);
    _CLR_SET_REG32(PERI->DIV_8_CTL[cfg->div8_index], PERI_DIV_8_CTL_INT8_DIV,
                   fast_hci_uart ? 16U : cfg->div8_int_div);
    PERI->DIV_CMD = PERI_DIV_CMD_ENABLE_Msk |
                    CY_PERI_DIV_CMD_PA_TYPE_SEL_Msk |
                    CY_PERI_DIV_CMD_PA_DIV_SEL_Msk |
                    _VAL2FLD(CY_PERI_DIV_CMD_TYPE_SEL, 0U) |
                    _VAL2FLD(CY_PERI_DIV_CMD_DIV_SEL, cfg->div8_index);

    if (rx_cb) {
#if defined(CPU_CORE_CORTEX_M0PLUS)
        // route scb irq via cm0 mux
        CPUSS->CM0_SYSTEM_INT_CTL[cfg->sys_int_src] =
            _VAL2FLD(CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_IDX, cfg->cm0_cpu_int_idx) |
            CPUSS_V2_CM0_SYSTEM_INT_CTL_CPU_INT_VALID_Msk;
        cpu_patch_irq_vector(cfg->irqn, NvicMux1_IRQHandler);
#endif
        NVIC_ClearPendingIRQ(cfg->irqn);
        NVIC_EnableIRQ(cfg->irqn);
        NVIC_SetPriority(cfg->irqn, 1);
        dev->INTR_RX_MASK = SCB_INTR_RX_MASK_NOT_EMPTY_Msk;
    }

    dev->CTRL |= SCB_CTRL_ENABLED_Msk;
    return 0;
}

static void _wait_ms(uint32_t ms)
{
    if (ms > 0U) {
        ztimer_sleep(ZTIMER_USEC, ms * 1000U);
    }
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    if ((uart >= UART_NUMOF) || (data == NULL) || (len == 0U)) {
        return;
    }

    const uart_conf_t *cfg = &uart_config[uart];
    CySCB_Type *dev = cfg->dev;
    uint32_t fifo_size = _FLD2BOOL(SCB_CTRL_BYTE_MODE, dev->CTRL) ?
                         SCB_FIFO_SIZE : (SCB_FIFO_SIZE / 2UL);

    // fill tx fifo
    for (size_t i = 0; i < len; i++) {
        while (_FLD2VAL(SCB_TX_FIFO_STATUS_USED, dev->TX_FIFO_STATUS) >= fifo_size) {
        }
        dev->TX_FIFO_WR = data[i];
    }

    // wait flush, avoid lockup
    uint32_t timeout_ms = 50U;
    while (timeout_ms--) {
        uint32_t pending = _FLD2VAL(SCB_TX_FIFO_STATUS_USED, dev->TX_FIFO_STATUS) +
                           _FLD2VAL(SCB_TX_FIFO_STATUS_SR_VALID, dev->TX_FIFO_STATUS);
        if (pending == 0UL) {
            return;
        }
        _wait_ms(1U);
    }
}

void uart_poweron(uart_t uart)
{
    if (uart < UART_NUMOF) {
        uart_config[uart].dev->CTRL |= SCB_CTRL_ENABLED_Msk;
    }
}

void uart_poweroff(uart_t uart)
{
    if (uart < UART_NUMOF) {
        uart_config[uart].dev->CTRL &= ~SCB_CTRL_ENABLED_Msk;
    }
}

void uart_enable_tx(uart_t uart)
{
    if (uart < UART_NUMOF) {
        const uart_conf_t *cfg = &uart_config[uart];
        psoc6_set_hsiom(cfg->tx_pin, cfg->hsiom_tx);
        psoc6_set_drive_mode(cfg->tx_pin, PSOC6_DM_STRONG);
    }
}

void uart_disable_tx(uart_t uart)
{
    if (uart < UART_NUMOF) {
        psoc6_set_drive_mode(uart_config[uart].tx_pin, PSOC6_DM_HIGHZ);
    }
}
