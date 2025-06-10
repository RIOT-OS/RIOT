#include "periph/uart.h"

#include "periph_cpu.h"

int uart_init(uart_t uart, uint32_t baud, uart_rx_cb_t rx_cb, void *arg) {
  // Set the UART pins to the correct function
  IO_BANK0->GPIO0_CTRL = FUNCTION_SELECT_UART;
  IO_BANK0->GPIO1_CTRL = FUNCTION_SELECT_UART;
  // Clear the ISO bits
  atomic_clear(&PADS_BANK0->GPIO0, PADS_BANK0_ISO_BITS);
  atomic_clear(&PADS_BANK0->GPIO1, PADS_BANK0_ISO_BITS);
  // Set IE bit for gpio1
  PADS_BANK0->GPIO1 = PADS_BANK0->GPIO1 | PADS_BANK0_GPIO0_IE_BITS;

  // We reset UART0 here, so we can be sure it is in a known state
  reset_component(RESET_UART0, RESET_UART0);

  UART0->UARTIBRD = IBRD;
  UART0->UARTFBRD = FBRD;
}

int uart_mode(uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
              uart_stop_bits_t stop_bits) {
  atomic_clear(&UART0->UARTCR, UART_UARTCR_UARTEN_BITS | UART_UARTCR_RXE_BITS |
                                   UART_UARTCR_TXE_BITS);
  // Set the data bits, parity, and stop bits
  UART0->UARTLCR_H = (uint32_t)data_bits << 5;

  switch (parity) {
    case UART_PARITY_NONE:
      break;
    default:
      // @todo: Implement other parity modes lel
      return UART_NOMODE;
  }

  UART0->UARTCR =
      UART_UARTCR_RXE_BITS | UART_UARTCR_TXE_BITS | UART_UARTCR_UARTEN_BITS;

  return UART_OK;
}

void uart_write(uart_t uart, const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    UART0->UARTDR = data[i];
    // Wait until the TX FIFO is empty before sending the next byte
    while (!(UART0->UARTFR & UART_UARTFR_TXFE_BITS));
  }
}

void uart_poweron(uart_t uart) {}
void uart_poweroff(uart_t uart) {}
