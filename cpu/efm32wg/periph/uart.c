/**
 * @ingroup     cpu_efm32
 * @{
 *
 * @file
 * @brief       Low-level UART driver implementation
 *
 * @author      Matuesz Kubaszek <mathir.km.riot@gmail.com
 *
 * @}
 */

#include "cpu.h"
#include "thread.h"
#include "sched.h"
#include "periph_conf.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "ringbuffer.h"
#include "em_usart.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_leuart.h"

static int
uart_init_blocking(uart_t _uart, uint32_t baudrate);

/* guard file in case no UART device was specified */
#if UART_NUMOF
/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
    uart_rx_cb_t rx_cb;
    void *arg;
} uart_conf_t;

#if UART_0_EN && UART_0_ENABLE_BUF
ringbuffer_t rb_uart0;
char buffer0[UART_0_BUFISIZE];
#endif
#if UART_1_EN && UART_1_ENABLE_BUF
ringbuffer_t rb_uart1;
char buffer1[UART_1_BUFSIZE];
#endif
#if UART_2_EN && UART_2_ENABLE_BUF
ringbuffer_t rb_uart2;
char buffer2[UART_2_BUFSIZE];
#endif

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg)
{
    /* do basic initialization */
    int res = uart_init_blocking(uart, baudrate);
    if(res < 0) {
        return res;
    }

    /* remember callback addresses */
    uart_config[uart].rx_cb = rx_cb;
    uart_config[uart].arg = arg;

    /* enable receive and send interrupt */
    switch(uart) {
#if UART_0_EN
    case UART_0:
        NVIC_SetPriority(UART_0_IRQ_RX_CHAN, UART_0_IRQ_RX_PRIO);
        NVIC_EnableIRQ(UART_0_IRQ_RX_CHAN);
        USART_IntEnable(UART_0_DEV, _USART_IEN_RXDATAV_MASK);
#if UART_0_ENABLE_BUF
        ringbuffer_init(rb_uart0, buffer0, UART_0_BUFISIZE);
        NVIC_SetPriority(UART_0_IRQ_TX_CHAN, UART_0_IRQ_TX_PRIO);
        NVIC_EnableIRQ(UART_0_IRQ_TX_CHAN);
        USART_IntEnable(UART_0_DEV, _USART_IEN_TXC_MASK);
#endif
        break;
#endif
#if UART_1_EN
        case UART_1:
        NVIC_SetPriority(UART_1_IRQ_RX_CHAN, UART_1_IRQ_RX_PRIO);
        NVIC_SetPriority(UART_1_IRQ_TX_CHAN, UART_1_IRQ_TX_PRIO);
        NVIC_EnableIRQ(UART_1_IRQ_RX_CHAN);
        NVIC_EnableIRQ(UART_1_IRQ_TX_CHAN);
        USART_IntEnable(UART_1_DEV, _USART_IEN_RXDATAV_MASK);
#if UART_1_ENABLE_BUF
        ringbuffer_init(rb_uart1, buffer0, UART_1_BUFISIZE);
        NVIC_SetPriority(UART_1_IRQ_TX_CHAN, UART_1_IRQ_TX_PRIO);
        NVIC_EnableIRQ(UART_1_IRQ_TX_CHAN);
        USART_IntEnable(UART_1_DEV, _USART_IEN_TXC_MASK);
#endif
        break;
#endif
        /* UART2 is considered as LEUART device which is functionable in low power mode */
#if UART_2_EN
    case UART_2:
        NVIC_SetPriority(UART_2_IRQ, UART_2_IRQ_PRIO);
        NVIC_EnableIRQ(UART_2_IRQ);
        LEUART_IntEnable(UART_2_DEV, LEUART_IEN_RXDATAV);
#if UART_2_ENABLE_BUF
        ringbuffer_init(&rb_uart2, buffer2, UART_2_BUFSIZE);
        LEUART_IntEnable(UART_2_DEV, _LEUART_IEN_TXBL_MASK);
#endif
        break;
#endif
    default:
        /* Invalid usart device */
        return -1;
    }
    return 0;
}

