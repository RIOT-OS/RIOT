/* provide address for UASRT register map by taking the address of the first
 * register (as provided by the vendor files) */
PROVIDE(USART_0_SFR = IE1);
PROVIDE(USART_1_SFR = IE2);

PROVIDE(USART_0     = U0CTL);
PROVIDE(USART_1     = U1CTL);
