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
#include "em_usart.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_leuart.h"

/* guard file in case no UART device was specified */
#if UART_NUMOF
/**
 * @brief Each UART device has to store two callbacks.
 */
typedef struct {
	uart_rx_cb_t rx_cb;
	uart_tx_cb_t tx_cb;
	void *arg;
} uart_conf_t;

/**
 * @brief Allocate memory to store the callback functions.
 */
static uart_conf_t uart_config[UART_NUMOF];

int uart_init(uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb,
		uart_tx_cb_t tx_cb, void *arg) {
	/* do basic initialization */
	int res = uart_init_blocking(uart, baudrate);
	if (res < 0) {
		return res;
	}

	/* remember callback addresses */
	uart_config[uart].rx_cb = rx_cb;
	uart_config[uart].tx_cb = tx_cb;
	uart_config[uart].arg = arg;

	/* enable receive interrupt */
	switch (uart) {
#if UART_0_EN
	while(1){}
	case UART_0:
		NVIC_SetPriority(UART_0_IRQ_RX_CHAN, UART_0_IRQ_RX_PRIO);
		NVIC_SetPriority(UART_0_IRQ_TX_CHAN, UART_0_IRQ_TX_PRIO);
		NVIC_EnableIRQ(UART_0_IRQ_RX_CHAN);
		NVIC_EnableIRQ(UART_0_IRQ_TX_CHAN);
		USART_IntEnable(UART_0_DEV, _USART_IEN_RXDATAV_MASK);
		break;
#endif
#if UART_1_EN
	case UART_1:
		NVIC_SetPriority(UART_1_IRQ_RX_CHAN, UART_1_IRQ_RX_PRIO);
		NVIC_SetPriority(UART_1_IRQ_TX_CHAN, UART_1_IRQ_TX_PRIO);
		NVIC_EnableIRQ(UART_1_IRQ_RX_CHAN);
		NVIC_EnableIRQ(UART_1_IRQ_TX_CHAN);
		USART_IntEnable(UART_1_DEV, _USART_IEN_RXDATAV_MASK);
		break;
#endif
		/* UART2 is considered as LEUART device which is functionable in low power mode */
#if UART_2_EN
	case UART_2:
		NVIC_SetPriority(UART_2_IRQ, UART_2_IRQ_PRIO);
		NVIC_EnableIRQ(UART_2_IRQ);
		LEUART_IntEnable(UART_2_DEV, LEUART_IEN_RXDATAV);
	break;
#endif
	}
	return 0;
}

int uart_init_blocking(uart_t _uart, uint32_t baudrate) {
#if UART_0_EN || UART_1_EN
	USART_TypeDef *uart;
	USART_InitAsync_TypeDef uartInit = USART_INITASYNC_DEFAULT;
#endif

#if UART_2_EN
	LEUART_Init_TypeDef leuartInit;
#endif


	switch (_uart) {
#if UART_0_EN
	case UART_0:
		uart = UART_0_DEV;
		/* Enabling clock to USART0 */
		UART_0_CLKEN();
		/* Output PIN */
		GPIO_PinModeSet(UART_0_PORT, UART_0_TX_PIN, gpioModePushPull, 1);
		/* Input PIN */
		GPIO_PinModeSet(UART_0_PORT, UART_0_RX_PIN, gpioModeInput, 0);
		/* Prepare struct for initializing UART in asynchronous mode*/
		uartInit.enable = usartDisable; /* Don't enable UART upon intialization */
		uartInit.refFreq = 0; /* Provide information on reference frequency. When set to 0, the reference frequency is */
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
		UART_1_CLKEN();
		/* Output PIN */
		GPIO_PinModeSet(UART_1_PORT, UART_1_TX_PIN, gpioModePushPull, 1);
		/* Input PIN */
		GPIO_PinModeSet(UART_1_PORT, UART_1_RX_PIN, gpioModeInput, 0);
		/* Prepare struct for initializing UART in asynchronous mode*/
		uartInit.enable = usartDisable; /* Don't enable UART upon intialization */
		uartInit.refFreq = 0; /* Provide information on reference frequency. When set to 0, the reference frequency is */
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
		UART_2_CLKEN();
		GPIO_PinModeSet(UART_2_PORT, UART_2_TX_PIN, gpioModePushPull, 1);
		GPIO_PinModeSet(UART_2_PORT, UART_2_RX_PIN, gpioModeInput, 0);
		LEUART_Reset(UART_2_DEV);
		leuartInit.enable = leuartEnable;
		leuartInit.baudrate = baudrate;
		leuartInit.databits = leuartDatabits8;
		leuartInit.parity = leuartNoParity;
		leuartInit.stopbits = leuartStopbits1;
		leuartInit.refFreq = 0;
		LEUART_Init(UART_2_DEV, &leuartInit);
		UART_2_DEV->ROUTE = LEUART_ROUTE_TXPEN | LEUART_ROUTE_RXPEN | UART_2_LOC;
		LEUART_Enable(UART_2_DEV, usartEnable);
		break;
#endif
	default:
		break;
	}
	return 0;
}