static int uart_init_blocking(uart_t _uart, uint32_t baudrate)
{
#if UART_0_EN || UART_1_EN
    USART_TypeDef *uart;
    USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;
#endif

#if UART_2_EN
    LEUART_Init_TypeDef leuartInit;
#endif

    switch(_uart) {
#if UART_0_EN
    case UART_0:
        uart = UART_0_DEV;
        /* Enabling clock to USART0 */
        CMU_ClockEnable(cmuClock_USART0, true);
        /* Output PIN */
        gpio_init(GPIO_T(UART_0_PORT, UART_0_TX_PIN), GPIO_DIR_PUSH_PULL,
                GPIO_PULLUP);
        gpio_init(GPIO_T(UART_0_PORT, UART_0_RX_PIN), GPIO_DIR_INPUT,
                GPIO_PULLDOWN);
        /* Prepare struct for initializing UART in asynchronous mode*/
        uartInit.enable = usartDisable; /* Don't enable UART upon intialization */
        uartInit.refFreq = 0; /* Provide information on reference frequency.
         When set to 0, the reference frequency is */
        uartInit.baudrate = baudrate; /* Baud rate */
        uartInit.oversampling = usartOVS16; /* Oversampling. Range is 4x, 6x, 8x or 16x */
        uartInit.databits = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
        uartInit.parity = usartNoParity; /* Parity mode */
        uartInit.stopbits = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
        uartInit.mvdis = false; /* Disable majority voting */
        uartInit.prsRxEnable = false; /* Enable USART Rx via Peripheral Reflex System */
        uartInit.prsRxCh = usartPrsRxCh0; /* Select PRS channel if enabled */
        /* Initialize USART with uartInit struct */
        USART_InitAsync(uart, &uartInit);
        uart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_0_LOC;
        USART_Enable(UART_0_DEV, usartEnable);
        break;

#endif
#if UART_1_EN
        case UART_1:
        uart = UART_1_DEV;
        /* Enabling clock to USART0 */
        uart = UART_1_DEV;
        CMU_ClockEnable(cmuClock_USART1, true);
        /* Output PIN */
        gpio_init(GPIO_T(UART_1_PORT, UART_1_TX_PIN), GPIO_DIR_PUSH_PULL, GPIO_PULLUP);
        gpio_init(GPIO_T(UART_1_PORT, UART_1_RX_PIN), GPIO_DIR_INPUT, GPIO_PULLDOWN);
        /* Prepare struct for initializing UART in asynchronous mode*/
        uartInit.enable = usartDisable; /* Don't enable UART upon intialization */
        uartInit.refFreq = 0; /* Provide information on reference frequency.
         When set to 0, the reference frequency is */
        uartInit.baudrate = baudrate; /* Baud rate */
        uartInit.oversampling = usartOVS16; /* Oversampling. Range is 4x, 6x, 8x or 16x */
        uartInit.databits = usartDatabits8; /* Number of data bits. Range is 4 to 10 */
        uartInit.parity = usartNoParity; /* Parity mode */
        uartInit.stopbits = usartStopbits1; /* Number of stop bits. Range is 0 to 2 */
        uartInit.mvdis = false; /* Disable majority voting */
        uartInit.prsRxEnable = false; /* Enable USART Rx via Peripheral Reflex System */
        uartInit.prsRxCh = usartPrsRxCh0; /* Select PRS channel if enabled */
        /* Initialize USART with uartInit struct */
        USART_InitAsync(uart, &uartInit);
        uart->ROUTE = UART_ROUTE_RXPEN | UART_ROUTE_TXPEN | UART_1_LOC;
        USART_Enable(UART_1_DEV, usartEnable);
        break;
#endif
#if UART_2_EN
    case UART_2:
        CMU_ClockEnable(cmuClock_LEUART0, true);
        gpio_init(GPIO_T(UART_2_PORT, UART_2_TX_PIN), GPIO_DIR_PUSH_PULL,
                GPIO_PULLUP);
        gpio_init(GPIO_T(UART_2_PORT, UART_2_RX_PIN), GPIO_DIR_INPUT,
                GPIO_PULLDOWN);
        LEUART_Reset(UART_2_DEV);
        leuartInit.enable = leuartEnable;
        leuartInit.baudrate = baudrate;
        leuartInit.databits = leuartDatabits8;
        leuartInit.parity = leuartNoParity;
        leuartInit.stopbits = leuartStopbits1;
        leuartInit.refFreq = 0;
        LEUART_Init(UART_2_DEV, &leuartInit);
        UART_2_DEV->ROUTE =
        LEUART_ROUTE_TXPEN | LEUART_ROUTE_RXPEN | UART_2_LOC;
        LEUART_Enable(UART_2_DEV, usartEnable);
        break;
#endif
    default:
        return -1;
    }
    return 0;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len)
{
    switch(uart) {
#if UART_0_EN
    case UART_0:
#if UART_0_ENABLE_BUF
        for(int count = 0; count < len; count++) {
            NVIC_DisableIRQ(UART_0_IRQ_TX_CHAN);
            ringbuffer_add_one(&rb_uart0, data[count]);
            NVIC_EnableIRQ(UART_0_IRQ_TX_CHAN);
        }
#else
        for(size_t i = 0; i < len; i++) {
            /* Check that transmit buffer is empty */
            while(!(UART_0_DEV->STATUS & USART_STATUS_TXBL))
            ;
            /* Write data to buffer */
            UART_0_DEV->TXDATA = (uint32_t)data[i];
        }
#endif
        break;
#endif
#if UART_1_EN
        case UART_1:
#if UART_1_ENABLE_BUF
        for(int count = 0; count < len; count++) {
            NVIC_DisableIRQ(UART_1_IRQ_TX_CHAN);
            ringbuffer_add_one(&rb_uart1, data[count]);
            NVIC_EnableIRQ(UART_1_IRQ_TX_CHAN);
        }
#else
        for(size_t i = 0; i < len; i++)
        {
            /* Check that transmit buffer is empty */
            while(!(UART_1_DEV->STATUS & USART_STATUS_TXBL))
            ;
            /* Write data to buffer */
            UART_1_DEV->TXDATA = (uint32_t)data[i];
        }
#endif
        break;
#endif
#if UART_2_EN
    case UART_2:
#if UART_2_ENABLE_BUF
        for(int count = 0; count < len; count++) {
            NVIC_DisableIRQ(UART_2_IRQ);
            ringbuffer_add_one(&rb_uart2, data[count]);
            NVIC_EnableIRQ(UART_2_IRQ);
        }
        LEUART_IntEnable(UART_2_DEV, LEUART_IF_TXBL);
#else
        for(size_t i = 0; i < len; i++)
        {
            /* Check that transmit buffer is empty */
            while (!(UART_2_DEV->STATUS & LEUART_STATUS_TXBL))
            ;
            /* Avoid deadlock if modifying the same register twice when freeze mode is */
            /* activated. */
            if (!(UART_2_DEV->FREEZE & LEUART_FREEZE_REGFREEZE))
            {
                /* Wait for any pending previous write operation to have been completed */
                /* in low frequency domain */
                while (UART_2_DEV->SYNCBUSY & LEUART_SYNCBUSY_TXDATA);
                ;
                UART_2_DEV->TXDATA = (uint32_t)data[i];
            }

        }
#endif
        break;
#endif
    }
}

void uart_poweron(uart_t uart)
{
    switch(uart) {
#if UART_0_EN
    case UART_0:
        CMU_ClockEnable(cmuClock_USART0, true);
        break;
#endif
#if UART_1_EN
        case UART_1:
        CMU_ClockEnable(cmuClock_USART1, true);
        break;
#endif
#if UART_2_EN
    case UART_2:
        CMU_ClockEnable(cmuClock_LEUART0, true);
        break;
#endif
    }
}

void uart_poweroff(uart_t uart)
{
    switch(uart) {
#if UART_0_EN
    case UART_0:
        CMU_ClockEnable(cmuClock_USART0, false);
        break;
#endif
#if UART_1_EN
        case UART_1:
        CMU_ClockEnable(cmuClock_USART1, false);
        break;
#endif
#if UART_2_EN
    case UART_2:
        CMU_ClockEnable(cmuClock_LEUART0, false);
        break;
#endif
    }
}

static inline void irq_read(uint8_t uartnum, uint8_t data)
{
    uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
}

#if UART_0_EN
void UART_0_TX_ISR(void)
{
    /* Check TX buffer level status */
    if(UART_0_DEV->STATUS & UART_STATUS_TXC) {
        int data = ringbuffer_get_one(&rb_uart0);
        if(-1 != data)
            /* Write data to buffer */
            UART_0_DEV->TXDATA = (uint32_t)data;
    }

    if(sched_context_switch_request) {
        thread_yield();
    }
}