void uart_tx_begin(uart_t uart) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		USART_IntEnable(UART_0_DEV, UART_IEN_TXBL);
		break;
#endif
#if UART_1_EN
	case UART_1:
		USART_IntEnable(UART_1_DEV, UART_IEN_TXBL);
		break;
#endif
#if UART_2_EN
	case UART_2:
		LEUART_IntEnable(UART_2_DEV, LEUART_IEN_TXBL);
		break;
#endif
	}
}

int uart_write(uart_t uart, char data) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		((USART_TypeDef *)UART_0_DEV)->TXDATA = (uint32_t) data;
		break;
#endif
#if UART_1_EN
	case UART_1:
		((USART_TypeDef *)UART_1_DEV)->TXDATA = (uint32_t) data;
		break;
#endif
#if UART_2_EN
	case UART_2:
		((LEUART_TypeDef *)UART_2_DEV)->TXDATA = (uint32_t) data;
		break;
#endif
	}

	return 0;
}

int uart_read_blocking(uart_t uart, char *data) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		(*data) = USART_Rx(UART_0_DEV);
		break;
#endif
#if UART_1_EN
	case UART_1:
		(*data) = USART_Rx(UART_1_DEV);
		break;
#endif
#if UART_2_EN
		case UART_2:
		(*data) = LEUART_Rx(UART_2_DEV);
		break;
#endif
	}

	return 1;
}

int uart_write_blocking(uart_t uart, char data) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		USART_Tx(UART_0_DEV, data);
		break;
#endif
#if UART_1_EN
	case UART_1:
		USART_Tx(UART_1_DEV, data);
		break;
#endif
#if UART_2_EN
		case UART_2:
		LEUART_Tx(UART_2_DEV, data);
		break;
#endif
	}
	return 1;
}

void uart_poweron(uart_t uart) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		UART_0_CLKEN();
		break;
#endif
#if UART_1_EN
	case UART_1:
		UART_1_CLKEN();
		break;
#endif
#if UART_2_EN
	case UART_2:
		UART_2_CLKEN();
		break;
#endif
	}
}

void uart_poweroff(uart_t uart) {
	switch (uart) {
#if UART_0_EN
	case UART_0:
		UART_0_CLKDIS();
		break;
#endif
#if UART_1_EN
	case UART_1:
		UART_1_CLKDIS();
		break;
#endif
#if UART_2_EN
	case UART_2:
		UART_2_CLKDIS();
		break;
#endif
	}
}

static inline void irq_write(uint8_t uartnum, USART_TypeDef *usart) {
	if (uart_config[uartnum].tx_cb(uart_config[uartnum].arg) == 0) {
		USART_IntDisable(usart, UART_IF_TXBL);
	}
}

static inline void irq_read(uint8_t uartnum, uint8_t data) {
	uart_config[uartnum].rx_cb(uart_config[uartnum].arg, data);
}

#if UART_0_EN
void UART_0_TX_ISR(void) {
	/* Check TX buffer level status */
	if (UART_0_DEV->STATUS & UART_STATUS_TXBL) {
		irq_write(UART_0, ((USART_TypeDef*)UART_0_DEV));
	}

	if (sched_context_switch_request) {
		thread_yield();
	}
}

void UART_0_RX_ISR(void) {
	/* Check for RX data valid interrupt */
	if (UART_0_DEV->STATUS & UART_STATUS_RXDATAV) {
		/* Copy data into RX Buffer */
		uint8_t rxData = USART_Rx(UART_0_DEV);
		irq_read(UART_0, rxData);
	}
	if (sched_context_switch_request) {
		thread_yield();
	}
}
#endif

#if UART_1_EN
void UART_1_TX_ISR(void) {
	/* Check TX buffer level status */
	if (UART_1_DEV->STATUS & UART_STATUS_TXBL) {
		irq_write(UART_1, UART_1_DEV);
	}

	if (sched_context_switch_request) {
		thread_yield();
	}
}

void UART_1_RX_ISR(void) {
	/* Check for RX data valid interrupt */
	if (UART_1_DEV->STATUS & UART_STATUS_RXDATAV) {
		/* Copy data into RX Buffer */
		uint8_t rxData = USART_Rx(UART_1_DEV);
		irq_read(UART_1, rxData);
	}
	if (sched_context_switch_request) {
		thread_yield();
	}
}
#endif

#if UART_2_EN
void UART_2_ISR(void) {
	/* Check for RX data valid interrupt */
	if(UART_2_DEV->STATUS & LEUART_STATUS_RXDATAV){
		/* Copy data into RX Buffer */
		uint8_t rxData = LEUART_Rx(UART_2_DEV);
		irq_read(UART_2, rxData);
	}
	/* Check TX buffer level status */
	if(UART_2_DEV->IF & LEUART_IF_TXBL & UART_2_DEV->IEN){
		if (uart_config[UART_2].tx_cb(uart_config[UART_2].arg) == 0) {
			LEUART_IntDisable(UART_2_DEV, UART_IEN_TXBL);
		}
	}

	if (sched_context_switch_request) {
		thread_yield();
	}
}
#endif

#endif /* UART_NUMOF */