void UART_0_RX_ISR(void)
{
    /* Check for RX data valid interrupt */
    if(UART_0_DEV->STATUS & UART_STATUS_RXDATAV) {
        /* Copy data into RX Buffer */
        uint8_t rxData = USART_Rx(UART_0_DEV);
        irq_read(UART_0, rxData);
    }
    if(sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#if UART_1_EN
void UART_1_TX_ISR(void)
{
    /* Check TX buffer level status */
    if(UART_1_DEV->STATUS & UART_STATUS_TXC) {
        int data = ringbuffer_get_one(&rb_uart1);
        if(-1 != data)
        /* Write data to buffer */
        UART_1_DEV->TXDATA = (uint32_t)data;
    }

    if(sched_context_switch_request) {
        thread_yield();
    }
}

void UART_1_RX_ISR(void)
{
    /* Check for RX data valid interrupt */
    if (UART_1_DEV->STATUS & UART_STATUS_RXDATAV)
    {
        /* Copy data into RX Buffer */
        uint8_t rxData = USART_Rx(UART_1_DEV);
        irq_read(UART_1, rxData);
    }
    if (sched_context_switch_request)
    {
        thread_yield();
    }
}
#endif

#if UART_2_EN
void UART_2_ISR(void)
{
    /* Check for RX data valid interrupt */
    if(UART_2_DEV->STATUS & LEUART_STATUS_RXDATAV) {
        /* Copy data into RX Buffer */
        uint8_t rxData = LEUART_Rx(UART_2_DEV);
        irq_read(UART_2, rxData);
    }
    /* Check TX buffer level status */
    if(UART_2_DEV->IF & LEUART_IF_TXBL & UART_2_DEV->IEN) {
#if UART_2_ENABLE_BUF
        int data = ringbuffer_get_one(&rb_uart2);
        if(-1 != data) {
            /* Avoid deadlock if modifying the same register twice when freeze mode is */
            /* activated. */
            if(!(UART_2_DEV->FREEZE & LEUART_FREEZE_REGFREEZE)) {
                /* Wait for any pending previous write operation to have been completed */
                /* in low frequency domain */
                while(UART_2_DEV->SYNCBUSY & LEUART_SYNCBUSY_TXDATA)
                    ;;
            }
            /* Write data to TX register */
            UART_2_DEV->TXDATA = (uint32_t)data;
        } else {
            LEUART_IntDisable(UART_2_DEV, LEUART_IF_TXBL);
        }
#endif
    }

    if(sched_context_switch_request) {
        thread_yield();
    }
}
#endif

#endif /* UART_NUMOF */
